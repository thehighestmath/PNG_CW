#include <png.h>
#include <unistd.h>
#include <cstdlib>
#include  <cstdio>
#include <cstdarg>
#include <iostream>
#include <cstring>
#define OPEN_FILE 100
#define PNG_DEBUG 3
#include <cstring>
#define PNG_colors 3
using namespace std;

// error in reading

struct Png{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_byte **row_pointers;
};

void read_png_file(char *file_name, struct Png *image);
void write_png_file(char *file_name, struct Png *image);
void negativ_RGB(struct Png *image,
                 int lh_x,int lh_y,
                 int rl_x, int rl_y);

void print_rectangle(struct Png *image,
                     int lh_x,int lh_y,
                     int rl_x, int rl_y,
                     bool fill=false,string color="white",
                     int thickness=5, string f_color ="black");

void division(struct Png *image, int N, int M,
              int thickness = 1, string color = "white");

void rotate(struct Png *image,
            int lh_x,int lh_y,
            int rl_x, int rl_y,
            int angle=90);


int main(int argc, char **argv) {
    /*if (argc != 2){
        fprintf(stderr,"Usage: program_name <file_in> <file_out>\n");
        return 0;
    }*/

    struct Png image={};
    char sorce[]="/home/kot/PNG_CW/img/green.png";
    char dest[]="/home/kot/PNG_CW/img/neg.txt.png";
    read_png_file(sorce, &image);
    if(image.color_type==6) {cout <<"RGBA. BYE\n"; return 0;}
    png_byte *r=&image.row_pointers[20][20*3];
    png_byte *f=&(*(*(image.row_pointers+20)+20*3));
    //printf("%d,%d,%d\n", sizeof(&image.row_pointers[20][20*3]), sizeof(image.row_pointers[20][20*3]), sizeof(r));
    //printf("%d\n",*(&image.row_pointers[20][20*3]+2));
    printf("%d %d \n",r[0],f[0]);
    //negativ_RGB(&image,0,0,image.width,image.height);
    int x1,y1,x2,y2;
    x1=50;
    y1=75;
    x2=690;
    y2=1000;
    //print_rectangle(&image,x1,y1,x2,y2);
    //division(&image,20,30,1);
    rotate(&image,0,0,image.height,image.height);
    write_png_file(dest, &image);
    cout<<"ok\n";
    return 0;
}



void read_png_file(char *file_name, struct Png *image) {
    int x,y;
    // 8 is the maximum size that can be checked
    png_byte header[8]= {0};

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp){
        cout << "Can't open file to read. Bye\n";
        exit(OPEN_FILE);
    }
    //png_bytep ptr_hdr= nullptr;

    fread(header, 1, 8, fp);
    //printf("%d\n", png_sig_cmp(header,0,8));
    if (png_sig_cmp(header, 0, 8)!=0) {
        cout << "file is not recognized as a PNG. Bye\n";
        exit(OPEN_FILE);
        // Some error handling: file is not recognized as a PNG
    }

    /* initialize stuff */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!image->png_ptr){
        // Some error handling: png_create_read_struct failed
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr){
        // Some error handling: png_create_info_struct failed
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during init_io
    }


    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
