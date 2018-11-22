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

	int backoff_time_normal, backoff_time_high, backoff_time_critical;
	int temperature_debug, throttle_happened;
	float temp_lim_upper, temp_lim_lower;
	float *thermal_stats;
	char *optimisation_list;

	Device(UsbDataLink *_data_link);
	~Device();
	
	bool remove(Graph *g);
	inline int get_status(myriadStatus_t &myriadState) {
		return data_link->get_status(myriadState);
	};
	inline int set_data(const char *name,
		const void *data, const unsigned int &length,
		const uint8_t &host_ready) {
		
		return data_link->set_data(name,
			data, length, host_ready);
	};
	inline int get_data(const char *name,
		void *data, const unsigned int &length, const unsigned int &offset,
		const uint8_t &host_ready) {
		
		return data_link->get_data(name,
			data, length, offset, host_ready);
	};
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
	int iterations;
	int network_throttle;
	unsigned noutputs;
	unsigned nstages;
	char *aux_buffer;
	char *debug_buffer;
	float *time_taken;
	void *user_param[2];
	void *output_data;

public:
	Graph(Device *device)
	:	dev(device),
		started(0),
		have_data(0),
		dont_block(0),
		input_idx(0),
		output_idx(0),
		failed(0),
		iterations(0),
		network_throttle(0),
		noutputs(0),
		nstages(0),
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

		SAFE_DELETE(aux_buffer);
		SAFE_DELETE(debug_buffer);
		SAFE_DELETE(time_taken);
		user_param[0] = user_param[1] = NULL;
		SAFE_FREE(output_data);

		EXIT();
	}

};

Device::Device(UsbDataLink *_data_link)
:	data_link(_data_link),
	thermal_stats(NULL), optimisation_list(NULL),
	backoff_time_normal(0), backoff_time_high(100), backoff_time_critical(10000),
	temperature_debug(0), throttle_happened(0),
	temp_lim_upper(95), temp_lim_lower(85)
{
	ENTER();
	EXIT();
}

Device::~Device() {
	ENTER();
	Mutex:AutoMutex auto_lock(lock);

	for (auto itr: graphs) {
		SAFE_DELETE(itr);
	}
	graphs.clear();
	thermal_stats = NULL;
	SAFE_DELETE(optimisation_list);

	EXIT();
}

bool Device::remove(Graph *g) {
	ENTER();
	
	Graph *found = NULL;
	for (auto itr: graphs) {
		if (itr == g) {
			found = itr;
			break;
		}
	}
	if (found) {
		thermal_stats = NULL;
		graphs.remove(found);
		RETURN(true, bool);
	}
	RETURN(false, bool);
}

static double time_in_seconds()
{
	static double s;
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	if (!s)
		s = ts.tv_sec + ts.tv_nsec * 1e-9;
	return ts.tv_sec + ts.tv_nsec * 1e-9 - s;
}

static unsigned read_32bits(const unsigned char *ptr)
{
	return ptr[0] | (ptr[1] << 8) | (ptr[2] << 16) | (ptr[3] << 24);
}

#if 0
static void allocate_device(const char* name, void **device_handle, void* f)
{
	struct Device *d = (struct Device *)calloc(1, sizeof(*d));
	d->data_link = f;
	d->temp_lim_upper = 95;
	d->temp_lim_lower = 85;
	d->backoff_time_normal = 0;
	d->backoff_time_high = 100;
	d->backoff_time_critical = 10000;
	d->temperature_debug = 0;
	pthread_mutex_init(&d->mm, 0);
	*device_handle = d;

	LOGD("done");
}

