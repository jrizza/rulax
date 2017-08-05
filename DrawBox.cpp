#include "include/DrawBox.h"

DrawBox::DrawBox(const unsigned int x, const unsigned int y, const unsigned int w, const unsigned int h) {
  x_pos = x;
  y_pos = y;
  width = w;
  height = h;
}

void DrawBox::getProperties(unsigned int *x, unsigned int *y, unsigned int *w, unsigned int *h) {
  *x = x_pos;
  *y = y_pos;
  *w = width;
  *h = height;
}

bool DrawBox::isInside(const unsigned int x, const unsigned int y) {
  if(width<=2)
    return (x>=x_pos && x<=(x_pos+width) && y>y_pos && y<(y_pos+height));
  if(height<=2)
    return (x>x_pos && x<(x_pos+width) && y>=y_pos && y<=(y_pos+height));
  return (x>x_pos && x<(x_pos+width-1) && y>y_pos && y<(y_pos+height-1));
  return true;
}
