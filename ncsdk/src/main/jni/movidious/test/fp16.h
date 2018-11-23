// This file came from ncsdk repository. saki
// Copied from Numpy

#include <stdlib.h>
#include "mvnc.h"

static unsigned int half2float(unsigned short h);
unsigned short float2half(unsigned f);
void floattofp16(mvnc_fp16_t *dst, const float *src, const size_t &nelem);
void fp16tofloat(float *dst, const mvnc_fp16_t *src, const size_t &nelem);


