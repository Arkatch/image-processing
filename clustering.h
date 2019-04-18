#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

#include "maxmin.h"

/*/
  

  void k_means_clustering(image_t *img, uint32_t k)
    Klasteryzacja obrazu za pomocą metody centroidów
    point_k *k - tablica struktur przechowuje współżędne środków klastrów, 
    uint32_t k_size - definiuje ilość środków
/*/

#ifndef __CLUSTERING__
#define __CLUSTERING__

typedef struct point_t{
  int16_t pix;
  int32_t index, distance;
  uint32_t pos;
}point_t;

typedef struct means_t{
  int16_t pix;
  uint32_t x, y;
}means_t;


void k_means_clustering(image_t *img, means_t *k, uint32_t k_size){
  uint32_t x, y, z, j, i;

  //Tablica struktur na nowe punkty
  point_t *clusters = (point_t*)malloc(img->size*sizeof(point_t));
  
  //[][0] wymiar liczba punktów
  //[][1] suma wartości pikseli
  uint64_t avg[k_size][2];
  uint64_t avg_tmp[k_size][2];

  //wypełnianie tablicy struktur pikselami i indeksami
  for(y = 0, i = 0; y < img->height; ++y)
    for(x = 0; x < img->width; ++x)
      clusters[i++] = (point_t){img->pixels[y * img->width + x], 0, 256, y * img->width+x}; 


  do{
    //Kopiowanie starych średnich wartości do bufora
    memcpy(avg_tmp, avg, sizeof(avg));
    //wyszukiwanie najmniejszej odległości dla danego punktu
    for(i = 0; i < img->size; ++i){
      j = 0;
      for(j = 0; j < k_size; ++j){
        if( abs(clusters[i].pix - k[j].pix) < clusters[i].distance ){
          clusters[i].index = j;
          clusters[i].distance = abs(clusters[i].pix - k[j].pix);
        }      
      }
    }
    /******zerowanie tablicy wartosci sreniej*****/
    for(j = 0; j < k_size; ++j) { 
      avg[j][0] = 0; 
      avg[j][1] = 0; 
    }
    /****Sumowanie wartości i zliczanie indeksów**/
    for(i = 0; i < img->size; ++i){
      avg[clusters[i].index][0]++;
      avg[clusters[i].index][1] += clusters[i].pix;
    }
    /*******obliczanie wartosci sreniej***********/
    //avg[j][0] może przyjąć wartość 0
    //Zabezpieczenie przed dzieleniem przez 0
    for(j = 0; j < k_size; ++j)
      { avg[j][1] = avg[j][1] / ( (avg[j][0] == 0) ? 1 : avg[j][0] ); }
    /**przypisywanie wartości śrenich do punktów**/
    for(i = 0; i < img->size; ++i)
      { clusters[i].distance = avg[clusters[i].index][1]; }
    
    /*!!! warunek kończący pętle !!!*/
    /*Operacje się kończą gdy wartości średnie nowych punktów
    /*i wartości śrenidnie punktów z poprzedniego kroku są takie same*/
  }while( memcmp(avg_tmp, avg, sizeof(avg)) == 0 );
  
  /**wypełnianie talicy nowo obliczonymi pikselami**/
  for(i = 0; i < img->size; ++i)
    { img->pixels[clusters[i].pos] = clusters[i].distance; }
  free(clusters);
}

#endif