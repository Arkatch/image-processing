#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

#include "../maxmin.h"
/*/
  void k_means_clustering(image_t *img, uint32_t k)
    Klasteryzacja obrazu za pomocą metody centroidów
    point_k *k - tablica struktur przechowuje współżędne środków klastrów, 
    uint32_t k_size - definiuje ilość środków
/*/

#ifndef __CLUSTERING24__
#define __CLUSTERING24__

meansrgb_t get_pixel_info24(image_t *img, uint32_t x, uint32_t y){
  return (meansrgb_t){
    (int32_t)img->pixels[y * img->width + x + 2], //red
    (int32_t)img->pixels[y * img->width + x + 1], //green
    (int32_t)img->pixels[y * img->width + x + 0], //blue
    x, y
  };
}

int32_t calc_distance(pointrgb_t *p, meansrgb_t *k){
  return sqrt(pow(p->r - k->r, 2) + pow(p->g - k->g, 2) + pow(p->b - k->b, 2));
}

void k_means24_clustering(image_t *img, meansrgb_t k[], uint32_t k_size){
  uint32_t x, y, z, j, i;

  //Tablica struktur na nowe punkty
  pointrgb_t *clusters = (pointrgb_t*)malloc(img->size*sizeof(pointrgb_t));
  
  //[][0] wymiar liczba punktów
  //[][1] r [][2] g [][3] b - suma wartosci
  uint64_t avg[k_size][4];
  uint64_t avg_tmp[k_size][4];
  memset(avg, 0, sizeof(uint64_t)*k_size*4);

  //wypełnianie tablicy struktur pikselami rgb i indeksami
  for(x = 0, i = 0; x < img->size; x+=3){
    clusters[i++] = (pointrgb_t){
      img->pixels[x+2], img->pixels[x+1], img->pixels[x], //r, g, b
      0, 16777216, x 
    }; 
  }
  do{
    //Kopiowanie starych średnich wartości do bufora
    memcpy(avg_tmp, avg, sizeof(avg));
    //wyszukiwanie najmniejszej odległości dla danego piksela
    for(x = 0, i = 0; x < img->size; x += 3, ++i){
      for(j = 0; j < k_size; ++j){
        if( calc_distance(&clusters[i], &k[j]) <= clusters[i].distance ){
          clusters[i].index = j;
          clusters[i].distance = calc_distance(&clusters[i], &k[j]);
        }      
      }
    }
    /******zerowanie tablicy wartosci sreniej*****/
    memset(avg, 0, sizeof(uint64_t)*k_size*4);
    /****Sumowanie wartości i zliczanie indeksów**/
    for(i = 0; i < img->size; ++i){
      avg[clusters[i].index][0]++;
      avg[clusters[i].index][1] += clusters[i].r;
      avg[clusters[i].index][2] += clusters[i].g;
      avg[clusters[i].index][3] += clusters[i].b;
    }
    /*******obliczanie wartosci sreniej***********/
    //avg[j][0] może przyjąć wartość 0
    //Zabezpieczenie przed dzieleniem przez 0
    for(j = 0; j < k_size; ++j){ 
      avg[j][1] = avg[j][1] / ( (avg[j][0] == 0) ? 1 : avg[j][0] ); 
      avg[j][2] = avg[j][2] / ( (avg[j][0] == 0) ? 1 : avg[j][0] );
      avg[j][3] = avg[j][3] / ( (avg[j][0] == 0) ? 1 : avg[j][0] );
    }
    /**przypisywanie wartości śrenich do punktów**/
    for(i = 0; i < img->size; ++i){ 
      clusters[i].r = avg[clusters[i].index][1]; 
      clusters[i].g = avg[clusters[i].index][2]; 
      clusters[i].b = avg[clusters[i].index][3]; 
    }
    
    /*!!! warunek kończący pętle !!!*/
    /*Operacje się kończą gdy wartości średnie nowych punktów
    /*i wartości śrenidnie punktów z poprzedniego kroku są takie same*/
  }while( memcmp(avg_tmp, avg, sizeof(avg)) == 0 );
  
  /**wypełnianie talicy nowo obliczonymi pikselami**/
  for(x = 0, i = 0; x < img->size; x+=3, ++i){ 
    img->pixels[clusters[i].pos+2] = clusters[i].r; 
    img->pixels[clusters[i].pos+1] = clusters[i].g; 
    img->pixels[clusters[i].pos+0] = clusters[i].b; 
  }
  free(clusters);
}


#endif
