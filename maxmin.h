#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>
/*/
  struktura ### image_t ###
  #struktura zawiera wszystkie informacje o pliku
  -header zawiera offbity
  -pixels zawiera bity danych
  -head_size wielkość nagłówka
  -size ilość danych

  Różne funkcje pomocnicze używane innych bibliotekach
  
  void merge_pixels(image_t *img, uint32_t mid, uint8_t *new_pixels, uint32_t size)
      Funkcja wpisuje piksele z tablicy new_pixels do tablicy pixels
      uint32_t mid - parametr określa gdzie jest "środek", czyli określa jak wielki filtr został zastosowany
        zazwyczaj przekazywany do funkcji pod nazwą filter_size, parametr jest wtedy równy mid = filter_size/2
      uint8_t *new_pixels - tablica pixeli które mają zostać wpisane do tablicy pixels[]
      uint32_t size  - rozmiar tablicy new_pixels
/*/

#ifndef __FUNCTIONS__
#define __FUNCTIONS__
#define max(a, b)(((a) > (b)) ? (a) : (b))
#define min(a, b)(((a) < (b)) ? (a) : (b))
#define swap(a,b,type){type c = a; a = b; b = c;}
#define pow2(a)((a)*(a))
#define PI 3.14159265 
#define PI2 6.283185307   /* 2PI */
#define PI2_3 2.094395102 /* 2PI/3*/
#define PI4_3 4.188790205 /* 4PI/3*/
#define PI_3 1.047197551  /* PI/3*/


//-------Nagłówek pliku bmp--------//
#pragma pack(push, 1)
typedef struct BITMAPFILEINFO {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
}infomin_t;
typedef struct BITMAPINFOHEADER {
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
}header_t;
typedef struct RGBQUAD {
  uint8_t rgbBlue;
  uint8_t rgbGreen;
  uint8_t rgbRed;
  uint8_t rgbReserved;
}rgbquad_t;
#pragma pack(pop)
typedef struct fullhead_t{
  infomin_t info;
  header_t head;
  rgbquad_t rgb[256];
}fullhead_t;
//-------------------------------//


//!!!!---Główna struktura pliku BMP-----!!!!//
//RGB/GRAY
typedef struct image_t{
  uint8_t *header;
  uint8_t *pixels;
  uint32_t head_size, size;
  int32_t width, height;
}image_t;
//HSI - convert.h - filter24.h
typedef struct imagehsi_t{
  uint8_t *header;
  double *pixels;
  uint32_t head_size, size;
  int32_t width, height;
}imagehsi_t;
//------------------------------------------//

//-------------rgbtogray.h------------------//
enum COLOR{
  BLUE,
  GREEN, 
  RED
};
// r,g,b ∈[0,255]
typedef struct rgb_t{
  uint8_t r, g, b;
}rgb_t;
// h∈[0,2PI], s∈[0,1], i∈[0,1]
typedef struct hsi_t{
  double h, s, i;
}hsi_t;
//------------------------------------------//

//---------Klasteryzacja 24 bitowa--------//
typedef struct pointrgb_t{
  int32_t r, g, b;
  int32_t index, distance;
  uint32_t pos;
}pointrgb_t;
typedef struct meansrgb_t{
  int32_t r, g, b;
  uint32_t x, y;
}meansrgb_t;
//-------------------------------//

//---------Klasteryzacja 8 bitowa--------//
typedef struct point_t{
  int16_t pix;
  int32_t index, distance;
  uint32_t pos;
}point_t;
typedef struct means_t{
  int16_t pix;
  uint32_t x, y;
}means_t;
//-------------------------------//

//----------------------------------------------------Funkcje-----------------------------------------------------//
//qsort {
int comp(const void * a, const void * b) {
  return (int16_t)(*(uint8_t*)a) - (int16_t)(*(uint8_t*)b);
}
int compar_16(const void *a, const void *b){
  int16_t _a = *(int16_t*)a, _b = *(int16_t*)b;
  if( _a == _b ) return 0;
  return _a > _b;
}
//qsort }

