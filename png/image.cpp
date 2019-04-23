#include "image.h"
#include "settings.h"

int Image::open_png_file(const char *file_name){
    png_uint_32 x,y;
    // 8 is the maximum size that can be checked
    png_byte header[8]= {0};

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp){
      //cout << "Can't open file to read. Bye\n";
      return -1;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)!=0) {
      //cout << "file is not recognized as a PNG. Bye\n";
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

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(Bitmap.color_type == PNG_COLOR_TYPE_RGB ||
       Bitmap.color_type == PNG_COLOR_TYPE_GRAY||
       Bitmap.color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(Bitmap.png_ptr,0xFF, PNG_FILLER_AFTER);

    //grey to rgb
    if (Bitmap.color_type == PNG_COLOR_TYPE_GRAY || Bitmap.color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb(Bitmap.png_ptr);



    png_read_update_info(Bitmap.png_ptr, Bitmap.info_ptr);

    //Bitmap.width = png_get_image_width(Bitmap.png_ptr, Bitmap.info_ptr);
    //Bitmap.height = png_get_image_height(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.color_type = png_get_color_type(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.bit_depth = png_get_bit_depth(Bitmap.png_ptr, Bitmap.info_ptr);
    Bitmap.number_of_passes = png_set_interlace_handling(Bitmap.png_ptr);

    Bitmap.color_type=6;
    printf("color_type = %d\n",Bitmap.color_type);


    //cout<< res_x<<"\n" << res_y<<"\n";

    /* read file */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
     // cout << "can't read image\n";
      return -1;
    }

    Bitmap.row_pointers = static_cast <png_byte**> (malloc(sizeof(png_byte *) * Bitmap.height));
    for (y = 0; y < Bitmap.height; y++){
      //image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));
      // т.к. изображение перевернутое, то указатели идут снизу вверх
        Bitmap.row_pointers[Bitmap.height-y-1] = static_cast<png_byte*> (malloc(png_get_rowbytes(Bitmap.png_ptr, Bitmap.info_ptr)));
    }
    png_read_image(Bitmap.png_ptr, Bitmap.row_pointers);

    Bitmap.Pixels = static_cast<Pixel_t**>(malloc(sizeof(Pixel_t *) * Bitmap.height));
        for (png_uint_32 i = 0; i < Bitmap.height; i++) {
            Bitmap.Pixels[i] = static_cast<Pixel_t*>(calloc(sizeof(Pixel_t), Bitmap.width));
        }
        for(y = 0; y < Bitmap.height; y++){
            for(x = 0; x < Bitmap.width; x++){
                png_byte *px = &Bitmap.row_pointers[y][x * 4];
                Bitmap.Pixels[y][x].red = uint8_t(px[0]);
                Bitmap.Pixels[y][x].green = uint8_t(px[1]);
                Bitmap.Pixels[y][x].blue = uint8_t(px[2]);
                Bitmap.Pixels[y][x].alpha = uint8_t(px[3]);
            }
        }

    fclose(fp);
    return 0;
}

int Image::save_png_file(const char *file_name){
    png_uint_32 y,x;
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        //cout << "Can't open file to write. Bye\n";
        return -1;
    }

    /* initialize stuff */
    Bitmap.png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!Bitmap.png_ptr){
        // Some error handling: png_create_write_struct failed
        //cout <<"error  png_create_write_struct failed\n";
        return -1;
    }

    Bitmap.info_ptr = png_create_info_struct(Bitmap.png_ptr);
    if (!Bitmap.info_ptr){
        // Some error handling: png_create_info_struct failed
        //cout <<"Some error handling: png_create_info_struct failed\n";
        return -1;
    }

    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        // Some error handling: error during init_io
        //cout <<"error during init_io\n";
        return -1;

    }

    png_init_io(Bitmap.png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        // Some error handling: error during writing header
        //cout <<"error during writing header\n";
        return -1;
    }

    png_set_IHDR(Bitmap.png_ptr, Bitmap.info_ptr, Bitmap.width, Bitmap.height,
                 Bitmap.bit_depth, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(Bitmap.png_ptr, Bitmap.info_ptr);

    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        // Some error handling: error during writing bytes
        //cout <<"error during writing bytes\n";
        return -1;
    }

    for(y=0;y<Bitmap.height;y++){
        for (x=0;x<Bitmap.width;x++) {
            png_byte *px = &Bitmap.row_pointers[y][x * 4];
            px[0]=png_byte(Bitmap.Pixels[y][x].red);
            px[1] = png_byte(Bitmap.Pixels[y][x].green);
            px[2] = png_byte(Bitmap.Pixels[y][x].blue);
            px[3] = png_byte(Bitmap.Pixels[y][x].alpha);
        }
    }

    /* write bytes */


    png_write_image(Bitmap.png_ptr, Bitmap.row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(Bitmap.png_ptr))){
        cout <<"error during end of write\n";
        return -1;
        // Some error handling: error during end of write
    }

    png_write_end(Bitmap.png_ptr, nullptr);

    /* cleanup heap allocation */

    for (y = 0; y < Bitmap.height-1; y++)
        //printf("%d --- ",y);
        //cout<<""; // спросить у ЧАЙКИ
        free(Bitmap.row_pointers[y]);
    free(Bitmap.row_pointers);

    fclose(fp);
    return 0;
}

