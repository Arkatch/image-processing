#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

#include "queue.h"
#include "maxmin.h"

#ifndef __SEGMENTATION__
#define __SEGMENTATION__
void growingregion_rec(
  uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, 
  int16_t threshold_min, int16_t threshold_max, uint8_t *new_pixels
){
  uint32_t pos = py*width + px;
  if( py < 0 || px < 0 || px >= width || py >= height || 
      new_pixels[pos] == 255 || 
      pixels[pos] < threshold_min||
      pixels[pos] > threshold_max
  ) return;
  new_pixels[pos] = 255;
  growingregion_rec(pixels, width, height, px, py+1, threshold_min, threshold_max, new_pixels);
  growingregion_rec(pixels, width, height, px, py-1, threshold_min, threshold_max, new_pixels);
  growingregion_rec(pixels, width, height, px+1, py, threshold_min, threshold_max, new_pixels);
  growingregion_rec(pixels, width, height, px-1, py, threshold_min, threshold_max, new_pixels);
}
void growingregion_queue(
  uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, 
  int16_t threshold_min, int16_t threshold_max, uint8_t *new_pixels
){
  queue new_queue;
  int32_t x, y, pos;
  queue_start(&new_queue);
  enqueue(&new_queue, px, py);
  while( is_empty_queue(&new_queue) == false ){
    dequeue(&new_queue, &x, &y);
    pos = y*width + x;
    if( new_pixels[pos] != 255 && pixels[pos] >= threshold_min && pixels[pos] <= threshold_max ){
      new_pixels[pos] = 255;
      if( x - 1 >= 0 )
        { enqueue(&new_queue, x-1, y); }
      if( x + 1 < width )
        { enqueue(&new_queue, x+1, y); }
      if( y + 1 < height )
        { enqueue(&new_queue, x, y+1); }
      if( y - 1 >= 0 )
        { enqueue(&new_queue, x, y-1); }
    }
  }
}
void growingregion(uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, int16_t threshold){
  uint32_t size = width*height;
  uint8_t new_pixels[size];
  memset(new_pixels, 0, size);
  growingregion_queue(pixels, width, height, px, py, pixels[py*width+px]-threshold, pixels[py*width+px]+threshold, new_pixels);
  memmove(pixels, new_pixels, size);
}

#endif