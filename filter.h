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

void median_filter(uint8_t * pixels, uint32_t width, uint32_t height, uint32_t filter_size) {
  uint8_t values[filter_size * filter_size];
  uint32_t v, x, y, z, i, mid = filter_size / 2;

  height = height - filter_size + 1;
  width = width - filter_size + 1;
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      i = 0;
      for (v = 0; v < filter_size; ++v)
        for (z = 0; z < filter_size; ++z)
          values[i++] = pixels[(y + v) * width + x + z];

      qsort(values, i, sizeof(uint8_t), comp);
      pixels[(y + mid) * width + x + mid] = values[i/2];
    }
  }
}
void min_filter(uint8_t * pixels, uint32_t width, uint32_t height, uint32_t filter_size) {
  uint8_t _min;
  uint32_t v, x, y, z;
  uint32_t mid = filter_size / 2;
  uint8_t new_pixels[width*height];
  uint32_t size_mask = 0;

  height = height - filter_size + 1;
  width = width - filter_size + 1;
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      _min = 255;
      for (v = 0; v < filter_size; ++v)
        for (z = 0; z < filter_size; ++z)
          _min = min(_min, pixels[(y + v) * width + x + z]);
      
      new_pixels[size_mask++] = _min;
    }
  }
  merge_pixels(pixels, width, height, mid, new_pixels, size_mask);
}
void max_filter(uint8_t * pixels, uint32_t width, uint32_t height, uint32_t filter_size) {
  uint8_t _max;
  uint32_t v, x, y, z;
  uint32_t mid = filter_size / 2;
  uint8_t new_pixels[width*height];
  uint32_t size_mask = 0;

  height = height - filter_size + 1;
  width = width - filter_size + 1;
  for (y = 0; y < height; ++y){
    for (x = 0; x < width; ++x) {
      _max = 0;
      for (v = 0; v < filter_size; ++v)
        for (z = 0; z < filter_size; ++z)
          _max = max(_max, pixels[(y + v) * width + x + z]);
      
      new_pixels[size_mask++] = _max;
    }
  }
  merge_pixels(pixels, width, height, mid, new_pixels, size_mask);
}
void artmetic_filter(uint8_t * pixels, uint32_t width, uint32_t height, uint32_t filter_size) {
  uint32_t v, x, y, z, avg;
  uint32_t mid = filter_size / 2;
  uint32_t n = filter_size * filter_size;

  uint8_t new_pixels[width*height];
  uint32_t size_mask = 0;

  height = height - filter_size + 1;
  width = width - filter_size + 1;
  for (y = 0; y < height; y+=filter_size) {
    for (x = 0; x < width; x+=filter_size) { 
      avg = 0;
      for (v = 0; v < filter_size; ++v) 
        for (z = 0; z < filter_size; ++z) 
          avg += pixels[(y + v) * width + x + z];
      new_pixels[size_mask++] = avg/n;
    }
  }
  merge_pixels(pixels, width, height, mid, new_pixels, size_mask);
}
#endif