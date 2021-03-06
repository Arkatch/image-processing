#include "../maxmin.h"

#ifndef __CONVERT__
#define __CONVERT__

//--------------Konwersja z RGB-HSI / HSI-RGB----------------//
hsi_t rgb_to_hsi(rgb_t rgb){
  double theta, sqr, h, s, i, r, g, b, _div;
  _div = (double)(rgb.r + rgb.g + rgb.b);
  r = rgb.r / _div;
  g = rgb.g / _div;
  b = rgb.b / _div;

  //gdy r == g == b to sqr == 0, a przez 0 dzielić nie można
  sqr = sqrt(pow(r - g, 2) + (r - b)*(g - b));
  theta = (sqr == 0) ? 0 : acos(0.5*((r - g) + (r - b)) / sqr );

  h = ((b <= g) ? theta : PI2 - theta);
  s = 1.0 - 3.0 * min(min(r, g), b);
  i = _div / 765.0; 
  return (hsi_t){fabs(h), fabs(s), fabs(i)};
}
rgb_t hsi_to_rgb(hsi_t hsi){
  double x = 0, y = 0, z = 0;
  x = hsi.i * (1 - hsi.s);
  if( hsi.h < PI2_3 ){ 
    y = hsi.i * (1 + hsi.s*cos(hsi.h) / cos(PI_3 - hsi.h));
    z = hsi.i * 3 - x - y;
    return (rgb_t){ y*255, z*255, x*255 };
  }
  else if( hsi.h < PI4_3 ){ 
    hsi.h -= PI2_3;
    y = hsi.i * (1 + hsi.s*cos(hsi.h) / cos(PI_3 - hsi.h));
    z = 3*hsi.i - x - y;
    return (rgb_t){ x*255, y*255, z*255 };
  }
  else{
    hsi.h -= PI4_3;
    y = hsi.i * (1 + hsi.s*cos(hsi.h) / cos(PI_3 - hsi.h));
    z = 3*hsi.i - x - y;
    return (rgb_t){ z*255, x*255, y*255 };
  }
}
//--------------------------------------------------------//


//----------------Przygotowywanie strukur do przechowywania RGB / HSI----------//
void hsi_img_template(imagehsi_t *dest, image_t *src){
  if( dest == NULL || src == NULL )
    { return; }
  *dest = (imagehsi_t){
    (uint8_t*)malloc(src->head_size),
    (double*)malloc(src->size * sizeof(double)),
    src->head_size, 
    src->size,
    src->width,
    src->height
  };
  memcpy(dest->header, src->header, src->head_size);
  memset(dest->pixels, 0, src->size * sizeof(double)); 
}
void rgb_img_template(image_t *dest, imagehsi_t *src){
  *dest = (image_t){
    (uint8_t*)malloc(src->head_size),
    (uint8_t*)malloc(src->size),
    src->head_size, 
    src->size,
    src->width,
    src->height
  };
  memcpy(dest->header, src->header, src->head_size);
  memset(dest->pixels, 0, src->size); 
}
//--------------------------------------------------------//

//--------------Konwersja obrazów-------------------------//
void convert_to_hsi(imagehsi_t *dest, image_t *src){
  hsi_img_template(dest, src);
  hsi_t tmp;
  for( uint32_t x = 0; x < dest->size; x += 3 ){
    tmp = rgb_to_hsi((rgb_t){src->pixels[x+2], src->pixels[x+1], src->pixels[x]});
    memcpy(&dest->pixels[x+2], &tmp.h, sizeof(tmp.h));
    memcpy(&dest->pixels[x+1], &tmp.s, sizeof(tmp.s));
    memcpy(&dest->pixels[x+0], &tmp.i, sizeof(tmp.i));
  }
}

void convert_to_rgb(image_t *dest, imagehsi_t *src){
  rgb_img_template(dest, src);
  rgb_t tmp;
  for( uint32_t x = 0; x < dest->size; x += 3 ){
    tmp = hsi_to_rgb((hsi_t){src->pixels[x+2], src->pixels[x+1], src->pixels[x]});
    memcpy(&dest->pixels[x+2], &tmp.r, sizeof(uint8_t));
    memcpy(&dest->pixels[x+1], &tmp.g, sizeof(uint8_t));
    memcpy(&dest->pixels[x+0], &tmp.b, sizeof(uint8_t));
  }
}

void gray_to_rgb(image_t *dest, image_t *src){
  *dest = (image_t){
    (uint8_t*)malloc(54),
    (uint8_t*)malloc(src->size*3),
    54,
    src->size * 3,
    src->width,
    src->height
  };
  infomin_t bbb = { 0x4D42, dest->size+54, 0, 0, 54 };
  header_t aaa = { 40, dest->width, dest->height, 1, 24, 0, dest->size, 0, 0, 256, 0 };

  memcpy(dest->header, &bbb, 14);
  memcpy(dest->header+14, &aaa, 40);

  for(int i = 0, j = 0; i < dest->size; i+=3, ++j){
    dest->pixels[i+2] = src->pixels[j];
    dest->pixels[i+1] = src->pixels[j];
    dest->pixels[i+0] = src->pixels[j];
  }
}
//--------------------------------------------------------//

//---------------Zapisywanie i kopiowanie HSI-----------------//
void save_hsi(imagehsi_t *hsi, FILE *bmp, bool clear_mem){
  image_t img;
  convert_to_rgb(&img, hsi);
  fwrite(img.header, img.head_size, 1, bmp);
  fwrite(img.pixels, img.size, 1, bmp);

  if( clear_mem ){ 
    free_img(&img);
    free_hsi(hsi);
    fclose(bmp);
  }
}
void copy_hsi(imagehsi_t *dest, imagehsi_t *src){
  *dest = (imagehsi_t){
    (uint8_t*)malloc(src->head_size),
    (double*)malloc(src->size * sizeof(double)),
    src->head_size, 
    src->size,
    src->width,
    src->height
  };
  memcpy(dest->header, src->header, src->head_size);
  memcpy(dest->pixels, src->pixels, src->size * sizeof(double));
}
//----------------------------------------------------------//
#endif