uint64_t file_size(FILE *file){
  if( file == NULL )
    { return 0; }
  fseek(file, 0, SEEK_END);
  uint64_t size = ftell(file);
  rewind(file);
  return size;
}

//-------------------------Wczytywanie, zapisywanie, kopiowanie pliku BMP-----------------------------//
//zwalnianie pamięci pliku bmp
void free_img(image_t *img){
  if( img->header != NULL )
    { free(img->header); }
  if( img->pixels != NULL )
    { free(img->pixels); }
}
void free_hsi(imagehsi_t *img){
  if( img->header != NULL )
    { free(img->header); }
  if( img->pixels != NULL )
    { free(img->pixels); }
}

//image_t *img - struktura do której ma zostać zapisany obraz, 
//FILE *bmp - plik z którego ma zostać wczytany obraz, plik jest zamykany po pobraniu danych
void load_img(image_t *img, FILE *bmp){
  infomin_t info;
  header_t headermin;
  fread(&info, sizeof(infomin_t), 1, bmp);
  fread(&headermin, sizeof(header_t), 1, bmp);
  rewind(bmp);
  if( headermin.biSizeImage == 0 )
    { headermin.biSizeImage = headermin.biWidth*headermin.biHeight; }

  *img = (image_t){
    (uint8_t*)malloc(info.bfOffBits),
    (uint8_t*)malloc(headermin.biSizeImage),
    info.bfOffBits,
    headermin.biSizeImage,
    headermin.biWidth,
    headermin.biHeight
  };
  fread(img->header, img->head_size, 1, bmp);
  fread(img->pixels, img->size, 1, bmp);
  fclose(bmp);
}
//bool clear_mem:
//true - jeśli pamięć obrazu ma być zwolniona, a plik zamknięty
//false - w przeciwnym przypadku
void save_img(image_t *img, FILE *bmp, bool clear_mem){
  fwrite(img->header, img->head_size, 1, bmp);
  fwrite(img->pixels, img->size, 1, bmp);

  if( clear_mem ){ 
    free_img(img);
    fclose(bmp);
  }
}
void copy_img(image_t *dest, image_t *src){
  *dest = (image_t){
    (uint8_t*)malloc(src->head_size),
    (uint8_t*)malloc(src->size),
    src->head_size, 
    src->size,
    src->width,
    src->height
  };
  memcpy(dest->header, src->header, src->head_size);
  memcpy(dest->pixels, src->pixels, src->size);
}
//----------------------------------------//

//-------------------Scalanie pikseli nowej------------------//

//Scalanie pikseli w obrazach 8-bitowych,
//uint32_t mid - wielkość filtra podzielona przez 2, 
//uint8_t *new_pixels - tablica nowych pikseli, 
//uint32_t size - wielkość tablicy nowych pikseli
void merge_pixels(image_t *img, uint32_t mid, uint8_t *new_pixels, uint32_t size){
  uint32_t p = 0, x, y;
  uint32_t new_height = img->height - mid;
  uint32_t new_width = img->width - mid;
  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      if( p >= size ) return;
      img->pixels[y*img->width + x] = new_pixels[p++];
    }
  }
}
//Scalanie pikseli w obrazach HSI 24-bitowych
//uint32_t mid - (wielokść_filtra / 2) * 3
void merge_pixels24(imagehsi_t *img, uint32_t mid, double *new_pixels, uint32_t size){
  uint32_t p = 0, x, y;
  uint32_t new_height = (img->height - mid) * 3;
  uint32_t new_width = (img->width - mid) * 3;
  for (y = mid; y < new_height; y += 3) {
    for (x = mid; x < new_width; x += 3) {
      if( p >= size ) return;
      img->pixels[y*img->width + x] = new_pixels[p++];
    }
  }
}
//-------------------------------------------------------------//
#endif

