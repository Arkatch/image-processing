#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"

#ifndef __CONVOL__
#define __CONVOL__

uint8_t sharpen_template(int *values, int oryginal_value){
  int sharpen_mask[] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
  int bit_value = 0, i;
  for(i = 0; i < 9; ++i)
    bit_value += values[i]*sharpen_mask[i];
  if( bit_value < 0 || bit_value > 255 )
    return oryginal_value;
  return bit_value;
}
uint8_t boxblur_template(int *values, int oryginal_value){
  int boxblur_mask[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  int bit_value = 0, i;
  for(i = 0; i < 9; ++i)
    bit_value += values[i]*boxblur_mask[i];
  if( bit_value == 0 )
    return oryginal_value;
  return bit_value/9;
}
uint8_t gaussianblur_template(int *values, int oryginal_value){
  int gaussianblur_mask[] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
  int bit_value = 0, i;
  for(i = 0; i < 9; ++i)
    bit_value += values[i]*gaussianblur_mask[i];
  if( bit_value == 0 )
    return oryginal_value;
  return bit_value/16;
}
void convolution_matrix(uint8_t * pixels, uint32_t width, uint32_t height, uint8_t (*mask_type)(int*, int)){
  uint8_t new_pixels[width * height];
  int32_t v, z, values[9];
  uint32_t size_mask = 0, x, y, i;

  uint32_t new_height = height - 1;
  uint32_t new_width = width - 1;
  for (y = 1; y < new_height; ++y) {
    for (x = 1; x < new_width; ++x) {
      i = 0;
      for (v = -1; v <= 1; ++v)
        for (z = -1; z <= 1; ++z)
          values[i++] = (int)pixels[(y + v) * width + x + z];
      new_pixels[size_mask++] = mask_type(values, pixels[y*width + x]);
    }
  }
  merge_pixels(pixels, width, height, 1, new_pixels, size_mask);
}

#endif