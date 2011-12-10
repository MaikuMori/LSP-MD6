#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"
#include "bmp_font.h"

//Generated font data. Sets 'bmp_font_data[][]'.
#include "bmp_font_data.c"

void bmp_print_char(BMPFile * bmp, char c, uint32_t x, uint32_t y, uint32_t color)
{
    int i, j, xx, yy;

    xx = x;
    yy = y - bmp_font_data[(int) c][1];

    for(i = 0; i < bmp_font_data[(int) c][2]; i++) {
        for(j = 0; j < 8; j++) {
            if (bmp_font_data[(int) c][i + 3] & (1 << j)) {
                bmp_set_pixel(bmp, xx, yy, color);
            }

            xx++;

            if (xx - x == bmp_font_data[(int) c][0]) {
                xx -= bmp_font_data[(int) c][0];
                yy++;
                break;
            }

        }
    }

}

void bmp_print(BMPFile * bmp, char * str, uint32_t x, uint32_t y, uint32_t color)
{
    while(*str != '\0') {
        bmp_print_char(bmp, *str, x, y, color);
        x += bmp_font_data[(int) *str][0];

        if (x >= bmp->width)
            break;
        str++;
    }
}
