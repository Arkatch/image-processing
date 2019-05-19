#include "../maxmin.h"
#include "../lib8bit/histogram.h"

/*/
  void gray_by_color(image_t *img, image_t *new_img, enum COLOR color)
    image_t *img - obraz z którego będą pobierane dane
    image_t *new_img - obraz do którego będą zapisywane dane
    enum COLOR color - kolor według którego ma być generowana skala szarości

/*/

#ifndef __RGBGRAY__
#define __RGBGRAY__

void convert_gray_mem(image_t *dest, image_t *src){
  /*/Alokacja pamięci na nowy obraz/*/
  *dest = (image_t){
    (uint8_t*)malloc(src->head_size+1024),
    (uint8_t*)malloc(src->width * src->height),
    src->head_size+1024, 
    src->width * src->height,
    src->width,
    src->height
  };
  /*/Tworzenie nowego nagłówka/*/
  fullhead_t head = {
    (infomin_t){ 0x4D42, dest->size+dest->head_size, 0, 0, 1078 },
    (header_t){ 40, src->width, src->height, 1, 8, 0, dest->size, 0, 0, 256, 0 }
  };
  /*/Ustawianie kolorów w tablicy rgb/*/
  for(uint32_t i = 0; i < 256; ++i)
    { head.rgb[i] = (rgbquad_t){(uint8_t)i, (uint8_t)i, (uint8_t)i, 0}; }

  memcpy(dest->header, &head, sizeof(fullhead_t));
  memset(dest->pixels, 150, dest->size);
}

/***********Konwersja do szarości*************/
void rgb_to_gray(image_t *img, image_t *new_img){
  uint32_t x, avg, p = 0;
  /*/Przygotowanie pustego pliku/*/
  convert_gray_mem(new_img, img);
  for(x = 0; x < img->size; x+=3){
    avg = (uint32_t)(img->pixels[x] + img->pixels[x+1] + img->pixels[x+2])/3;
    new_img->pixels[p++] = avg;
  }
}
void gray_by_color(image_t *img, image_t *new_img, enum COLOR color){
  uint32_t x, p = 0;
  /*/Przygotowanie pustego pliku/*/
  convert_gray_mem(new_img, img);
  for(x = 0; x < img->size; x+=3)
    { new_img->pixels[p++] = img->pixels[x + color]; }
  switch ( color )
  {
    case RED: draw_histogram(new_img, "gen24/redhistogram.bmp"); break;
    case GREEN: draw_histogram(new_img, "gen24/greenhistogram.bmp"); break;
    case BLUE: draw_histogram(new_img, "gen24/bluehistogram.bmp"); break;
  }
}
/********************************************/

/*******Tylko jeden kolor na obrazie********/
void color_mask(image_t *img, void(*mask)(uint8_t*, uint8_t*, uint8_t*)){
  for(uint32_t x = 0; x < img->size; x+=3)
    { mask(&img->pixels[x+2], &img->pixels[x+1], &img->pixels[x]); }
}
void red_mask(uint8_t* r, uint8_t* g, uint8_t* b){
  if( *r < *g || *r < *b ){
    uint16_t avg = (*r + *g + *b) / 3;
    *r = avg; *g = avg; *b = avg;
  }
}
void green_mask(uint8_t* r, uint8_t* g, uint8_t* b){
  if( *g < *r || *g < *b ){
    uint16_t avg = (*r + *g + *b) / 3;
    *r = avg; *g = avg; *b = avg;
  }
}
void blue_mask(uint8_t* r, uint8_t* g, uint8_t* b){
  if( *b < *g || *b < *r ){
    uint16_t avg = (*r + *g + *b) / 3;
    *r = avg; *g = avg; *b = avg;
  }
}
/****************************************/

#endif
