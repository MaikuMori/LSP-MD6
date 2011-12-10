#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    //BMP Header
    unsigned char magic[2];
    uint32_t filesz;
    uint16_t creator1;
    uint16_t creator2;
    uint32_t bmp_offset;

    //DIB Header
    uint32_t header_sz;
    int32_t width;
    int32_t height;
    uint16_t nplanes;
    uint16_t bitspp;
    uint32_t compress_type;
    uint32_t bmp_bytesz;
    int32_t hres;
    int32_t vres;
    uint32_t ncolors;
    uint32_t nimpcolors;

    //Pixel data
    uint32_t * data_p;
} BMPFile;

//Make a color from red, green and blue components.
#define BMP_RGB(r, g, b) ((0 << 24) | (r << 16) | (g << 8) | (b << 0))

//Some predefined default colors.
#define BMP_BLACK BMP_RGB(0, 0, 0)
#define BMP_WHITE BMP_RGB(255, 255, 255)
#define BMP_COLOR1 BMP_RGB(103, 0, 31)
#define BMP_COLOR2 BMP_RGB(178, 24, 43)
#define BMP_COLOR3 BMP_RGB(214, 96, 77)
#define BMP_COLOR4 BMP_RGB(244, 165, 130)
#define BMP_COLOR5 BMP_RGB(253, 219, 199)
#define BMP_COLOR6 BMP_RGB(247, 247, 247)
#define BMP_COLOR7 BMP_RGB(209, 229, 240)
#define BMP_COLOR8 BMP_RGB(246, 197, 222)
#define BMP_COLOR9 BMP_RGB(67, 147, 195)
#define BMP_COLOR10 BMP_RGB(33, 102, 172)
#define BMP_COLOR11 BMP_RGB(5, 48, 97)

//Create a bmp file.
BMPFile * bmp_create(int32_t width, int32_t height);
//Write the bmp file to HDD.
bool bmp_save(BMPFile * bmp, char * filename);
//Set specific pixel in the bmp file.
void bmp_set_pixel(BMPFile * bmp, uint32_t x, uint32_t y, uint32_t color);
//Fill whole image with a single color.
void bmp_fill(BMPFile * bmp, uint32_t color);
//Draw a rectangle at arbitary location.
void bmp_draw_rectangle(BMPFile * bmp, uint32_t x1, uint32_t y1,
                        uint32_t x2, uint32_t y2, uint32_t color);
//Draw a line at arbitary location.
void bmp_draw_line(BMPFile * bmp, uint32_t x1, uint32_t y1,
                        uint32_t x2, uint32_t y2, uint32_t color);
//Draw a circle at arbitary location.
void bmp_draw_circle(BMPFile * bmp, uint32_t x, uint32_t y,
                     uint32_t radius, uint32_t color);
//Free bmp file.
void bmp_free(BMPFile * bmp);
