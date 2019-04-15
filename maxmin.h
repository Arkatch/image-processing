#ifndef __FUNCTIONS__
#define __FUNCTIONS__
#define max(a, b)(((a) > (b)) ? (a) : (b))
#define min(a, b)(((a) < (b)) ? (a) : (b))

int comp(const void * a, const void * b) {
  return ( * (uint8_t * ) a - * (uint8_t * ) b);
}
void merge_pixels(uint8_t *pixels, uint32_t width, uint32_t height, uint32_t mid, uint8_t *new_pixels, uint32_t size){
  uint32_t p = 0, x, y;
  for (y = 0; y < height; ++y) {
    for (x = 0; x < width; ++x) {
      if( p >= size ) return;
      pixels[(y + mid) * width + x + mid] = new_pixels[p++];
    }
  }
}
#endif