#include "../maxmin.h"
/*/
  void k_means_clustering(image_t *img, uint32_t k)
    Klasteryzacja obrazu za pomocą metody centroidów
    point_k *k - tablica struktur przechowuje współżędne środków klastrów, 
    uint32_t k_size - definiuje ilość środków
/*/

#ifndef __CLUSTERING__
#define __CLUSTERING__

means_t get_pixel_info(image_t *img, uint32_t x, uint32_t y){
  return (means_t){ img->pixels[y * img->width + x], x, y };
}

void k_means_clustering(image_t *img, means_t *k, uint32_t k_size){
  uint32_t x, y, z, j, i;

  //Tablica struktur na nowe punkty
  point_t *clusters = (point_t*)malloc(img->size*sizeof(point_t));
  
  //[][0] wymiar liczba punktów
  //[][1] suma wartości pikseli
  uint64_t avg[k_size][2];
  uint64_t avg_tmp[k_size][2];

  //wypełnianie tablicy struktur pikselami i indeksami
  for(x = 0, i = 0; x < img->size; ++x)
    clusters[i++] = (point_t){img->pixels[x], 0, 256, x}; 


  do{
    //Kopiowanie starych średnich wartości do bufora
    memcpy(avg_tmp, avg, sizeof(avg));
    //wyszukiwanie najmniejszej odległości dla danego punktu
    for(i = 0; i < img->size; ++i){
      for(j = 0; j < k_size; ++j){
        if( abs(clusters[i].pix - k[j].pix) < clusters[i].distance ){
          clusters[i].index = j;
          clusters[i].distance = abs(clusters[i].pix - k[j].pix);
        }      
      }
    }
    /******zerowanie tablicy wartosci sreniej*****/
    memset(avg, 0, sizeof(uint64_t)*k_size*2);

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
