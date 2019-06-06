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
#include "queue3d.h"


#ifndef __SEGMENTATION3D__
#define __SEGMENTATION3D__
#define FLOAT 1
#define SIGNED16 2
#define UNSIGNED8 3
#define __RAW16MIN__ -1024
#define __RAW16MAX__ 3071
#define __RAW8MIN__ 0
#define __RAW8MAX__ 255
#define __RAW16FLAG__ 3072
//x,y,z - corrdinate of pixel, name - name of imageraw_t structure
#define INDEX(x,y,z,name) (((z)*(name->width)*(name->height)) + ((y) * (name->width)) + (x))
#define HITARRAYGEN(BOXSIZE, __ARRNAME__, __VALUE__)\
  int16_t __ARRNAME__[BOXSIZE * BOXSIZE];\
  memset(__ARRNAME__, __INT16_MAX__, BOXSIZE*BOXSIZE*sizeof(int16_t));\
  do{\
    for(int __i = 0; __i < BOXSIZE; ++__i){\
      for(int __j = 0; __j < BOXSIZE; ++__j){\
        if(__i == 0 || __i == BOXSIZE - 1)\
          __ARRNAME__[__i*BOXSIZE + __j] = __VALUE__;\
        else if(__j == 0 || __j == BOXSIZE - 1)\
          __ARRNAME__[__i*BOXSIZE + __j] = __VALUE__;\
      }\
    }\
  }while(0)

#define isN(img, x, y, z)((img->pixels[INDEX(x, y-1, z, img)]))
#define isS(img, x, y, z)((img->pixels[INDEX(x, y+1, z, img)]))
#define isW(img, x, y, z)((img->pixels[INDEX(x-1, y, z, img)]))
#define isE(img, x, y, z)((img->pixels[INDEX(x+1, y, z, img)]))
#define isU(img, x, y, z)((img->pixels[INDEX(x, y, z+1, img)]))
#define isB(img, x, y, z)((img->pixels[INDEX(x, y, z-1, img)]))

typedef struct imageraw16_t{
  int16_t *pixels;
  int32_t width, height, length;
  int32_t size;
}imageraw16_t;

typedef struct imageraw8_t{
  uint8_t *pixels;
  int32_t width, height, length;
  int32_t size;
}imageraw8_t;

typedef struct imageraw32f_t{
  float *pixels;
  int32_t width, height, length;
  int32_t size;
}imageraw32f_t;

typedef struct mhdheader_t{
  int32_t width, height, length;
  char *elem_type, *filename;
}mhdheader_t;

typedef struct xyz_t{
  int32_t x,y,z;
}xyz_t;

void free_raw32(imageraw32f_t *img){
  if(img == NULL || img->pixels == NULL) return;
  free(img->pixels);
}
//--------------------RAW HEADER------------------------//
void free_mhd(mhdheader_t *header){
  free(header->filename);
  free(header->elem_type);
  header->filename = NULL;
  header->elem_type = NULL;
}
mhdheader_t load_mhd(const char *fileinfoname){
  if( fileinfoname == NULL ) exit(1);
  //Load *.mhd file header
  FILE *file = fopen(fileinfoname, "rb");
  if( file == NULL ) exit(2);
  int fsize = file_size(file);
  char *info = (char*)calloc(fsize+1, sizeof(char)), *search = NULL;
  fread(info, 1, fsize, file);
  fclose(file);

  //Load file *.raw file information from *.mhd file
  int32_t width, height, length;
  search = strstr(info, "DimSize = ") + strlen("DimSize = ");
  if( search == NULL || sscanf(search, "%d %d %d", &width, &height, &length) != 3 ) {
    free(info); 
    exit(3);
  }

  //Load *.raw file name
  search = strstr(info, "ElementDataFile = ") + strlen("ElementDataFile = ");
  char *path = strrchr(fileinfoname, '/');
  int pathsize = (path != NULL) ? (path - fileinfoname + 1) : 0;
  char *tmp = (char *)calloc(fsize - (search - info) + 1, sizeof(char)); 
  char *filename = (char *)calloc(fsize - (search - info) + 1 + pathsize, sizeof(char));
  memcpy(filename, fileinfoname, pathsize);
  if( search == NULL || sscanf(search, "%s", tmp) != 1 ) {
    free(info); free(filename); 
    exit(4);
  }
  strcat(filename, tmp);
  free(tmp);

  //Load *.raw image type 
  search = strstr(info, "ElementType = ") + strlen("ElementType = ");
  char *elem_type = (char *)calloc(fsize - (search - info) + 1, sizeof(char));
  if( search == NULL || sscanf(search, "%s", elem_type) != 1 ) {
    free(info); free(filename); free(elem_type); 
    exit(5);
  }
  free(info);
  return (mhdheader_t){width, height, length, elem_type, filename};
}
//----------------------------------------------------//

