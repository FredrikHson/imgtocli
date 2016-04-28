#!/bin/bash
outputfile=$(mktemp -u)
cat $0 | tail -n +6 | /usr/bin/c++ -I$(dirname $0) -lIL -lILU -xc++ - -o $outputfile && $outputfile $@
rm $outputfile
exit 0
#include <stdio.h>
#include <stdlib.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>

unsigned char getclosestcolor(unsigned char r, unsigned char g, unsigned char b);
void plotPixel(unsigned char top, unsigned char bottom)
{
    printf("\e[48;5;%d;38;5;%dm▄", top, bottom);
}
unsigned int getoffset(unsigned int x, unsigned int y, unsigned int w)
{
    return x * 3 + y * w * 3;
}
unsigned char getColor(unsigned int offset, unsigned char* data)
{
    unsigned char r = data[offset];
    unsigned char g = data[offset + 1];
    unsigned char b = data[offset + 2];
    return getclosestcolor(r, g, b);
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
    iluImageParameter(ILU_FILTER,ILU_SCALE_MITCHELL);
    iluScale(target_w, target_h, 1);
    unsigned char* data = new unsigned char[target_w * target_h * 3];
    ilCopyPixels(0, 0, 0, target_w, target_h, 1, IL_RGB, IL_UNSIGNED_BYTE, data);

    for(int j = 0; j < target_h / 2; j++)
    {
        for(int i = 0; i < target_w; i++)
        {
            unsigned char topcolor = getColor(getoffset(i, j * 2, target_w), data);
            unsigned char bottomcolor = getColor(getoffset(i, j * 2 + 1, target_w), data);

            plotPixel(topcolor, bottomcolor);

        }

        printf("\n");

    }

    printf("\e[0m");
    delete [] data;
    return 0;
}