void Image::negativ_RGB(int x1, int y1, int x2, int y2){

    if (x2 < x1)
            std::swap(x1, x2);
    if (y2 < y1)
        std::swap(y1, y2);

    for (int y=y1;y<y2;y++) {
        for (int x=x1;x<x2;x++) {
            Bitmap.Pixels[y][x].red=255-Bitmap.Pixels[y][x].red;
            Bitmap.Pixels[y][x].green=255-Bitmap.Pixels[y][x].green;
            Bitmap.Pixels[y][x].blue=255-Bitmap.Pixels[y][x].blue;



        }
    }
}

int Image::print_rectangle(int x1,int y1,int x2, int y2, int thickness, QColor line_color){


    if (x2 < x1)
            std::swap(x1, x2);
    if (y2 < y1)
        std::swap(y1, y2);
    int x,y;
    if(thickness>(x2-x1)|| thickness>(y2-y1)) return -1;

    for(y=y1;y<y1+thickness;y++){
        for (x=x1; x<x2;x++) {
            Bitmap.Pixels[y][x].red = static_cast<uint8_t>( line_color.red());
            Bitmap.Pixels[y][x].green = static_cast<uint8_t>(  line_color.green());
            Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( line_color.blue());

        }
    }

    for (y = y1;y<=y2;y++) {
            //png_byte *row = image->row_pointers[y];
            for (x = x2;x > x2-thickness;x--) {
                Bitmap.Pixels[y][x].red = static_cast<uint8_t>( line_color.red());
                Bitmap.Pixels[y][x].green = static_cast<uint8_t>(  line_color.green());
                Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( line_color.blue());

            }
        }

        for (y = y2;y>y2-thickness;y--) {
            //png_byte *row = image->row_pointers[y];
            for(x = x1;x<x2;x++){
                Bitmap.Pixels[y][x].red = static_cast<uint8_t>( line_color.red());
                Bitmap.Pixels[y][x].green = static_cast<uint8_t>(  line_color.green());
                Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( line_color.blue());

            }
        }

        for ( y=y2;y>y1;y--) {
            //png_byte *row = image->row_pointers[y];
            for (x=x1;x<x1+thickness;x++) {
                //png_byte *ptr = &(row[x * 3]);
                Bitmap.Pixels[y][x].red = static_cast<uint8_t>( line_color.red());
                Bitmap.Pixels[y][x].green = static_cast<uint8_t>(  line_color.green());
                Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( line_color.blue());

            }
        }
        return 0;

}

void Image::rec_fill(int x1,int y1,int x2, int y2, int thickness, QColor fill_color){

    if (x2 < x1)
            std::swap(x1, x2);
    if (y2 < y1)
        std::swap(y1, y2);

    for (int y=y1+thickness;y<=y2-thickness;y++) {

                for(int x = x1+thickness;x<=x2-thickness;x++){

                    Bitmap.Pixels[y][x].red = static_cast<uint8_t>( fill_color.red());
                    Bitmap.Pixels[y][x].green = static_cast<uint8_t>( fill_color.green());
                    Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( fill_color.blue());

                }
            }
}

