/* Generated by Edge Impulse
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
// Generated on: 09.03.2021 23:19:23

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "edge-impulse-sdk/tensorflow/lite/c/builtin_op_data.h"
#include "edge-impulse-sdk/tensorflow/lite/c/common.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/kernels/micro_ops.h"

#if EI_CLASSIFIER_PRINT_STATE
#if defined(__cplusplus) && EI_C_LINKAGE == 1
extern "C" {
    extern void ei_printf(const char *format, ...);
}
#else
extern void ei_printf(const char *format, ...);
#endif
#endif

#if defined __GNUC__
#define ALIGN(X) __attribute__((aligned(X)))
#elif defined _MSC_VER
#define ALIGN(X) __declspec(align(X))
#elif defined __TASKING__
#define ALIGN(X) __align(X)
#endif

namespace {

constexpr int kTensorArenaSize = 144;

#if defined(EI_CLASSIFIER_ALLOCATION_STATIC)
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16);
#elif defined(EI_CLASSIFIER_ALLOCATION_STATIC_HIMAX)
#pragma Bss(".tensor_arena")
uint8_t tensor_arena[kTensorArenaSize] ALIGN(16);
#pragma Bss()
#else
#define EI_CLASSIFIER_ALLOCATION_HEAP 1
uint8_t* tensor_arena = NULL;
#endif

static uint8_t* tensor_boundary;
static uint8_t* current_location;

template <int SZ, class T> struct TfArray {
  int sz; T elem[SZ];
};
enum used_operators_e {
  OP_FULLY_CONNECTED, OP_SOFTMAX,  OP_LAST
};
struct TensorInfo_t { // subset of TfLiteTensor used for initialization from constant memory
  TfLiteAllocationType allocation_type;
  TfLiteType type;
  void* data;
  TfLiteIntArray* dims;
  size_t bytes;
  TfLiteQuantization quantization;
};
struct NodeInfo_t { // subset of TfLiteNode used for initialization from constant memory
  struct TfLiteIntArray* inputs;
  struct TfLiteIntArray* outputs;
  void* builtin_data;
  used_operators_e used_op_index;
};

TfLiteContext ctx{};
TfLiteTensor tflTensors[11];
TfLiteRegistration registrations[OP_LAST];
TfLiteNode tflNodes[4];

const TfArray<2, int> tensor_dimension0 = { 2, { 1,33 } };
const TfArray<1, float> quant0_scale = { 1, { 0.011671335436403751, } };
const TfArray<1, int> quant0_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant0 = { (TfLiteFloatArray*)&quant0_scale, (TfLiteIntArray*)&quant0_zero, 0 };
const ALIGN(8) int32_t tensor_data1[20] = { -216, 491, -590, 276, -407, 1992, -151, 1981, 1239, 12, 3951, 262, -454, 649, 3767, 1017, 6, 3951, 836, 4, };
const TfArray<1, int> tensor_dimension1 = { 1, { 20 } };
const TfArray<1, float> quant1_scale = { 1, { 0.00017400365322828293, } };
const TfArray<1, int> quant1_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant1 = { (TfLiteFloatArray*)&quant1_scale, (TfLiteIntArray*)&quant1_zero, 0 };
const ALIGN(8) int32_t tensor_data2[10] = { -84, -103, 825, 2104, 486, -54, 49, -333, 1400, 15, };
const TfArray<1, int> tensor_dimension2 = { 1, { 10 } };
const TfArray<1, float> quant2_scale = { 1, { 0.00028524620574899018, } };
const TfArray<1, int> quant2_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant2 = { (TfLiteFloatArray*)&quant2_scale, (TfLiteIntArray*)&quant2_zero, 0 };
const ALIGN(8) int32_t tensor_data3[4] = { 216, -148, 59, -124, };
const TfArray<1, int> tensor_dimension3 = { 1, { 4 } };
const TfArray<1, float> quant3_scale = { 1, { 0.0020321700721979141, } };
const TfArray<1, int> quant3_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant3 = { (TfLiteFloatArray*)&quant3_scale, (TfLiteIntArray*)&quant3_zero, 0 };
const ALIGN(8) int8_t tensor_data4[20*33] = { 
  -21, -15, -13, 3, -4, 2, -19, -18, -20, 2, 19, -15, -20, 15, 6, 18, 17, -11, 12, -17, 15, -23, 17, -16, -13, -13, -20, 9, -9, -9, -16, -23, 3, 
  69, 14, 34, 5, 17, 21, 14, 59, 33, 68, 78, 57, 14, 1, -7, -3, -10, -21, -47, 60, 110, 45, -19, 19, -23, -2, 17, 8, -14, -56, -40, 46, 4, 
  -3, -24, -18, 9, 0, 4, -22, -7, -13, 3, -9, -12, -20, -9, 12, -19, -15, -5, 13, -25, 13, -4, -18, -2, -17, 12, -4, -11, -4, -20, -21, 11, -24, 
  16, 13, -25, 12, 11, 5, -17, -11, -39, -22, -6, 14, 28, 8, 9, -4, -1, 0, 91, -33, -117, -1, 99, -6, 35, 14, -5, -4, 20, 71, 59, 39, 11, 
  -5, 8, -19, 6, 0, 19, -18, -15, -1, 3, 2, -4, -6, -10, -19, -1, -11, -10, 1, -1, -1, -13, -9, 4, 0, -11, 18, 9, 20, 0, 9, -13, -12, 
  59, 3, 22, 14, 17, 10, 7, 39, 35, 22, 36, 41, 44, 2, 28, 16, -12, 8, -49, 51, 82, 34, -19, 25, -53, 5, -1, -11, 10, -65, -55, 41, 8, 
  4, 27, 18, 10, 11, 6, 6, 41, 23, 48, 6, -18, -17, -21, 37, 23, -3, 12, -27, -13, 44, 14, 8, -1, 7, 1, -9, 3, 5, -20, -8, -11, -25, 
  -47, 42, -33, -1, -12, 21, 4, -52, -36, -72, -32, -32, 0, 2, -9, 1, 3, -14, 79, -28, -112, -40, 33, -7, 49, -8, 21, 10, 11, 91, 55, 30, -13, 
  65, 12, 38, -3, 31, 12, -6, 33, 53, 30, 36, 76, 21, -1, -11, -2, 3, -5, -52, 35, 101, 49, -29, 8, -52, 12, -27, 2, 15, -67, -71, 17, -8, 
  76, 16, 21, 5, -25, 0, -1, 17, 27, -9, 52, 80, -10, 43, -8, -19, 1, 15, 92, 46, 55, -8, 84, 32, 76, 4, 12, -12, 22, 96, 54, 42, 49, 
  -71, -4, -35, -6, 31, -8, 7, -23, -70, -32, -50, -40, 33, -21, 13, 0, -9, 19, -47, -70, -88, 9, -46, 0, -65, -13, -35, 14, 10, -107, -81, -2, -68, 
  94, 7, 5, 2, 8, 21, 5, 30, 36, 52, 60, 115, 9, -11, -7, 14, 4, 14, -34, 60, 91, 63, 69, 17, 5, -8, 4, 16, -16, -8, -34, 51, 21, 
  -5, 18, -11, -14, 16, -7, 6, -24, -18, 21, -3, -9, -13, 20, 23, -19, 15, 2, -45, 1, 14, 0, 9, -3, -22, -6, -20, 6, 17, -32, 2, -5, -9, 
  33, 10, -18, 9, -2, -7, 10, -24, -23, -17, 20, 61, 11, -8, -5, -1, -10, -9, 76, 0, -103, -13, 94, -18, 34, 26, 21, 7, 22, 77, 63, 41, 28, 
  -57, -3, -11, -6, 15, -8, 11, -34, -54, -60, -71, -25, 3, -12, 1, -5, -7, -6, 67, -59, -99, -13, 30, 20, 51, 13, -5, 0, -6, 56, 48, 17, 22, 
  85, 30, 22, 22, 10, 19, -14, 22, 29, 60, 66, 127, 7, 27, 23, -15, -4, 15, 84, 73, 45, 57, 120, -10, 41, 11, 32, 8, 2, 81, 53, 37, 41, 
  30, 26, 35, 14, 9, 1, -1, 18, 24, 32, 39, 48, -28, -25, -14, 11, 12, 7, -55, 25, 60, -10, 43, 25, 37, 9, -5, 1, 20, 48, 23, 17, 18, 
  -51, 3, -36, -19, 20, -9, 4, -16, -42, -10, -42, -41, 1, -57, 11, 9, 6, 2, -74, -43, -51, -24, -43, -57, -18, -6, -20, 19, 10, -35, -36, -24, -39, 
  -11, 10, -2, 9, 4, 7, -2, -45, -49, -48, -38, 19, 17, 12, -23, 3, -6, -1, 55, -9, -84, -30, 61, 30, 32, -42, -4, -6, 4, 63, 74, 32, 43, 
  59, 19, 46, 24, 18, 1, 2, 18, 59, 61, 58, 101, 3, 30, 20, 10, 1, -25, -33, 62, 84, 55, 54, -26, 3, 16, -4, 18, -23, -33, -32, -5, 43, 
};
const TfArray<2, int> tensor_dimension4 = { 2, { 20,33 } };
const TfArray<1, float> quant4_scale = { 1, { 0.014908632263541222, } };
const TfArray<1, int> quant4_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant4 = { (TfLiteFloatArray*)&quant4_scale, (TfLiteIntArray*)&quant4_zero, 0 };
const ALIGN(8) int8_t tensor_data5[10*20] = { 
  -17, -1, 7, 4, 22, -18, -8, 4, -24, -26, -9, -16, 23, 21, -8, -1, -25, 3, -9, 2, 
  -18, 4, -14, 9, 7, 0, 19, -12, -14, -19, 1, -22, -6, -10, -21, -24, -10, 16, 12, -27, 
  -20, 51, 5, -29, 14, 30, 57, -39, 47, 10, 21, 5, 40, -42, 5, -9, -2, 101, -27, -3, 
  -14, -21, -23, 10, -5, 30, 15, 74, 18, -45, 93, -12, -2, -15, 60, 10, -17, 85, 23, -51, 
  2, 39, -8, 12, -26, 35, -20, 16, 7, 75, -29, 21, -41, 58, 25, 57, 44, -76, 50, 52, 
  15, 100, -17, 45, -17, 33, 11, -50, 82, 58, -35, 127, -23, 87, -27, 108, 39, -21, -4, 100, 
  23, -6, 12, 69, 15, -5, -27, 20, 15, 73, -25, 56, -49, 50, 20, 31, 42, -37, 36, 16, 
  -15, 8, -16, -27, -7, -14, -11, -24, 15, 15, 14, 21, -8, -4, -5, -16, -22, -3, -14, 4, 
  -14, 8, 4, 34, 23, 14, 12, 22, -8, -34, 68, 9, 21, 28, 54, 0, -38, 83, 15, -20, 
  -8, 56, 1, 6, -15, 49, 54, -61, 56, 4, -7, 47, -3, -7, -51, 29, 45, 26, 2, 49, 
};
const TfArray<2, int> tensor_dimension5 = { 2, { 10,20 } };
const TfArray<1, float> quant5_scale = { 1, { 0.015761962160468102, } };
const TfArray<1, int> quant5_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant5 = { (TfLiteFloatArray*)&quant5_scale, (TfLiteIntArray*)&quant5_zero, 0 };
const ALIGN(8) int8_t tensor_data6[4*10] = { 
  -13, -6, 40, 68, -92, -127, -127, 0, 47, -69, 
  23, -1, 28, 26, -11, 11, 7, -24, 32, 38, 
  -42, -39, -86, -11, 50, 20, 47, -11, -12, -44, 
  -36, 31, 37, -69, 16, 29, -16, -32, -88, 67, 
};
const TfArray<2, int> tensor_dimension6 = { 2, { 4,10 } };
const TfArray<1, float> quant6_scale = { 1, { 0.015805801376700401, } };
const TfArray<1, int> quant6_zero = { 1, { 0 } };
const TfLiteAffineQuantization quant6 = { (TfLiteFloatArray*)&quant6_scale, (TfLiteIntArray*)&quant6_zero, 0 };
const TfArray<2, int> tensor_dimension7 = { 2, { 1,20 } };
const TfArray<1, float> quant7_scale = { 1, { 0.018097124993801117, } };
const TfArray<1, int> quant7_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant7 = { (TfLiteFloatArray*)&quant7_scale, (TfLiteIntArray*)&quant7_zero, 0 };
const TfArray<2, int> tensor_dimension8 = { 2, { 1,10 } };
const TfArray<1, float> quant8_scale = { 1, { 0.12857115268707275, } };
const TfArray<1, int> quant8_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant8 = { (TfLiteFloatArray*)&quant8_scale, (TfLiteIntArray*)&quant8_zero, 0 };
const TfArray<2, int> tensor_dimension9 = { 2, { 1,4 } };
const TfArray<1, float> quant9_scale = { 1, { 0.6409231424331665, } };
const TfArray<1, int> quant9_zero = { 1, { 73 } };
const TfLiteAffineQuantization quant9 = { (TfLiteFloatArray*)&quant9_scale, (TfLiteIntArray*)&quant9_zero, 0 };
const TfArray<2, int> tensor_dimension10 = { 2, { 1,4 } };
const TfArray<1, float> quant10_scale = { 1, { 0.00390625, } };
const TfArray<1, int> quant10_zero = { 1, { -128 } };
const TfLiteAffineQuantization quant10 = { (TfLiteFloatArray*)&quant10_scale, (TfLiteIntArray*)&quant10_zero, 0 };
const TfLiteFullyConnectedParams opdata0 = { kTfLiteActRelu, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs0 = { 3, { 0,4,1 } };
const TfArray<1, int> outputs0 = { 1, { 7 } };
const TfLiteFullyConnectedParams opdata1 = { kTfLiteActRelu, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs1 = { 3, { 7,5,2 } };
const TfArray<1, int> outputs1 = { 1, { 8 } };
const TfLiteFullyConnectedParams opdata2 = { kTfLiteActNone, kTfLiteFullyConnectedWeightsFormatDefault, false, false };
const TfArray<3, int> inputs2 = { 3, { 8,6,3 } };
const TfArray<1, int> outputs2 = { 1, { 9 } };
const TfLiteSoftmaxParams opdata3 = { 1 };
const TfArray<1, int> inputs3 = { 1, { 9 } };
const TfArray<1, int> outputs3 = { 1, { 10 } };
const TensorInfo_t tensorData[] = {
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension0, 33, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant0))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data1, (TfLiteIntArray*)&tensor_dimension1, 80, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant1))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data2, (TfLiteIntArray*)&tensor_dimension2, 40, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant2))}, },
  { kTfLiteMmapRo, kTfLiteInt32, (void*)tensor_data3, (TfLiteIntArray*)&tensor_dimension3, 16, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant3))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data4, (TfLiteIntArray*)&tensor_dimension4, 660, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant4))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data5, (TfLiteIntArray*)&tensor_dimension5, 200, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant5))}, },
  { kTfLiteMmapRo, kTfLiteInt8, (void*)tensor_data6, (TfLiteIntArray*)&tensor_dimension6, 40, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant6))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 48, (TfLiteIntArray*)&tensor_dimension7, 20, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant7))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension8, 10, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant8))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 16, (TfLiteIntArray*)&tensor_dimension9, 4, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant9))}, },
  { kTfLiteArenaRw, kTfLiteInt8, tensor_arena + 0, (TfLiteIntArray*)&tensor_dimension10, 4, {kTfLiteAffineQuantization, const_cast<void*>(static_cast<const void*>(&quant10))}, },
};const NodeInfo_t nodeData[] = {
  { (TfLiteIntArray*)&inputs0, (TfLiteIntArray*)&outputs0, const_cast<void*>(static_cast<const void*>(&opdata0)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs1, (TfLiteIntArray*)&outputs1, const_cast<void*>(static_cast<const void*>(&opdata1)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs2, (TfLiteIntArray*)&outputs2, const_cast<void*>(static_cast<const void*>(&opdata2)), OP_FULLY_CONNECTED, },
  { (TfLiteIntArray*)&inputs3, (TfLiteIntArray*)&outputs3, const_cast<void*>(static_cast<const void*>(&opdata3)), OP_SOFTMAX, },
};
static std::vector<void*> overflow_buffers;
static TfLiteStatus AllocatePersistentBuffer(struct TfLiteContext* ctx,
                                                 size_t bytes, void** ptr) {
  if (current_location - bytes < tensor_boundary) {
    // OK, this will look super weird, but.... we have CMSIS-NN buffers which
    // we cannot calculate beforehand easily.
    *ptr = malloc(bytes);
    if (*ptr == NULL) {
      printf("ERR: Failed to allocate persistent buffer of size %d\n", (int)bytes);
      return kTfLiteError;
    }
    overflow_buffers.push_back(*ptr);
    return kTfLiteOk;
  }

  current_location -= bytes;

  *ptr = current_location;
  return kTfLiteOk;
}
typedef struct {
  size_t bytes;
  void *ptr;
} scratch_buffer_t;
static std::vector<scratch_buffer_t> scratch_buffers;

static TfLiteStatus RequestScratchBufferInArena(struct TfLiteContext* ctx, size_t bytes,
                                                int* buffer_idx) {
  scratch_buffer_t b;
  b.bytes = bytes;

  TfLiteStatus s = AllocatePersistentBuffer(ctx, b.bytes, &b.ptr);
  if (s != kTfLiteOk) {
    return s;
  }

  scratch_buffers.push_back(b);

  *buffer_idx = scratch_buffers.size() - 1;

  return kTfLiteOk;
}

static void* GetScratchBuffer(struct TfLiteContext* ctx, int buffer_idx) {
  if (buffer_idx > static_cast<int>(scratch_buffers.size()) - 1) {
    return NULL;
  }
  return scratch_buffers[buffer_idx].ptr;
}
} // namespace

  TfLiteStatus trained_model_init( void*(*alloc_fnc)(size_t,size_t) ) {
#ifdef EI_CLASSIFIER_ALLOCATION_HEAP
  tensor_arena = (uint8_t*) alloc_fnc(16, kTensorArenaSize);
  if (!tensor_arena) {
    printf("ERR: failed to allocate tensor arena\n");
    return kTfLiteError;
  }
#endif
  tensor_boundary = tensor_arena;
  current_location = tensor_arena + kTensorArenaSize;
  ctx.AllocatePersistentBuffer = &AllocatePersistentBuffer;
  ctx.RequestScratchBufferInArena = &RequestScratchBufferInArena;
  ctx.GetScratchBuffer = &GetScratchBuffer;
  ctx.tensors = tflTensors;
  ctx.tensors_size = 11;
  for(size_t i = 0; i < 11; ++i) {
    tflTensors[i].type = tensorData[i].type;
    tflTensors[i].is_variable = 0;

#if defined(EI_CLASSIFIER_ALLOCATION_HEAP)
    tflTensors[i].allocation_type = tensorData[i].allocation_type;
#else
    tflTensors[i].allocation_type = (tensor_arena <= tensorData[i].data && tensorData[i].data < tensor_arena + kTensorArenaSize) ? kTfLiteArenaRw : kTfLiteMmapRo;
#endif
    tflTensors[i].bytes = tensorData[i].bytes;
    tflTensors[i].dims = tensorData[i].dims;

#if defined(EI_CLASSIFIER_ALLOCATION_HEAP)
    if(tflTensors[i].allocation_type == kTfLiteArenaRw){
      uint8_t* start = (uint8_t*) ((uintptr_t)tensorData[i].data + (uintptr_t) tensor_arena);

     tflTensors[i].data.data =  start;
    }
    else{
       tflTensors[i].data.data = tensorData[i].data;
    }
#else
    tflTensors[i].data.data = tensorData[i].data;
#endif // EI_CLASSIFIER_ALLOCATION_HEAP
    tflTensors[i].quantization = tensorData[i].quantization;
    if (tflTensors[i].quantization.type == kTfLiteAffineQuantization) {
      TfLiteAffineQuantization const* quant = ((TfLiteAffineQuantization const*)(tensorData[i].quantization.params));
      tflTensors[i].params.scale = quant->scale->data[0];
      tflTensors[i].params.zero_point = quant->zero_point->data[0];
    }
    if (tflTensors[i].allocation_type == kTfLiteArenaRw) {
      auto data_end_ptr = (uint8_t*)tflTensors[i].data.data + tensorData[i].bytes;
      if (data_end_ptr > tensor_boundary) {
        tensor_boundary = data_end_ptr;
      }
    }
  }
  if (tensor_boundary > current_location /* end of arena size */) {
    printf("ERR: tensor arena is too small, does not fit model - even without scratch buffers\n");
    return kTfLiteError;
  }
  registrations[OP_FULLY_CONNECTED] = *tflite::ops::micro::Register_FULLY_CONNECTED();
  registrations[OP_SOFTMAX] = *tflite::ops::micro::Register_SOFTMAX();

  for(size_t i = 0; i < 4; ++i) {
    tflNodes[i].inputs = nodeData[i].inputs;
    tflNodes[i].outputs = nodeData[i].outputs;
    tflNodes[i].builtin_data = nodeData[i].builtin_data;
    tflNodes[i].custom_initial_data = nullptr;
    tflNodes[i].custom_initial_data_size = 0;
    if (registrations[nodeData[i].used_op_index].init) {
      tflNodes[i].user_data = registrations[nodeData[i].used_op_index].init(&ctx, (const char*)tflNodes[i].builtin_data, 0);
    }
  }
  for(size_t i = 0; i < 4; ++i) {
    if (registrations[nodeData[i].used_op_index].prepare) {
      TfLiteStatus status = registrations[nodeData[i].used_op_index].prepare(&ctx, &tflNodes[i]);
      if (status != kTfLiteOk) {
        return status;
      }
    }
  }
  return kTfLiteOk;
}