//--------------------RAW 16bit-----------------------//
void free_raw16(imageraw16_t *img){
  if(img == NULL || img->pixels == NULL) return;
  free(img->pixels);
  img->pixels = NULL;
}
void copy_raw16(imageraw16_t *dest, imageraw16_t *src){
  if( dest->pixels != NULL )
    free_raw16(dest);
  dest->pixels = (int16_t*)calloc(src->size, sizeof(int16_t));
  if( dest->pixels == NULL ){
    free_raw16(src);
    exit(EXIT_FAILURE);
  }
  memcpy(dest->pixels, src->pixels, src->size*sizeof(int16_t));
  dest->width = src->width;
  dest->height = src->height;
  dest->length = src->length;
  dest->size = src->size;
}
//#fileinfoname# - nazwa nagłówka *.mhd pliku *.raw
int load_raw16(imageraw16_t *img, const char *fileinfoname){
  if( fileinfoname == NULL || img == NULL ) exit(2);
  
  //Load fileinfo
  mhdheader_t header = load_mhd(fileinfoname);
  int width = header.width, height=header.height, length=header.length;
  //Checking type of element
  if(strcmp("MET_SHORT", header.elem_type) != 0) { 
    free_mhd(&header); 
    exit(EXIT_FAILURE); 
  }
  img->width = width; img->height = height; img->length = length;
  img->size = width * height * length;

  //Alocation memory for image
  img->pixels = (int16_t*)calloc(length*height*width, sizeof(int16_t));
  if( img->pixels == NULL ) { 
    free_mhd(&header); 
    exit(EXIT_FAILURE); 
  }
  //Load image data to structure from *.raw file
  FILE *file = fopen(header.filename, "rb");
  if( file == NULL ){ 
    free_raw16(img); 
    free_mhd(&header); 
    exit(EXIT_FAILURE);
  }
  fread(img->pixels, sizeof(int16_t), width*height*length, file);
  fclose(file);
  free_mhd(&header);
  return 0;
}
int save_raw16(imageraw16_t *img, const char *filename, bool clear){
  FILE *file = fopen(filename, "wb");
  if(file == NULL){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  fwrite(img->pixels, sizeof(int16_t), img->size, file);
  fclose(file);
  if(clear) free_raw16(img);
  return 0;
}
//----------------------------------------------------//

//---------------------RAW 8bit-----------------------//
void free_raw8(imageraw8_t *img){
  if(img == NULL || img->pixels == NULL) return;
  free(img->pixels);
  img->pixels = NULL;
}
void copy_raw8(imageraw8_t *dest, imageraw8_t *src){
  if( dest->pixels != NULL )
    free_raw8(dest);
  dest->pixels = (uint8_t*)calloc(src->size, sizeof(uint8_t));
  if( dest->pixels == NULL ){
    free_raw8(src);
    exit(EXIT_FAILURE);
  }
  memcpy(dest->pixels, src->pixels, src->size*sizeof(uint8_t));
  dest->width = src->width;
  dest->height = src->height;
  dest->length = src->length;
  dest->size = src->size;
}
int load_raw8(imageraw8_t *img, const char *fileinfoname){
  if( fileinfoname == NULL || img == NULL ) exit(1);
  
  //Load fileinfo
  mhdheader_t header = load_mhd(fileinfoname);
  int width = header.width, height=header.height, length=header.length;
  //Checking type of element
  if(strcmp("MET_UCHAR", header.elem_type) != 0) { 
    free_mhd(&header); 
    exit(EXIT_FAILURE); 
  }

  img->width = width; img->height = height; img->length = length;
  img->size = width * height * length * sizeof(uint8_t);

  //Alocation memory for image
  img->pixels = (uint8_t*)calloc(length*height*width, sizeof(uint8_t));
  if( img->pixels == NULL ) { 
    free_mhd(&header); 
    exit(EXIT_FAILURE); 
  }

  //Load image data to structure from *.raw file
  FILE *file = fopen(header.filename, "rb");
  if( file == NULL ){ 
    free_raw8(img); 
    free_mhd(&header); 
    exit(EXIT_FAILURE);
  }
  fread(img->pixels, sizeof(uint8_t), width*height*length, file);
  fclose(file);
  free_mhd(&header);
  return 0;
}
int save_raw8(imageraw8_t *img, const char *filename, bool clear){
  FILE *file = fopen(filename, "wb");
  if(file == NULL){
    free_raw8(img);
    exit(EXIT_FAILURE);
  }
  fwrite(img->pixels, sizeof(uint8_t), img->size, file);
  fclose(file);
  if(clear) { free_raw8(img); }
  return 0;
}

void contrast_stretchraw8(imageraw8_t *img, int32_t range_min, int32_t rande_max){
  float p, size = img->width * img->height;
  int16_t a = range_min, b = rande_max, r = b - a;
  int32_t x, y, z, i, index, histogram[256];
  for(z = 0; z < img->length; ++z)
  {
    for(y = 0; y < img->height; ++y)
    for(x = 0; x < img->width; ++x){
      index = INDEX(x,y,z,img);
      p = (img->pixels[index] - a) / (float)r * 255;
      if(p < 0) p = 0;
      if(p > 255) p = 255;
      img->pixels[index] = p;
    }
  }
}
//----------------------------------------------------//

//--------------------Convert-------------------------//
void convertraw_16_8(imageraw8_t *dest, imageraw16_t *src, bool clear){
  dest->pixels = (uint8_t*)calloc(src->size, sizeof(uint8_t));
  if( dest->pixels == NULL ){
    free_raw16(src);
    exit(EXIT_FAILURE);
  }
  dest->width = src->width;
  dest->height = src->height;
  dest->length = src->length;
  dest->size = src->size;
  //Convert (-1024;3071) to (0;255) with right shift
  for(int32_t i = 0; i < src->size; ++i)
    dest->pixels[i] = (src->pixels[i] - __RAW16MIN__) >> 4;
  if(clear) { free_raw16(src); }
}
//----------------------------------------------------//

//Zamień kolor piksela #color_to_set# na kolor #color#
void setcolor(imageraw16_t *img, int16_t color_to_set, int16_t color){
  if(color_to_set < __RAW16MIN__ || color_to_set > __RAW16MAX__) exit(EXIT_FAILURE);
  if(color < __RAW16MIN__ || color > __RAW16MAX__) exit(EXIT_FAILURE);
  for(int i = 0; i < img->size; ++i)
    if( img->pixels[i] == color_to_set )
      img->pixels[i] = color;

}
//Zamień wszystkie piksele mniejsze od #threshold# na __RAW16MIN__
void binraw16_down(imageraw16_t *img, int16_t threshold){
  if(threshold < __RAW16MIN__ || threshold > __RAW16MAX__) exit(EXIT_FAILURE);
  for(int i = 0; i < img->size; ++i)
    if(img->pixels[i] < threshold) 
       img->pixels[i] = __RAW16MIN__;
}
//Zamień wszystkie piksele większe od #threshold# na __RAW16MAX__
void binraw16_up(imageraw16_t *img, int16_t threshold){
  if(threshold < __RAW16MIN__ || threshold > __RAW16MAX__) exit(EXIT_FAILURE);
  for(int i = 0; i < img->size; ++i)
    if(img->pixels[i] > threshold)
      img->pixels[i] = __RAW16MAX__;
}
//Binaryzacja obrazów *.raw według ustalonego progu #threshold#
void binraw16(imageraw16_t *img, int16_t threshold){
  if(threshold < __RAW16MIN__ || threshold > __RAW16MAX__) exit(EXIT_FAILURE);
  for(int i = 0; i < img->size; ++i)
    img->pixels[i] = (img->pixels[i] > threshold) ? __RAW16MAX__ : __RAW16MIN__;
}
//Wszystkie wartości które == __RAW16FLAG__ zmień na __RAW16MAX, inne zamimeń na __RAW16MIN__
void binraw16_flag(imageraw16_t *img){
  for(int i = 0; i < img->size; ++i)
    img->pixels[i] = (img->pixels[i] == __RAW16FLAG__) ? __RAW16MAX__ :__RAW16MIN__;
}

void binraw8_flag(imageraw8_t *img, uint8_t flag){
  for(int i = 0; i < img->size; ++i)
    img->pixels[i] = (img->pixels[i] == flag) ? flag : 0;
}


int16_t test_mask(int16_t *values, int16_t oryginal_value){
  //sobel operator 3d
  int16_t mask[] = {
    -2, 0, 2, -4, 0, 4, -2, 0, -2,
    -4, 0, 2, -8, 0, 4, -4, 0, 2,
    -2, 0, 2, -4, 0, 4, -2, 0, 2
  };
  int bit_value = 0, i, j, m = 0;
  for(i = 0; i < 27; ++i)
    bit_value += values[i]*mask[i]; 
  
  return (bit_value < __RAW16MIN__ || bit_value > __RAW16MAX__) ? oryginal_value : bit_value;
}


void conv_16(imageraw16_t *img, int16_t (*mask_type)(int16_t*, int16_t)){
  time_t c = clock();
  int16_t *new_pixels = (int16_t*)calloc(img->size, sizeof(int16_t));
  if( new_pixels == NULL ){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels, img->size*sizeof(int16_t));

  int16_t values[27];
  int32_t x, y, z, i, j, k, m = 0, index;
  int32_t height = img->height - 1, width = img->width - 1, length = img->length - 1;

  for (z = 1; z < length; ++z){
    for (y = 1; y < height; ++y){
      for (x = 1; x < width; ++x){
        m = 0;
        for(i = -1; i <= 1; ++i)
        for(j = -1; j <= 1; ++j)
        for(k = -1; k <= 1; ++k)
          values[m++] = img->pixels[INDEX(x+k, y+j, z+i, img)];
        index = INDEX(x, y, z, img);
        new_pixels[index] = (*mask_type)(values, img->pixels[index]);
      }
    }
  }
  memcpy(img->pixels, new_pixels, img->size*sizeof(int16_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}

/*
  Funkcja sprawdza sąsiadów piksela w wymiarze Z, 
  jeżeli piksel ma inną wartość niż __RAW16MIN__, 
  oraz conajmniej jeden z sąsiadów tego piksela ma wartość __RAW16MIN__, 
  to zamień tego piskela w __RAW16MIN__
*/
void test_16(imageraw16_t *img, int32_t depth){
  time_t c = clock(); 
  int16_t *new_pixels = (int16_t*)calloc(img->size, sizeof(int16_t));
  if( new_pixels == NULL ){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels, img->size*sizeof(int16_t));
  int32_t x, y, z, i, index;
  int32_t height = img->height, width = img->width, length = img->length - depth;
  for (z = depth; z < length; ++z)
    for (y = 0; y < height; ++y)
      for (x = 0; x < width; ++x){
        index = INDEX(x, y, z, img);
        if( img->pixels[index] != __RAW16MIN__ ){
          for(i = -depth; i <= depth; ++i){ 
            if(img->pixels[INDEX(x, y, z+i, img)] == __RAW16MIN__ ){
              new_pixels[index] = __RAW16MIN__;
              break;
            }
          }
        }
      }
  memcpy(img->pixels, new_pixels, img->size*sizeof(int16_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}
void test_5_16(imageraw16_t *img, int32_t depth){
  time_t c = clock(); 
  int16_t *new_pixels = (int16_t*)calloc(img->size, sizeof(int16_t));
  if( new_pixels == NULL ){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels, img->size*sizeof(int16_t));
  int32_t x, y, z, i, index, _min;
  int32_t height = img->height, width = img->width, length = img->length - depth;
  for (z = 0; z < length; ++z)
  {
    for (y = 0; y < height; ++y)
    for (x = 0; x < width; ++x){
      _min = __INT16_MAX__;
      for(i = 0; i < depth; ++i)
        _min = min(_min, img->pixels[INDEX(x,y,z+i,img)]);
      for(i = 0; i < depth; ++i)
        new_pixels[INDEX(x,y,z+i,img)] = _min;
    }
  }
  memcpy(img->pixels, new_pixels, img->size*sizeof(int16_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}

/*
  Funkcja sprawdza czy w sześcianie o rozmiarze blocksize x blocksize x blocksize,
  jest najwięcej pikseli __RAW16MIN__, 
  jeżeli tak, to wypełnij cały sześcian wartościami __RAW16MIN__
*/
void test_2_16(imageraw16_t *img, int32_t blocksize){
  time_t c = clock(); 
  int16_t *new_pixels = (int16_t*)calloc(img->size, sizeof(int16_t));
  if( new_pixels == NULL ){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels, img->size*sizeof(int16_t));
  int32_t x, y, z, i, j, k, index;
  int32_t blc = blocksize/2, minvalues, cmpsize = (blocksize*blocksize*blocksize)/2;
  int32_t height = img->height - blc;
  int32_t width = img->width - blc; 
  int32_t length = img->length - blc;

  for (z = blc; z < length; ++z)
    for (y = blc; y < height; ++y)
      for (x = blc; x < width; ++x){
        minvalues = 0;
        for(i = -blc; i <= blc; ++i)
        for(j = -blc; j <= blc; ++j)
        for(k = -blc; k <= blc; ++k)
          if( img->pixels[INDEX(x+k, y+j, z+i, img)] == __RAW16MIN__ )
            ++minvalues;
        if( minvalues >= cmpsize )
          for(i = -blc; i <= blc; ++i)
          for(j = -blc; j <= blc; ++j)
          for(k = -blc; k <= blc; ++k)
            new_pixels[INDEX(x+k, y+j, z+i, img)] = __RAW16MIN__;
      }
  memcpy(img->pixels, new_pixels, img->size*sizeof(int16_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}

/*
  funkcja zamienia wyszystkie piksele na tło, jeżeli obramowanie 
  pikseli w tablicy o rozmiarze boxsize x boxsize jest równe kolorowi background
  Można to porównać do hitormiss
  np:
  backgroud == 0
  |0|0|0|
  |0|1|0|
  |0|0|0| Obramowanie piksela == background, więc całą ramkę wypełniamy 0

  |0|0|0|0|0|
  |0|1|0|0|0|
  |0|1|1|0|0|
  |0|1|1|1|0|
  |0|0|0|0|0| W takim przypadku też środek zostaje wypełniony 0

*/
void test_4_16(imageraw16_t *img, int32_t boxsize, int16_t backgroud){
  time_t c = clock(); 
  HITARRAYGEN(boxsize, hitmiss, backgroud);

  int32_t x, y, z, i, j, k, hit, mid = boxsize/2;
  int32_t hitmax = boxsize * 4 - 4;
  int32_t height = img->height - mid, width = img->width - mid, length = img->length;

  for (z = 0; z < length; ++z)
  {
    for (y = mid; y < height; ++y)
    for (x = mid; x < width; ++x)
    {
      hit = 0; k = 0;
      for(i = -mid; i <= mid; ++i)
      for(j = -mid; j <= mid; ++j)
        if( hitmiss[k++] == img->pixels[INDEX(x+j, y+i, z, img)] )
          ++hit;
      
      if(hit == hitmax)
      for(i = -mid; i <= mid; ++i)
      for(j = -mid; j <= mid; ++j)
        img->pixels[INDEX(x+j, y+i, z, img)] = backgroud;
    }
  }
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}

//-----------------------FILTERS 3d-----------------------//
int16_t medianraw16_3d(int16_t *pixels, int32_t size){
  qsort(pixels, size, sizeof(int16_t), compar_16);
  return pixels[size/2];
}
int16_t minraw16_3d(int16_t *pixels, int32_t size){
  int16_t _min = *pixels;
  for(int32_t i = 1; i < size; ++i)
    _min = min(_min, pixels[i]);
  return _min;
}
void filterraw16_3d(imageraw16_t *img, int32_t blocksize, int16_t(*filtr_type)(int16_t*, int32_t)){
  time_t c = clock(); 
  int16_t *new_pixels = (int16_t*)calloc(img->size, sizeof(int16_t));
  if( new_pixels == NULL ){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels, img->size*sizeof(int16_t));
  int32_t x, y, z, i, j, k, index;
  int32_t mid = blocksize/2, cmpsize = (blocksize*blocksize*blocksize);
  int32_t height = img->height - mid;
  int32_t width = img->width - mid; 
  int32_t length = img->length - mid;
  int16_t pixels[cmpsize];

  for(z = 0; z < mid; ++z)
  for(y = mid; y < height; ++y)
  for(x = mid; x < width; ++x){
    index = 0;
    for(i = 0; i < blocksize; ++i)
    for(j = -mid; j <= mid; ++j)
    for(k = -mid; k <= mid; ++k)
      pixels[index++] = img->pixels[INDEX(x+k, y+j, z+i, img)];
    new_pixels[INDEX(x-mid,y-mid,z,img)] = (*filtr_type)(pixels, cmpsize);
  }

  for (z = mid; z < length; ++z)
  for (y = mid; y < height; ++y)
  for (x = mid; x < width; ++x){
    index = 0;
    for(i = -mid; i <= mid; ++i)
    for(j = -mid; j <= mid; ++j)
    for(k = -mid; k <= mid; ++k)
      pixels[index++] = img->pixels[INDEX(x+k, y+j, z+i, img)];
    new_pixels[INDEX(x,y,z,img)] = (*filtr_type)(pixels, cmpsize);
  }

  for(z = img->length - 1; z > img->length - 1 - mid; --z)
  for(y = mid; y < height; ++y)
  for(x = mid; x < width; ++x){
    index = 0;
    for(i = 0; i < blocksize; ++i)
    for(j = -mid; j <= mid; ++j)
    for(k = -mid; k <= mid; ++k)
      pixels[index++] = img->pixels[INDEX(x+k, y+j, z-i, img)];
    new_pixels[INDEX(x-mid,y-mid,z,img)] = (*filtr_type)(pixels, cmpsize);
  }
  memcpy(img->pixels, new_pixels, img->size*sizeof(int16_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}
//--------------------------------------------------------//

//-----------------------FILTERS--------------------------//
int16_t avg_templateraw16(int16_t *values, int32_t sqrsize){
  int sum = 0;
  for(int i = 0; i < sqrsize; ++i)
    sum += values[i];
  return sum/sqrsize;
}
int16_t median_templateraw16(int16_t *values, int32_t sqrsize){
  qsort(values, sqrsize, sizeof(int16_t), compar_16);
  return values[sqrsize/2];
}
//frame >= 0, użyj filtra na obrazie o numerze frame
//frame < 0, użyj filtra na wszystkich obrazach
void filterraw16(imageraw16_t *img, int32_t size, int32_t frame, int16_t (*filtr_type)(int16_t*, int32_t)){
  time_t c = clock();
  int32_t sqrsize = size*size;
  int32_t x, y, z, i, j, k, h, index;
  int16_t *new_pixels, pixels[sqrsize];
  int32_t mid = size/2, length = img->length;
  int32_t height = img->height - mid, width = img->width - mid;
  int32_t frame_size = (frame >= 0) ? img->height*img->width : img->size;
  if(frame >= 0){
    length = frame + 1;
    index = INDEX(0, 0, frame, img);
    new_pixels = (int16_t*)calloc(frame_size, sizeof(int16_t));
  }
  else{
    frame = 0;
    index = 0;
    new_pixels = (int16_t*)calloc(frame_size, sizeof(int16_t));
  }
  if( new_pixels == NULL ){
    free_raw16(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels+index, frame_size*sizeof(int16_t));

  for (z = frame, h = 0; z < length; ++z, ++h)
  {
    for (y = mid; y < height; ++y)
    for (x = mid; x < width; ++x)
    {
      k = 0;
      for(i = -mid; i <= mid; ++i)
      for(j = -mid; j <= mid; ++j)
        pixels[k++] = img->pixels[INDEX(x+j, y+i, z, img)];
      new_pixels[INDEX(x, y, h, img)] = filtr_type(pixels, sqrsize);
    }
  }
  memcpy(img->pixels+index, new_pixels, frame_size*sizeof(int16_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}

float growingregionraw16_3d(imageraw16_t *img, int32_t px, int32_t py, int32_t pz, int16_t threshold, int16_t color){
  time_t c = clock(); 
  int16_t t_min = img->pixels[INDEX(px, py, pz, img)]-threshold;
  int16_t t_max = img->pixels[INDEX(px, py, pz, img)]+threshold;
  queue3d new_queue;
  int32_t x, y, z, pos;
  queue_start3d(&new_queue);
  enqueue3d(&new_queue, px, py, pz);
  while( is_empty_queue3d(&new_queue) == false )
  {
    dequeue3d(&new_queue, &x, &y, &z);
    pos = INDEX(x, y, z, img);
    if( img->pixels[pos] != color && img->pixels[pos] >= t_min && img->pixels[pos] <= t_max ){
      img->pixels[pos] = color;
      if( x - 1 >= 0 )
        { enqueue3d(&new_queue, x-1, y, z); }
      if( x + 1 < img->width )
        { enqueue3d(&new_queue, x+1, y, z); }
      if( y + 1 < img->height )
        { enqueue3d(&new_queue, x, y+1, z); }
      if( y - 1 >= 0 )
        { enqueue3d(&new_queue, x, y-1, z); }
      if( z + 1 < img->length )
        { enqueue3d(&new_queue, x, y, z+1); }
      if( z - 1 >= 0 )
        { enqueue3d(&new_queue, x, y, z-1); }
    }
  }
  c = clock() - c;
  printf("Queue: Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
  return (float)c/CLOCKS_PER_SEC;
}

float growingregionraw16_test(imageraw16_t *img, int32_t px, int32_t py, int32_t pz, int16_t threshold, int16_t color){
  time_t c = clock();
  bool flag = true;
  int32_t mid = 2;
  int32_t i, j, k;
  int16_t t_min = img->pixels[INDEX(px, py, pz, img)]-threshold, tmp_1 = t_min;
  int16_t t_max = img->pixels[INDEX(px, py, pz, img)]+threshold, tmp_2 = t_max;
  queue3d new_queue;
  int32_t x, y, z, pos, inx;
  queue_start3d(&new_queue);
  enqueue3d(&new_queue, px, py, pz);
  while( is_empty_queue3d(&new_queue) == false )
  {
    dequeue3d(&new_queue, &x, &y, &z);
    if(x<0||x>=img->width||y<0||y>=img->height||z<0||z>=img->length)
      continue;
    pos = INDEX(x, y, z, img);
    tmp_1 = img->length - z;
    if( tmp_1 < 220 ) tmp_2 = t_max;
    else if(tmp_1 < 240) tmp_2 = t_max + 5;
    else if(tmp_1 < 280) tmp_2 = t_max + 60;
    else if(tmp_1 < 310) tmp_2 = t_max + 90;
    if( img->pixels[pos] != color && img->pixels[pos] >= t_min && img->pixels[pos] <= tmp_2 ){
      img->pixels[pos] = color;
      enqueue3d(&new_queue, x-1, y, z);
      enqueue3d(&new_queue, x+1, y, z);
      enqueue3d(&new_queue, x, y+1, z); 
      enqueue3d(&new_queue, x, y-1, z); 
      enqueue3d(&new_queue, x, y, z+1); 
      enqueue3d(&new_queue, x, y, z-1); 
      enqueue3d(&new_queue, x-1, y-1, z);
      enqueue3d(&new_queue, x+1, y+1, z);
      enqueue3d(&new_queue, x-1, y-1, z+1);
      enqueue3d(&new_queue, x+1, y+1, z+1);
      enqueue3d(&new_queue, x-1, y-1, z-1);
      enqueue3d(&new_queue, x+1, y+1, z-1);
    }
  }
  c = clock() - c;
  printf("Queue: Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
  return (float)c/CLOCKS_PER_SEC;
}

float growingregionraw8_3d(imageraw8_t *img, int32_t px, int32_t py, int32_t pz, uint8_t threshold, uint8_t color){
  time_t c = clock(); 
  int16_t t_min = img->pixels[INDEX(px, py, pz, img)]-threshold;
  int16_t t_max = img->pixels[INDEX(px, py, pz, img)]+threshold;
  queue3d new_queue;
  int32_t x, y, z, pos;
  queue_start3d(&new_queue);
  enqueue3d(&new_queue, px, py, pz);
  while( is_empty_queue3d(&new_queue) == false )
  {
    dequeue3d(&new_queue, &x, &y, &z);
    pos = INDEX(x, y, z, img);
    if( img->pixels[pos] != color && img->pixels[pos] >= t_min && img->pixels[pos] <= t_max ){
      img->pixels[pos] = color;
      if( x - 1 >= 0 )
        { enqueue3d(&new_queue, x-1, y, z); }
      if( x + 1 < img->width )
        { enqueue3d(&new_queue, x+1, y, z); }
      if( y + 1 < img->height )
        { enqueue3d(&new_queue, x, y+1, z); }
      if( y - 1 >= 0 )
        { enqueue3d(&new_queue, x, y-1, z); }
      if( z + 1 < img->length )
        { enqueue3d(&new_queue, x, y, z+1); }
      if( z - 1 >= 0 )
        { enqueue3d(&new_queue, x, y, z-1); }
    }
  }
  c = clock() - c;
  printf("Queue: Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
  return (float)c/CLOCKS_PER_SEC;
}

//--------------------------Thinning---------------------------------//
typedef bool(*corrdinate_t)(imageraw8_t*, int32_t, int32_t, int32_t);

bool gotNorth(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  return img->pixels[INDEX(x, y-1, z, img)];
}
bool gotSouth(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  return img->pixels[INDEX(x, y+1, z, img)];
}
bool gotWest(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  return img->pixels[INDEX(x-1, y, z, img)];
}
bool gotEast(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  return img->pixels[INDEX(x+1, y, z, img)];
}
bool gotUp(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  return img->pixels[INDEX(x, y, z+1, img)];
}
bool gotBottom(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  return img->pixels[INDEX(x, y, z-1, img)];
}

int32_t *get_neighbors(imageraw8_t *img, int32_t x, int32_t y, int32_t z){
  int32_t *neighbors = (int32_t*)calloc(27, sizeof(int32_t));
  int32_t i, j, k, p = 0;
  
  for(i = -1; i <= 1; ++i)
  for(j = -1; j <= 1; ++j)
  for(k = -1; k <= 1; ++k)
    neighbors[p++] = !!img->pixels[INDEX(x+k, y+j, z+i, img)];
  return neighbors;
}

uint8_t *load_LUT(){
  time_t c = clock();
  FILE *fileLUT = fopen("lookuptable.bin", "rb");
  if(fileLUT == NULL){
    exit(5);
  }
  int64_t fsize = file_size(fileLUT) * 8;
  uint8_t *arrayLUT = (uint8_t*)calloc(fsize, sizeof(uint8_t));
  if(arrayLUT == NULL){
    fclose(fileLUT);
    exit(6);
  }
  uint8_t buff = 0;
  for(int32_t fs = 0; fs < fsize; fs+=8){
    fread(&buff, 1, sizeof(uint8_t), fileLUT);
    for(int32_t t = 0, p = 7; t < 8; ++t, --p)
      arrayLUT[fs+t] = (buff >> p) & 1;
  }
  fclose(fileLUT);
  c = clock() - c;
  printf("Total time load data: %.2f\n", (float)c/CLOCKS_PER_SEC);
  return arrayLUT;
}

/*
  Fast 3D Thinning of Medical Image Data based on Local Neighborhood Lookups
  Tobias Post, Christina Gillmann, Thomas Wischgoll and Hans Hagen
  https://diglib.eg.org/handle/10.2312/eurovisshort20161159
*/
void thinning_3d(imageraw8_t *img)
{
  uint8_t *arrayLUT = load_LUT();
  int32_t *neighbors;
  bool changes = true;
  int32_t x, y, z, i, j, k, n, p, dir, LUT_index, num_of_can;
  int32_t width = img->width - 1, height = img->height - 1, length = img->length - 1;
  int32_t in[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,14,15,16,17,18,19,20,21,22,23,24,25,26};
  corrdinate_t direction[6] = { gotNorth, gotSouth, gotWest, gotEast, gotUp, gotBottom};
  xyz_t *candidates = (xyz_t*)calloc(img->size, sizeof(xyz_t));

  time_t c = clock();
  while(changes)
  {
    num_of_can = 0;
    changes = false;
    for(dir = 0; dir <= 5; ++dir)
    {
      for(z = 1; z < length; ++z)
      for(y = 1; y < height; ++y)
      for(x = 1; x < width; ++x)
      {
        if(!img->pixels[INDEX(x, y, z, img)] || direction[dir](img, x, y, z)) 
          continue;

        neighbors = get_neighbors(img, x, y, z);
        LUT_index = 0;
        for(i = 0, j = 25; i < 26; ++i, --j)
          LUT_index |= neighbors[in[i]] << j;
        free(neighbors);

        if(arrayLUT[LUT_index])
          candidates[num_of_can++] = (xyz_t){x, y, z};
      }/*z, y, x*/

      for(n = 0; n < num_of_can; ++n){
        x = candidates[n].x;
        y = candidates[n].y;
        z = candidates[n].z;

        neighbors = get_neighbors(img, x, y, z);
        LUT_index = 0;
        for(i = 0, j = 25; i < 26; ++i, --j)
          LUT_index |= neighbors[in[i]] << j;
        free(neighbors);

        if(arrayLUT[LUT_index]){
          img->pixels[INDEX(x, y, z, img)] = __RAW8MIN__;
          changes = true;
        }
      }/*canditates*/
    }/*dir*/
  }/*changes*/
  free(arrayLUT);
  free(candidates);
  c = clock() - c;
  printf("Total time: %.2f\n", (float)c/CLOCKS_PER_SEC);
}
//---------------------------------------------------------------------//
//----------------------Thickening--------------------//
void thickening_3d(imageraw8_t *img){
  time_t c = clock(); 
  uint8_t *new_pixels = (uint8_t*)calloc(img->size, sizeof(uint8_t));
  if( new_pixels == NULL ){
    free_raw8(img);
    exit(EXIT_FAILURE);
  }
  memcpy(new_pixels, img->pixels, img->size*sizeof(uint8_t));
  int32_t x, y, z, i, j, k, index;
  int32_t height = img->height - 1;
  int32_t width = img->width - 1; 
  int32_t length = img->length - 1;

  for (z = 1; z < length; ++z)
  for (y = 1; y < height; ++y)
  for (x = 1; x < width; ++x)
    if(img->pixels[INDEX(x,y,z,img)])
      for(i = -1; i <= 1; ++i)
      for(j = -1; j <= 1; ++j)
      for(k = -1; k <= 1; ++k)
        new_pixels[INDEX(x+k,y+j,z+i,img)] = __RAW8MAX__;
  memcpy(img->pixels, new_pixels, img->size*sizeof(uint8_t));
  free(new_pixels);
  c = clock() - c;
  printf("Done in %.2fs\n", (float)c/CLOCKS_PER_SEC);
}
//----------------------------------------------------//


#endif
