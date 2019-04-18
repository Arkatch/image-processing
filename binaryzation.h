#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"

/*/
  uint8_t * pixels - tablica zawierająca piksele obrazu, które maja zostać edytowane
  uint32_t width, uint32_t height - szerokość i wysokość obrazu
  
  void image_negative(uint8_t *pixels, uint32_t width, uint32_t height)
      Funkcja zmienia obraz na negatyw za pomocą dopełnienia pikseli
  
  void binarization(uint8_t *pixels, uint32_t width, uint32_t height, uint8_t threshold)
      Funkcja przyjmuje jako parametr wartość |uint8_t threshold|
        wszystkie wartości >threshold są zamieniane na wartości 255 (kolor biały)
        wszystkie wartości <=threshold są zamieniane na wartości 0 (kolor czarny)

  uint8_t otsu_threshold(uint8_t *pixels, uint32_t width, uint32_t height)
      Funkcja wylicza i zwraca threshold wyznaczony metodą Otsu

  uint8_t median_threshold(uint8_t *pixels, uint32_t width, uint32_t height)
      Funkcja wylicza i zwraca threshold wyznaczony za pomocą mediany

  void bernsenmethod(uint8_t *pixels, uint32_t width, uint32_t height, uint32_t size)
      Metoda Bernsena binaryzuje obraz za pomocą wartości w otoczeniu piksela (parametr size określa wielkość np 3 da nam siatke 3x3)
      
/*/

#ifndef __BIN__
#define __BIN__
void binarization(image_t *img, uint8_t threshold){
  uint32_t x, y;
  for (y = 0; y < img->height; ++y)
    for (x = 0; x < img->width; ++x)
      img->pixels[y * img->width + x] = (img->pixels[y* img->width + x] > threshold) ? 255 : 0;
}

void image_negative(image_t *img){
  uint32_t x, y, index;
  for(y = 0; y < img->height; ++y){
    for(x = 0; x < img->width; ++x){
      index = y * img->width+x;
      img->pixels[index] = 255 - img->pixels[index];
    }
  }
}

uint8_t median_threshold(image_t *img){
  uint32_t x, y, i = 0;
  uint8_t new_pixels[img->size];
  for(y = 0; y < img->height; ++y)
    for(x = 0; x < img->width; ++x)
      new_pixels[i++] = img->pixels[y * img->width+x];
  qsort(new_pixels, 1, img->size, comp);
  return new_pixels[img->size/2];
}

uint8_t otsu_threshold(image_t *img){
  uint32_t i;
  float p[256] = {0};
  uint32_t hist[256] = {0};
  uint32_t x, y;
  for (y = 0; y < img->height; ++y)
    for (x = 0; x < img->width; ++x)
      ++hist[img->pixels[y * img->width + x]];
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
void otsumethod(image_t *img){
  uint8_t k = otsu_threshold(img);
  binarization(img, k);
}
void bernsenmethod(image_t *img, uint32_t size){
  int32_t v, z, mid = size/2;
  uint32_t x, y, threshold, i;
  uint32_t s = size*size, new_size = 0;
  uint8_t values[s], _min, _max;
  uint8_t new_pixels[img->size];
    
  uint32_t new_height = img->height - mid; 
  uint32_t new_width = img->width - mid;
  image_t tmp;

  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      i = 0; _min = 255; _max = 0;
      for(v = -mid; v <= mid; ++v){
        for(z = -mid; z <= mid; ++z){
          values[i] = img->pixels[(y + v) * img->width + x + z];
          _min = min(_min, values[i]);
          _max = max(_max, values[i]);
          ++i;
        }
      }
      threshold = (_min + _max)/2;
      tmp = (image_t){NULL, values, 0, 0, size, size};
      binarization(&tmp, (uint8_t)threshold);
      new_pixels[new_size++] = tmp.pixels[s/2];
    }
  }
  merge_pixels(img, size/2, new_pixels, new_size);
}
#endif