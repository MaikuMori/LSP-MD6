#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"
#include "bmp_font.h"

#define X 200
#define Y 200

int main(void)
{
    BMPFile * bmp;
    int i;
    bmp = bmp_create(X, Y);

    bmp_fill(bmp, BMP_COLOR1);

    bmp_draw_rectangle(bmp, 100, 100, 110, 110, BMP_COLOR11);

    bmp_set_pixel(bmp, 100, 100, BMP_COLOR5);
    bmp_set_pixel(bmp, 100, 110, BMP_COLOR5);
    bmp_set_pixel(bmp, 110, 100, BMP_COLOR5);
    bmp_set_pixel(bmp, 110, 110, BMP_COLOR5);

    for(i=0; i < X; i++) {
        bmp_set_pixel(bmp, i, 25, BMP_RGB(255,0,0));
        bmp_set_pixel(bmp, i, 125, BMP_RGB(0,255,0));
        bmp_set_pixel(bmp, i, 150, BMP_RGB(0,0,255));
    }

    bmp_draw_line(bmp, 170, 20, 150, 160, BMP_COLOR4);

    bmp_draw_circle(bmp, 75, 150, 25, BMP_COLOR9);

    bmp_print(bmp, "Test goes here! (2\\2) @$%", 40, 40, BMP_BLACK);

    if(!bmp_save(bmp, "test.bmp")) {
        printf("Failed to write to file!\n");
    }
    bmp_free(bmp);

    return EXIT_SUCCESS;
}