unsigned char inputpal[720] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x87,
    0x00, 0x00, 0xaf, 0x00, 0x00, 0xd7, 0x00, 0x00, 0xff,
    0x00, 0x5f, 0x00, 0x00, 0x5f, 0x5f, 0x00, 0x5f, 0x87,
    0x00, 0x5f, 0xaf, 0x00, 0x5f, 0xd7, 0x00, 0x5f, 0xff,
    0x00, 0x87, 0x00, 0x00, 0x87, 0x5f, 0x00, 0x87, 0x87,
    0x00, 0x87, 0xaf, 0x00, 0x87, 0xd7, 0x00, 0x87, 0xff,
    0x00, 0xaf, 0x00, 0x00, 0xaf, 0x5f, 0x00, 0xaf, 0x87,
    0x00, 0xaf, 0xaf, 0x00, 0xaf, 0xd7, 0x00, 0xaf, 0xff,
    0x00, 0xd7, 0x00, 0x00, 0xd7, 0x5f, 0x00, 0xd7, 0x87,
    0x00, 0xd7, 0xaf, 0x00, 0xd7, 0xd7, 0x00, 0xd7, 0xff,
    0x00, 0xff, 0x00, 0x00, 0xff, 0x5f, 0x00, 0xff, 0x87,
    0x00, 0xff, 0xaf, 0x00, 0xff, 0xd7, 0x00, 0xff, 0xff,
    0x5f, 0x00, 0x00, 0x5f, 0x00, 0x5f, 0x5f, 0x00, 0x87,
    0x5f, 0x00, 0xaf, 0x5f, 0x00, 0xd7, 0x5f, 0x00, 0xff,
    0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x5f, 0x5f, 0x5f, 0x87,
    0x5f, 0x5f, 0xaf, 0x5f, 0x5f, 0xd7, 0x5f, 0x5f, 0xff,
    0x5f, 0x87, 0x00, 0x5f, 0x87, 0x5f, 0x5f, 0x87, 0x87,
    0x5f, 0x87, 0xaf, 0x5f, 0x87, 0xd7, 0x5f, 0x87, 0xff,
    0x5f, 0xaf, 0x00, 0x5f, 0xaf, 0x5f, 0x5f, 0xaf, 0x87,
    0x5f, 0xaf, 0xaf, 0x5f, 0xaf, 0xd7, 0x5f, 0xaf, 0xff,
    0x5f, 0xd7, 0x00, 0x5f, 0xd7, 0x5f, 0x5f, 0xd7, 0x87,
    0x5f, 0xd7, 0xaf, 0x5f, 0xd7, 0xd7, 0x5f, 0xd7, 0xff,
    0x5f, 0xff, 0x00, 0x5f, 0xff, 0x5f, 0x5f, 0xff, 0x87,
    0x5f, 0xff, 0xaf, 0x5f, 0xff, 0xd7, 0x5f, 0xff, 0xff,
    0x87, 0x00, 0x00, 0x87, 0x00, 0x5f, 0x87, 0x00, 0x87,
    0x87, 0x00, 0xaf, 0x87, 0x00, 0xd7, 0x87, 0x00, 0xff,
    0x87, 0x5f, 0x00, 0x87, 0x5f, 0x5f, 0x87, 0x5f, 0x87,
    0x87, 0x5f, 0xaf, 0x87, 0x5f, 0xd7, 0x87, 0x5f, 0xff,
    0x87, 0x87, 0x00, 0x87, 0x87, 0x5f, 0x87, 0x87, 0x87,
    0x87, 0x87, 0xaf, 0x87, 0x87, 0xd7, 0x87, 0x87, 0xff,
    0x87, 0xaf, 0x00, 0x87, 0xaf, 0x5f, 0x87, 0xaf, 0x87,
    0x87, 0xaf, 0xaf, 0x87, 0xaf, 0xd7, 0x87, 0xaf, 0xff,
    0x87, 0xd7, 0x00, 0x87, 0xd7, 0x5f, 0x87, 0xd7, 0x87,
    0x87, 0xd7, 0xaf, 0x87, 0xd7, 0xd7, 0x87, 0xd7, 0xff,
    0x87, 0xff, 0x00, 0x87, 0xff, 0x5f, 0x87, 0xff, 0x87,
    0x87, 0xff, 0xaf, 0x87, 0xff, 0xd7, 0x87, 0xff, 0xff,
    0xaf, 0x00, 0x00, 0xaf, 0x00, 0x5f, 0xaf, 0x00, 0x87,
    0xaf, 0x00, 0xaf, 0xaf, 0x00, 0xd7, 0xaf, 0x00, 0xff,
    0xaf, 0x5f, 0x00, 0xaf, 0x5f, 0x5f, 0xaf, 0x5f, 0x87,
    0xaf, 0x5f, 0xaf, 0xaf, 0x5f, 0xd7, 0xaf, 0x5f, 0xff,
    0xaf, 0x87, 0x00, 0xaf, 0x87, 0x5f, 0xaf, 0x87, 0x87,
    0xaf, 0x87, 0xaf, 0xaf, 0x87, 0xd7, 0xaf, 0x87, 0xff,
    0xaf, 0xaf, 0x00, 0xaf, 0xaf, 0x5f, 0xaf, 0xaf, 0x87,
    0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xd7, 0xaf, 0xaf, 0xff,
    0xaf, 0xd7, 0x00, 0xaf, 0xd7, 0x5f, 0xaf, 0xd7, 0x87,
    0xaf, 0xd7, 0xaf, 0xaf, 0xd7, 0xd7, 0xaf, 0xd7, 0xff,
    0xaf, 0xff, 0x00, 0xaf, 0xff, 0x5f, 0xaf, 0xff, 0x87,
    0xaf, 0xff, 0xaf, 0xaf, 0xff, 0xd7, 0xaf, 0xff, 0xff,
    0xd7, 0x00, 0x00, 0xd7, 0x00, 0x5f, 0xd7, 0x00, 0x87,
    0xd7, 0x00, 0xaf, 0xd7, 0x00, 0xd7, 0xd7, 0x00, 0xff,
    0xd7, 0x5f, 0x00, 0xd7, 0x5f, 0x5f, 0xd7, 0x5f, 0x87,
    0xd7, 0x5f, 0xaf, 0xd7, 0x5f, 0xd7, 0xd7, 0x5f, 0xff,
    0xd7, 0x87, 0x00, 0xd7, 0x87, 0x5f, 0xd7, 0x87, 0x87,
    0xd7, 0x87, 0xaf, 0xd7, 0x87, 0xd7, 0xd7, 0x87, 0xff,
    0xd7, 0xaf, 0x00, 0xd7, 0xaf, 0x5f, 0xd7, 0xaf, 0x87,
    0xd7, 0xaf, 0xaf, 0xd7, 0xaf, 0xd7, 0xd7, 0xaf, 0xff,
    0xd7, 0xd7, 0x00, 0xd7, 0xd7, 0x5f, 0xd7, 0xd7, 0x87,
    0xd7, 0xd7, 0xaf, 0xd7, 0xd7, 0xd7, 0xd7, 0xd7, 0xff,
    0xd7, 0xff, 0x00, 0xd7, 0xff, 0x5f, 0xd7, 0xff, 0x87,
    0xd7, 0xff, 0xaf, 0xd7, 0xff, 0xd7, 0xd7, 0xff, 0xff,
    0xff, 0x00, 0x00, 0xff, 0x00, 0x5f, 0xff, 0x00, 0x87,
    0xff, 0x00, 0xaf, 0xff, 0x00, 0xd7, 0xff, 0x00, 0xff,
    0xff, 0x5f, 0x00, 0xff, 0x5f, 0x5f, 0xff, 0x5f, 0x87,
    0xff, 0x5f, 0xaf, 0xff, 0x5f, 0xd7, 0xff, 0x5f, 0xff,
    0xff, 0x87, 0x00, 0xff, 0x87, 0x5f, 0xff, 0x87, 0x87,
    0xff, 0x87, 0xaf, 0xff, 0x87, 0xd7, 0xff, 0x87, 0xff,
    0xff, 0xaf, 0x00, 0xff, 0xaf, 0x5f, 0xff, 0xaf, 0x87,
    0xff, 0xaf, 0xaf, 0xff, 0xaf, 0xd7, 0xff, 0xaf, 0xff,
    0xff, 0xd7, 0x00, 0xff, 0xd7, 0x5f, 0xff, 0xd7, 0x87,
    0xff, 0xd7, 0xaf, 0xff, 0xd7, 0xd7, 0xff, 0xd7, 0xff,
    0xff, 0xff, 0x00, 0xff, 0xff, 0x5f, 0xff, 0xff, 0x87,
    0xff, 0xff, 0xaf, 0xff, 0xff, 0xd7, 0xff, 0xff, 0xff,
    0x08, 0x08, 0x08, 0x12, 0x12, 0x12, 0x1c, 0x1c, 0x1c,
    0x26, 0x26, 0x26, 0x30, 0x30, 0x30, 0x3a, 0x3a, 0x3a,
    0x44, 0x44, 0x44, 0x4e, 0x4e, 0x4e, 0x58, 0x58, 0x58,
    0x60, 0x60, 0x60, 0x66, 0x66, 0x66, 0x76, 0x76, 0x76,
    0x80, 0x80, 0x80, 0x8a, 0x8a, 0x8a, 0x94, 0x94, 0x94,
    0x9e, 0x9e, 0x9e, 0xa8, 0xa8, 0xa8, 0xb2, 0xb2, 0xb2,
    0xbc, 0xbc, 0xbc, 0xc6, 0xc6, 0xc6, 0xd0, 0xd0, 0xd0,
    0xda, 0xda, 0xda, 0xe4, 0xe4, 0xe4, 0xee, 0xee, 0xee
};

unsigned char getclosestcolor(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char closest = 0;
    float closestdist = 10000000.0f;

    for(unsigned short i = 0; i < 239; i ++)
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

    return closest + 16;
}