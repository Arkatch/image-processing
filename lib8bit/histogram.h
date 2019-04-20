#include "../maxmin.h"

/*/
  uint8_t *pixels_histogram(uint8_t *pixels, uint32_t width, uint32_t height)
      Funkcja tworzy histogram obrazu
      Wartość zwracana:
        funkcja zwraca wskaźnik do tablicy statycznej o rozmiarze [256]
/*/

#ifndef __HISTOGRAM__
#define __HISTOGRAM__

uint32_t draw_histogram(image_t *img, const char *filename){
  FILE *histogram = fopen(filename, "wb");
  FILE *hist_template = fopen("template.bin", "rb"); /*dane nagłówka/*/
  if( histogram == NULL || hist_template == NULL )
    { return 1; }
  
  if( histogram == NULL ) { return 1; }
  if( hist_template == NULL ) { return 2; }

  image_t img_hist;
  load_img(&img_hist, hist_template);

  uint32_t i, y;
  uint32_t hist[256] = {0}, _max = 0, param = 1;

  //Obliczanie ilości pikseli w każdym odcieniu
  for(i = 0; i < img->size; ++i)
    ++hist[img->pixels[i]];

  //Wyszukiwanie wartości największej
  for(i = 0; i < 256; ++i)
    _max = max(_max, hist[i]);
  //wyszukiwanie największego dzielnika, aby wykres zmieścił się w obrazie
  while( _max/param > 160 ) ++param;
  
  for(i = 0; i < 256; ++i) {
    for(y = 65+(hist[i] / param); y > 65 ; --y)
      img_hist.pixels[y * (img_hist.width) +(32+i)] = 0;
  }
  save_img(&img_hist, histogram, true);
  fclose(hist_template);
  return 0;
}
void contrast_stretch(image_t *img){
  uint32_t i;
  uint16_t a = 255, b = 0, r;

  for(i = 0; i < img->size; ++i){
    a = min(a, img->pixels[i]);
    b = max(b, img->pixels[i]);
  }
  r = b - a;
  for(i = 0; i < img->size; ++i)
    { img->pixels[i] = (img->pixels[i]-a)*r/255; }
}
#endif