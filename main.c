#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>

#include "maxmin.h"
#include "convolution_matrix.h"
#include "filter.h"
#include "edgedetection.h"
#include "binaryzation.h"
#include "segmentation.h"
#include "clustering.h"
#include "histogram.h"

int main() {
  srand(time(NULL));
  setlocale(LC_ALL, "pl_PL.utf8");

  char str[50] = {0};
  printf("Podaj nazwę pliku: ");
  scanf("%49s", str);

  FILE *bmp = fopen(str, "rb");
  FILE *file;

  image_t img, img_edit, corupt;
  load_img(&img, bmp);
  
  if( bmp == NULL ){
    printf("Nie można otworzyć/utworzyć plików, lub plik o danej nazwie nie istnieje!");
    return 1;
  }
  
  //---------------------------------//
  printf("Informacje o pliku:\n");
  printf("Rozmiar: %u bajtów\nWymiary: %ux%u px\nOffbits: %u\n",
    img.size, img.width, img.height, img.head_size);
  printf("|----------------------------|\n");
  //---------------------------------//
  
  //------Tworzenie histogramu------//
  copy_img(&img_edit, &img);
  draw_histogram(&img_edit);
  printf("Stworzono histogram.\n");
  //---------------------------------//

  //------Algorytm centroidów--------//
  copy_img(&img_edit, &img);
  means_t k[7] = {
    {.x = 123, .y = 12, .pix = img_edit.pixels[12*img_edit.width+123]},
    {.x = 455, .y = 470, .pix = img_edit.pixels[470*img_edit.width+455]},
    {.x = 55, .y = 300, .pix = img_edit.pixels[55*img_edit.width+300]},
    {.x = 184, .y = 98, .pix = img_edit.pixels[184*img_edit.width+98]},
    {.x = 450, .y = 32, .pix = img_edit.pixels[32*img_edit.width+450]},
    {.x = 134, .y = 199, .pix = img_edit.pixels[199*img_edit.width+134]},
    {.x = 355, .y = 253, .pix = img_edit.pixels[253*img_edit.width+355]}
  };
  k_means_clustering(&img_edit, k, 7);
  file = fopen("gen/means.bmp", "wb");
  save_img(&img_edit, file, true);

  printf("Algorytm centroidów.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //--------------Negacja------------//
  copy_img(&img_edit, &img);
  image_negative(&img_edit);
  file = fopen("gen/neg.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Negacja.\n");
  printf("|----------------------------|\n");
  //---------------------------------//
  
  //--------Poprawianie kontrastu----//
  copy_img(&img_edit, &img);
  contrast_stretch(&img_edit);
  file = fopen("gen/contrast.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Poprawa kontrastu.\n");
  printf("|----------------------------|\n");  
  //---------------------------------//

  //-----------Sobel operator--------//
  copy_img(&img_edit, &img);
  edge_detection(&img_edit, sobel_template);
  file = fopen("gen/sobel.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano Operator Sobela.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Laplacian operator----//
  copy_img(&img_edit, &img);
  edge_detection(&img_edit, laplacian_template);
  file = fopen("gen/lap.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano Operator Laplace'a.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Prewitt operator----//
  copy_img(&img_edit, &img);
  edge_detection(&img_edit, prewitt_template);
  file = fopen("gen/pre.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano Operator Prewitt.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Metoda Otsu-----------//
  copy_img(&img_edit, &img);
  otsumethod(&img_edit);
  file = fopen("gen/ots.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano Metodę Otsu.\n");
  printf("|----------------------------|\n");
  //---------------------------------//
  
  //--------Metoda Bernsen-----------//
  copy_img(&img_edit, &img);
  bernsenmethod(&img_edit, 9);
  file = fopen("gen/ber.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano Metodę Bernsen.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //---Segmentacja rozrost obszaru---//
  //###Piksel poczštkowy i zakres ###//
  uint32_t _x = 20, _y = 450, _threshold = 10;
  copy_img(&img_edit, &img);
  growingregion(&img_edit, _x, _y, _threshold);
  file = fopen("gen/seg.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano rozrost obszaru.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //----Dodaj filtr do obrazu--------//
  copy_img(&img_edit, &img);
  convolution_matrix(&img_edit, boxblur_template);
  file = fopen("gen/filter.bmp", "wb");
  save_img(&img_edit, file, true);
  printf("Zastosowano filtr wyostrzający.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //----Pobieranie % zakłóceń--------//
  printf("Podaj procent zakłóceń (0 - 100): ");
  int coruptproc = 0;
  while( scanf("%d", &coruptproc) != 1 && (coruptproc < 0 || coruptproc > 100) ){
    while( getchar()!='\n' );
    printf("Zakres od 0 - 100: ");
  }
  //---------------------------------//

  //-wypelnij szumem okreslony procent pixeli-//
  copy_img(&corupt, &img);
  add_salt(&corupt, coruptproc);
  file = fopen("gen/corupt.bmp", "wb");
  save_img(&corupt, file, false);
  fclose(file);
  printf("%d%% obrazu zostało zakłócone.\n", coruptproc);
  printf("|----------------------------|\n");
  //---------------------------------//

  //---------------------------------//
  printf("Podaj wielkość filtra (3, 5, 7, 9...): ");
  int filter_size = 0;
  while( scanf("%d", &filter_size) != 1 && (filter_size < 3 || filter_size%2 != 1) ){
    while( getchar()!='\n' );
    printf("Podaj wielkość filtra (3, 5, 7, 9...): ");
  }
  //---------------------------------//

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
  //---------------------------------/*/

  //--Zamykanie plików i zwalnianie pamięci---//
  free_img(&img);
  //---------------------------------//
  return 0;
}