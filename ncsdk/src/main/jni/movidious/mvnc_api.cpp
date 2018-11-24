//
// Created by saki on 2018/11/21.
//

#if 0	// set 0 is you need debug log, otherwise set 1
	#ifndef LOG_NDEBUG
		#define	LOG_NDEBUG
	#endif
	#undef USE_LOGALL
#else
	#define USE_LOGALL
	#undef LOG_NDEBUG
	#undef NDEBUG
#endif

#include <dlfcn.h>		// For dladdr
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <string>

// common
#include "utilbase.h"
#include "common_utils.h"

// ncs
#include "mvnc.h"
#include "mvnc_api.h"
#include "mvnc_internal.h"
#include "usb_data_link.h"

extern int run_test(serenegiant::usb::ncs::MvNcApi *api, const std::string &base_path);

namespace serenegiant {
namespace usb {
namespace ncs {

class Graph;

class Device {
friend MvNcApi;

	mutable Mutex lock;
	UsbDataLink *data_link;
	std::list<Graph *> graphs; // List of associated graphs

	mvnc_int32_t config_backoff_time_normal, config_backoff_time_high, config_backoff_time_critical;
	mvnc_int32_t config_temperature_debug;
	mvnc_float_t config_temp_lim_upper, config_temp_lim_lower;
	mvnc_float_t *thermal_stats;
	mvnc_int32_t throttle_happened;
	char *optimisation_list;

public:
	Device(UsbDataLink *_data_link);
	~Device();
	
	int remove(Graph *graph);
	inline int get_status(myriadStatus_t &myriadState) {
		return data_link->get_status(myriadState);
	};
	inline int set_data(const char *name,
		const void *data, const uint32_t &length,
		const bool &host_ready) {
		
		return data_link->set_data(name,
			data, length, host_ready);
	};
	inline int get_data(const char *name,
		void *data, const uint32_t &length, const uint32_t &offset,
		const bool &host_ready) {
		
		return data_link->get_data(name,
			data, length, offset, host_ready);
	};
	int soft_reset();
};

class Graph {
	friend MvNcApi;

	Device *dev;
	int started;
	int have_data;
	int dont_block;
	int input_idx;
	int output_idx;
	int failed;

	mvnc_int32_t config_iterations;
	mvnc_int32_t config_network_throttle;
	uint32_t noutputs;
	uint32_t nstages;
	size_t aux_buffer_len;
	char *aux_buffer;
	char *debug_buffer;
	mvnc_float_t *time_taken;
	mvnc_fp16_t *output_data;
	void *user_param[2];

public:
	Graph(Device *device)
	:	dev(device),
		started(0),
		have_data(0),
		dont_block(0),
		input_idx(0),
		output_idx(0),
		failed(0),
		config_iterations(0),
		config_network_throttle(0),
		noutputs(0),
		nstages(0),
		aux_buffer_len(0),
		aux_buffer(NULL),
		debug_buffer(NULL),
		time_taken(NULL),
		output_data(NULL)
	{
		ENTER();

		user_param[0] = user_param[1] = NULL;
		
		EXIT();
	}

	~Graph() {
		ENTER();

		aux_buffer_len = 0;
		SAFE_DELETE(aux_buffer);
		debug_buffer = NULL;
		time_taken = NULL;
		SAFE_DELETE(output_data);
		user_param[0] = user_param[1] = NULL;

		EXIT();
	}