/*
    o - grey
    2-RGB
    3-RGB with palette
    4-grey+alpha
    6-RGB+alpha
*/
    //cout << "color_type = " << image->color_type << '\n';

    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);

    image->number_of_passes = png_set_interlace_handling(image->png_ptr);

    printf("color_type = %d\n",image->color_type);

    // png формат может содержать 16 бит на канал, но нам нужно только 8, поэтому сужаем канал
    if (image->bit_depth == 16) png_set_strip_16(image->png_ptr);
    // преобразуем файл если он содержит палитру в нормальный RGB
    if (image->color_type == PNG_COLOR_TYPE_PALETTE && image->bit_depth <= 8) png_set_palette_to_rgb(image->png_ptr);
    //image->color_type=2;   // RGB with palette->RGB
    // если в грэйскейле меньше бит на канал чем 8, то конвертим к нормальному 8-битному
    if (image->color_type == PNG_COLOR_TYPE_GRAY && image->bit_depth < 8) png_set_expand_gray_1_2_4_to_8(image->png_ptr);
    // и добавляем полный альфа-канал
    if (png_get_valid(image->png_ptr, image->info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(image->png_ptr);
    //grey to rgb
    if (image->color_type == PNG_COLOR_TYPE_GRAY || image->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(image->png_ptr);



    png_read_update_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);
    image->number_of_passes = png_set_interlace_handling(image->png_ptr);


    printf("color_type = %d\n",image->color_type);


    /* read file */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        cout << "can't read image\n";
        exit(1);
    }

    image->row_pointers = (png_byte **) malloc(sizeof(png_byte *) * image->height);
    for (y = 0; y < image->height; y++)
        //image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));
        // т.к. изображение перевернутое, то указатели идут снизу вверх
        image->row_pointers[image->height-y-1] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));

    png_read_image(image->png_ptr, image->row_pointers);

    fclose(fp);
}

void write_png_file(char *file_name, struct Png *image) {
    int x,y;
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        cout << "Can't open file to write. Bye\n";
        exit(OPEN_FILE);
    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!image->png_ptr){
        // Some error handling: png_create_write_struct failed
        cout <<"error  png_create_write_struct failed\n";
        exit(1);
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr){
        // Some error handling: png_create_info_struct failed
        cout <<"Some error handling: png_create_info_struct failed\n";
        exit(1);
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during init_io
        cout <<"error during init_io\n";
        exit(1);

    }

    png_init_io(image->png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during writing header
        cout <<"error during writing header\n";
        exit(1);
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during writing bytes
        cout <<"error during writing bytes\n";
        exit(1);
    }

    png_write_image(image->png_ptr, image->row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        cout <<"error during end of write\n";
        exit(1);
        // Some error handling: error during end of write
    }

    png_write_end(image->png_ptr, nullptr);

    /* cleanup heap allocation */
    for (y = 0; y < image->height; y++)
        //printf("%d --- ",y);
        cout<<""; // спросить у ЧАЙКИ
        free(image->row_pointers[y]);
    free(image->row_pointers);

    fclose(fp);
}

void print_rectangle(struct Png *image,
                     int lh_x,int lh_y,
                     int rl_x, int rl_y,
                     bool fill,string color,
                     int thickness, string f_color){
    //work with RGB
    //switch color
    //если за выход!!

    int x1=lh_x;
    int y1=lh_y;

    int x2=rl_x;
    int y2=lh_y;

    int x3=rl_x;
    int y3=rl_y;

    int x4=lh_x;
    int y4=rl_y;
// border
    for (int y=y1;y<y1+thickness;y++) {
        //png_byte *row = image->row_pointers[y];
        for(int x = x1;x<x2;x++){
            //png_byte *ptr = &(row[x * 3]);
            png_byte *ptr = &image->row_pointers[y][x*3];
            ptr[0]=255;
            ptr[1]=255;
            ptr[2]=255;
        }
    }

    for (int y=y2;y<=y3;y++) {
        //png_byte *row = image->row_pointers[y];
        for (int x=x2;x>x2-thickness;x--) {
            //png_byte *ptr = &(row[x * 3]);
            png_byte *ptr = &image->row_pointers[y][x*3];
            ptr[0]=255;
            ptr[1]=255;
            ptr[2]=255;
        }
    }

    for (int y=y3;y>y3-thickness;y--) {
        //png_byte *row = image->row_pointers[y];
        for(int x = x1;x<x2;x++){
            //png_byte *ptr = &(row[x * 3]);
            png_byte *ptr = &image->row_pointers[y][x*3];
            ptr[0]=255;
            ptr[1]=255;
            ptr[2]=255;
        }
    }

    for (int y=y4;y>y1;y--) {
        //png_byte *row = image->row_pointers[y];
        for (int x=x4;x<x4+thickness;x++) {
            //png_byte *ptr = &(row[x * 3]);
            png_byte *ptr = &image->row_pointers[y][x*3];
            ptr[0]=255;
            ptr[1]=255;
            ptr[2]=255;
        }
    }

    //fill
    if(fill==true){
        for (int y=y1+thickness;y<=y3-thickness;y++) {
            //png_byte *row = image->row_pointers[y];
            for(int x = x1+thickness;x<=x3-thickness;x++){
                //png_byte *ptr = &(row[x * 3]);
                png_byte *ptr = &image->row_pointers[y][x*3];
                ptr[0]=0;
                ptr[1]=0;
                ptr[2]=0;
            }
        }
    }


}