void Image::division(int N, int M, int thickness, QColor color){
    unsigned int space_y=(Bitmap.height -(N-1)*thickness)/N;
    unsigned int space_x=(Bitmap.width-(M-1)*thickness)/M;
    int comst=0;
    for (int y=space_y;y<Bitmap.height-space_y;y=y+space_y) {
            comst=y;
            for(y;y<comst+thickness;y++){
                for(png_uint_32 x=0;x<Bitmap.width;x++){
                    Bitmap.Pixels[y][x].red = static_cast<uint8_t>( color.red());
                    Bitmap.Pixels[y][x].green = static_cast<uint8_t>( color.green());
                    Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( color.blue());
                }
                //printf("%d\n",y);
            }
        }
        for(int y=0;y<Bitmap.height;y++){
            //png_byte *row = image->row_pointers[y];
            for (int x=space_x;x<Bitmap.width-space_x;x=x+space_x) {
                comst=x;
                for (x;x<comst+thickness;x++) {
                    //png_byte *ptr = &(row[x * 3]);
                    Bitmap.Pixels[y][x].red = static_cast<uint8_t>( color.red());
                    Bitmap.Pixels[y][x].green = static_cast<uint8_t>( color.green());
                    Bitmap.Pixels[y][x].blue = static_cast<uint8_t>( color.blue());
                }
            }
        }

}

int Image::rotate(int x1,int y1,
            int x2, int y2,
                   int angle){
    if (x2 < x1)
            std::swap(x1, x2);
    if (y2 < y1)
        std::swap(y1, y2);
    Pixel_t **Pixels2 = static_cast<Pixel_t **>(malloc(sizeof(Pixel_t *) * Bitmap.height));
    for (int i = 0; i < Bitmap.height; i++) {
        Pixels2[i] = static_cast<Pixel_t*>(calloc(sizeof(Pixel_t), Bitmap.width));
    }

    for (int y = 0; y < Bitmap.height; y++){
        for (int x = 0; x < Bitmap.width; x++){
            Pixels2[y][x] = Bitmap.Pixels[y][x];
        }
    }

    if(angle == 90){
        int mid_x=x2-(x2-x1)/2;
        int mid_y=y2-(y2-y1)/2;
        /*if((x2-x1)/2>(y2-y1/2)||(x2-x1)/2>Bitmap.height-(y2-y1/2)){
            return -1;
        }
        if((y2-y1)/2>(x2-x1/2)||(y2-y1)/2>Bitmap.width-(x2-x1/2)){
            return -1;
        }*/

        int lenx=x2-x1;
        int leny=y2-y1;
        Pixel_t **tmp = static_cast<Pixel_t **>(malloc(sizeof(Pixel_t *)*lenx));
        for (int i = 0; i < lenx; i++) {
            tmp[i] = static_cast<Pixel_t*>(calloc(sizeof(Pixel_t), leny));
        }

        for (int y = 0; y < leny; y++){
            for (int x = 0; x < lenx; x++){

                //Pixels2[y][x]= Bitmap.Pixels[y][x];
                tmp[x][leny-1-y]=Pixels2[y+y1][x+x1];
            }
        }

        for (int y = y1; y < y2; y++){
            for (int x = x1; x < x2; x++){

                //Pixels2[y][x]= Bitmap.Pixels[y][x];
                Pixels2[y][x].red=255;
                Pixels2[y][x].green=255;
                Pixels2[y][x].blue=255;
            }
        }

        for (int y = 0; y < lenx; y++){
            for (int x = 0; x < leny; x++){

                //Pixels2[y][x]= Bitmap.Pixels[y][x];
                if(mid_y-lenx/2+y>=0 && mid_y-lenx/2+y<Bitmap.height
                        &&mid_x-leny/2+x>=0 && mid_x-leny/2+x<Bitmap.width){
                    Pixels2[mid_y-lenx/2+y][mid_x-leny/2+x]=tmp[y][x];
                }
            }
        }
        for (int i=0;i<lenx;i++) {
            free(tmp[i]);
        }
        free(tmp);

    }

    if(angle == 270){
        int mid_x=x2-(x2-x1)/2;
        int mid_y=y2-(y2-y1)/2;
        /*if((x2-x1)/2>(y2-y1/2)||(x2-x1)/2>Bitmap.height-(y2-y1/2)){
            return -1;
        }
        if((y2-y1)/2>(x2-x1/2)||(y2-y1)/2>Bitmap.width-(x2-x1/2)){
            return -1;
        }*/

        int lenx=x2-x1;
        int leny=y2-y1;
        Pixel_t **tmp = static_cast<Pixel_t **>(malloc(sizeof(Pixel_t *)*lenx));
        for (int i = 0; i < lenx; i++) {
            tmp[i] = static_cast<Pixel_t*>(calloc(sizeof(Pixel_t), leny));
        }

        for (int y = 0; y < leny; y++){
            for (int x = 0; x < lenx; x++){

                //Pixels2[y][x]= Bitmap.Pixels[y][x];
                tmp[x][y]=Pixels2[y+y1][x+x1];
            }
        }

        for (int y = y1; y < y2; y++){
            for (int x = x1; x < x2; x++){

                //Pixels2[y][x]= Bitmap.Pixels[y][x];
                Pixels2[y][x].red=255;
                Pixels2[y][x].green=255;
                Pixels2[y][x].blue=255;
            }
        }

        for (int y = 0; y < lenx; y++){
            for (int x = 0; x < leny; x++){

                //Pixels2[y][x]= Bitmap.Pixels[y][x];
                if(mid_y-lenx/2+y>=0 && mid_y-lenx/2+y<Bitmap.height
                        &&mid_x-leny/2+x>=0 && mid_x-leny/2+x<Bitmap.width){
                    Pixels2[mid_y-lenx/2+y][mid_x-leny/2+x]=tmp[y][x];
                }
            }
        }
        for (int i=0;i<lenx;i++) {
            free(tmp[i]);
        }
        free(tmp);

    }

    if(angle == 180){

        for (int y = y1; y < y2; y++){
            for (int x = x1; x < x2; x++){
                Pixels2[y1+y2-y][x1+x2-x]= Bitmap.Pixels[y][x];
            }
        }
    }

    if(angle == 360){

        for (int y = y1; y < y2; y++){
            for (int x = x1; x < x2; x++){
                Pixels2[y][x1+x2-x]= Bitmap.Pixels[y][x];
            }
        }
    }


    Pixel_t **Pixels1 = Bitmap.Pixels;
    Bitmap.Pixels = Pixels2;

    for(int y = 0; y < Bitmap.height; y++) {
        free(Pixels1[y]);
    }
    free(Pixels1);
    return 0;
}

