#include "../maxmin.h"

/*/
    Wszystkie funkcje działają na przestrzeni barw HSI, 
    więc w funkcjach trzeba używać struktury imagehsi_t, 
    którą następnie trzeba skonwerterować do struktury image_t z paletą RGB

    // r,g,b ∈[0,255]
    // h∈[0,2PI], s∈[0,1], i∈[0,1]

/*/

#ifndef __FILTER24__
#define __FILTER24__

void add_salt24(imagehsi_t *img, uint8_t proc) {
  uint32_t x;
  for (x = 0; x < img->size; x += 3)
      if ((uint8_t) (rand() % 101) < proc)
        img->pixels[x] = (rand() % 2) ? 0 : 1;
}
double median_template24(double *values, uint32_t size){
  qsort(values, size, sizeof(double), comp);
  return values[size/2];
}
double min_template24(double *values, uint32_t size){
  double _min = 1;  
  uint32_t i = 0;
  while( i < size ) 
    { _min = min(_min, values[i++]); }
  return _min;
}
double max_template24(double *values, uint32_t size){
  double _max = 0; 
  uint32_t i = 0;
  while( i < size ) 
    { _max = max(_max, values[i++]); }
  return _max;
}
double avg_template24(double *values, uint32_t size){
  double avg = 0; 
  uint32_t i = 0;
  while( i < size ) 
    { avg += values[i++]; }
  return avg/size;
}

void image_filter24(imagehsi_t *img, uint32_t filter_size, double (*filtr_type)(double*, uint32_t)) {
  double values[filter_size * filter_size]; 
  double *new_pixels = (double*)malloc(img->size * sizeof(double));
  int32_t i, j, mid = (filter_size / 2) * 3;
  uint32_t x, y, s, size_mask = 0, index = 0;
  
  uint32_t new_height = (img->height - mid) * 3;
  uint32_t new_width = (img->width - mid) * 3;

  for (y = mid; y < new_height; y += 3) {
    for (x = mid; x < new_width; x += 3) {
      s = 0;
      for (i = -mid; i <= mid; i += 3)
        for (j = -mid; j <= mid; j += 3){ 
          index = (y + i) * (img->width) + x + j;
          values[s++] = img->pixels[index]; 
        }
      new_pixels[size_mask++] = filtr_type(values, s);
    }
  }
  merge_pixels24(img, mid, new_pixels, size_mask);
  free(new_pixels);
}


#endif