//
// Created by saki on 2018/11/21.
//

#ifndef MOVIDIUSTEST_MVNC_API_H
#define MOVIDIUSTEST_MVNC_API_H

#include <vector>

// common
#include "Mutex.h"
// ncsdk
#include "mvnc.h"

namespace serenegiant {
namespace usb {
namespace ncs {

using namespace android;

class UsbDataLink;
class Device;
class Graph;

class MvNcApi {
private:
	mutable Mutex lock;
	std::vector<struct Device *> devices;
	int log_level;
	Device *find(const UsbDataLink *data_link);
	bool is_device_exist(const Device *device);
	bool is_graph_exist(const Graph *graph);

	int deallocate_graph(Graph *graph);
	mvncStatus get_optimisation_list(Device *d);
	mvncStatus send_opt_data(const Graph *g);
protected:
public:
	MvNcApi();
	virtual ~MvNcApi();
	
	int add(UsbDataLink *data_link);
	int remove(UsbDataLink *data_link);
	const size_t get_device_nums();
	void *get_device(const size_t &ix);
	int run(const char *data_path);
	
	mvncStatus allocate_graph(
		const void *device_handle, void **graph_handle,
		const void *graph_file, const size_t &graph_file_length);
	mvncStatus deallocate_graph(void *graph_handle);
	mvncStatus set_graph_option(
		const void *graph_handle, const mvncGraphOptions &option,
		const void *data, const size_t &data_length);
	mvncStatus get_graph_option(
		const void *graph_handle, const mvncGraphOptions &option,
		void *data, size_t &data_length);
	mvncStatus set_global_option(
		const mvncGlobalOptions &option,
		const void *data, const size_t &data_length);
	mvncStatus get_global_option(
	  	const mvncGlobalOptions &option,
		void *data, size_t &data_length);
	mvncStatus set_device_option(
		const void *device_handle, const mvncDeviceOptions &option,
		const void *data, const size_t &data_length);
	mvncStatus get_device_option(
		const void *device_handle, const mvncDeviceOptions &option,
		void *data, size_t &data_length);
	mvncStatus load_tensor(
		const void *graph_handle,
		const mvnc_fp16_t *input_tensor, const size_t &input_tensor_length,
		void *userParam);
	mvncStatus get_result(
	  	const void *graph_handle,
	  	mvnc_fp16_t **output_data, size_t &output_data_length,
	  	void **user_param);
};

}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant

#endif //MOVIDIUSTEST_MVNC_API_H
