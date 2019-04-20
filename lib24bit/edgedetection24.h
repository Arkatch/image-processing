#include "../maxmin.h"

#ifndef __EDGE24__
#define __EDGE24__

double sobel_template24(double *values){
  double gx[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
  double gy[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
  double x_value = 0, y_value = 0;
  for(uint8_t i = 0; i < 9; ++i){
    x_value += values[i] * gx[i];
    y_value += values[i] * gy[i];
  }
  return sqrt(x_value*x_value + y_value*y_value);
}

void edge_detection24(imagehsi_t *img, double (*detection_type)(double*)){
  uint32_t x, y, s;
  int32_t i, j, mid = 3, size_mask = 0;
  double values[9];
  double *new_pixels = (double*)malloc(img->size * sizeof(double));
  

  uint32_t new_height = (img->height - mid)*3; 
  uint32_t new_width = (img->width - mid)*3; 
  for (y = mid; y < new_height; y += 3) {
    for (x = mid; x < new_width; x += 3) {
      s = 0;
      for (i = -mid; i <= mid; i += 3)
        for (j = -mid; j <= mid; j += 3)
          { values[s++] = img->pixels[(y + i) * img->width + x + j]; }
      new_pixels[size_mask++] = detection_type(values);
    }
  }
  merge_pixels24(img, 3, new_pixels, size_mask);
  free(new_pixels);
}

#endif