void division(struct Png *image, int N, int M,
              int thickness,string color){

    // исключения по толщине

    int space_y=(image->height-(N-1)*thickness)/N;
    int space_x=(image->width-(M-1)*thickness)/M;
    int comst=0;
    for (int y=space_y;y<image->height-space_y;y=y+space_y) {
        comst=y;
        for(y;y<comst+thickness;y++){
            //png_byte *row = image->row_pointers[y];
            for(int x=0;x<image->width;x++){
                //png_byte *ptr = &(row[x * 3]);
                png_byte *ptr = &image->row_pointers[y][x*3];
                ptr[0]=255;
                ptr[1]=255;
                ptr[2]=255;
            }
            printf("%d\n",y);
        }
    }
    for(int y=0;y<image->height;y++){
        //png_byte *row = image->row_pointers[y];
        for (int x=space_x;x<image->width-space_x;x=x+space_x) {
            comst=x;
            for (x;x<comst+thickness;x++) {
                //png_byte *ptr = &(row[x * 3]);
                png_byte *ptr = &image->row_pointers[y][x*3];
                ptr[0]=255;
                ptr[1]=255;
                ptr[2]=255;
            }
        }
    }

}

void negativ_RGB(struct Png *image,
                 int lh_x,int lh_y,
                 int rl_x, int rl_y) {
    int x,y;

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGB){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
        cout <<" color_type of input file must be PNG_COLOR_TYPE_RGB\n";
        exit(1);
    }

    /*for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x*3]);
            ptr[0] = 255-ptr[0];
            ptr[1] = 255-ptr[1];
            ptr[2] = 255 - ptr[2];
        }
        printf("curren succes is %d\% \n",(100*y/image->height)%100);
    }*/
    for (y = lh_y; y < rl_y; y++) {
        //png_byte *row = image->row_pointers[y];
        for (x = lh_x; x < rl_x; x++) {
            png_byte *ptr = &image->row_pointers[y][x*3];
            ptr[0] = 255 - ptr[0];
            ptr[1] = 255 - ptr[1];
            ptr[2] = 255 - ptr[2];
        }
        printf("curren succes is %d\% , %d\n", (100 * y / image->height) % 100);
    }
}

