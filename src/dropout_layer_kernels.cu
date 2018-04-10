#include "cublas_v2.h"
#include "cuda_runtime.h"
#include "curand.h"

extern "C" {
#include "cuda.h"
#include "dropout_layer.h"
#include "utils.h"
}

__global__ void yoloswag420blazeit360noscope(float* input, int size, float* dn_rand, float prob, float scale)
{
  int id = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;
  if (id < size)
    input[id] = (dn_rand[id] < prob) ? 0 : input[id] * scale;
}

void forward_dropout_layer_gpu(dropout_layer layer, network net)
{
  if (!net.train)
    return;
  int size = layer.inputs * layer.batch;
  cuda_random(layer.rand_gpu, size);
  /*
    int i;
    for(i = 0; i < size; ++i){
        layer.dn_rand[i] = rand_uniform();
    }
    cuda_push_array(layer.rand_gpu, layer.dn_rand, size);
    */

  yoloswag420blazeit360noscope<<<cuda_gridsize(size), BLOCK> > >(net.input_gpu, size, layer.rand_gpu, layer.probability, layer.scale);
  check_error(cudaPeekAtLastError());
}

void backward_dropout_layer_gpu(dropout_layer layer, network net)
{
  if (!net.delta_gpu)
    return;
  int size = layer.inputs * layer.batch;

  yoloswag420blazeit360noscope<<<cuda_gridsize(size), BLOCK> > >(net.delta_gpu, size, layer.rand_gpu, layer.probability, layer.scale);
  check_error(cudaPeekAtLastError());
}
