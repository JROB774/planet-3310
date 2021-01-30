#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_NO_SIMD

#include "STBIMAGE.C"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef uint32_t U32;
typedef uint8_t U8;

int main (int argc, char** argv)
{
    if (argc <= 3) // Arguments needed for the image to pack, the output name, and data name.
    {
        fprintf(stderr, "DATAPACK: Requires both input, output, and name args!\n");
    }
    else
    {
        const char* input = argv[1];
        const char* output = argv[2];
        const char* name = argv[3];

        int w,h,bpp;
        U8* data = stbi_load(input, &w,&h,&bpp,4); // Force to RGBA.
        if (!data)
        {
            fprintf(stderr, "DATAPACK: Failed to load input image!\n");
        }
        else
        {
            if (w % 7 != 0 || h % 8 != 0)
            {
                fprintf(stderr, "DATAPACK: Invalid size for input image!\n");
            }
            else
            {
                FILE* file = fopen(output, "w");
                if (!file)
                {
                    fprintf(stderr, "DATAPACK: Failed to create output file!\n");
                }
                else
                {
                    fprintf(file, "static const U8 %s[] =\n", name);
                    fprintf(file, "{\n");

                    int counter = 0;

                    U32* pixels = (U32*)data;
                    for (int iy=0; iy<h; ++iy)
                    {
                        for (int ix=0; ix<w; ix+=7)
                        {
                            // We pack each row of 7 pixels into a single byte.
                            U8 byte = 0;
                            for (int i=0; i<7; ++i)
                            {
                                if (pixels[iy*w+(ix+i)] == 0xFF000000)
                                {
                                    byte |= (1 << (7-i));
                                }
                            }
                            fprintf(file, "0x%02X,", byte);

                            counter++;
                            if (counter != 0 && counter % 8 == 0)
                            {
                                fprintf(file, "\n");
                            }
                        }
                    }

                    fprintf(file, "};\n");

                    fclose(file);
                }
            }

            stbi_image_free(data);
        }
    }

    return 0;
}