mvncStatus mvncOpenDevice(const char *name, void **device_handle)
{
	int rc;
	char name2[MVNC_MAX_NAME_SIZE] = "";
	char* device_name;
	char* saved_name = NULL;
	char* temp = NULL; //save to be able to free memory
	int second_name_available = 0;

	if (!name || !device_handle)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	temp = saved_name = strdup(name);

	device_name = strtok_r(saved_name, ":", &saved_name);
	if (device_name == NULL) {
		free(temp);
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	lock.lock();
	if (!initialized)
		initialize();


	rc = load_fw_file(device_name);
	if (rc != MVNC_OK) {
		free(temp);
		return rc;
	}
	if (saved_name && strlen(saved_name) > 0) {
		device_name = strtok_r(NULL, ":", &saved_name);
		second_name_available = 1;
	}

	// Now we should have a new /dev/ttyACM, try to open it
	double waittm = time_in_seconds() + STATUS_WAIT_TIMEOUT;
	while (time_in_seconds() < waittm) {
		void *f = usblink_open(device_name);

		//we might fail in case name changed after boot and we don't have it
		if (f == NULL && !second_name_available) {
			int count = 0;
			while (1) {
				name2[0] = '\0';
				rc = usb_find_device(count, name2,
						     sizeof(name2), NULL,
						     DEFAULT_OPEN_VID,
						     DEFAULT_OPEN_PID);
				if (rc < 0)	//Error or no more devices found
					break;

				//check if we already have name2 open
				// if not, check if it's not already busy
				if (is_device_opened(name2) < 0 &&
				    (f = usblink_open(name2)))
					break;
				count++;
			}
		}

		if (f) {
			myriadStatus_t status;

			if (!usblink_getmyriadstatus(f, &status) && status == MYRIAD_WAITING) {
				allocate_device(strlen(name2) > 0 ? name2 : device_name, device_handle, f);
				free(temp);
				lock.unlock();
				RETURN(MVNC_OK;
			} else {
				LOGD("found, but cannot get status");
				usblink_close(f);
			}
		}
		// Error opening it, continue searching
		usleep(10000);
	}
	free(temp);
	lock.unlock();
	RETURN(MVNC_ERROR, mvncStatus);
}

//static int find_device(void *device_handle)
//{
//	struct Device *d = devices;
//
//	while (d) {
//		if (d == device_handle)
//			return 0;
//		d = d->next;
//	}
//
//	return -1;
//}

//static int find_graph(void *graph_handle)
//{
//	struct Device *d = devices;
//
//	while (d) {
//		struct Graph *g = d->graphs;
//		while (g) {
//			if (g == graph_handle)
//				return 0;
//			g = g->next;
//		}
//		d = d->next;
//	}
//
//	return -1;
//}

#endif
//********************************************************************************
//********************************************************************************
MvNcApi::MvNcApi()
: log_level(0)
{
}

MvNcApi::~MvNcApi() {
	Mutex::Autolock auto_lock(lock);
	
	for (auto itr: devices) {
		SAFE_DELETE(itr);
	}
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
	Mutex::Autolock autolock(lock);
	return devices.size();
};

/*public*/
void *MvNcApi::get_device(const size_t &ix) {
	Mutex::Autolock autolock(lock);
	if ((ix >= 0) && (ix < devices.size())) {
		return devices.at(ix);
	}
	return NULL;
}

/*public*/
int MvNcApi::run(const char *data_path) {
	ENTER();

	const std::string path(data_path);
	int result = run_test(this, path);

	RETURN(result, int);
}

//======================================================================
mvncStatus MvNcApi::allocate_graph(
	const void *device_handle, void **graph_handle,
	const void *graph_file, unsigned int graph_file_length) {

	if (!device_handle || !graph_handle || !graph_file)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	if (graph_file_length < HEADER_LENGTH + STAGE_LENGTH ||
	  graph_file_length > 512 * 1024 * 1024)
		RETURN(MVNC_UNSUPPORTED_GRAPH_FILE, mvncStatus);

	unsigned char *graph = (unsigned char *) graph_file;
	if (graph[VERSION_OFFSET] != GRAPH_VERSION) {
		RETURN(MVNC_UNSUPPORTED_GRAPH_FILE, mvncStatus);
	}

	unsigned nstages = graph[N_STAGES_OFFSET] + (graph[N_STAGES_OFFSET + 1] << 8);
	unsigned noutputs
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
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	if (!d->graphs.empty()) {
		lock.unlock();
		RETURN(MVNC_BUSY, mvncStatus);
	}

	myriadStatus_t status;
	double timeout = time_in_seconds() + 10;
	do {
		if (d->get_status(status)) {
			lock.unlock();
			RETURN(MVNC_ERROR, mvncStatus);
		}
		usleep(10000);
	} while (status != MYRIAD_WAITING && time_in_seconds() < timeout);

	if (status != MYRIAD_WAITING) {
		lock.unlock();
		RETURN(MVNC_ERROR, mvncStatus);
	}

	if (d->set_data("blobFile", graph_file, graph_file_length, 0)) {
		lock.unlock();
		RETURN(MVNC_ERROR, mvncStatus);
	}

	struct Graph *g = new Graph(d);
	g->nstages = nstages;
	g->noutputs = noutputs;

	// aux_buffer
	g->aux_buffer = new char[224 + nstages * sizeof(*g->time_taken)];
	if (!g->aux_buffer) {
		free(g);
		lock.unlock();
		RETURN(MVNC_OUT_OF_MEMORY, mvncStatus);
	}

	if (d->set_data("auxBuffer", g->aux_buffer,
			    224 + nstages * sizeof(*g->time_taken), 0)) {
		free(g->aux_buffer);
		free(g);
		lock.unlock();
		RETURN(MVNC_ERROR, mvncStatus);
	}

	g->debug_buffer = g->aux_buffer;
	g->time_taken = (float *) (g->aux_buffer + 224);

	// output_data
	g->output_data = calloc(noutputs, 2);
	if (!g->output_data) {
		free(g->aux_buffer);
		free(g);
		lock.unlock();
		RETURN(MVNC_OUT_OF_MEMORY, mvncStatus);
	}

	g->dev->thermal_stats = (float *) (g->aux_buffer + DEBUG_BUFFER_SIZE);

	g->iterations = 1;
	g->network_throttle = 1;
	d->graphs.push_back(g);
	*graph_handle = g;
	lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::deallocate_graph(void *graph_handle) {
	if (!graph_handle) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Graph *g = reinterpret_cast<Graph *>(graph_handle);
	lock.lock();
	if (is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	struct Device *d = g->dev;

	d->lock.lock();
	if (deallocate_graph(g)) {
		d->lock.unlock();
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	d->lock.unlock();
	lock.unlock();
	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::set_graph_option(
	const void *graph_handle, const int option,
	const void *data, const unsigned int data_length) {

	if (!graph_handle || !data || data_length != 4)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.lock();
	lock.unlock();
	switch (option) {
	case MVNC_ITERATIONS:
		g->iterations = *(int *) data;
		break;
	case MVNC_NETWORK_THROTTLE:
		g->network_throttle = *(int *) data;
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
	const void *graph_handle, const int option,
	void *data, unsigned int *data_length) {

	if (!graph_handle || !data || !data_length)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.lock();
	lock.unlock();
	switch (option) {
	case MVNC_ITERATIONS:
		*(int *) data = g->iterations;
		*data_length = sizeof(int);
		break;
	case MVNC_NETWORK_THROTTLE:
		*(int *) data = g->network_throttle;
		*data_length = sizeof(int);
		break;
	case MVNC_DONT_BLOCK:
		*(int *) data = g->dont_block;

		*data_length = sizeof(int);
		break;
	case MVNC_TIME_TAKEN:
		*(float **) data = g->time_taken;
		*data_length = sizeof(*g->time_taken) * g->nstages;
		break;
	case MVNC_DEBUG_INFO:
		*(char **) data = g->debug_buffer;
		*data_length = DEBUG_BUFFER_SIZE;
		break;
	default:
		g->dev->lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	g->dev->lock.unlock();
	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::set_global_option(
	const int option, const void *data,
	const unsigned int data_length)
{
	if (!data || data_length != 4)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	switch (option) {
	case MVNC_LOG_LEVEL:
		log_level = *(int *) data;
		break;
	default:
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::get_global_option(const int option,
	void *data, unsigned int *data_length)
{
	if (!data || !data_length)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	switch (option) {
	case MVNC_LOG_LEVEL:
		*(int *) data = log_level;
		*data_length = sizeof(log_level);
		break;
	default:
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}
	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::set_device_option(
	const void *device_handle, const int &option,
	const void *data, const unsigned int data_length) {

	ENTER();

	if (device_handle == 0 && option == MVNC_LOG_LEVEL) {
		LOGW("Warning: MVNC_LOG_LEVEL is not a Device Option,"
                "please use mvncSetGlobalOption()!");
		RETURN(set_global_option(option, data, data_length), mvncStatus);
	}

	if (!device_handle || !data || data_length != 4)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

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
		d->temp_lim_lower = *(float *) data;
		break;
	case MVNC_TEMP_LIM_HIGHER:
		d->temp_lim_upper = *(float *) data;
		break;
	case MVNC_BACKOFF_TIME_NORMAL:
		d->backoff_time_normal = *(int *) data;
		break;
	case MVNC_BACKOFF_TIME_HIGH:
		d->backoff_time_high = *(int *) data;
		break;
	case MVNC_BACKOFF_TIME_CRITICAL:
		d->backoff_time_critical = *(int *) data;
		break;
	case MVNC_TEMPERATURE_DEBUG:
		d->temperature_debug = *(int *) data;
		break;
	default:
		d->lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}
	d->lock.unlock();

	RETURN(MVNC_OK, mvncStatus);
}

mvncStatus MvNcApi::get_device_option(
	const void *device_handle, const int option,
	void *data, unsigned int *data_length)
{
	ENTER();

	mvncStatus rc;

	if (device_handle == 0 && option == MVNC_LOG_LEVEL) {
		LOGW("Warning: MVNC_LOG_LEVEL is not a Device Option,"
                 "please use get_global_option()!");
		RETURN(get_global_option(option, data, data_length), mvncStatus);
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
		*(float *) data = d->temp_lim_lower;
		*data_length = sizeof(int);
		break;
	case MVNC_TEMP_LIM_HIGHER:
		*(float *) data = d->temp_lim_upper;
		*data_length = sizeof(int);
		break;
	case MVNC_BACKOFF_TIME_NORMAL:
		*(int *) data = d->backoff_time_normal;
		*data_length = sizeof(int);
		break;
	case MVNC_BACKOFF_TIME_HIGH:
		*(int *) data = d->backoff_time_high;
		*data_length = sizeof(int);
		break;
	case MVNC_BACKOFF_TIME_CRITICAL:
		*(int *) data = d->backoff_time_critical;
		*data_length = sizeof(int);
		break;
	case MVNC_TEMPERATURE_DEBUG:
		*(int *) data = d->temperature_debug;
		*data_length = sizeof(int);
		break;
	case MVNC_THERMAL_STATS:
		if (!d->thermal_stats) {
			d->lock.unlock();
			RETURN(MVNC_NO_DATA, mvncStatus);
		}
		*(float **) data = d->thermal_stats;
		*data_length = THERMAL_BUFFER_SIZE;
		break;
	case MVNC_OPTIMISATION_LIST:
		rc = get_optimisation_list(d);
		if (rc) {
			d->lock.unlock();
			RETURN(rc, mvncStatus);
		}
		*(char **) data = d->optimisation_list;
		*data_length = OPTIMISATION_LIST_BUFFER_SIZE;
		break;
	case MVNC_THERMAL_THROTTLING_LEVEL:
		*(int *) data = d->throttle_happened;
		*data_length = sizeof(int);
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
	const void *input_tensor, unsigned int input_tensor_length,
	void *userParam) {

	if (!graph_handle || !input_tensor || input_tensor_length < 2) {
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);
	}

	if (!g->started) {
		if (send_opt_data(g)) {
			lock.unlock();
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
			RETURN(MVNC_ERROR, mvncStatus);
		}
		lock.unlock();
		usleep(1000);
		lock.lock();
		if (is_graph_exist(g)) {
			lock.unlock();
			RETURN(MVNC_GONE, mvncStatus);
		}
	}
	g->dev->lock.lock();
	lock.unlock();

	if (g->dev->set_data(g->input_idx ? "input2" : "input1",
	     input_tensor, input_tensor_length, g->have_data == 0)) {
		lock.unlock();
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
	void **output_data, unsigned int *output_data_length,
	void **user_param) {

	ENTER();
	
	mvncStatus rc;
	int unlock_own = 0;

	if (!graph_handle || !output_data || !output_data_length)
		RETURN(MVNC_INVALID_PARAMETERS, mvncStatus);

	Graph *g = (Graph *)graph_handle;
	lock.lock();
	if (!is_graph_exist(g)) {
		lock.unlock();
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
		lock.unlock();
		if (!g->dev->get_data("output", g->output_data,
			2 * g->noutputs, 0, 0)) {

			unsigned int length = DEBUG_BUFFER_SIZE + THERMAL_BUFFER_SIZE +
				 sizeof(int) + sizeof(*g->time_taken) * g->nstages;

			if (g->dev->get_data("auxBuffer", g->aux_buffer,
				 length, 0, g->have_data == 2)) {
				g->failed = 1;
				g->dev->lock.unlock();
				RETURN(MVNC_ERROR, mvncStatus);
			}
			unlock_own = 1;
			break;
		}
		g->dev->lock.unlock();
		usleep(1000);
		lock.lock();
		if (!is_graph_exist(g)) {
			lock.unlock();
			RETURN(MVNC_GONE, mvncStatus);
		}
	} while (time_in_seconds() < timeout);

	g->dev->throttle_happened = *(int *) (g->aux_buffer + DEBUG_BUFFER_SIZE
						+ THERMAL_BUFFER_SIZE);
	*output_data = g->output_data;
	*output_data_length = 2 * g->noutputs;
	*user_param = g->user_param[g->output_idx];
	g->output_idx = !g->output_idx;
	g->have_data--;

	if (unlock_own) {
		rc = *g->debug_buffer ? MVNC_MYRIAD_ERROR : MVNC_OK;
		if (rc)
			g->failed = 1;
		g->dev->lock.unlock();
	} else {
		rc = MVNC_TIMEOUT;
		g->failed = 1;
		lock.unlock();
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
			return true;
		}
	}

	RETURN(false, bool);
}

/*private*/
bool MvNcApi::is_graph_exist(const Graph *graph) {
	ENTER();

	for (auto dev: devices) {
		for (auto g: dev->graphs) {
			if (g == graph) {
				return true;
			}
		}
	}

	RETURN(false, bool);
}

/*private*/
// Defined here as it will be used twice
int MvNcApi::deallocate_graph(Graph *g)
{
	bool removed = g->dev->remove(g);
	if (removed) {
		SAFE_DELETE(g);
	}

	return -!removed;
}

/*private*/
mvncStatus MvNcApi::get_optimisation_list(Device *d) {
	int i, config[10];
	double timeout;
	myriadStatus_t status;
	char *p;

	if (d->optimisation_list)
		RETURN(MVNC_OK, mvncStatus);

	d->optimisation_list = new char[OPTIMISATION_LIST_BUFFER_SIZE];
	if (!d->optimisation_list) {
		RETURN(MVNC_OUT_OF_MEMORY, mvncStatus);
	}

	memset(config, 0, sizeof(config));
	config[0] = 1;
	config[1] = 1;
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
	if (d->set_data("config", config, sizeof(config), 0)) {
		RETURN(MVNC_ERROR, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

/*private*/
mvncStatus MvNcApi::send_opt_data(const Graph *g) {
	ENTER();

	int config[10];	// FIXME int32_t ?

	config[0] = 1;		// Version
	config[1] = 0;		// Query disable
	config[2] = g->iterations;
	config[3] = (int)g->dev->temp_lim_upper;
	config[4] = (int)g->dev->temp_lim_lower;
	config[5] = g->dev->backoff_time_normal;
	config[6] = g->dev->backoff_time_high;
	config[7] = g->dev->backoff_time_critical;
	config[8] = g->dev->temperature_debug;
	config[9] = g->network_throttle;

	if (g->dev->set_data("config", config, sizeof(config), 0)) {
		RETURN(MVNC_ERROR, mvncStatus);
	}

	RETURN(MVNC_OK, mvncStatus);
}

}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant

