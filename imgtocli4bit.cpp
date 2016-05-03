#!/bin/bash
outputfile=$(mktemp -u)
cat $0 | tail -n +6 | /usr/bin/c++ -I$(dirname $0) -lIL -lILU -xc++ - -o $outputfile && $outputfile "$@"
rm $outputfile
exit 0
#include <stdio.h>
#include <stdlib.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <memory.h>
extern unsigned char inputpal[48];
unsigned char getclosestcolor(short r, short g, short b);



int plotPixel(unsigned char top, unsigned char bottom, char* buffer, int* bufferoffset)
{
    return sprintf(buffer + *bufferoffset, "\33[48;5;%d;38;5;%dm▄", top, bottom);
}
int main(int argc, char* argv[])
{
    int w, h;
    struct winsize term;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &term);
    h = term.ws_row * 2;
    w = term.ws_col;


    ilInit();
    iluInit();
    ilLoadImage(argv[1]);
    int img_w    = ilGetInteger(IL_IMAGE_WIDTH);
    int img_h    = ilGetInteger(IL_IMAGE_HEIGHT);
    int target_w = img_w;
    int target_h = img_h;

    if(img_w > w)
    {
        target_w = w;
        target_h = img_h * (float)target_w / (float)img_w;
    }

    if(target_h > h)
    {
        target_h = h;
        target_w = img_w * (float)target_h / (float)img_h;
    }

    iluImageParameter(ILU_FILTER, ILU_SCALE_MITCHELL);
    iluScale(target_w, target_h, 1);
    unsigned char* data = new unsigned char[target_w * target_h * 3];
    ilCopyPixels(0, 0, 0, target_w, target_h, 1, IL_RGB, IL_UNSIGNED_BYTE, data);
    int bytesused  = 0;
    int buffersize = (target_h / 2 * target_w * 23) + (target_h / 2 * 5) + 1;
    char* buffer   = new char[buffersize];
    memset(buffer, 0, buffersize);
    int bufferoffset = 0;

    short* datadither = new short [ target_w * target_h * 3];
    unsigned char* datadither8bit = new unsigned char [ target_w * target_h ];

    for(int i = 0; i < (target_w * target_h * 3); i++)
    {
        datadither[i] = data[i];
    }

    for(int j = 0; j < target_h; j++)
    {
        for(int i = 0; i < target_w; i++)
        {
#define GETOFFSET(x,y,w) ((x) * 3 + (y) * w * 3)
            unsigned int offset = GETOFFSET(i, j, target_w);
            short r = datadither[offset];
            short g = datadither[offset + 1];
            short b = datadither[offset + 2];
            unsigned char closestcolor = getclosestcolor(r, g, b);
            short new_r = inputpal[(closestcolor) * 3];
            short new_g = inputpal[(closestcolor) * 3 + 1];
            short new_b = inputpal[(closestcolor) * 3 + 2];
            short rquanterror = r - new_r;
            short gquanterror = g - new_g;
            short bquanterror = b - new_b;

            if((i + 1) < target_w)
            {
                unsigned int currentpixel = GETOFFSET(i + 1, j, target_w);
                datadither[currentpixel] +=  rquanterror * (7.0 / 16.0);
                datadither[currentpixel + 1] += gquanterror * (7.0 / 16.0);
                datadither[currentpixel + 2] += bquanterror * (7.0 / 16.0);
            }

            if((i - 1) >= 0 && (j + 1) < target_h)
            {
                unsigned int currentpixel = GETOFFSET(i - 1, j + 1, target_w);
                datadither[currentpixel] +=  rquanterror * (3.0 / 16.0);
                datadither[currentpixel + 1] += gquanterror * (3.0 / 16.0);
                datadither[currentpixel + 2] += bquanterror * (3.0 / 16.0);
            }

            if((j + 1) < target_h)
            {
                unsigned int currentpixel = GETOFFSET(i , j + 1, target_w);
                datadither[currentpixel] +=  rquanterror * (5.0 / 16.0);
                datadither[currentpixel + 1] += gquanterror * (5.0 / 16.0);
                datadither[currentpixel + 2] += bquanterror * (5.0 / 16.0);
            }

            if((i + 1) < target_w && (j + 1) < target_h)
            {
                unsigned int currentpixel = GETOFFSET(i + 1, j + 1, target_w);
                datadither[currentpixel] +=  rquanterror * (1.0 / 16.0);
                datadither[currentpixel + 1] += gquanterror * (1.0 / 16.0);
                datadither[currentpixel + 2] += bquanterror * (1.0 / 16.0);
            }

            datadither8bit[i + j * target_w] = closestcolor;


        }

    }

    for(int j = 0; j < target_h / 2; j++)
    {
        for(int i = 0; i < target_w; i++)
        {
            unsigned int offset = i  + j * 2 * target_w  ;
            unsigned char topcolor = datadither8bit[offset];
            unsigned char bottomcolor =  datadither8bit[offset + target_w];

            bytesused = plotPixel(topcolor, bottomcolor, buffer, &bufferoffset);
            bufferoffset += bytesused;

        }

        bytesused = sprintf(buffer + bufferoffset, "\33[0m\n");
        bufferoffset += bytesused;

    }

    fwrite(buffer, bufferoffset, 1, stdout);
    fprintf(stdout, "\33[0m");
    fflush(stdout);
    delete [] data;
    delete [] datadither;
    delete [] datadither8bit;
    delete [] buffer;
    return 0;
}


unsigned char inputpal[48] =
{
    0x00,0x00,0x00,
    0x80,0x00,0x00,
    0x00,0x80,0x00,
    0x80,0x80,0x00,
    0x00,0x00,0x80,
    0x80,0x00,0x80,
    0x00,0x80,0x80,
    0xc0,0xc0,0xc0,
    0x80,0x80,0x80,
    0xff,0x00,0x00,
    0x00,0xff,0x00,
    0xff,0xff,0x00,
    0x00,0x00,0xff,
    0xff,0x00,0xff,
    0x00,0xff,0xff,
    0xff,0xff,0xff,
};

unsigned char getclosestcolor(short r, short g, short b)
{
    unsigned char closest = 0;
    float closestdist = 10000000.0f;

    for(unsigned short i = 0; i < 16; i++)
    {
        short rdist = r - inputpal[i * 3];
        short gdist = g - inputpal[i * 3 + 1];
        short bdist = b - inputpal[i * 3 + 2];
        float dist  = rdist * rdist + gdist * gdist + bdist * bdist;

        if(dist < closestdist)
        {
            closestdist = dist;
            closest = i;
        }
    }

    return closest;
}
