#ifndef __FUNCTIONS__
#define __FUNCTIONS__
#define max(a, b)(((a) > (b)) ? (a) : (b))
#define min(a, b)(((a) < (b)) ? (a) : (b))

int comp(const void * a, const void * b) {
  return ( * (uint8_t * ) a - * (uint8_t * ) b);
}
void merge_pixels(uint8_t *pixels, uint32_t width, uint32_t height, uint32_t mid, uint8_t *new_pixels, uint32_t size){
  uint32_t p = 0, x, y;

  uint32_t new_height = height - mid;
  uint32_t new_width = width - mid;
  for (y = mid; y < new_height; ++y) {
    for (x = mid; x < new_width; ++x) {
      if( p >= size ) return;
      pixels[y*width + x] = new_pixels[p++];
    }
  }
}
#endif