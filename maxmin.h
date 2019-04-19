#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
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
}rgb_t;
#pragma pack(pop)

typedef struct fullhead_t{
  infomin_t info;
  header_t head;
  rgb_t rgb[256];
}fullhead_t;

typedef struct image_t{
  uint8_t *header;
  uint8_t *pixels;
  uint32_t head_size, size;
  int32_t width, height;
}image_t;

int comp(const void * a, const void * b) {
  return (int16_t)(*(uint8_t*)a) - (int16_t)(*(uint8_t*)b);
}
uint64_t file_size(FILE *file){
  if( file == NULL )
    { return 0; }
  fseek(file, 0, SEEK_END);
  uint64_t size = ftell(file);
  rewind(file);
  return size;
}
void free_img(image_t *img){
  if( img == NULL ) { return; }
  if( img->header != NULL )
    { free(img->header); }
  if( img->pixels != NULL )
    { free(img->pixels); }
}

bool load_img(image_t *img, FILE *bmp){
  if( bmp == NULL || img == NULL)
    { return false; }
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
  return true;
}
bool save_img(image_t *img, FILE *bmp, bool clear_mem){
  if( bmp == NULL || img == NULL )
    { return false; }
  
  fwrite(img->header, img->head_size, 1, bmp);
  fwrite(img->pixels, img->size, 1, bmp);

  if( clear_mem ){ 
    free_img(img);
    fclose(bmp);
  }
  return true;
}
bool copy_img(image_t *dest, image_t *src){
  if( dest == NULL || src == NULL ||
      src->pixels == NULL ||
      src->header == NULL
  ) { return false; }

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
  return true;
}


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
#endif
