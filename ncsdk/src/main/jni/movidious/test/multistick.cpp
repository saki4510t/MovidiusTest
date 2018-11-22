// This file came from ncsdk repository. saki
// Copyright (c) 2017-2018 Intel Corporation. All Rights Reserved
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#if 0    // set 0 is you need debug log, otherwise set 1
#ifndef LOG_NDEBUG
#define	LOG_NDEBUG
#endif
#undef USE_LOGALL
#else
#define USE_LOGALL
#undef LOG_NDEBUG
#undef NDEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>

// common
#include "utilbase.h"

#include "mvnc_api.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image_resize.h"

#include "fp16.h"
#include <mvnc.h>


// somewhat arbitrary buffer size for the device name
#define NAME_SIZE 100

// from current director to examples base director
// #define APP_BASE_DIR "../"

#define EXAMPLES_BASE_DIR "data/"

// graph file names - assume the graph file is in the current directory.
#define GOOGLENET_GRAPH_FILE_NAME EXAMPLES_BASE_DIR "graphs/googlenet.graph"
#define SQUEEZENET_GRAPH_FILE_NAME EXAMPLES_BASE_DIR "graphs/squeezenet.graph"

// image file name - assume we are running in this directory: ncsdk/examples/caffe/GoogLeNet/cpp
#define GOOGLENET_IMAGE_FILE_NAME EXAMPLES_BASE_DIR "images/nps_electric_guitar.png"
#define SQUEEZENET_IMAGE_FILE_NAME EXAMPLES_BASE_DIR "images/nps_baseball.png"

using namespace serenegiant::usb::ncs;


// 16 bits.  will use this to store half precision floats since C++ has no 
// built in support for it.
typedef unsigned short half;

// GoogleNet image dimensions, network mean values for each channel in BGR order.
const unsigned int networkDimGoogleNet = 224;
const unsigned int networkDimSqueezeNet = 227;
float networkMeanGoogleNet[] = {0.40787054 * 255.0, 0.45752458 * 255.0, 0.48109378 * 255.0};
float networkMeanSqueezeNet[] = {0.40787054 * 255.0, 0.45752458 * 255.0, 0.48109378 * 255.0};

// Prototypes
void *LoadGraphFile(const char *path, unsigned int *length);

half *LoadImage(const char *path, unsigned int reqsize, float *mean);
// end prototypes

// Reads a graph file from the file system and copies it to a buffer
// that is allocated internally via malloc.
// Param path is a pointer to a null terminate string that must be set to the path to the 
//            graph file on disk before calling
// Param length must must point to an integer that will get set to the number of bytes 
//              allocated for the buffer
// Returns pointer to the buffer allcoated. 
// Note: The caller must free the buffer returned.
void *LoadGraphFile(const char *path, size_t &length) {
	ENTER();
	
	FILE *fp;
	char *buf;
	
	fp = fopen(path, "rb");
	if (fp == NULL) {
		RET(NULL);
	}
	fseek(fp, 0, SEEK_END);
	length = (size_t) ftell(fp);
	rewind(fp);
	if (!(buf = (char *) malloc(length))) {
		fclose(fp);
		RET(NULL);
	}
	if (fread(buf, 1, length, fp) != length) {
		fclose(fp);
		free(buf);
		RET(NULL);
	}
	fclose(fp);
	
	RET(buf);
}

