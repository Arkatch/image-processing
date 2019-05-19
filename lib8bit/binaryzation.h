#include "../maxmin.h"

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
  for (uint32_t x = 0; x < img->size; ++x)
    { img->pixels[x] = (img->pixels[x] > threshold) ? 255 : 0; }
}

void image_negative(image_t *img){
  for (uint32_t x = 0; x < img->size; ++x)
    { img->pixels[x] = 255 - img->pixels[x]; }
}

uint8_t median_threshold(image_t *img){
  uint8_t new_pixels[img->size];
  for (uint32_t x = 0; x < img->size; ++x)
    { new_pixels[x] = img->pixels[x]; }
  qsort(new_pixels, 1, img->size, comp);
  return new_pixels[img->size/2];
}

uint8_t otsu_threshold(image_t *img){
  uint32_t i;
  float p[256] = {0};
  uint32_t hist[256] = {0};
  
  for (uint32_t x = 0; x < img->size; ++x)
    { ++hist[img->pixels[x]]; }

  for(i = 0; i < 256; ++i)
    { p[i] = hist[i]/256.0; }

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
  uint32_t x, y, threshold;
  uint32_t s = size*size, new_size = 0;
  uint8_t value, _min, _max;
  uint8_t new_pixels[img->size];
    
  uint32_t new_height = img->height - mid; 
  uint32_t new_width = img->width - mid;

  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      _min = 255; _max = 0;
      for(v = -mid; v <= mid; ++v){
        for(z = -mid; z <= mid; ++z){
          value = img->pixels[(y + v) * img->width + x + z];
          _min = min(_min, value);
          _max = max(_max, value);
        }
      }
      threshold = (_min + _max)/2;
      value = img->pixels[y * img->width + x];
      new_pixels[new_size++] = (value > threshold) ? 255 : 0;
    }
  }
  merge_pixels(img, size/2, new_pixels, new_size);
}

void bernsenmethodmedian(image_t *img, uint32_t size){
  int32_t v, z, mid = size/2, sq = size*size;
  uint32_t x, y, threshold, i, new_size = 0;
  uint8_t new_pixels[img->size], k[sq];
    
  uint32_t new_height = img->height - mid; 
  uint32_t new_width = img->width - mid;

  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) 
    {
      i = 0;
      for(v = -mid; v <= mid; ++v)
        for(z = -mid; z <= mid; ++z)
          { k[i++] = img->pixels[(y + v) * img->width + x + z]; }
      qsort(&k, sq, 1, comp);
      new_pixels[new_size++] = (k[sq/2] > img->pixels[y * img->width + x]) ? 255 : 0;
    }
  }
  merge_pixels(img, size/2, new_pixels, new_size);
}

void bernsenmethod24(imagehsi_t *img, uint32_t filter_size){
  int32_t v, z, mid = (filter_size/2) * 3;
  uint32_t x, y, threshold, i;
  uint32_t new_size = 0;
  double value, _min, _max;
  double *new_pixels = (double*)malloc(img->size * sizeof(double));
  memcpy(new_pixels, img->pixels, img->size * sizeof(double));
    
  uint32_t new_height = (img->height - mid) * 3;
  uint32_t new_width = (img->width - mid) * 3;

  for (y = mid; y < new_height; y += 3) {
    for (x = mid; x < new_width; x += 3) {
      _min = 1; _max = 0;
      for(v = -mid; v <= mid; v += 3){
        for(z = -mid; z <= mid; z += 3){
          value = img->pixels[(y + v) * img->width + x + z];
          _min = min(_min, value);
          _max = max(_max, value);
        }
      }
      threshold = (_min + _max)/2.0;
      value =  img->pixels[y * img->width + x];
      new_pixels[new_size] = (value < threshold) ? 0 : 1;
      new_size += 3;
    }
  }
  memcpy(img->pixels, new_pixels, img->size * sizeof(double));
  free(new_pixels);
}
#endif
