#include "maxmin.h"
#include "lib8bit/queue.h"
#include "lib24bit/bmp24rgb.h"
#include "lib8bit/bmp8gray.h"

#ifndef __CELLS__
#define __CELLS__

//Wkrótce
//void CoherenceEnhancingDiffusion();
//hit-or-miss

typedef struct statistic_t{
  int size;
  int *values;
  rgb_t *color;
}statistic_t;

bool colcmp(rgb_t col_1, rgb_t col_2){
  return col_1.b == col_2.b && col_1.g == col_2.g && col_1.r == col_2.r;
}

//-----------Wypełnianie kolorem------------//
int32_t fill_with_color(image_t *img, uint32_t px, uint32_t py, uint8_t color_fill, uint8_t color){
  queue new_queue;
  int32_t x = 0, y = 0, pos = 0, size = 0, tmp = -1;
  queue_start(&new_queue);
  enqueue(&new_queue, px, py);
  while( is_empty_queue(&new_queue) == false ){
    dequeue(&new_queue, &x, &y);
    tmp = new_queue.count;
    pos = y * img->width + x;
    if( img->pixels[pos] == color ){
      img->pixels[pos] = color_fill;
      ++size;
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
  return size;
}
//--------------------------------//

//----Usuwanie pojedynczych pikseli, które są np w środku komórki----//
void clean(image_t *img, int color_fill, int color, int min_size){
  uint32_t x, y;
  for(y = 0; y < img->height; ++y)
  for(x = 0; x < img->width; ++x){
    if(img->pixels[y * img->width + x] == color){
      if( fill_with_color(img, x, y, color_fill, color) < min_size )
        fill_with_color(img, x, y, abs(255-color), color_fill);
    }
  }
}
//--------------------------------//

//----------Wypełnianie komórek losowymi kolorami i tworzenie statystyki------//
statistic_t *fill_with_rand(image_t *img, rgb_t color){
  statistic_t *test = (statistic_t*)malloc(sizeof(statistic_t));
  *test = (statistic_t){
    0,
    (int*)calloc(1, sizeof(int)),
    (rgb_t*)calloc(1, sizeof(rgb_t))
  };
  for(int i = 0; i < img->height; ++i){
    for(int j = 0; j < img->width*3; j+=3){
      rgb_t rand_c, tmp = {
        .r = img->pixels[i*img->width*3 + j + 2],
        .g = img->pixels[i*img->width*3 + j + 1],
        .b = img->pixels[i*img->width*3 + j + 0]
      };
      if( colcmp(tmp, color) ){
        queue new_queue;
        int32_t x = 0, y = 0, pos = 0, cells = 0;
        queue_start(&new_queue);
        enqueue(&new_queue, j, i);
        rand_c = (rgb_t){rand()%220+20,rand()%220+20,rand()%220+20};
        while( is_empty_queue(&new_queue) == false ){
          dequeue(&new_queue, &x, &y);
          pos = y * img->width*3 + x;
          tmp = (rgb_t){
            .r = img->pixels[pos+2],
            .g = img->pixels[pos+1],
            .b = img->pixels[pos+0]
          };
          if( colcmp(tmp, color) ){
            ++cells;
            img->pixels[pos + 2] = rand_c.r;
            img->pixels[pos + 1] = rand_c.g;
            img->pixels[pos + 0] = rand_c.b;
            if( x - 3 >= 0 )
              { enqueue(&new_queue, x-3, y); }
            if( x + 3 < img->width*3 )
              { enqueue(&new_queue, x+3, y); }
            if( y + 1 < img->height )
              { enqueue(&new_queue, x, y+1); }
            if( y - 1 >= 0 )
              { enqueue(&new_queue, x, y-1); }
          }
        }
        test->values[test->size] = cells;
        test->color[test->size] = rand_c;
        test->size++;
        test->values = (int*)realloc(test->values, (test->size+1)*sizeof(int));
        test->color = (rgb_t*)realloc(test->color, (test->size+1)*sizeof(rgb_t));
      }
    }
  }
  return test;
}

//Sortowanie statystyk utworzonych przez funkcje fill_with_rand
void sort_stat(statistic_t* tab){
  tab->size--;
  for(int i = 0; i < tab->size; ++i)
    for(int j = 0; j < tab->size - i; ++j)
      if( tab->values[j] > tab->values[j+1] ){
        swap(tab->values[j], tab->values[j+1], int);
        swap(tab->color[j], tab->color[j+1], rgb_t);
      }
}

//Zapisywanie statystyk do pliku
void save_stat(statistic_t* tab, FILE *file){
  for(int j = 0; j < tab->size; ++j){
    fprintf(file, "%d. Liczba pikseli: %d, RGB(%d, %d, %d)\n", j+1, 
      tab->values[j], tab->color[j].r, 
      tab->color[j].g, tab->color[j].b);
  }
  free(tab); fclose(file);
}
//----------------------------------------------------------------------------------//

//###### Funkcje morfologiczne ######//
/*
###########
  pruning() - funkcja licząca liczbę dróg do piksela (oparta na funkcja transitions())
  Jeżeli piksel ma tylko jedną lub zero dróg do innego piksela
  to zmień jego wartość na 0
  Przykłady pikseli, które zostaną usunięte: 
  |0|0|0| |0|0|1| |0|0|0| |0|0|0|
  |0|1|1| |0|1|1| |0|1|0| |0|1|0|
  |0|0|0| |0|0|0| |1|1|1| |0|0|0|
  Takie np. piksele zostaną: 
  |1|0|0| |0|0|0| |0|0|0|
  |0|1|0| |1|1|1| |0|1|1|
  |0|0|1| |0|0|0| |1|0|0|
  Robić do momentu gdy obraz się nie zmienia 

###########
  funkcja oparta na: Zhang-Suen thinning algorithm
  thinning():
  Algorytm wykonujemy do momentu, aż w obrazie nie wystąpią nowe zmiany
  |P9|P2|P3|
  |P8|P1|P4|
  |P7|P6|P5|
  Głównym pikselem jest piksel P1, dla którego:
  -liczymy liczbę sąsiadów (funkcja neighbors())
  -liczymy liczbę przemian z N -> 0, dla P2,P3,P4,P5,P6,P7,P8,P9,P2, gdzie N > 0 (funkcja transitions())
  
  piksel zmieniamy na 0 gdy:
  2 <= liczba sąsiadów <= 6      &&
  liczba przemian == 1           &&
  P2 == 0 || P4 == 0 || P6 == 0  &&
  P4 == 0 || P6 == 0 || P8 == 0
*/

//Liczenie sąsiadów piskela
int neighbors(int *arr, size_t size){
  int nums = 0;
  for(size_t i = 0; i < size; ++i)
      nums += (i != size/2 && !!arr[i]);
  return nums;
}
//Liczenie półaczeń z innymi pikselami
int transitions(int *arr){
  int tab[8] = {
    arr[1], arr[2], arr[5], arr[8], 
    arr[7], arr[6], arr[3], arr[0]
  };
  int nums = tab[7] && tab[0] == 0;
  for(int i = 0; i < 7; ++i)
    nums += (tab[i] && tab[i+1] == 0);
  return nums;
}
//#################################//
//hitcolor 255
bool thickening(int32_t *pix){
  if( pix[4] == 1 )
    return false;
  // -1 - tych pikseli nie sprawdzamy, są nieistotne
  int i, j;
  bool hit;
  int32_t b[8][9] = {
    {1,1,-1,1,0,-1,1,-1,0},  
    {1,1,1,-1,0,1,0,-1,-1},  
    {0,-1,1,-1,0,1,-1,1,1},  
    {-1,-1,0,1,0,-1,1,1,1},  
    {-1,1,1,-1,0,1,0,-1,1},
    {0,-1,-1,-1,0,1,1,1,1},
    {1,-1,0,1,0,-1,1,1,-1},
    {1,1,1,1,0,-1-1,-1,0}
  };
  for(i = 0; i < 8; ++i){
    hit = true;
    for(j = 0; j < 9; ++j){
      if( b[i][j] != -1 && b[i][j] != !!pix[j] )
        hit = false;
    }
    if( hit )
      return true;
  }
  return false;
}
//hitcolor 0
bool thinning(int32_t *pix){
  int n = neighbors(pix, 9);
  int s = transitions(pix);
  bool p2p4p6 = pix[1]*pix[5]*pix[7] == 0;
  bool p4p6p8 = pix[3]*pix[5]*pix[7] == 0;
  return pix[4] != 0 && s == 1 && n >= 2 && n <= 6 && p2p4p6 && p4p6p8;
}
//hitcolor 0
bool pruning(int32_t *pix){
  return pix[4] != 0 && transitions(pix) == 1;
}
//#################################//

//--------------------------Morfologia---------------------------------//

//uint8_t hitcolor - kolor który zastępuje trafiony piksel
void morphology(image_t *img, bool(*method)(int32_t*), uint8_t hitcolor){
  bool changed = true;
  int32_t pix[9], p;
  int32_t x, y, i, j, index;
  uint8_t *new_pixels = (uint8_t*)malloc(img->size*sizeof(uint8_t));

  int times = 0;
  while( changed ){
    changed = false;
    memcpy(new_pixels, img->pixels, img->size);
    for(y = 1; y < img->height - 1; ++y){
      for(x = 1; x < img->width - 1; ++x)
      {
        p = 0;
        index = y*img->width + x; 
        for(i = -1; i <= 1; ++i)
          for(j = -1; j <= 1; ++j)
            pix[p++] = img->pixels[(y+i)*img->width + x + j];
        if( method(pix) )
          { new_pixels[index] = hitcolor; changed = true; }
      }
    }
    memcpy(img->pixels, new_pixels, img->size);
  }
  free(new_pixels);
}
//-------------------------------------------------------------------------//


//-------------Główna funkcja----------------//
statistic_t *separate_cells(image_t *img, image_t *color, int precision){
  //Czas wykonania funkcji zależy głównie od tego jak duży filtr medianowy się zastosuje
  //Złożoność rośnie kwadratowo, w przypadku filtru o wielkości 21, trzeba dla każdego z pikseli
  //wyszukać medianę z 21*21 innych pikseli
  //funkcja dla precision == 23 potrzebuje 30 sekund

  //Binaryzowania obrazu z wykorzystaniem medianowej metody Bernsen 
  bernsenmethodmedian(img, precision);
  //otsumethod(img);

  //Czyszczenie obrazu z pojedynczych, niezwiązanych z krawędziami pikseli
  clean(img, 254, 255, 200);

  //Filtrowanie obrazu medianowo
  image_filter(img, 5, median_template);

  //Zastosowanie rozszerzania
  convolution_matrix(img, dilation_template);

  //Filtrowanie obrazu, tym razem filtrem minimalnym, aby zmienijszyć wielkość krawędzi
  image_filter(img, 3, min_template);

  //czyszczenie obrazu z ostatnich wolnych pojedynczych pikseli
  clean(img, 254, 255, 200);
  
  //Szkieletyzacja i usuwanie niezwiązanym z niczym linii 
  morphology(img, thinning, 0);
  morphology(img, pruning, 0);

  //Zamiana obrazu na RGB, kolorowanie i liczenie komórek
  gray_to_rgb(color, img);
  return fill_with_rand(color, (rgb_t){0,0,0});
}
//-------------------------------------------//

#endif
