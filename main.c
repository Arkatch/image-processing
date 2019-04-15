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

#pragma pack(push, 1)
struct BITMAPFILEINFO {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};
struct BITMAPINFOHEADER {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};
struct RGBQUAD {
  uint8_t rgbBlue;
  uint8_t rgbGreen;
  uint8_t rgbRed;
  uint8_t rgbReserved;
};
#pragma pack(pop)

//tablica pikseli parametr +
//losowe wartosci 0 - 255 szum sól i pieprz +
//procentowe zaburzanie +
//odfiltrowanie obrazu filtrem medianowym +
//filtracja splotowa (kernel image processing) +
//image segmentation +
//sobel operator +

int main() {
  srand(time(NULL));
  setlocale(LC_ALL, "pl_PL.utf8");

  char str[50]={0};
  printf("Podaj nazwę pliku: ");
  scanf("%49s", str);

  FILE * bmp = fopen(str, "rb");
  FILE * bmp_corupted = fopen("corupted.bmp", "wb");
  FILE * bmp_filter = fopen("filter.bmp", "wb");
  FILE *bmp_kernel = fopen("kernel.bmp", "wb");
  FILE *bmp_sobel = fopen("sobel.bmp", "wb");
  FILE *bmp_laplacian = fopen("laplacian.bmp", "wb");
  FILE *bmp_prewitt = fopen("prewitt.bmp", "wb");
  FILE *bmp_min = fopen("min.bmp", "wb");
  FILE *bmp_max = fopen("max.bmp", "wb");
  FILE *bmp_otsu = fopen("otsu.bmp", "wb");
  FILE *bmp_segment = fopen("segment.bmp", "wb");
  FILE *bmp_bernsen = fopen("bernsen.bmp", "wb");

  if( bmp == NULL || bmp_corupted == NULL || bmp_filter == NULL ){
    printf("Nie można otworzyć/utworzyć plików, lub plik o danej nazwie nie istnieje!");
    return 1;
  }

  //--Struktury na dane i odczytywanie informacji o pliku--//
  struct BITMAPFILEINFO info;
  struct BITMAPINFOHEADER header;
  struct RGBQUAD rgb[256]; //AF

  fread(&info, sizeof(info), 1, bmp);
  fread(&header, sizeof(header), 1, bmp);
  fread(rgb, sizeof(struct RGBQUAD), 256, bmp); //AF
  //---------------------------------//

  //--Przewijanie pliku na poczštek i pobranie  całego nagłówka od poczštku--//
  fseek(bmp, 0L,  SEEK_SET);
  uint32_t size_header = info.bfOffBits;
  uint8_t *main_header = malloc(size_header);
  fread(main_header, size_header, 1, bmp);
  //---------------------------------//

  //--Tablica [y*x] dane o wartosci poszczegolnych pikseli--//
  uint8_t *pixels_orygin = malloc( header.biSizeImage );
  uint8_t *pixels_kernel = malloc( header.biSizeImage );
  uint8_t *pixels_corupt = malloc( header.biSizeImage );
  uint8_t *pixels_median = malloc( header.biSizeImage );
  uint8_t *pixels_sobelo = malloc( header.biSizeImage );
  uint8_t *pixels_laplac = malloc( header.biSizeImage );
  uint8_t *pixels_prewit = malloc( header.biSizeImage );
  uint8_t *pixels_min = malloc( header.biSizeImage );
  uint8_t *pixels_max = malloc( header.biSizeImage );
  uint8_t *pixels_otsu = malloc( header.biSizeImage );
  uint8_t *pixels_segment = malloc( header.biSizeImage );
  uint8_t *pixels_bernsen = malloc( header.biSizeImage );
  //---------------------------------//

  //--wczytywanie i kopiowanie danych--//
  fread(pixels_orygin, header.biSizeImage, 1, bmp);
  memcpy(pixels_kernel, pixels_orygin, header.biSizeImage);
  memcpy(pixels_corupt, pixels_orygin, header.biSizeImage);
  memcpy(pixels_sobelo, pixels_orygin, header.biSizeImage);
  memcpy(pixels_laplac, pixels_orygin, header.biSizeImage);
  memcpy(pixels_prewit, pixels_orygin, header.biSizeImage);
  memcpy(pixels_otsu, pixels_orygin, header.biSizeImage);
  memcpy(pixels_segment, pixels_orygin, header.biSizeImage);
  memcpy(pixels_bernsen, pixels_orygin, header.biSizeImage);
  //---------------------------------//

  //--Dodawanie nagłówków do plików--//
  fwrite(main_header, size_header, 1, bmp_corupted);
  fwrite(main_header, size_header, 1, bmp_sobel);
  fwrite(main_header, size_header, 1, bmp_laplacian);
  fwrite(main_header, size_header, 1, bmp_prewitt);
  fwrite(main_header, size_header, 1, bmp_kernel);
  fwrite(main_header, size_header, 1, bmp_min);
  fwrite(main_header, size_header, 1, bmp_max);
  fwrite(main_header, size_header, 1, bmp_filter);
  fwrite(main_header, size_header, 1, bmp_otsu);
  fwrite(main_header, size_header, 1, bmp_segment);
  fwrite(main_header, size_header, 1, bmp_bernsen);
  //---------------------------------//

  /*/--Dodawanie LUT do plików--//
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_corupted); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_sobel); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_laplacian); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_prewitt);//AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_kernel); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_min); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_max); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_filter); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_otsu); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_segment); //AF
  fwrite(rgb, sizeof(struct RGBQUAD), 256, bmp_bernsen); //AF
  //---------------------------------/*/
  
  //---------------------------------//
  printf("Informacje o pliku:\n");
  printf("Rozmiar: %u bajtów\nWymiary: %ux%u px\nTyp: %u\nOffbits: %u\n",
    header.biSizeImage, header.biWidth, header.biHeight, info.bfType, info.bfOffBits);
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Sobel operator--------//
  edge_detection(pixels_sobelo, header.biWidth, header.biHeight, sobel_template);
  fwrite(pixels_sobelo, header.biSizeImage, 1, bmp_sobel);
  printf("Zastosowano Operator Sobela.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Laplacian operator----//
  edge_detection(pixels_laplac, header.biWidth, header.biHeight, laplacian_template);
  fwrite(pixels_laplac, header.biSizeImage, 1, bmp_laplacian);
  printf("Zastosowano Operator Laplace'a.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Prewitt operator----//tt
  edge_detection(pixels_prewit, header.biWidth, header.biHeight, prewitt_template);
  fwrite(pixels_prewit, header.biSizeImage, 1, bmp_prewitt);
  printf("Zastosowano Operator Prewitt.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //-----------Metoda Otsu-----------//
  otsumethod(pixels_otsu, header.biWidth, header.biHeight);
  fwrite(pixels_otsu, header.biSizeImage, 1, bmp_otsu);
  printf("Zastosowano Metodę Otsu.\n");
  printf("|----------------------------|\n");
  //---------------------------------//
  
  //--------Metoda Bernsen-----------//
  bernsenmethod(pixels_bernsen, header.biWidth, header.biHeight, 9);
  fwrite(pixels_bernsen, header.biSizeImage, 1, bmp_bernsen);
  printf("Zastosowano Metodę Bernsen.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //---Segmentacja rozrost obszaru---//
  //###Piksel poczštkowy i zakres ###//
  uint32_t _x = 467, _y = 513, _threshold = 15;
  growingregion(pixels_segment, header.biWidth, header.biHeight, _x, _y, _threshold);
  fwrite(pixels_segment, header.biSizeImage, 1, bmp_segment);
  printf("Zastosowano rozrost obszaru.\n");
  printf("|----------------------------|\n");
  //---------------------------------//

  //----Dodaj filtr do obrazu--------//
  convolution_matrix(pixels_kernel, header.biWidth, header.biHeight, boxblur_template);
  printf("Zastosowano filtr wyostrzający.\n");
  fwrite(pixels_kernel, header.biSizeImage, 1, bmp_kernel);
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
  add_salt(pixels_corupt, header.biWidth, header.biHeight, coruptproc);
  fwrite(pixels_corupt, header.biSizeImage, 1, bmp_corupted);
  printf("%d%% obrazu zostało zakłócone.\n", coruptproc);
  printf("|----------------------------|\n");
  //---------------------------------//

  //---------------------------------//
  printf("Podaj wielkoć filtra (3, 5, 7, 9...): ");
  int filter_size = 0;
  while( scanf("%d", &filter_size) != 1 && (filter_size < 3 || filter_size%2 != 1) ){
    while( getchar()!='\n' );
    printf("Podaj wielkość filtra (3, 5, 7, 9...): ");
  }
  //---------------------------------//

  //----odfiltruj szum minimum-------//
  memcpy(pixels_min, pixels_corupt, header.biSizeImage);
  image_filter(pixels_min, header.biWidth, header.biHeight, filter_size, min_template);
  fwrite(pixels_min, header.biSizeImage, 1, bmp_min);
  //---------------------------------//

  //----odfiltruj szum maksimum------//
  memcpy(pixels_max, pixels_corupt, header.biSizeImage);
  image_filter(pixels_max, header.biWidth, header.biHeight, filter_size, max_template);
  fwrite(pixels_max, header.biSizeImage, 1, bmp_max);
  //---------------------------------//
  
  //----odfiltruj szum medianowo-----//
  memcpy(pixels_median, pixels_corupt, header.biSizeImage);
  image_filter(pixels_median, header.biHeight, header.biWidth, filter_size, median_template);
  fwrite(pixels_median, header.biSizeImage, 1, bmp_filter);
  printf("Obraz został odfiltrowany filtrem o rozmiarze %dx%d\n", filter_size, filter_size);
  //---------------------------------//

  //--Zamykanie plików i zwalnianie pamięci---//
  free(pixels_orygin);
  free(pixels_corupt);
  free(pixels_kernel);
  free(pixels_median);
  free(main_header);
  free(pixels_sobelo);
  free(pixels_laplac);
  free(pixels_min);
  free(pixels_max);
  free(pixels_otsu);
  free(pixels_segment);
  free(pixels_prewit);
  free(pixels_bernsen);

  fclose(bmp);
  fclose(bmp_otsu);
  fclose(bmp_segment);
  fclose(bmp_laplacian);
  fclose(bmp_prewitt);
  fclose(bmp_kernel);
  fclose(bmp_corupted);
  fclose(bmp_filter);
  fclose(bmp_sobel);
  fclose(bmp_bernsen);
  fclose(bmp_min);
  fclose(bmp_max);
  //---------------------------------//
  return 0;
}
