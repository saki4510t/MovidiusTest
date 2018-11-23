// This file came from ncsdk repository. saki
// Copied from Numpy

#include <stdlib.h>
#include "mvnc.h"

mvnc_fp32_t half2float(const mvnc_fp16_t &h);
mvnc_fp16_t float2half(const mvnc_fp32_t &f);
void floattofp16(mvnc_fp16_t *dst, const float *src, const size_t &nelem);
void fp16tofloat(float *dst, const mvnc_fp16_t *src, const size_t &nelem);


