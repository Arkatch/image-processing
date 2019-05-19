#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

#include "maxmin.h"
#include "lib8bit/bmp8gray.h"
#include "lib24bit/bmp24rgb.h"
#include "cells_counting.h"

int cmp(const void *a, const void *b){
  return *(int*)a - *(int*)b;
}

int main() {
  srand(time(NULL));
  setlocale(LC_ALL, "pl_PL.utf8");

  /*/-----------------------------------------//
  image_t img, gray, rgb;
  
  FILE *file = fopen("1.bmp","rb");
  if( file == NULL )
    return 1;
  
  //Wczytanie obrazu i konwersja obrazu do 8bit
  load_img(&img, file);
  rgb_to_gray(&img, &gray);

  //Wyznaczanie krawędzi, kolorowanie i wyznaczanie statystyki
  statistic_t *arr = separate_cells(&gray, &rgb, 17); //<- 17 - rozmiar wielkości filtra bersena

  //Zapisywanie obrazu 8bit
  file = fopen("gray.bmp", "wb");
  save_img(&gray, file, true);

  //Zapisywanie obrazu pokolorowanego
  file = fopen("rgb.bmp", "wb");
  save_img(&rgb, file, true);

  //Sortowanie i zapisywanie statystyk
  file = fopen("stat.txt", "wb");
  sort_stat(arr);
  save_stat(arr, file);

  //-----------------------------------------/*/

  // for(int i = 1; i <= 15; ++i){
  //   if(i==2) continue;
  //   char filename[30];
  //   sprintf(filename, "impr/%d_c.bmp", i);
  //   FILE *f = fopen(filename, "rb");

  //   image_t img, color, gray; 
  //   load_img(&img, f);
  //   rgb_to_gray(&img, &gray);

  //   //---------------------START--------------------//
  //   time_t c = clock(); 
  //   printf("Start...\n");

  //   statistic_t *tmp = separate_cells(&gray, &color, 17);
  //   sprintf(filename, "impr/gen/%d_stat.txt", i);
  //   f = fopen(filename, "wb");
  //   fprintf(f, "Zblizona liczba: %d\n", tmp->size);

  //   sort_stat(tmp);
  //   for(int j = 0; j < tmp->size; ++j)
  //     fprintf(f, "%d. Liczba pikseli: %d, RGB(%d, %d, %d)\n", j+1, tmp->values[j], tmp->color[j].r, tmp->color[j].g, tmp->color[j].b);
  //   free(tmp);

  //   c = clock() - c;
  //   printf("End: %ld Second: %.2f\n", c, (float)c/CLOCKS_PER_SEC); 
  //   //-----------------------END--------------------//
    
  //   sprintf(filename, "impr/gen/%d_gray.bmp", i);
  //   f = fopen(filename, "wb");
  //   save_img(&gray, f, true);

  //   sprintf(filename, "impr/gen/%d_color.bmp", i);
  //   f = fopen(filename, "wb");
  //   save_img(&color, f, true);
  // }

  /*/------------------------//
  char strrgb[50] = {0};
  char strgray[50] = {0};
  printf("Podaj nazwę pliku RGB: ");
  scanf("%49s", strrgb);
  while(getchar()!='\n');
  
  printf("Podaj nazwę pliku GRAY: ");
  scanf("%49s", strgray);
  while(getchar()!='\n');

  FILE *bmp = fopen(strrgb, "rb");
  FILE *bmp_gray = fopen(strgray, "rb");
  if( bmp == NULL ){
    printf("Nie można otworzyć/utworzyć plików, lub plik o danej nazwie nie istnieje!");
    return 1;
  }
  //------------------------//

  //--------Deklaracje-------//
  FILE *file;
  imagehsi_t hsi, hsi_corrupt, hsi_edit;
  image_t img, img_edit, new_img, corupt;
  load_img(&img, bmp);
  //------------------------//

  //-------Informacje o pliku--------//
  printf("Informacje o pliku RGB:\n");
  printf("Rozmiar: %u bajtów\nWymiary: %ux%u px\nOffbits: %u\n ",
    img.size, img.width, img.height, img.head_size);
  printf("|----------------------------|\n");
  //---------------------------------/*/

  /*/-----------------------HSI 24 bit BMP---------------------------//
  //---------Szum RGB(HSI)---------------//
  file = fopen("gen24/corrupt.bmp", "wb");  
  convert_to_hsi(&hsi, &img);   //#####przejście w tryb HSI#####
  add_salt24(&hsi, 15);         //dodawanie 15% szumu do obrazu
  copy_hsi(&hsi_corrupt, &hsi); //skopiowanie obrazu do nowej struktury
  save_hsi(&hsi, file, true);   //zapisanie obrazu, zamkniecie pliku i wyczyszczenie pamięci gdy parametr 3 == true
  //gdyby kończyć tu działanie programu, to trzeba wyczyścić jeszcze hsi free_hsi(&hsi)
  //-------------------------------------//

  //--------Filtrowanie medianowo--------//
  file = fopen("gen24/median.bmp", "wb");
  copy_hsi(&hsi, &hsi_corrupt);
  image_filter24(&hsi, 3, median_template24);
  save_hsi(&hsi, file, true);  
  //-------------------------------------//

  //--------Filtrowanie minimum----------//
  file = fopen("gen24/min.bmp", "wb");
  copy_hsi(&hsi, &hsi_corrupt);
  image_filter24(&hsi, 3, min_template24);
  save_hsi(&hsi, file, true);
  //-------------------------------------//

  //--------Filtrowanie maksimum---------//
  file = fopen("gen24/max.bmp", "wb");
  copy_hsi(&hsi, &hsi_corrupt);
  image_filter24(&hsi, 3, max_template24);
  save_hsi(&hsi, file, true);
  //-------------------------------------//

  //--------Filtrowanie średnia----------//
  file = fopen("gen24/avg.bmp", "wb");
  copy_hsi(&hsi, &hsi_corrupt);
  image_filter24(&hsi, 3, avg_template24);
  save_hsi(&hsi, file, true);
  //-------------------------------------//

  free_hsi(&hsi_corrupt); //zwalnianie pamięci
  convert_to_hsi(&hsi, &img); //do zwolnienia

  //--------Wykrywanie krawędzi (zapisywanie w RGB)----------//
  file = fopen("gen24/sobelRGB.bmp", "wb");
  copy_hsi(&hsi_edit, &hsi);
  edge_detection24(&hsi_edit, sobel_template24);
  save_hsi(&hsi_edit, file, true);
  //-------------------------------------//

  //--------Wykrywanie krawędzi (zapisywanie w GRAY)----------//
  //Trzeba wykonać trochę więcej operacji
  file = fopen("gen24/sobelGRAY.bmp", "wb");      //Plik do zapisania obrazu
  copy_hsi(&hsi_edit, &hsi);                      //Skopiowanie oryginalnego obrazu do nowej struktury
  edge_detection24(&hsi_edit, sobel_template24);  //Wykrywanie krawędzi
  convert_to_rgb(&img_edit, &hsi_edit);           //Konwersja do RGB
  rgb_to_gray(&img_edit, &new_img);               //Konwersja do GRAY
  save_img(&new_img, file, true);                 //Zapisanie obrazu
  free_img(&img_edit); free_hsi(&hsi_edit);       //Zwolnienie pamięci
  //-------------------------------------//
  //----------------------------------------------------------------/*/

  /*/-----------------------RGB 24 bit BMP---------------------------//
  //------Algorytm centroidów RGB--------//
  file = fopen("gen24/clust.bmp", "wb");
  copy_img(&img_edit, &img);
  meansrgb_t kr[10] = {
    get_pixel_info24(&img_edit, 55, 180),
    get_pixel_info24(&img_edit, 600, 180),
    get_pixel_info24(&img_edit, 444, 222),
    get_pixel_info24(&img_edit, 1, 8),
    get_pixel_info24(&img_edit, 288, 355),
    get_pixel_info24(&img_edit, 22, 355),
    get_pixel_info24(&img_edit, 288, 22),
    get_pixel_info24(&img_edit, 586, 222),
    get_pixel_info24(&img_edit, 586, 200)
  };
  k_means24_clustering(&img_edit, kr, 10);
  save_img(&img_edit, file, true);
  //-------------------------------------//

  //-------------Do szarosci-------------//
  file = fopen("gen24/avggray.bmp", "wb");
  copy_img(&img_edit, &img);
  rgb_to_gray(&img_edit, &new_img);
  save_img(&new_img, file, true);
  //-------------------------------------//

  //-----------Szary po kolorach---------//
  //Czerwony
  file = fopen("gen24/byred.bmp", "wb");
  copy_img(&img_edit, &img);
  gray_by_color(&img_edit, &new_img, RED);
  save_img(&new_img, file, true);

  //Zielony
  file = fopen("gen24/bygreen.bmp", "wb");
  copy_img(&img_edit, &img);
  gray_by_color(&img_edit, &new_img, GREEN);
  save_img(&new_img, file, true);

  //Niebieski
  file = fopen("gen24/byblue.bmp", "wb");
  copy_img(&img_edit, &img);
  gray_by_color(&img_edit, &new_img, BLUE);
  save_img(&new_img, file, true);
  //-------------------------------------//

  //-----------Jeden kolor---------------//
  //Czerwony
  file = fopen("gen24/filred.bmp", "wb");
  copy_img(&img_edit, &img);
  color_mask(&img_edit, red_mask);
  save_img(&img_edit, file, true);
  //Zielony
  file = fopen("gen24/filgreen.bmp", "wb");
  copy_img(&img_edit, &img);
  color_mask(&img_edit, green_mask);
  save_img(&img_edit, file, true);
  //Niebieski
  file = fopen("gen24/filblue.bmp", "wb");
  copy_img(&img_edit, &img);
  color_mask(&img_edit, blue_mask);
  save_img(&img_edit, file, true);
  //-------------------------------------//
  //----------------------------------------------------------------/*/

  /*/-----------------------GRAY 8 bit BMP---------------------------//
  //------Otwieranie pliku 8 bitowego---------//
  free_img(&img);
  load_img(&img, bmp_gray);
  //------------------------------------------//
  
  //-------Informacje o pliku--------//
  printf("Informacje o pliku GRAY:\n");
  printf("Rozmiar: %u bajtów\nWymiary: %ux%u px\nOffbits: %u\n ",
    img.size, img.width, img.height, img.head_size);
  printf("|----------------------------|\n");
  //---------------------------------//

  //------Algorytm centroidów--------//
  copy_img(&img_edit, &img);
  means_t k[7] = {
    get_pixel_info(&img, 12, 27),
    get_pixel_info(&img, 455, 470),
    get_pixel_info(&img, 55, 300),
    get_pixel_info(&img, 184, 98),
    get_pixel_info(&img, 450, 32),
    get_pixel_info(&img, 134, 199),
    get_pixel_info(&img, 355, 254)
  };
  k_means_clustering(&img_edit, k, 7);
  file = fopen("gen/means.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //--------------Negacja------------//
  copy_img(&img_edit, &img);
  image_negative(&img_edit);
  file = fopen("gen/neg.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//
  
  //--------Poprawianie kontrastu----//
  copy_img(&img_edit, &img);
  contrast_stretch(&img_edit);
  file = fopen("gen/contrast.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //-----------Sobel operator--------//
  copy_img(&img_edit, &img);
  edge_detection(&img_edit, sobel_template);
  file = fopen("gen/sobel.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //-----------Laplacian operator----//
  copy_img(&img_edit, &img);
  edge_detection(&img_edit, laplacian_template);
  file = fopen("gen/lap.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //-----------Prewitt operator----//
  copy_img(&img_edit, &img);
  edge_detection(&img_edit, prewitt_template);
  file = fopen("gen/pre.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //-----------Metoda Otsu-----------//
  copy_img(&img_edit, &img);
  otsumethod(&img_edit);
  file = fopen("gen/ots.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//
  
  //--------Metoda Bernsen-----------//
  copy_img(&img_edit, &img);
  bernsenmethod(&img_edit, 5);
  file = fopen("gen/ber.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //---Segmentacja rozrost obszaru---//
  //###Piksel poczštkowy i zakres ###//
  uint32_t _x = 20, _y = 450, _threshold = 10;
  copy_img(&img_edit, &img);
  growingregion(&img_edit, _x, _y, _threshold);
  file = fopen("gen/seg.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //----Dodaj filtr do obrazu--------//
  copy_img(&img_edit, &img);
  convolution_matrix(&img_edit, boxblur_template);
  file = fopen("gen/filter.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //procent uszkodzenia
  uint8_t coruptproc = 10;

  //-wypelnij szumem okreslony procent pixeli-//
  copy_img(&corupt, &img);
  add_salt(&corupt, coruptproc);
  file = fopen("gen/corupt.bmp", "wb");
  save_img(&corupt, file, false);
  fclose(file);
  printf("%d%% obrazu zostało zakłócone.\n", coruptproc);
  printf("|----------------------------|\n");
  //---------------------------------//

  //Wielkość filtra
  int filter_size = 3;

  //----odfiltruj szum minimum-------//
  copy_img(&img_edit, &corupt);
  image_filter(&img_edit, filter_size, min_template);
  file = fopen("gen/min.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //----odfiltruj szum maksimum------//
  copy_img(&img_edit, &corupt);
  image_filter(&img_edit, filter_size, max_template);
  file = fopen("gen/max.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//
  
  //----odfiltruj szum medianowo-----//
  copy_img(&img_edit, &corupt);
  image_filter(&img_edit, filter_size, median_template);
  file = fopen("gen/median.bmp", "wb");
  save_img(&img_edit, file, true);
  //---------------------------------//

  //------Tworzenie histogramu------//
  copy_img(&img_edit, &img);
  draw_histogram(&img_edit, "gen/bmpgrayhist.bmp");
  printf("Stworzono histogram.\n");
  //---------------------------------//

  //----------------------------------------------------------------/*/

  //--Zamykanie plików i zwalnianie pamięci---//
  //free_img(&img);
  //---------------------------------/*/
  return 0;
}
