#include "image-processing/color-convert/kernels/rgb2gray.hpp"
#include "gtest/gtest.h"
#include <fstream>
#include <png.h>
#include <stdexcept>
#include <vector>

std::vector<unsigned char> read_raw_image(const std::string &filename,
                                          int width, int height) {

  size_t size = width * height * 3;

  std::vector<unsigned char> data(size);

  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  file.read(reinterpret_cast<char *>(data.data()), size);
  if (!file) {
    throw std::runtime_error("Failed to read data from file: " + filename);
  }

  return data;
}

void save_png(const std::string &filename,
              const std::vector<unsigned char> &gray_data, int width,
              int height) {
  FILE *fp = fopen(filename.c_str(), "wb");
  if (!fp)
    return;

  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info = png_create_info_struct(png);

  if (setjmp(png_jmpbuf(png))) {
    png_destroy_write_struct(&png, &info);
    fclose(fp);
    return;
  }

  png_init_io(png, fp);
  png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, info);

  std::vector<png_bytep> rows(height);
  for (int y = 0; y < height; ++y) {
    rows[y] = const_cast<png_bytep>(&gray_data[y * width]);
  }

  png_write_image(png, rows.data());
  png_write_end(png, nullptr);

  png_destroy_write_struct(&png, &info);
  fclose(fp);
}

// Test the rgb_2_gray_native function
TEST(RGB2GrayTest, NativeConversion) {
  int width = 1920;
  int height = 1080;
  auto input_image = read_raw_image("/tmp/geometric_image.rgb", width, height);
  std::vector<unsigned char> output_image(width * height);

  ASSERT_TRUE(image_processing::color_convert::kernels::rgb_2_gray(
      input_image.data(), output_image.data(), width, height,
      image_processing::color_convert::AlgoType::kNativeCpu))
      << "Expected rgb_2_gray to return true but it returned false.";

  EXPECT_EQ(output_image[100 * width + 100], 76);
  EXPECT_EQ(output_image[50 * width + 400], 29);

  // save_png("/tmp/geometric_image.gray.png", output_image, width, height);
}

// Test the rgb_2_gray_parallel function
TEST(RGB2GrayTest, ParallelConversion) {
  int width = 1920;
  int height = 1080;
  auto input_image = read_raw_image("/tmp/geometric_image.rgb", width, height);
  std::vector<unsigned char> output_image(width * height);

  ASSERT_TRUE(image_processing::color_convert::kernels::rgb_2_gray(
      input_image.data(), output_image.data(), width, height,
      image_processing::color_convert::AlgoType::kParallelCpu))
      << "Expected rgb_2_gray to return true but it returned false.";

  EXPECT_EQ(output_image[100 * width + 100], 76);
  EXPECT_EQ(output_image[50 * width + 400], 29);

  // save_png("/tmp/geometric_image.gray.png", output_image, width, height);
}

// Test the rgb_2_gray_simd function
TEST(RGB2GrayTest, SIMDConversion) {
  int width = 1920;
  int height = 1080;
  auto input_image = read_raw_image("/tmp/geometric_image.rgb", width, height);
  std::vector<unsigned char> output_image(width * height);

  ASSERT_TRUE(image_processing::color_convert::kernels::rgb_2_gray(
      input_image.data(), output_image.data(), width, height,
      image_processing::color_convert::AlgoType::kSimdCpu))
      << "Expected rgb_2_gray to return true but it returned false.";

  EXPECT_TRUE(std::abs(output_image[100 * width + 100] - 76) <= 2);
  EXPECT_TRUE(std::abs(output_image[50 * width + 400] - 29) <= 2);

  // save_png("/tmp/geometric_image.gray.png", output_image, width, height);
}