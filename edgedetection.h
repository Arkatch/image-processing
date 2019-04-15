#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"

#ifndef __EDGE__
#define __EDGE__
enum METHOD{
  PREWITT = 1,
  SOBEL,
  LAPLACIAN
};
uint8_t prewitt_template(int8_t *values){
  int8_t p1[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
  int8_t p2[] = { 0, 1, 1, -1, 0, 1, -1, -1, 0 };
  int8_t p3[] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
  int8_t p4[] = { 1, 1, 0, 1, 0, -1, 0, -1, -1 };
  int8_t p5[] = { 1, 0, -1, 1, 0, -1, 1, 0, -1 };
  int8_t p6[] = { 0, -1, -1, 1, 0, -1, 1, 1, 0 };
  int8_t p7[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
  int8_t p8[] = { -1, -1, 0, -1, 0, 1, 0, 1, 1 };
  int p1_v = 0, p2_v = 0, p3_v = 0, p4_v = 0, p5_v = 0,
    p6_v = 0, p7_v = 0, p8_v = 0;
  for(int i = 0; i < 9; ++i){
    p1_v += values[i] * p1[i];
    p2_v += values[i] * p2[i];
    p3_v += values[i] * p3[i];
    p4_v += values[i] * p4[i];
    p5_v += values[i] * p5[i];
    p6_v += values[i] * p6[i];
    p7_v += values[i] * p7[i];
    p8_v += values[i] * p8[i];
  }
  p1_v *= p1_v; p2_v *= p2_v;
  p3_v *= p3_v; p4_v *= p4_v;
  p5_v *= p5_v; p6_v *= p6_v;
  p7_v *= p7_v; p8_v *= p8_v;
  return sqrt(p1_v+p2_v+p3_v+p4_v+p5_v+p6_v+p7_v+p8_v);
}
uint8_t sobel_template(int8_t *values){
  int8_t gx[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
  int8_t gy[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
  int x_value = 0, y_value = 0, i;
  for( i = 0; i < 9; ++i){
    x_value += values[i] * gx[i];
    y_value += values[i] * gy[i];
  }
  return sqrt(x_value*x_value + y_value*y_value);
}
uint8_t laplacian_template(int8_t *values){
  int32_t v1[] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
  int32_t v2[] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
  int x_value = 0, y_value = 0, i;
  for( i = 0; i < 9; ++i){
    x_value += values[i] * v1[i];
    y_value += values[i] * v2[i];
  }
  return sqrt(x_value*x_value + y_value*y_value);
}
void edge_detection(uint8_t * pixels, uint32_t width, uint32_t height, enum METHOD type){
  uint32_t x, y, v, z;
  uint32_t filter_size = 3;

  int32_t i = 0, size_mask = 0;
  int8_t values[9];
  uint8_t new_pixels[width * height];

  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      i = 0;
      for (v = 0; v < filter_size; ++v)
        for (z = 0; z < filter_size; ++z)
          values[i++] = pixels[(y + v) * width + x + z];
      if( type == PREWITT )
        { new_pixels[size_mask++] = prewitt_template(values); }
      else if( type == SOBEL )
        { new_pixels[size_mask++] = sobel_template(values); }
      else if( type == LAPLACIAN )
        { new_pixels[size_mask++] = laplacian_template(values); }
      else 
        { return; } 
    }
  }
  merge_pixels(pixels, width, height, 1, new_pixels, size_mask);
}
#endif