	int soft_reset() {
		ENTER();

		int rc = 0;
		failed = 0;
		config_iterations = config_network_throttle = 1;
		memset(aux_buffer, 0, aux_buffer_len);
	
		// FIXME エンディアンの変換が必要な気がする
		rc = dev->set_data("auxBuffer", aux_buffer, aux_buffer_len, 0);
		if (rc) {
			LOGE("set_data failed,err=%d", rc);
			rc = MVNC_ERROR;
		}
		memset(output_data, 0, noutputs + sizeof(mvnc_fp16_t));

		RETURN(rc, int);
	}
};

Device::Device(UsbDataLink *_data_link)
:	data_link(_data_link),
	thermal_stats(NULL), optimisation_list(NULL),
	config_backoff_time_normal(0), config_backoff_time_high(100), config_backoff_time_critical(10000),
	config_temperature_debug(0), throttle_happened(0),
	config_temp_lim_upper(95), config_temp_lim_lower(85)
{
	ENTER();
	EXIT();
}

Device::~Device() {
	ENTER();

	Mutex::Autolock auto_lock(lock);

	for (auto itr: graphs) {
		SAFE_DELETE(itr);
	}
	graphs.clear();
	thermal_stats = NULL;
	SAFE_DELETE(optimisation_list);

	EXIT();
}

int Device::soft_reset() {
	ENTER();

	for (auto itr: graphs) {
		itr->soft_reset();
	}
	
	if (optimisation_list) {
		memset(optimisation_list, 0, OPTIMISATION_LIST_BUFFER_SIZE);
	}
	config_backoff_time_normal = 0;
	config_backoff_time_high = 100;
	config_backoff_time_critical = 10000;
	config_temperature_debug = throttle_happened = 0;
	config_temp_lim_upper = 95;
	config_temp_lim_lower = 85;

	RETURN(0, int);
}

/**
 * 指定したgraphを削除する
 * @param graph
 * @return 0:削除できた, -1:削除できなかった
 */
int Device::remove(Graph *graph) {
	ENTER();
	
	Graph *found = NULL;
	for (auto itr: graphs) {
		if (itr == graph) {
			found = itr;
			break;
		}
	}
	if (found) {
		thermal_stats = NULL;
		graphs.remove(found);
		RETURN(0, int);
	}

	RETURN(-1, int);
}

static double time_in_seconds() {

	static double s;
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	if (!s) {
		s = ts.tv_sec + ts.tv_nsec * 1e-9;
	}

	return ts.tv_sec + ts.tv_nsec * 1e-9 - s;
}

static uint32_t read_32bits(const uint8_t *ptr) {
	// リトルエンディアン
	return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

//********************************************************************************
//********************************************************************************
MvNcApi::MvNcApi()
: log_level(0)
{
	ENTER();
	EXIT();
}

MvNcApi::~MvNcApi() {
	ENTER();

	Mutex::Autolock auto_lock(lock);
	
	for (auto itr: devices) {
		SAFE_DELETE(itr);
	}

	EXIT();
}

/*public*/
int MvNcApi::add(UsbDataLink *data_link) {
	ENTER();

	int result = -1;
	lock.lock();
	{
		Device *device = find(data_link);
		if (!device) {
			device = new Device(data_link);
			devices.push_back(device);
			result = 0;
		} else {
			LOGW("already added");
		}
	}
	lock.unlock();
	
	RETURN(result, int);
}

/*public*/
int MvNcApi::remove(UsbDataLink *data_link) {
	ENTER();

	Device *device = NULL;

	lock.lock();
	{
		for (auto itr = devices.begin(); itr != devices.end(); itr++) {
			if ((*itr)->data_link == data_link) {
				device = *itr;
				devices.erase(itr);
				break;
			}
		}
	}
	lock.unlock();

	SAFE_DELETE(device);
	
	RETURN(0, int);
}

/*public*/
const size_t MvNcApi::get_device_nums() {
	ENTER();

	Mutex::Autolock autolock(lock);

	RETURN(devices.size(), size_t);
};

/*public*/
void *MvNcApi::get_device(const size_t &ix) {
	ENTER();

	Mutex::Autolock autolock(lock);
	if ((ix >= 0) && (ix < devices.size())) {
		RET(devices.at(ix));
	}

	RET(NULL);
}

/*public*/
int MvNcApi::run(const char *data_path) {
	ENTER();

	int result = MVNC_DEVICE_NOT_FOUND;
	size_t device_num;
	lock.lock();
	{
		device_num = devices.size();
	}
	lock.unlock();
	if (device_num) {
		const std::string path(data_path);
		soft_reset();
		result = run_test(this, path);
		soft_reset();
	}

	RETURN(result, int);
}

/*public*/
int MvNcApi::soft_reset() {
	ENTER();

	Mutex::Autolock auto_lock(lock);

	for (auto itr: devices) {
		itr->soft_reset();
	}
	RETURN(0, int);
}

//======================================================================
mvncStatus MvNcApi::allocate_graph(
	const void *device_handle, void **graph_handle,
	const void *graph_file, const size_t &graph_file_length) {

	ENTER();

	if (!device_handle || !graph_handle || !graph_file) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	if ((graph_file_length < HEADER_LENGTH + STAGE_LENGTH) ||
		(graph_file_length > 512 * 1024 * 1024)) {

		RETURN(MVNC_UNSUPPORTED_GRAPH_FILE, mvncStatus);
	}

	uint8_t *graph = (uint8_t *) graph_file;
	if (graph[VERSION_OFFSET] != GRAPH_VERSION) {
		RETURN(MVNC_UNSUPPORTED_GRAPH_FILE, mvncStatus);
	}

	uint32_t nstages = graph[N_STAGES_OFFSET] + (graph[N_STAGES_OFFSET + 1] << 8);
	uint32_t noutputs
		= read_32bits(graph + N_OUTPUTS_OFFSET + (nstages - 1) * STAGE_LENGTH)
		* read_32bits(graph + N_OUTPUTS_OFFSET + (nstages - 1) * STAGE_LENGTH + 4)
        * read_32bits(graph + X_OUT_STRIDE_OFFSET + (nstages - 1) * STAGE_LENGTH) / 2;

	// A reasonable check on graph correctness
	if (noutputs > 64 * 1024 * 1024) {
		RETURN(MVNC_UNSUPPORTED_GRAPH_FILE, mvncStatus);
	}

	lock.lock();
	Device *d = NULL;
	for (auto dev: devices) {
		if (dev == device_handle) {
			d = dev;
			break;
		}
	}
	if (!d) {
		lock.unlock();
		LOGE("could not find specific device %p", device_handle);
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	if (!d->graphs.empty()) {
		lock.unlock();
		LOGE("graph is already allocated");
		RETURN(MVNC_BUSY, mvncStatus);
	}

	myriadStatus_t status;
	double timeout = time_in_seconds() + 10;
	do {
		int rc = d->get_status(status);
		if (rc) {
			lock.unlock();
			LOGE("get status failed,err=%d", rc);
			RETURN(MVNC_ERROR, mvncStatus);
		}
		usleep(10000);
	} while (status != MYRIAD_WAITING && time_in_seconds() < timeout);

	if (status != MYRIAD_WAITING) {
		lock.unlock();
		RETURN(MVNC_ERROR, mvncStatus);
	}

	// FIXME エンディアンの変換が必要な気がする
	if (d->set_data("blobFile", graph_file, graph_file_length, 0)) {
		lock.unlock();
		RETURN(MVNC_ERROR, mvncStatus);
	}

	struct Graph *g = new Graph(d);
	g->nstages = nstages;
	g->noutputs = noutputs;

	// aux_buffer
	g->aux_buffer_len = 224 + nstages * sizeof(*g->time_taken);
	g->aux_buffer = new char[g->aux_buffer_len];
	if (!g->aux_buffer) {
		SAFE_DELETE(g);
		lock.unlock();
		RETURN(MVNC_OUT_OF_MEMORY, mvncStatus);
	}
	memset(g->aux_buffer, 0, g->aux_buffer_len);

	// FIXME エンディアンの変換が必要な気がする
	if (d->set_data("auxBuffer", g->aux_buffer, g->aux_buffer_len, 0)) {
		SAFE_DELETE(g);
		lock.unlock();
		RETURN(MVNC_ERROR, mvncStatus);
	}

	g->debug_buffer = g->aux_buffer;
	// FIXME エンディアンの変換が必要な気がする, 32ビットのfloatだからfp32_tにしてから変換？
	g->time_taken = (mvnc_float_t *) (g->aux_buffer + 224);

	// output_data
	g->output_data = new mvnc_fp16_t[noutputs];
	if (!g->output_data) {
		SAFE_DELETE(g);
		lock.unlock();
		RETURN(MVNC_OUT_OF_MEMORY, mvncStatus);
	}
	memset(g->output_data, 0, noutputs + sizeof(mvnc_fp16_t));

	// FIXME エンディアンの変換が必要な気がする, 32ビットのfloatだからfp32_tにしてから変換？
	g->dev->thermal_stats = (mvnc_float_t *) (g->aux_buffer + DEBUG_BUFFER_SIZE);

	g->config_iterations = 1;
	g->config_network_throttle = 1;
	d->graphs.push_back(g);
	*graph_handle = g;
	lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::deallocate_graph(void *graph_handle) {
	ENTER();

	if (!graph_handle) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Graph *g = reinterpret_cast<Graph *>(graph_handle);
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	struct Device *d = g->dev;

	d->lock.lock();
	if (internal_deallocate_graph(g)) {
		d->lock.unlock();
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	d->lock.unlock();
	lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::set_graph_option(
	const void *graph_handle, const mvncGraphOptions &option,
	const void *data, const size_t &data_length) {

	ENTER();

	if (!graph_handle || !data || data_length != 4) {	// FIXME data_lenが4バイト固定なのはいかん気がする
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.lock();
	lock.unlock();
	switch (option) {
	case MVNC_ITERATIONS:
		// FIXME エンディアンの変換が必要な気がする
		g->config_iterations = *(int32_t *) data;
		break;
	case MVNC_NETWORK_THROTTLE:
		// FIXME エンディアンの変換が必要な気がする
		g->config_network_throttle = *(int32_t *) data;
		break;
	case MVNC_DONT_BLOCK:
		g->dont_block = *(int *) data;
		break;
	default:
		g->dev->lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::get_graph_option(
	const void *graph_handle, const mvncGraphOptions &option,
	void *data, size_t &data_length) {

	ENTER();

	if (!graph_handle || !data) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.lock();
	lock.unlock();
	switch (option) {
	case MVNC_ITERATIONS:
		*(int *) data = g->config_iterations;
		data_length = sizeof(mvnc_int32_t);
		break;
	case MVNC_NETWORK_THROTTLE:
		*(int *) data = g->config_network_throttle;
		data_length = sizeof(mvnc_int32_t);
		break;
	case MVNC_DONT_BLOCK:
		*(int *) data = g->dont_block;

		data_length = sizeof(int);
		break;
	case MVNC_TIME_TAKEN:
		*(mvnc_float_t **) data = g->time_taken;
		data_length = sizeof(mvnc_float_t) * g->nstages;
		break;
	case MVNC_DEBUG_INFO:
		*(char **) data = g->debug_buffer;
		data_length = DEBUG_BUFFER_SIZE;
		// 一応nullターミネートしておく
		g->debug_buffer[DEBUG_BUFFER_SIZE-1] = '\0';
		break;
	default:
		g->dev->lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::set_global_option(
	const mvncGlobalOptions &option,
	const void *data, const size_t &data_length) {

	ENTER();

	if (!data || data_length != sizeof(int)) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	switch (option) {
	case MVNC_LOG_LEVEL:
		log_level = *(int *) data;
		break;
	default:
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::get_global_option(
	const mvncGlobalOptions &option,
	void *data, size_t &data_length) {

	ENTER();

	if (!data) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	switch (option) {
	case MVNC_LOG_LEVEL:
		*(int *) data = log_level;
		data_length = sizeof(log_level);
		break;
	default:
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::set_device_option(
	const void *device_handle, const mvncDeviceOptions &option,
	const void *data, const size_t &data_length) {

	ENTER();

	if (UNLIKELY(!device_handle && !option)) {
		LOGW("Warning: MVNC_LOG_LEVEL is not a Device Option,"
                "please use mvncSetGlobalOption()!");
		RETURN(set_global_option(MVNC_LOG_LEVEL, data, data_length), mvncStatus);
	}

	if (!device_handle || !data || data_length != 4) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Device *d = (Device *)device_handle;
	lock.lock();
	if (is_device_exist(d)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	d->lock.lock();
	lock.unlock();
	switch (option) {
	case MVNC_TEMP_LIM_LOWER:
		d->config_temp_lim_lower = *(mvnc_float_t *) data;
		break;
	case MVNC_TEMP_LIM_HIGHER:
		d->config_temp_lim_upper = *(mvnc_float_t *) data;
		break;
	case MVNC_BACKOFF_TIME_NORMAL:
		d->config_backoff_time_normal = *(mvnc_int32_t *) data;
		break;
	case MVNC_BACKOFF_TIME_HIGH:
		d->config_backoff_time_high = *(mvnc_int32_t *) data;
		break;
	case MVNC_BACKOFF_TIME_CRITICAL:
		d->config_backoff_time_critical = *(mvnc_int32_t *) data;
		break;
	case MVNC_TEMPERATURE_DEBUG:
		d->config_temperature_debug = *(mvnc_int32_t *) data;
		break;
	default:
		d->lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}
	d->lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::get_device_option(
	const void *device_handle, const mvncDeviceOptions &option,
	void *data, size_t &data_length)
{
	ENTER();

	mvncStatus rc;

	if (UNLIKELY(!device_handle && !option)) {
		LOGW("Warning: MVNC_LOG_LEVEL is not a Device Option,"
                 "please use get_global_option()!");
		RETURN(get_global_option(MVNC_LOG_LEVEL, data, data_length), mvncStatus);
	}

	if (!device_handle || !data || !data_length)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	Device *d = (Device *)device_handle;
	lock.lock();
	if (!is_device_exist(d)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	d->lock.lock();
	lock.unlock();

	switch (option) {
	case MVNC_TEMP_LIM_LOWER:
		*(float *) data = d->config_temp_lim_lower;
		data_length = sizeof(mvnc_float_t);
		break;
	case MVNC_TEMP_LIM_HIGHER:
		*(float *) data = d->config_temp_lim_upper;
		data_length = sizeof(mvnc_float_t);
		break;
	case MVNC_BACKOFF_TIME_NORMAL:
		*(int *) data = d->config_backoff_time_normal;
		data_length = sizeof(mvnc_int32_t);
		break;
	case MVNC_BACKOFF_TIME_HIGH:
		*(int *) data = d->config_backoff_time_high;
		data_length = sizeof(mvnc_int32_t);
		break;
	case MVNC_BACKOFF_TIME_CRITICAL:
		*(int *) data = d->config_backoff_time_critical;
		data_length = sizeof(mvnc_int32_t);
		break;
	case MVNC_TEMPERATURE_DEBUG:
		*(int *) data = d->config_temperature_debug;
		data_length = sizeof(mvnc_int32_t);
		break;
	case MVNC_THERMAL_STATS:
		if (!d->thermal_stats) {
			d->lock.unlock();
			RETURN(MVNC_NO_DATA, mvncStatus);
		}
		*(mvnc_float_t **) data = d->thermal_stats;
		data_length = THERMAL_BUFFER_SIZE;
		break;
	case MVNC_OPTIMISATION_LIST:
		rc = get_optimisation_list(d);
		if (rc) {
			d->lock.unlock();
			RETURN(rc, mvncStatus);
		}
		*(char **) data = d->optimisation_list;
		data_length = OPTIMISATION_LIST_BUFFER_SIZE;
		break;
	case MVNC_THERMAL_THROTTLING_LEVEL:
		*(mvnc_int32_t *) data = d->throttle_happened;
		data_length = sizeof(mvnc_int32_t);
		break;
	default:
		d->lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}
	d->lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::load_tensor(
	const void *graph_handle,
	const mvnc_fp16_t *input_tensor, const size_t &input_tensor_length,
	void *userParam) {

	if (!graph_handle || !input_tensor || input_tensor_length < 2) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	int rc;
	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	if (!g->started) {
		rc = send_opt_data(g);
		if (rc) {
			lock.unlock();
			LOGE("send_opt_data failed,err %d", rc);
			RETURN(MVNC_ERROR, mvncStatus);
		}
		g->started = 1;
	}

	while (g->have_data == 2) {
		if (g->dont_block) {
			lock.unlock();
			RETURN(MVNC_BUSY, mvncStatus);
		}
		if (g->failed) {
			lock.unlock();
			LOGE("unexpectedly failed flag triggered %d", g->failed);
			RETURN(MVNC_ERROR, mvncStatus);
		}
		lock.unlock();
		usleep(1000);
		lock.lock();
		if (!is_graph_exist(g)) {
			lock.unlock();
			LOGE("Graph is gone");
			RETURN(MVNC_GONE, mvncStatus);
		}
	}
	g->dev->lock.lock();
	lock.unlock();

	// FIXME エンディアンの変換が必要な気がする
	rc = g->dev->set_data(g->input_idx ? "input2" : "input1",
		input_tensor, input_tensor_length, g->have_data == 0);
	
	if (rc) {
		lock.unlock();
		LOGE("set_data failed,err=%d", rc);
		RETURN(MVNC_ERROR, mvncStatus);
	}

	g->user_param[g->input_idx] = userParam;
	g->input_idx = !g->input_idx;
	g->have_data++;
	g->dev->lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::get_result(
	const void *graph_handle,
	mvnc_fp16_t **output_data, size_t &output_data_length,
	void **user_param) {

	ENTER();
	
	mvncStatus rc;
	int unlock_own = 0;

	if (!graph_handle || !output_data) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
		LOGE("specific graph not found");
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}
	while (!g->have_data) {
		if (g->dont_block) {
			lock.unlock();
			RETURN(MVNC_NO_DATA, mvncStatus);
		}
		lock.unlock();
		usleep(1000);
		lock.lock();
		if (!is_graph_exist(g)) {
			lock.unlock();
			RETURN(MVNC_GONE, mvncStatus);
		}
	}

	double timeout = time_in_seconds() + STATUS_WAIT_TIMEOUT;
	do {
		g->dev->lock.lock();
		lock.unlock();
		int err = g->dev->get_data("output", g->output_data,
			sizeof(mvnc_fp16_t) * g->noutputs, 0, 0);
		if (!err) {
			unsigned int length = DEBUG_BUFFER_SIZE + THERMAL_BUFFER_SIZE +
				 sizeof(mvnc_int32_t) + sizeof(*g->time_taken) * g->nstages;

			err = g->dev->get_data("auxBuffer", g->aux_buffer,
				 length, 0, g->have_data == 2);

			if (err) {
				g->failed = 1;
				g->dev->lock.unlock();
				LOGE("get_data(auxBuffer) failed,err=%d", err);
				RETURN(MVNC_ERROR, mvncStatus);
			}
			unlock_own = 1;
			break;
		} else if (err && (err != USB_ERROR_BUSY)) {
			g->failed = 1;
			g->dev->lock.unlock();
			LOGE("get_data(output) failed,err=%d", err);
			RETURN(MVNC_ERROR, mvncStatus);
		}
		g->dev->lock.unlock();
		usleep(1000);
		lock.lock();
		if (!is_graph_exist(g)) {
			lock.unlock();
			LOGE("unexpectedly graph is gone");
			RETURN(MVNC_GONE, mvncStatus);
		}
	} while (time_in_seconds() < timeout);
	// FIXME エンディアンの変換が必要な気がする
	g->dev->throttle_happened = *(mvnc_int32_t *) (
		g->aux_buffer + DEBUG_BUFFER_SIZE + THERMAL_BUFFER_SIZE);
	*output_data = g->output_data;
	output_data_length = g->noutputs * sizeof(mvnc_fp16_t);
	*user_param = g->user_param[g->output_idx];
	g->output_idx = !g->output_idx;
	g->have_data--;

	if (unlock_own) {
		rc = *g->debug_buffer ? MVNC_MYRIAD_ERROR : MVNC_OK;
		if (rc) {
			g->failed = 1;
		}
		g->dev->lock.unlock();
	} else {
		rc = MVNC_TIMEOUT;
		g->failed = 1;
		lock.unlock();
		LOGE("time out");
	}

	if (rc == MVNC_MYRIAD_ERROR) {
		// get_graph_option内でも排他制御しているのでここでないとだめ
		char *message = NULL;
		size_t len;
		get_graph_option(graph_handle, MVNC_DEBUG_INFO, &message, len);
		LOGE("myriad err:%s", message);
	}

	RETURN(rc, mvncStatus);
}

/*private*/
Device *MvNcApi::find(const UsbDataLink *data_link) {
	ENTER();

	Device *device = NULL;

	for (auto itr: devices) {
		if (itr->data_link == data_link) {
			device = itr;
		}
	}

	RET(device);
}

bool MvNcApi::is_device_exist(const Device *device) {
	ENTER();

	for (auto dev: devices) {
		if (dev == device) {
			RETURN(true, bool);
		}
	}

	RETURN(false, bool);
}

/**
 * 指定したgraphが存在しているかどうかをチェック
 * @param graph
 * @return 存在していればtrue, していなければfalse(これはncsdkのfind_graphと論理が逆なので注意)
 */
/*private*/
bool MvNcApi::is_graph_exist(const Graph *graph) {
//	ENTER();

	for (auto dev: devices) {
		for (auto g: dev->graphs) {
			if (g == graph) {
				return true;
			}
		}
	}

	return false; // RETURN(false, bool);
}

/**
 * 指定したgraphを削除
 */
/*private*/
// Defined here as it will be used twice
int MvNcApi::internal_deallocate_graph(Graph *graph) {
	ENTER();

	if (UNLIKELY(!graph)) {
		RETURN(MVNC_INVALID_PARAMETERS, int);
	}
	int err = graph->dev->remove(graph);
	if (LIKELY(!err)) {
		SAFE_DELETE(graph);
	}

	RETURN(err, int);
}

/*private*/
mvncStatus MvNcApi::get_optimisation_list(Device *d) {
	ENTER();

	int i, config[10];
	double timeout;
	myriadStatus_t status;
	char *p;

	if (d->optimisation_list) {
		RETURN(MVNC_OK, mvncStatus);
	}

	d->optimisation_list = new char[OPTIMISATION_LIST_BUFFER_SIZE];
	if (UNLIKELY(!d->optimisation_list)) {
		RETURN(MVNC_OUT_OF_MEMORY, mvncStatus);
	}
	memset(d->optimisation_list, 0, OPTIMISATION_LIST_BUFFER_SIZE);
	memset(config, 0, sizeof(config));
	config[0] = 1;
	config[1] = 1;

	// FIXME エンディアンの変換が必要な気がする
	if (d->set_data("config", config, sizeof(config), 1)) {
		RETURN(MVNC_ERROR, mvncStatus);
	}

	timeout = time_in_seconds() + STATUS_WAIT_TIMEOUT;
	do {
		if (d->get_status(status)) {
			RETURN(MVNC_ERROR, mvncStatus);
		}
		usleep(10000);
	} while (status != MYRIAD_WAITING &&
		 status != MYRIAD_FINISHED && time_in_seconds() < timeout);

	if (status != MYRIAD_WAITING && status != MYRIAD_FINISHED) {
		RETURN(MVNC_TIMEOUT, mvncStatus);
	}

	if (d->get_data("optimizationList",
		d->optimisation_list, OPTIMISATION_LIST_BUFFER_SIZE, 0, 0)) {

		RETURN(MVNC_ERROR, mvncStatus);
	}

	for (i = 0; i < MAX_OPTIMISATIONS; i++) {
		p = strchr(d->optimisation_list + i * OPTIMISATION_NAME_LEN, '~');
		if (p) {
			*p = 0;
		}
	}

	config[1] = 0;
	// FIXME エンディアンの変換が必要な気がする
	if (d->set_data("config", config, sizeof(config), 0)) {
		RETURN(MVNC_ERROR, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

/*private*/
mvncStatus MvNcApi::send_opt_data(const Graph *g) {
	ENTER();

	mvnc_int32_t config[10];

	// FIXME エンディアンの変換が必要な気がする
	config[0] = 1;		// Version
	config[1] = 0;		// Query disable
	config[2] = g->config_iterations;
	config[3] = (mvnc_int32_t)g->dev->config_temp_lim_upper;
	config[4] = (mvnc_int32_t)g->dev->config_temp_lim_lower;
	config[5] = g->dev->config_backoff_time_normal;
	config[6] = g->dev->config_backoff_time_high;
	config[7] = g->dev->config_backoff_time_critical;
	config[8] = g->dev->config_temperature_debug;
	config[9] = g->config_network_throttle;

	if (g->dev->set_data("config", config, sizeof(config), 0)) {
		RETURN(MVNC_ERROR, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant

