#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bmp.h"

BMPFile * bmp_create(int32_t width, int32_t height)
{
    BMPFile * bmp;

    bmp = (BMPFile *) malloc(sizeof(BMPFile));

    if (bmp == NULL) {
        return NULL;
    }

    //Setup header.
    bmp->magic[0] = 'B';
    bmp->magic[1] = 'M';
    bmp->filesz = 54 + (width * height * 4);
    bmp->creator1 = 0;
    bmp->creator2 = 0;
    bmp->bmp_offset = 54;

    //Set up DIP header.
    bmp->header_sz = 40;
    bmp->width = width;
    bmp->height = height;
    bmp->nplanes = 1;
    bmp->bitspp = 32; //32bbp, bigger file but easier to implement.
    bmp->compress_type = 0; //BI_RBG.
    bmp->bmp_bytesz = (width * height * 4);
    bmp->hres = 2835; //2835 pixels/meter.
    bmp->vres = 2835; //2835 pixels/meter.
    bmp->ncolors = 0;
    bmp->nimpcolors = 0;

    //Allocate space for pixel data.
    bmp->data_p = (uint32_t *) malloc(width * height * 4);

    if (bmp->data_p == NULL) {
        free(bmp);
        return NULL;
    }

    return bmp;
}

bool bmp_save(BMPFile * bmp, char * filename)
{
    FILE * fp;
    int32_t written = 0;

    fp = fopen(filename, "wb");

    if (!fp)
        return false;

    //Write header.
    written += fwrite(&(bmp->magic), 2, 1, fp);
    written += fwrite(&(bmp->filesz), 4, 1, fp);
    written += fwrite(&(bmp->creator1), 2, 1, fp);
    written += fwrite(&(bmp->creator2), 2, 1, fp);
    written += fwrite(&(bmp->bmp_offset), 4, 1, fp);

    //Write DIB header.
    written += fwrite(&(bmp->header_sz), 4, 1, fp);
    written += fwrite(&(bmp->width), 4, 1, fp);
    written += fwrite(&(bmp->height), 4, 1, fp);
    written += fwrite(&(bmp->nplanes), 2, 1, fp);
    written += fwrite(&(bmp->bitspp), 2, 1, fp);
    written += fwrite(&(bmp->compress_type), 4, 1, fp);
    written += fwrite(&(bmp->bmp_bytesz), 4, 1, fp);
    written += fwrite(&(bmp->hres), 4, 1, fp);
    written += fwrite(&(bmp->vres), 4, 1, fp);
    written += fwrite(&(bmp->ncolors), 4, 1, fp);
    written += fwrite(&(bmp->nimpcolors), 4, 1, fp);

    //Write pixel data.
    written += fwrite(bmp->data_p, bmp->bmp_bytesz, 1, fp);

    fclose(fp);

    //Check if we wrote all data.
    return (written == 5 + 11 + 1) ? true : false;
}

void bmp_set_pixel(BMPFile * bmp, uint32_t x, uint32_t y, uint32_t color)
{
    assert(bmp && y < bmp->height && x < bmp->width);

    bmp->data_p[((bmp->height - 1 - y) * bmp->width) + x] = color;
}

void bmp_fill(BMPFile * bmp, uint32_t color)
{
    int32_t i;

    assert(bmp);

    for(i = 0; i < bmp->width * bmp->height; i++) {
        bmp->data_p[i] = color;
    }
}

void bmp_draw_rectangle(BMPFile * bmp, uint32_t x1, uint32_t y1,
                        uint32_t x2, uint32_t y2, uint32_t color)
{
    int32_t x, y;
    uint32_t tmp;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    if (y1 > y2) {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    for(y = y1; y < y2; y++) {
        for(x = x1; x < x2; x++) {
            bmp_set_pixel(bmp, x, y, color);
        }
    }
}

void bmp_draw_line(BMPFile * bmp, uint32_t x1, uint32_t y1,
                   uint32_t x2, uint32_t y2, uint32_t color)
{
    uint32_t dx, dy;
    int32_t sx, sy, err, err2;

    if (x1 < x2) {
        sx = 1;
        dx = x2 - x1;
    } else {
        sx = -1;
        dx = x1 - x2;
    }

    if (y1 < y2) {
        sy = 1;
        dy = y2 - y1;
    } else {
        sy = -1;
        dy = y1 - y2;
    }

    err = dx - dy;

    for (;;)
    {
        bmp_set_pixel(bmp, x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        err2 = 2 * err;

        if (err2 > -1 * (int32_t) dy) {
            err -= dy;
            x1 += sx;
        }

        if (err2 < (int32_t) dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void bmp_draw_circle(BMPFile * bmp, uint32_t x, uint32_t y,
                     uint32_t radius, uint32_t color)
{
    int32_t f = 1 - radius;
    int32_t ddF_x = 1;
    int32_t ddF_y = -2 * radius;
    int32_t cx = 0;
    int32_t cy = radius;

    bmp_set_pixel(bmp, x, y + radius, color);
    bmp_set_pixel(bmp, x, y - radius, color);
    bmp_set_pixel(bmp, x + radius, y, color);
    bmp_set_pixel(bmp, x - radius, y, color);

    while(cx < cy)
    {
        // ddF_x == 2 * x + 1;
        // ddF_y == -2 * y;
        // f == x*x + y*y - radius*radius + 2*x - y + 1;
        if(f >= 0)
        {
            cy--;
            ddF_y += 2;
            f += ddF_y;
        }
        cx++;
        ddF_x += 2;
        f += ddF_x;
        bmp_set_pixel(bmp, x + cx, y + cy, color);
        bmp_set_pixel(bmp, x - cx, y + cy, color);
        bmp_set_pixel(bmp, x + cx, y - cy, color);
        bmp_set_pixel(bmp, x - cx, y - cy, color);
        bmp_set_pixel(bmp, x + cy, y + cx, color);
        bmp_set_pixel(bmp, x - cy, y + cx, color);
        bmp_set_pixel(bmp, x + cy, y - cx, color);
        bmp_set_pixel(bmp, x - cy, y - cx, color);
    }
}

void bmp_free(BMPFile * bmp)
{
    free(bmp->data_p);
    free(bmp);
}
