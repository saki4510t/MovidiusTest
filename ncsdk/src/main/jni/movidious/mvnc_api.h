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

	int deallocate_graph(Graph *g);
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
	
	mvncStatus allocate_graph(
		const void *device_handle, const void **graph_handle,
		const void *graph_file, unsigned int graph_file_length);
	mvncStatus deallocate_graph(void *graph_handle);
	mvncStatus set_graph_option(
		const void *graph_handle, const int option,
		const void *data, const unsigned int data_length);
	mvncStatus get_graph_option(
		const void *graph_handle, const int option,
		void *data, unsigned int *data_length);
	mvncStatus set_global_option(
		const int option, const void *data,
		const unsigned int data_length);
	mvncStatus get_global_option(const int option,
		void *data, unsigned int *data_length);
	mvncStatus set_device_option(
		const void *device_handle, const int &option,
		const void *data, const unsigned int data_length);
	mvncStatus get_device_option(
		const void *device_handle, const int option,
		void *data, unsigned int *data_length);
	mvncStatus load_tensor(
		const void *graph_handle,
		const void *input_tensor, unsigned int input_tensor_length,
		void *userParam);
	mvncStatus get_result(
	  	const void *graph_hHandle,
	  	void **output_data, unsigned int *output_data_length,
	  	void **user_param);
};

}	// namespace ncs
}	// namespace usb
}	// namespace serenegiant

#endif //MOVIDIUSTEST_MVNC_API_H
