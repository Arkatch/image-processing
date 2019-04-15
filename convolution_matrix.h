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

enum TYPE{
  SHARPEN = 1,
  BOXBLUR,
  GAUSBLUR
};
uint8_t sharpen(int *values, int oryginal_value){
  int sharpen_mask[] = { 0, -1, 0, -1, 5, -1, 0, -1, 0 };
  int bit_value = 0, i;
  for(i = 0; i < 9; ++i)
    bit_value += values[i]*sharpen_mask[i];
  if( bit_value < 0 || bit_value > 255 )
    return oryginal_value;
  return bit_value;
}
uint8_t boxblur(int *values, int oryginal_value){
  int boxblur_mask[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
  int bit_value = 0, i;
  for(i = 0; i < 9; ++i)
    bit_value += values[i]*boxblur_mask[i];
  if( bit_value == 0 )
    return oryginal_value;
  return bit_value/9;
}
uint8_t gaussianblur_mask(int *values, int oryginal_value){
  int gaussianblur_mask[] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
  int bit_value = 0, i;
  for(i = 0; i < 9; ++i)
    bit_value += values[i]*gaussianblur_mask[i];
  if( bit_value == 0 )
    return oryginal_value;
  return bit_value/16;
}
void convolution_matrix(uint8_t * pixels, uint32_t width, uint32_t height, enum TYPE type){
  uint32_t filter_size = 3;

  int values[filter_size*filter_size];
  uint32_t x, y, v, z, i;
  height = height - filter_size + 1;
  width = width - filter_size + 1;
  int bit_value = 0;
  uint8_t new_pixels[width * height];
  uint32_t size_mask = 0;

  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      i = 0;
      for (v = 0; v < filter_size; ++v)
        for (z = 0; z < filter_size; ++z)
          values[i++] = (int)pixels[(y + v) * width + x + z];

      if( type == SHARPEN ) 
        { bit_value = sharpen(values, pixels[(y + 1) * width + x + 1]); }
      else if( type == BOXBLUR ) 
        { bit_value = boxblur(values, pixels[(y + 1) * width + x + 1]); }
      else if( type == GAUSBLUR ) 
        { bit_value = gaussianblur_mask(values, pixels[(y + 1) * width + x + 1]); }
      else 
        { return; }

      new_pixels[size_mask++] = bit_value;
    }
  }
  merge_pixels(pixels, width, height, 1, new_pixels, size_mask);
}

#endif