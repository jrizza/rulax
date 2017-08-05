#ifndef DRAWBOX_H
#define DRAWBOX_H

using namespace std;

class DrawBox {
  private:
    unsigned int x_pos;
    unsigned int y_pos;
    unsigned int width;
    unsigned int height;
  public:
    DrawBox(const unsigned int, const unsigned int, const unsigned int, const unsigned int);
    void getProperties(unsigned int *, unsigned int *, unsigned int *, unsigned int *);
    bool isInside(const unsigned int, const unsigned int);
};
#endif
