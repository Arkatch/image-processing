#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"

#ifndef __SEGMENTATION__
#define __SEGMENTATION__

void growingregion_up(
  uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, 
  int16_t threshold_min, int16_t threshold_max, uint8_t *new_pixels
){
  if( px < 0 || py >= height || px >= width ||
      new_pixels[py*width + px] == 255 || 
      pixels[py*width + px] < threshold_min||
      pixels[py*width + px] > threshold_max
  ) return;
  new_pixels[py*width + px] = 255;
  growingregion_up(pixels, width, height, px, py+1, threshold_min, threshold_max, new_pixels);
  growingregion_up(pixels, width, height, px+1, py, threshold_min, threshold_max, new_pixels);
  growingregion_up(pixels, width, height, px-1, py, threshold_min, threshold_max, new_pixels);
}
void growingregion_down(
  uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, 
  int16_t threshold_min, int16_t threshold_max, uint8_t *new_pixels
){
  if( px < 0 || px >= width || py < 0 || 
      new_pixels[py*width + px] == 255 || 
      pixels[py*width + px] < threshold_min||
      pixels[py*width + px] > threshold_max
  ) return;
  new_pixels[py*width + px] = 255;
  growingregion_down(pixels, width, height, px, py-1, threshold_min, threshold_max, new_pixels);
  growingregion_down(pixels, width, height, px+1, py, threshold_min, threshold_max, new_pixels);
  growingregion_down(pixels, width, height, px-1, py, threshold_min, threshold_max, new_pixels);
}
void growingregion_left(
  uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, 
  int16_t threshold_min, int16_t threshold_max, uint8_t *new_pixels
){
  if( px < 0 || py < 0 || py >= height || 
      new_pixels[py*width + px] == 255 || 
      pixels[py*width + px] < threshold_min||
      pixels[py*width + px] > threshold_max
  ) return;
  new_pixels[py*width + px] = 255;
  growingregion_left(pixels, width, height, px, py+1, threshold_min, threshold_max, new_pixels);
  growingregion_left(pixels, width, height, px, py-1, threshold_min, threshold_max, new_pixels);
  growingregion_left(pixels, width, height, px-1, py, threshold_min, threshold_max, new_pixels);
}
void growingregion_right(
  uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, 
  int16_t threshold_min, int16_t threshold_max, uint8_t *new_pixels
){
  if( py < 0 || px >= width || py >= height || 
      new_pixels[py*width + px] == 255 || 
      pixels[py*width + px] < threshold_min||
      pixels[py*width + px] > threshold_max
  ) return;
  new_pixels[py*width + px] = 255;
  growingregion_right(pixels, width, height, px, py+1, threshold_min, threshold_max, new_pixels);
  growingregion_right(pixels, width, height, px, py-1, threshold_min, threshold_max, new_pixels);
  growingregion_right(pixels, width, height, px+1, py, threshold_min, threshold_max, new_pixels);
}
void growingregion(uint8_t *pixels, uint32_t width, uint32_t height, int32_t px, int32_t py, int16_t threshold){
  uint32_t size = width*height, x, y;
  uint8_t new_pixels[size];
  memset(new_pixels, 0, size);
  
  //py = abs((long)py-(long)height); //bmp upside down
  growingregion_up(pixels, width, height, px, py, pixels[py*width+px]-threshold, pixels[py*width+px]+threshold, new_pixels);
  growingregion_down(pixels, width, height, px, py, pixels[py*width+px]-threshold, pixels[py*width+px]+threshold, new_pixels);
  growingregion_right(pixels, width, height, px, py, pixels[py*width+px]-threshold, pixels[py*width+px]+threshold, new_pixels);
  growingregion_left(pixels, width, height, px, py, pixels[py*width+px]-threshold, pixels[py*width+px]+threshold, new_pixels);
  
  memmove(pixels, new_pixels, size);
}

#endif