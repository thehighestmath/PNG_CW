#ifndef IMAGE_H
#define IMAGE_H

#include <png.h>
#include <cstdint>
#include <iostream>
#include <cstring>

#include <QPixmap>
#include <map>

using namespace std;

class Image{

    typedef struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            //uint8_t alpha;
        } Pixel_t;

    typedef struct Png{
        png_uint_32 width, height;
        int number_of_passes;
        png_byte color_type;
        png_byte bit_depth;

        png_infop info_ptr;
        png_structp png_ptr;
        png_byte **row_pointers;
        Pixel_t **Pixels;

    } Bitmap_t;

public:
    Bitmap_t Bitmap;
    int read_png_file(const char *file_name);
    int write_png_file(const char *file_name);
    void negativ_RGB(int lh_x,int lh_y, int rl_x, int rl_y);

    void print_rectangle(int lh_x,int lh_y,
                         int rl_x, int rl_y,
                         bool fill=false,string color="white",
                         int thickness=5, string f_color ="black");

    void division(int N, int M,
                  int thickness, QColor color);

    void rotate(int lh_x,int lh_y,
                int rl_x, int rl_y,
                int angle);

    QPixmap get_pixmap();

};
#endif // IMAGE_H
