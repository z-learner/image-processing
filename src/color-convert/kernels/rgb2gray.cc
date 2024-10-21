#include "image-processing/color-convert/kernels/rgb2gray.hpp"
#include "cpu/rgb2gray.hpp"
#include <assert.h>
#include <iostream>
namespace image_processing {

namespace color_convert {

namespace kernels {
bool rgb_2_gray(const unsigned char *input, unsigned char *output, int width,
                int height, AlgoType algo_type) {
  if (input == nullptr || output == nullptr || width <= 0 || height <= 0) {
    return false;
  }
  bool ret = false;
  switch (algo_type) {
  case AlgoType::kNativeCpu:
    ret = rgb_2_gray_native(input, output, width, height);
    break;
  case AlgoType::kParallelCpu:
    ret = rgb_2_gray_parallel(input, output, width, height);
    break;
  case AlgoType::kSimdCpu:
    ret = rgb_2_gray_simd(input, output, width, height);
    break;

  default:
    assert(false);
    break;
  }

  return ret;
}
} // namespace kernels
} // namespace color_convert
} // namespace image_processing