void Image::pattern(int type, int thickness, QColor color, QColor color_fill){
    png_uint_32 x,y;
    if(type==2){
        // left
        for (y=0;y<Bitmap.height;y++) {
            for (x=0;x<thickness;x++) {
                Bitmap.Pixels[y][x].red=color.red();
                Bitmap.Pixels[y][x].green=color.green();
                Bitmap.Pixels[y][x].blue=color.blue();
            }
        }
        // right
        for (y=Bitmap.height-1;y>0;y--) {
            for (x=Bitmap.width-1;x>Bitmap.width-1-thickness;x--) {
                Bitmap.Pixels[y][x].red=color.red();
                Bitmap.Pixels[y][x].green=color.green();
                Bitmap.Pixels[y][x].blue=color.blue();
            }
        }
        // top
        for(x=0;x<Bitmap.width;x++){
            for (y=0;y<thickness;y++) {
                Bitmap.Pixels[y][x].red=color.red();
                Bitmap.Pixels[y][x].green=color.green();
                Bitmap.Pixels[y][x].blue=color.blue();
            }
        }
        //bottom
        for(x=0;x<Bitmap.width;x++){
            for (y=Bitmap.height-1;y>Bitmap.height-1-thickness;y--) {
                Bitmap.Pixels[y][x].red=color.red();
                Bitmap.Pixels[y][x].green=color.green();
                Bitmap.Pixels[y][x].blue=color.blue();
            }
        }
    }
    if (type==1){
        for (y=0;y<Bitmap.height;y++) {
            for (x=0;x<Bitmap.width;x++) {
                Bitmap.Pixels[y][x].red=color.red();
                Bitmap.Pixels[y][x].green=color.green();
                Bitmap.Pixels[y][x].blue=color.blue();
            }
        }
    }
    if(type=66){
        int n=Bitmap.width/100;
        int m=Bitmap.height/100;
        for (int i=0;i<n;i++) {
            int tmp=Bitmap.width/n;
            Image::KOH(0,4, tmp*i, Bitmap.height-1, tmp*(i+1), Bitmap.height-1,1,color, color_fill);
            Image::KOH(0,4, tmp*i, 0, tmp*(i+1), 0,-1,color, color_fill);
            //Image::line(tmp*i, Bitmap.height-1, tmp*(i+1), Bitmap.height-1,color);
            // запустить с 1 чтобы убрать нижние полосы
            Image::KOH(0,1, tmp*i, 0, tmp*(i+1), 0,-1,color_fill, color_fill);
            Image::KOH(0,1, tmp*i, Bitmap.height-1, tmp*(i+1), Bitmap.height-1,1,color_fill, color_fill);
        }
        for(int i=0;i<m;i++){
            int tmp=Bitmap.height/m;
            Image::KOH(0,4, 0,tmp*i,0,tmp*(i+1),1,color,color_fill);
            Image::KOH(0,4, Bitmap.width-1,tmp*i,Bitmap.width,tmp*(i+1),-1,color,color_fill);

            Image::KOH(0,1, 0,tmp*i,0,tmp*(i+1),1,color_fill,color_fill);
            Image::KOH(0,1, Bitmap.width-1,tmp*i,Bitmap.width,tmp*(i+1),-1,color_fill,color_fill);
        }
        //Image::KOH(0,5, 0, Bitmap.height-1, Bitmap.width-1, Bitmap.height-1,1,color, color_fill);
        //Image::KOH(0,5, Bitmap.width-1,0,Bitmap.width-1, Bitmap.height-1, -1,color, color_fill);
        //Image::KOH(0,5, 0, 0, 0, Bitmap.height-1, 1,color, color_fill);




    }

}