// Reads an image file from disk (8 bit per channel RGB .jpg or .png or other formats 
// supported by stbi_load.)  Resizes it, subtracts the mean from each channel, and then 
// converts to an array of half precision floats that is suitable to pass to mvncLoadTensor.  
// The returned array will contain 3 floats for each pixel in the image the first float 
// for a pixel is it's the Blue channel value the next is Green and then Red.  The array 
// contains the pixel values in row major order.
// Param path is a pointer to a null terminated string that must be set to the path of the 
//            to read before calling.
// Param reqsize must be set to the width and height that the image will be resized to.  
//               Its assumed width and height are the same size.
// Param mean must be set to point to an array of 3 floating point numbers.  The three
//            numbers are the mean values for the blue, green, and red channels in that order.
//            each B, G, and R value from the image will have this value subtracted from it.
// Returns a pointer to a buffer that is allocated internally via malloc.  this buffer contains
//         the 16 bit float values that can be passed to mvncLoadTensor().  The returned buffer 
//         will contain reqSize*reqSize*3 half floats.
half *LoadImage(const char *path, unsigned int reqSize, float *mean) {
	ENTER();

	int width, height, cp, i;
	unsigned char *img, *imgresized;
	float *imgfp32;
	half *imgfp16;
	
	img = stbi_load(path, &width, &height, &cp, 3);
	if (!img) {
		LOGE("Error - the image file %s could not be loaded", path);
		RET(NULL);
	}
	imgresized = (unsigned char *) malloc(3 * reqSize * reqSize);
	if (!imgresized) {
		free(img);
		perror("malloc");
		RET(NULL);
	}
	stbir_resize_uint8(img, width, height, 0, imgresized, reqSize, reqSize, 0, 3);
	free(img);
	imgfp32 = (float *) malloc(sizeof(*imgfp32) * reqSize * reqSize * 3);
	if (!imgfp32) {
		free(imgresized);
		perror("malloc");
		RET(NULL);
	}
	for (i = 0; i < reqSize * reqSize * 3; i++) {
		imgfp32[i] = imgresized[i];
	}
	free(imgresized);
	imgfp16 = (half *) malloc(sizeof(*imgfp16) * reqSize * reqSize * 3);
	if (!imgfp16) {
		free(imgfp32);
		perror("malloc");
		RET(NULL);
	}
	for (i = 0; i < reqSize * reqSize; i++) {
		float blue, green, red;
		blue = imgfp32[3 * i + 2];
		green = imgfp32[3 * i + 1];
		red = imgfp32[3 * i + 0];
		
		imgfp32[3 * i + 0] = blue - mean[0];
		imgfp32[3 * i + 1] = green - mean[1];
		imgfp32[3 * i + 2] = red - mean[2];
		
		// uncomment to see what values are getting passed to load_tensor() before conversion to half float
		//LOGD("Blue: %f, Grean: %f,  Red: %f", imgfp32[3*i+0], imgfp32[3*i+1], imgfp32[3*i+2]);
	}
	floattofp16((unsigned char *) imgfp16, imgfp32, 3 * reqSize * reqSize);
	free(imgfp32);

	RET(imgfp16);
}


// Loads a compiled network graph onto the NCS device.
// Param deviceHandle is the open device handle for the device that will allocate the graph
// Param graphFilename is the name of the compiled network graph file to load on the NCS
// Param graphHandle is the address of the graph handle that will be created internally.
//                   the caller must call mvncDeallocateGraph when done with the handle.
// Returns true if works or false if doesn't.
bool LoadGraphToNCS(MvNcApi *api, void *deviceHandle, const char *graphFilename, void **graphHandle) {
	mvncStatus retCode;
	
	// Read in a graph file
	size_t graphFileLen;
	void *graphFileBuf = LoadGraphFile(graphFilename, graphFileLen);
	
	// allocate the graph
	retCode = api->allocate_graph(deviceHandle, graphHandle, graphFileBuf, graphFileLen);
	free(graphFileBuf);
	if (retCode != MVNC_OK) {   // error allocating graph
		LOGE("Could not allocate graph for file: %s", graphFilename);
		LOGE("Error from allocate_graph is: %d", retCode);
		RETURN(false, bool);
	}
	
	// successfully allocated graph.  Now graphHandle is ready to go.
	// use graphHandle for other API calls and call mvncDeallocateGraph
	// when done with it.
	LOGD("Successfully allocated graph for %s", graphFilename);
	
	RETURN(true, bool);
}


