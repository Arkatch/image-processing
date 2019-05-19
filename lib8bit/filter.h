#include "../maxmin.h"

/*/
  uint8_t * pixels - tablica zawierająca piksele obrazu, które maja zostać edytowane
  uint32_t width, uint32_t height - szerokość i wysokość obrazu

  void add_salt(uint8_t * pixels, uint32_t width, uint32_t height, uint8_t proc)
      Funkcja zakłóca obraz wartościami 0 i 255 w określonym procencie
      procent zakłóceń określa się parametrem "proc", zakres od 0 do 100

  uint8_t *_template(uint8_t *values, uint32_t size)  
      Funkcje *_template przyjmują za parametr tablice o rozmiarze [size]
      (size to iloczyn rozmiaru filtra 3x3 = 9, 5x5 = 25 etc.)
    Wartość zwracana:
      w zależności od funkcji, mediana, wartość minimalna, maksymalna lub średnia w tablicy
  
  void image_filter(uint8_t * pixels, uint32_t width, uint32_t height, uint32_t filter_size, uint8_t (*filtr_type)(uint8_t*, uint32_t))
      Funkcja jako parametr przyjmuje 
        rozmiar filtra - wartości nieparzyste >= 3, 5, 9..., tworzona potem jest tablica o rozmiarze 3x3, 5x5...
        funkcję filtrującą *_template w zależności jakie chcemy mieć filtrowanie

/*/
#ifndef __FILTER__
#define __FILTER__

void add_salt(image_t *img, uint8_t proc) {
  for (uint32_t  x = 0; x < img->size; ++x)
    if ((uint8_t) rand() % 101 < proc)
      { img->pixels[x] = (rand() % 2) ? 0 : 255; }
}
uint8_t median_template(uint8_t *values, uint32_t size){
  qsort(values, size, sizeof(uint8_t), comp);
  return values[size/2];
}
uint8_t min_template(uint8_t *values, uint32_t size){
  uint8_t _min = 255;  
  uint32_t i = 0;
  while( i < size ) { _min = min(_min, values[i]); ++i; }
  return _min;
}
uint8_t max_template(uint8_t *values, uint32_t size){
  uint8_t _max = 0; 
  uint32_t i = 0;
  while( i < size ) { _max = max(_max, values[i]); ++i; }
  return _max;
}
uint8_t avg_template(uint8_t *values, uint32_t size){
  uint32_t avg = 0, i = 0;
  while( i < size ) { avg += values[i]; ++i; }
  return (uint8_t)(avg/size);
}

void image_filter(image_t *img, uint32_t filter_size, uint8_t (*filtr_type)(uint8_t*, uint32_t)) {
  uint8_t values[filter_size * filter_size]; 
  uint8_t new_pixels[img->size];
  int32_t v, z, mid = filter_size / 2;
  uint32_t x, y, i, size_mask = 0;
  
  uint32_t new_height = img->height - mid;
  uint32_t new_width = img->width - mid;

  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      i = 0;
      for (v = -mid; v <= mid; ++v)
        for (z = -mid; z <= mid; ++z)
          values[i++] = img->pixels[(y + v) * (img->width) + x + z];
      new_pixels[size_mask++] = filtr_type(values, i);
    }
  }
  merge_pixels(img, mid, new_pixels, size_mask);
}

#endif