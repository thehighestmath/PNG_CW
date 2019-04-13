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
            uint8_t alpha;
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
    int open_png_file(const char *file_name);
    int save_png_file(const char *file_name);
    void negativ_RGB(int x1,int y1, int x2, int y2);

    int print_rectangle(int x1,int y1,int x2, int y2, int thickness, QColor line_color);
    void rec_fill(int x1,int y1,int x2, int y2, int thickness, QColor fill_color);

    void division(int N, int M,
                  int thickness, QColor color);

    int rotate(int x1,int y1,
                int x2, int y2,
                int angle);


    QPixmap get_pixmap();


};
#endif // IMAGE_H