// Runs an inference and outputs result to console
// Param graphHandle is the graphHandle from mvncAllocateGraph for the network that 
//                   will be used for the inference
// Param imageFileName is the name of the image file that will be used as input for
//                     the neural network for the inference
// Param networkDim is the height and width (assumed to be the same) for images that the
//                     network expects. The image will be resized to this prior to inference.
// Param networkMean is pointer to array of 3 floats that are the mean values for the network
//                   for each color channel, blue, green, and red in that order.
// Returns tru if works or false if doesn't
bool DoInferenceOnImageFile(MvNcApi *api, void *graphHandle,
  const char *imageFileName, unsigned int networkDim, float *networkMean) {
	mvncStatus retCode;

	ENTER();

	// LoadImage will read image from disk, convert channels to floats
	// subtract network mean for each value in each channel.  Then, convert
	// floats to half precision floats and return pointer to the buffer
	// of half precision floats (Fp16s)
	half *imageBufFp16 = LoadImage(imageFileName, networkDim, networkMean);
	
	// calculate the length of the buffer that contains the half precision floats.
	// 3 channels * width * height * sizeof a 16bit float
	unsigned int lenBufFp16 = 3 * networkDim * networkDim * sizeof(*imageBufFp16);
	
	// start the inference with load_tensor()
	retCode = api->load_tensor(graphHandle, imageBufFp16, lenBufFp16, NULL);
	free(imageBufFp16);
	if (retCode != MVNC_OK) {   // error loading tensor
		LOGE("Error - Could not load tensor");
		LOGE("    mvncStatus from load_tensor is: %d", retCode);
		RETURN(false, bool);
	}
	
	// the inference has been started, now call get_result() for the
	// inference result
	LOGD("Successfully loaded the tensor for image %s", imageFileName);
	
	void *resultData16;
	void *userParam;
	size_t lenResultData;
	retCode = api->get_result(graphHandle, &resultData16, lenResultData, &userParam);
	if (retCode != MVNC_OK) {
		LOGE("Error - Could not get result for image %s", imageFileName);
		LOGE("    mvncStatus from get_result is: %d", retCode);
		RETURN(false, bool);
	}
	
	// Successfully got the result.  The inference result is in the buffer pointed to by resultData
	LOGD("Successfully got the inference result for image %s", imageFileName);
	//LOGD("resultData is %d bytes which is %d 16-bit floats.", lenResultData, lenResultData/(int)sizeof(half));
	
	// convert half precision floats to full floats
	size_t numResults = lenResultData / sizeof(half);
	float *resultData32;
	resultData32 = (float *) malloc(numResults * sizeof(*resultData32));
	fp16tofloat(resultData32, (unsigned char *) resultData16, numResults);
	
	float maxResult = 0.0;
	int maxIndex = -1;
	for (int index = 0; index < numResults; index++) {
		// LOGD("Category %d is: %f", index, resultData32[index]);
		if (resultData32[index] > maxResult) {
			maxResult = resultData32[index];
			maxIndex = index;
		}
	}
	LOGD("Index of top result is: %d", maxIndex);
	LOGD("Probability of top result is: %f", resultData32[maxIndex]);
	
	RETURN(true, bool);
}

// Main entry point for the program
int run_test(MvNcApi *api, const std::string &base_path) {
	ENTER();
	
	void *devHandle1;
	void *devHandle2;
	void *graphHandleGoogleNet = NULL;
	void *graphHandleSqueezeNet = NULL;
	
	devHandle1 = api->get_device(0);
	devHandle2 = api->get_device(1);
	if (!devHandle1 && !devHandle2) {
		RETURN(-1, int);
	}
	
	LOGD("number of connected device(s) %" FMT_SIZE_T, api->get_device_nums());

	if (devHandle1) {
		std::string path = base_path;
		if (!LoadGraphToNCS(api, devHandle1,
		  path.append(GOOGLENET_GRAPH_FILE_NAME).c_str(),
		  &graphHandleGoogleNet)) {

			RETURN(-2, int);
		}
	}
	if (devHandle2) {
		std::string path = base_path;
		if (!LoadGraphToNCS(api, devHandle2,
		  path.append(SQUEEZENET_GRAPH_FILE_NAME).c_str(),
		  &graphHandleSqueezeNet)) {
			
			if (graphHandleGoogleNet) {
				api->deallocate_graph(graphHandleGoogleNet);
			}
			graphHandleGoogleNet = NULL;

			RETURN(-2, int);
		}
	}
	
	if (devHandle1) {
		std::string path = base_path;
		LOGV("--- NCS 1 inference ---");
		DoInferenceOnImageFile(api, graphHandleGoogleNet,
		  path.append(GOOGLENET_IMAGE_FILE_NAME).c_str(),
		  networkDimGoogleNet, networkMeanGoogleNet);
		LOGV("-----------------------");
	}
	
	if (devHandle2) {
		std::string path = base_path;
		LOGV("--- NCS 2 inference ---");
		DoInferenceOnImageFile(api, graphHandleSqueezeNet,
		  path.append(SQUEEZENET_IMAGE_FILE_NAME).c_str(),
		  networkDimSqueezeNet, networkMeanSqueezeNet);
		LOGV("-----------------------");
	}
	
	if (graphHandleSqueezeNet) {
		api->deallocate_graph(graphHandleSqueezeNet);
		graphHandleSqueezeNet = NULL;
	}
	if (graphHandleGoogleNet) {
		api->deallocate_graph(graphHandleGoogleNet);
		graphHandleGoogleNet = NULL;
	}
	
	RETURN(0, int);
}