void Image::line(int x1, int y1, int x2, int y2, QColor color){
    const int deltaX = abs(x2 - x1);
        const int deltaY = abs(y2 - y1);
        const int signX = x1 < x2 ? 1 : -1;
        const int signY = y1 < y2 ? 1 : -1;
        //
        int error = deltaX - deltaY;
        //
        Bitmap.Pixels[y2][x2].red=color.red();
        Bitmap.Pixels[y2][x2].green=color.green();
        Bitmap.Pixels[y2][x2].blue=color.blue();
        while(x1 != x2 || y1 != y2)
       {
            Bitmap.Pixels[y1][x1].red=color.red();
            Bitmap.Pixels[y1][x1].green=color.green();
            Bitmap.Pixels[y1][x1].blue=color.blue();
            const int error2 = error * 2;
            //
            if(error2 > -deltaY)
            {
                error -= deltaY;
                x1 += signX;
            }
            if(error2 < deltaX)
            {
                error += deltaX;
                y1 += signY;
            }
        }

}

void Image::KOH(int k, int max_k, int x1, int y1, int x2, int y2, int l, QColor color, QColor color_fill){
    int y,x,i,j;
    double tmp;
    // точка на 1/3 длины
    double x3_d = x1 + (x2 - x1)/3;
    double y3_d = y1 + (y2 - y1)/3;

    // точка на 2/3 длины
    double y4_d = y1 + 2 * (y2 - y1)/3;
    double x4_d = x1 + 2 * (x2 - x1)/3;
    // длина основания
    double L = sqrt(pow((x1 - x2),2) + pow((y1 - y2),2));
    // высота нового равностороннего треугольника
    double h = L /(2 * sqrt(3));
    // углы между линией и осью ОХ
    double sina = (y2 - y1)/L;
    double cosa = (x2 - x1)/L;
    // вершина галочки
    double x5_d = (x2 + x1)/2 + h * l * sina;
    double y5_d = (y2 + y1)/2 - h * l * cosa ;

    // drow
    //swap!
    int x3,x4,x5,y3,y4,y5;
    x3=x3_d;
    x4=x4_d;
    x5=x5_d;
    y3=y3_d;
    y4=y4_d;
    y5=y5_d;

    Image::line(x3,y3,x4,y4, color_fill);
/*
    Bitmap.Pixels[y1][x1].red=0;
    Bitmap.Pixels[y1][x1].green=0;
    Bitmap.Pixels[y1][x1].blue=0;

    Bitmap.Pixels[y2][x2].red=0;
    Bitmap.Pixels[y2][x2].green=0;
    Bitmap.Pixels[y2][x2].blue=0;

    Bitmap.Pixels[y3][x3].red=0;
    Bitmap.Pixels[y3][x3].green=0;
    Bitmap.Pixels[y3][x3].blue=0;

    Bitmap.Pixels[y4][x4].red=0;
    Bitmap.Pixels[y4][x4].green=0;
    Bitmap.Pixels[y4][x4].blue=0;

    Bitmap.Pixels[y5][x5].red=0;
    Bitmap.Pixels[y5][x5].green=0;
    Bitmap.Pixels[y5][x5].blue=0;
*/

    // рисуем "галочку"
    Image::line(x3,y3,x5,y5,color);
    Image::line(x5,y5,x4,y4,color);

    k++;// текущий порядок кривй Коха
    if(k<max_k){
        KOH(k,max_k,x1,y1,x3,y3,l,color, color_fill);
        KOH(k,max_k,x3,y3,x5,y5,l,color, color_fill);
        KOH(k,max_k,x5,y5,x4,y4,l,color, color_fill);
        KOH(k,max_k,x4,y4,x2,y2,l,color, color_fill);
    }

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
            image->setPixel(x, y, pixel.rgba());
        }
    }
    return QPixmap::fromImage(*image);
}
