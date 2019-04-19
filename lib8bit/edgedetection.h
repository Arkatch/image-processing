#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "../maxmin.h"

/*/
  uint8_t * pixels - tablica zawierająca piksele obrazu, które maja zostać edytowane
  uint32_t width, uint32_t height - szerokość i wysokość obrazu

  uint8_t *_template(uint8_t *values)
    Funkcja przyjmuje jako parametr tablice pikseli o rozmiarze 9 (3x3)
    Wartość zwracana: 
      wynik dwuwymiarowego dyskretnego splotu macierzy
  
  void edge_detection(uint8_t * pixels, uint32_t width, uint32_t height, uint8_t (*detection_type)(uint8_t*))
      Funckja jako parametr przyjmuje funkcje *_template z których pomocą wyznacza sie krawędzie

/*/

#ifndef __EDGE__
#define __EDGE__
uint8_t prewitt_template(uint8_t *values){
  int8_t p1[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
  int8_t p2[] = { 0, 1, 1, -1, 0, 1, -1, -1, 0 };
  int8_t p3[] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
  int8_t p4[] = { 1, 1, 0, 1, 0, -1, 0, -1, -1 };
  int p1_v = 0, p2_v = 0, p3_v = 0, p4_v = 0;
  for(int i = 0; i < 9; ++i){
    p1_v += values[i] * p1[i];
    p2_v += values[i] * p2[i];
    p3_v += values[i] * p3[i];
    p4_v += values[i] * p4[i];
  }
  p1_v *= p1_v; p2_v *= p2_v;
  p3_v *= p3_v; p4_v *= p4_v;
  return sqrt(p1_v+p2_v+p3_v+p4_v);
}
uint8_t sobel_template(uint8_t *values){
  int8_t gx[] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
  int8_t gy[] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
  int x_value = 0, y_value = 0, i;
  for( i = 0; i < 9; ++i){
    x_value += values[i] * gx[i];
    y_value += values[i] * gy[i];
  }
  return sqrt(x_value*x_value + y_value*y_value);
}
uint8_t laplacian_template(uint8_t *values){
  int32_t v1[] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
  int32_t v2[] = { 1, 1, 1, 1, -8, 1, 1, 1, 1 };
  int x_value = 0, y_value = 0, i;
  for( i = 0; i < 9; ++i){
    x_value += values[i] * v1[i];
    y_value += values[i] * v2[i];
  }
  return sqrt(x_value*x_value + y_value*y_value);
}
void edge_detection(image_t *img, uint8_t (*detection_type)(uint8_t*)){
  uint32_t x, y;
  int32_t i, v, z, size_mask = 0;
  uint8_t new_pixels[img->size], values[9];

  uint32_t new_height = img->height - 1; 
  uint32_t new_width = img->width - 1; 
  for (y = 1; y < new_height; ++y) {
    for (x = 1; x < new_width; ++x) {
      i = 0;
      for (v = -1; v <= 1; ++v)
        for (z = -1; z <= 1; ++z)
          values[i++] = img->pixels[(y + v) * img->width + x + z];
      new_pixels[size_mask++] = detection_type(values);
    }
  }
  merge_pixels(img, 1, new_pixels, size_mask);
}
#endif