static const int inTensorIndices[] = {
  0, 
};
TfLiteTensor* trained_model_input(int index) {
  return &ctx.tensors[inTensorIndices[index]];
}

static const int outTensorIndices[] = {
  10, 
};
TfLiteTensor* trained_model_output(int index) {
  return &ctx.tensors[outTensorIndices[index]];
}

TfLiteStatus trained_model_invoke() {
  for(size_t i = 0; i < 4; ++i) {
    TfLiteStatus status = registrations[nodeData[i].used_op_index].invoke(&ctx, &tflNodes[i]);

#if EI_CLASSIFIER_PRINT_STATE
    ei_printf("layer %lu\n", i);
    ei_printf("    inputs:\n");
    for (size_t ix = 0; ix < tflNodes[i].inputs->size; ix++) {
      auto d = tensorData[tflNodes[i].inputs->data[ix]];

      size_t data_ptr = (size_t)d.data;

      if (d.allocation_type == kTfLiteArenaRw) {
        data_ptr = (size_t)tensor_arena + data_ptr;
      }

      if (d.type == TfLiteType::kTfLiteInt8) {
        int8_t* data = (int8_t*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes; jx++) {
          ei_printf("%d ", data[jx]);
        }
      }
      else {
        float* data = (float*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes / 4; jx++) {
          ei_printf("%f ", data[jx]);
        }
      }
      ei_printf("\n");
    }
    ei_printf("\n");

    ei_printf("    outputs:\n");
    for (size_t ix = 0; ix < tflNodes[i].outputs->size; ix++) {
      auto d = tensorData[tflNodes[i].outputs->data[ix]];

      size_t data_ptr = (size_t)d.data;

      if (d.allocation_type == kTfLiteArenaRw) {
        data_ptr = (size_t)tensor_arena + data_ptr;
      }

      if (d.type == TfLiteType::kTfLiteInt8) {
        int8_t* data = (int8_t*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes; jx++) {
          ei_printf("%d ", data[jx]);
        }
      }
      else {
        float* data = (float*)data_ptr;
        ei_printf("        %lu (%zu bytes, ptr=%p, alloc_type=%d, type=%d): ", ix, d.bytes, data, (int)d.allocation_type, (int)d.type);
        for (size_t jx = 0; jx < d.bytes / 4; jx++) {
          ei_printf("%f ", data[jx]);
        }
      }
      ei_printf("\n");
    }
    ei_printf("\n");
#endif // EI_CLASSIFIER_PRINT_STATE

    if (status != kTfLiteOk) {
      return status;
    }
  }
  return kTfLiteOk;
}

TfLiteStatus trained_model_reset( void (*free_fnc)(void* ptr) ) {
#ifdef EI_CLASSIFIER_ALLOCATION_HEAP
  free_fnc(tensor_arena);
#endif
  scratch_buffers.clear();
  for (size_t ix = 0; ix < overflow_buffers.size(); ix++) {
    free(overflow_buffers[ix]);
  }
  overflow_buffers.clear();
  return kTfLiteOk;
}