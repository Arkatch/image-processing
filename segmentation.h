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

/*/

  void growingregion(image_t *img, int32_t px, int32_t py, int16_t threshold)
  int32_t px i int32_t py - piksel od którego startujemy, od niego też zależy jakie wartości będą kolejno przyłączane 
  int16_t threshold - zakres który mówi jakie piksele mamy mamy dołączać
    przykład: piksel startowy ma wartość pixels[py*width+px] = 95, a nasz
    threshold wynosi 10, piksele które będą dołączane do obszaru mają więc w tym przypadku wartość od 85 do 105
/*/

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
void growingregion_queue(image_t *img, int32_t px, int32_t py, int16_t t_min, int16_t t_max, uint8_t *new_pixels){
  queue new_queue;
  int32_t x, y, pos;
  queue_start(&new_queue);
  enqueue(&new_queue, px, py);
  while( is_empty_queue(&new_queue) == false ){
    dequeue(&new_queue, &x, &y);
    pos = y * img->width + x;
    if( new_pixels[pos] != 255 && img->pixels[pos] >= t_min && img->pixels[pos] <= t_max ){
      new_pixels[pos] = 255;
      if( x - 1 >= 0 )
        { enqueue(&new_queue, x-1, y); }
      if( x + 1 < img->width )
        { enqueue(&new_queue, x+1, y); }
      if( y + 1 < img->height )
        { enqueue(&new_queue, x, y+1); }
      if( y - 1 >= 0 )
        { enqueue(&new_queue, x, y-1); }
    }
  }
}
void growingregion(image_t *img, int32_t px, int32_t py, int16_t threshold){
  uint8_t new_pixels[img->size];
  memset(new_pixels, 0, img->size);
  int16_t t_min = img->pixels[py*(img->width)+px]-threshold;
  int16_t t_max = img->pixels[py*(img->width)+px]+threshold;
  growingregion_queue(img, px, py, t_min, t_max, new_pixels);
  memmove(img->pixels, new_pixels, img->size);
}

#endif