#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"

#ifndef __FILTER__
#define __FILTER__

void add_salt(uint8_t * pixels, uint32_t width, uint32_t height, uint8_t proc) {
  uint32_t x, y;
  for (y = 0; y < height; ++y)
    for (x = 0; x < width; ++x)
      if ((uint8_t) rand() % 101 < proc)
        pixels[y * width + x] = (rand() % 2) ? 0 : 255;
}

uint8_t median_template(uint8_t *values, uint32_t size){
  qsort(values, size, sizeof(uint8_t), comp);
  return values[size/2];
}
uint8_t min_template(uint8_t *values, uint32_t size){
  uint8_t _min = 255;  
  uint32_t i = 0;
  while( i < size ) { _min = min(_min, values[i]); ++i; }
  return _min;
}
uint8_t max_template(uint8_t *values, uint32_t size){
  uint8_t _max = 0; 
  uint32_t i = 0;
  while( i < size ) { _max = max(_max, values[i]); ++i; }
  return _max;
}
uint8_t avg_template(uint8_t *values, uint32_t size){
  uint32_t avg, i = 0;
  while( i < size ) { avg += values[i]; ++i; }
  return (uint8_t)(avg/size);
}

void image_filter(uint8_t * pixels, uint32_t width, uint32_t height, uint32_t filter_size, uint8_t (*filtr_type)(uint8_t*, uint32_t)) {
  uint8_t values[filter_size * filter_size];
  uint8_t new_pixels[width*height];
  int32_t v, z, mid = filter_size / 2;
  uint32_t x, y, i, size_mask = 0;
  
  uint32_t new_height = height - mid;
  uint32_t new_width = width - mid;

  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      i = 0;
      for (v = -mid; v <= mid; ++v)
        for (z = -mid; z <= mid; ++z)
          values[i++] = pixels[(y + v) * width + x + z];
      new_pixels[size_mask++] = filtr_type(values, i);
    }
  }
  merge_pixels(pixels, width, height, mid, new_pixels, size_mask);
}

#endif