void rotate(struct Png *image,
            int lh_x,int lh_y,
            int rl_x, int rl_y,
            int angle){
    // error(r<l)
    int size=rl_x-lh_x;
    int layer_count=size/2;
    for(int i=0;i<angle/90;i++){
        for (int layer=0;layer<layer_count;layer++) {
            int first = layer+       lh_y;
            int last = size-first-1+ lh_y;
            for (int element=first;element<last;element++) {
                int offset =element-first;
                /*
                png_byte top= image->row_pointers[first][element*3];
                png_byte right_side = image->row_pointers[element][last*3];
                png_byte bottom =image->row_pointers[last][(last-offset)*3];
                png_byte left_side =image->row_pointers[last-offset][first*3];
                image->row_pointers[first][element*3] =left_side;
                image->row_pointers[element][last*3] = top;
                image->row_pointers[last][(last-offset)*3] = right_side;
                image->row_pointers[last-offset][first*3] =bottom;
                 */
                png_byte top[PNG_colors];
                png_byte right_side[PNG_colors];
                png_byte bottom[PNG_colors];
                png_byte left_side[PNG_colors];
                top[0]=*(&image->row_pointers[first][element*3]);
                top[1]=*(&image->row_pointers[first][element*3]+1);
                top[2]=*(&image->row_pointers[first][element*3]+2);
                right_side[0]=*(&image->row_pointers[element][last*3]);
                right_side[1]=*(&image->row_pointers[element][last*3]+1);
                right_side[2]=*(&image->row_pointers[element][last*3]+2);
                bottom[0]=*(&image->row_pointers[last][(last-offset)*3]);
                bottom[1]=*(&image->row_pointers[last][(last-offset)*3]+1);
                bottom[2]=*(&image->row_pointers[last][(last-offset)*3]+2);
                left_side[0]=*(&image->row_pointers[last-offset][first*3]);
                left_side[1]=*(&image->row_pointers[last-offset][first*3]+1);
                left_side[2]=*(&image->row_pointers[last-offset][first*3]+2);


                *(&image->row_pointers[first][element*3])=left_side[0];
                *(&image->row_pointers[first][element*3]+1)=left_side[1];
                *(&image->row_pointers[first][element*3]+2)=left_side[2];

                *(&image->row_pointers[element][last*3])=top[0];
                *(&image->row_pointers[element][last*3]+1)=top[1];
                *(&image->row_pointers[element][last*3]+2)=top[2];

                *(&image->row_pointers[last][(last-offset)*3])=right_side[0];
                *(&image->row_pointers[last][(last-offset)*3]+1)=right_side[1];
                *(&image->row_pointers[last][(last-offset)*3]+2)=right_side[2];

                *(&image->row_pointers[last-offset][first*3])=bottom[0];
                *(&image->row_pointers[last-offset][first*3]+1)=bottom[1];
                *(&image->row_pointers[last-offset][first*3]+2)=bottom[2];




             /*

            png_byte *top= &image->row_pointers[first][element];
            png_byte *right_side = &image->row_pointers[element][last];
            png_byte *bottom = &image->row_pointers[last][(last-offset)];
            png_byte *left_side = &image->row_pointers[last-offset][first];
            image->row_pointers[first][element]=*left_side;
            image->row_pointers[element][last] = *top;
            image->row_pointers[last][(last-offset)] = *right_side;
            image->row_pointers[last-offset][first] = *bottom;
            */
             /*
            png_byte* top=0;
            png_byte* right_side=0;
            png_byte* bottom=0;
            png_byte* left_side=0;
            png_byte p=(*(*(image->row_pointers+first)+element*3));
            memmove(top, &(*(*(image->row_pointers+first)+element*3)), 3);
            //memmove(top, &image->row_pointers[first][element*3], sizeof(3*image->row_pointers[first][element*3]));
            memmove(right_side, &image->row_pointers[element][last*3], sizeof(3*image->row_pointers[element][last*3]));
            memmove(bottom, &image->row_pointers[last][(last-offset)*3], sizeof(3*image->row_pointers[last][(last-offset)*3]));
            memmove(left_side, &image->row_pointers[last-offset][first*3], sizeof(3*image->row_pointers[last-offset][first*3]));

            memmove(&image->row_pointers[first][element*3],left_side, sizeof(3*image->row_pointers[first][element*3]));
            memmove(&image->row_pointers[element][last*3],top, sizeof(3*image->row_pointers[element][last*3]));
            memmove(&image->row_pointers[last][(last-offset)*3],right_side, sizeof(3*image->row_pointers[last][(last-offset)*3]));
            memmove(&image->row_pointers[last-offset][first*3],bottom, sizeof(3*image->row_pointers[last-offset][first*3]));
            */




            }
        }
    }
}

void process_file(struct Png *image) {
    int x,y;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        // Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
        cout <<" input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA\n";
        exit(1);

    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
        cout <<" color_type of input file must be PNG_COLOR_TYPE_RGBA\n";
        exit(1);
    }

    for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * 4]);
            printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
                   x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

            /* set red value to 0 and green value to the blue one */
            ptr[0] = 0;
            ptr[1] = ptr[2];
        }
    }
}

