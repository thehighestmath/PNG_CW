#include "image.h"

int Image::read_png_file(const char *file_name){
    png_uint_32 x,y;
    // 8 is the maximum size that can be checked
    png_byte header[8]= {0};

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp){
      cout << "Can't open file to read. Bye\n";
      return -1;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)!=0) {
      cout << "file is not recognized as a PNG. Bye\n";
      return -1;
      // Some error handling: file is not recognized as a PNG
    }

    /* initialize stuff */
    Bitmap.png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!Bitmap.png_ptr){
      return -1;
    }

    Bitmap.info_ptr = png_create_info_struct(Bitmap.png_ptr);
    if (!Bitmap.info_ptr){
      return -1;
    }

    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
      return -1;
    }



    png_init_io(Bitmap.png_ptr, fp);
    png_set_sig_bytes(Bitmap.png_ptr, 8);

    png_read_info(Bitmap.png_ptr, Bitmap.info_ptr);

    Bitmap.width = png_get_image_width(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.height = png_get_image_height(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.color_type = png_get_color_type(Bitmap.png_ptr, Bitmap.info_ptr);
    /*
    o - grey
    2-RGB
    3-RGB with palette
    4-grey+alpha
    6-RGB+alpha
    */
    //cout << "color_type = " << image->color_type << '\n';

    Bitmap.bit_depth = png_get_bit_depth(Bitmap.png_ptr, Bitmap.info_ptr);

    Bitmap.number_of_passes = png_set_interlace_handling(Bitmap.png_ptr);

    printf("color_type = %d\n",Bitmap.color_type);

    // png формат может содержать 16 бит на канал, но нам нужно только 8, поэтому сужаем канал
    if (Bitmap.bit_depth == 16) png_set_strip_16(Bitmap.png_ptr);
    // преобразуем файл если он содержит палитру в нормальный RGB
    if (Bitmap.color_type == PNG_COLOR_TYPE_PALETTE && Bitmap.bit_depth <= 8) png_set_palette_to_rgb(Bitmap.png_ptr);
    //image->color_type=2;   // RGB with palette->RGB
    // если в грэйскейле меньше бит на канал чем 8, то конвертим к нормальному 8-битному
    if (Bitmap.color_type == PNG_COLOR_TYPE_GRAY && Bitmap.bit_depth < 8) png_set_expand_gray_1_2_4_to_8(Bitmap.png_ptr);
    // и добавляем полный альфа-канал
    if (png_get_valid(Bitmap.png_ptr, Bitmap.info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(Bitmap.png_ptr);
    //grey to rgb
    if (Bitmap.color_type == PNG_COLOR_TYPE_GRAY || Bitmap.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(Bitmap.png_ptr);



    png_read_update_info(Bitmap.png_ptr, Bitmap.info_ptr);

    Bitmap.width = png_get_image_width(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.height = png_get_image_height(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.color_type = png_get_color_type(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.bit_depth = png_get_bit_depth(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.number_of_passes = png_set_interlace_handling(Bitmap.png_ptr);


    printf("color_type = %d\n",Bitmap.color_type);


    /* read file */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
      cout << "can't read image\n";
      return -1;
    }

    Bitmap.row_pointers = static_cast <png_byte**> (malloc(sizeof(png_byte *) * Bitmap.height));
    for (y = 0; y < Bitmap.height; y++)
      //image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));
      // т.к. изображение перевернутое, то указатели идут снизу вверх
    Bitmap.row_pointers[Bitmap.height-y-1] = static_cast<png_byte*> (malloc(png_get_rowbytes(Bitmap.png_ptr, Bitmap.info_ptr)));

    png_read_image(Bitmap.png_ptr, Bitmap.row_pointers);

    Bitmap.Pixels = static_cast<Pixel_t**>(malloc(sizeof(Pixel_t *) * Bitmap.height));
        for (png_uint_32 i = 0; i < Bitmap.height; i++) {
            Bitmap.Pixels[i] = static_cast<Pixel_t*>(calloc(sizeof(Pixel_t), Bitmap.width));
        }
        for(y = 0; y < Bitmap.height; y++){
            for(x = 0; x < Bitmap.width; x++){
                png_byte *px = &Bitmap.row_pointers[y][x * 3];
                Bitmap.Pixels[y][x].red = uint8_t(px[0]);
                Bitmap.Pixels[y][x].green = uint8_t(px[1]);
                Bitmap.Pixels[y][x].blue = uint8_t(px[2]);
                //Bitmap.Pixels[y][x].alpha = uint8_t(px[3]);
            }
        }

    fclose(fp);
    return 0;
}

int Image::write_png_file(const char *file_name){
    png_uint_32 y;
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        cout << "Can't open file to write. Bye\n";
        return -1;
    }

    /* initialize stuff */
    Bitmap.png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!Bitmap.png_ptr){
        // Some error handling: png_create_write_struct failed
        cout <<"error  png_create_write_struct failed\n";
        exit(1);
    }

    Bitmap.info_ptr = png_create_info_struct(Bitmap.png_ptr);
    if (!Bitmap.info_ptr){
        // Some error handling: png_create_info_struct failed
        cout <<"Some error handling: png_create_info_struct failed\n";
        exit(1);
    }

    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        // Some error handling: error during init_io
        cout <<"error during init_io\n";
        exit(1);

    }

    png_init_io(Bitmap.png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        // Some error handling: error during writing header
        cout <<"error during writing header\n";
        exit(1);
    }

    png_set_IHDR(Bitmap.png_ptr, Bitmap.info_ptr, Bitmap.width, Bitmap.height,
                 Bitmap.bit_depth, Bitmap.color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(Bitmap.png_ptr, Bitmap.info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        // Some error handling: error during writing bytes
        cout <<"error during writing bytes\n";
        exit(1);
    }

    png_write_image(Bitmap.png_ptr, Bitmap.row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        cout <<"error during end of write\n";
        exit(1);
        // Some error handling: error during end of write
    }

    png_write_end(Bitmap.png_ptr, nullptr);

    /* cleanup heap allocation */

    for (y = 0; y < Bitmap.height; y++)
        //printf("%d --- ",y);
        cout<<""; // спросить у ЧАЙКИ
        free(Bitmap.row_pointers[y]);
    free(Bitmap.row_pointers);

    fclose(fp);
    return 0;
}

QPixmap Image::get_pixmap()
{
    QImage *image = new QImage(Bitmap.width, Bitmap.height, QImage::Format_RGB16);
    QColor pixel;
    for (png_uint_32 y = 0; y < Bitmap.height; y++) {
        for (png_uint_32 x = 0; x < Bitmap.width; x++) {
            pixel.setRed(Bitmap.Pixels[y][x].red);
            pixel.setGreen(Bitmap.Pixels[y][x].green);
            pixel.setBlue(Bitmap.Pixels[y][x].blue);
            image->setPixel(x, y, pixel.rgb());
        }
    }
    return QPixmap::fromImage(*image);
}
