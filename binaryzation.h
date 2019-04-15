#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"

#ifndef __BIN__
#define __BIN__
void binarization(uint8_t *pixels, uint32_t width, uint32_t height, uint8_t threshold){
  uint32_t x, y;
  for (y = 0; y < height; ++y)
    for (x = 0; x < width; ++x)
      pixels[y*width + x] = (pixels[y*width + x] > threshold) ? 255 : 0;
}
uint8_t otsuthreshold(uint8_t *pixels, uint32_t width, uint32_t height){
  uint32_t i;
  float p[256] = {0};
  uint32_t hist[256] = {0};
  uint32_t x, y;
  for (y = 0; y < height; ++y)
    for (x = 0; x < width; ++x)
      ++hist[pixels[y * width + x]];
  for(i = 0; i < 256; ++i)
    p[i] = hist[i]/256.0;

  uint32_t L = 256, k, max_k = 0; // <- wartość szukana
  float w0, w1, u0, u1, sigma, maxsigma=0;
  for(k = 1; k < L; ++k){
    w0=0; w1=0; u0=0; u1=0;
    for(i = 0; i <= k-1; ++i) w0 += p[i];
    for(i = k; i <= L-1; ++i) w1 += p[i];
    if( w0 == 0 || w1 == 0 ) continue;

    for(i = 0; i <= k-1; ++i) u0 += i*p[i];
    u0 /= w0;
    for(i = k; i <= L-1; ++i) u1 += i*p[i];
    u1 /= w1;
    sigma = w0*w1*(u0-u1)*(u0-u1);
    if( maxsigma < sigma ){
      maxsigma = sigma;
      max_k = k;
    } 
  }
  return max_k;
}
void otsumethod(uint8_t *pixels, uint32_t width, uint32_t height){
  uint8_t k = otsuthreshold(pixels, width, height);
  binarization(pixels, width, height, k);
}
void bernsenmethod(uint8_t *pixels, uint32_t width, uint32_t height, uint32_t size){
  int32_t v, z, mid = size/2;
  uint32_t x, y, threshold, i;
  uint32_t s = size*size, new_size = 0;
  uint8_t values[s], _min, _max;
  uint8_t new_pixels[width*height];
    
  uint32_t new_height = height - mid; 
  uint32_t new_width = width - mid; 
  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      i = 0; _min = 255; _max = 0;
      for(v = -mid; v <= mid; ++v){
        for(z = -mid; z <= mid; ++z){
          values[i] = pixels[(y + v) * width + x + z];
          _min = min(_min, values[i]);
          _max = max(_max, values[i]);
          ++i;
        }
      }
      threshold = (_min + _max)/2;
      binarization(values, size, size, (uint8_t)threshold);
      new_pixels[new_size++] = values[s/2];
    }
  }
  merge_pixels(pixels, width, height, size/2, new_pixels, new_size);
}
#endif