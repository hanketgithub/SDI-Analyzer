//
//  main.cpp
//  sdi
//
//  Created by Hank Lee on 5/31/23.
//  Copyright (c) 2023 Hank Lee. All rights reserved.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "parser.h"

using namespace std;

int main(int argc, char *argv[])
{
    int ifd;
    int ofd;

    ssize_t rd_sz;

    uint32_t width;
    uint32_t height;
    uint32_t wxh;

    char *cp;
    char output[256] = { 0 };

    uint32_t frame_cnt;


    if (argc < 4)
    {
        fprintf(stderr, "useage: %s [input_file] [width] [height]\n", argv[0]);        
        return -1;
    }

    rd_sz       = 0;
    width       = 0;
    height      = 0;
    wxh         = 0;
    cp          = NULL;
    frame_cnt   = 0;

    // get input file name from comand line
    ifd = open(argv[1], O_RDONLY);
    if (ifd < 0)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    // specify output file name
    cp = strrchr(argv[1], '.');
    strncpy(output, argv[1], cp - argv[1]);
    strcat(output, "_sdi");
    strcat(output, ".bin");

    ofd = open
            (
             output,
             O_WRONLY | O_CREAT | O_TRUNC,
             S_IRUSR
            );

    width   = atoi(argv[2]);
    height  = atoi(argv[3]);

    wxh = width * height;

    uint8_t *img = (uint8_t *) calloc(1, wxh * 3 / 2);

#if 0
    //
    // 2 x EAV + 2 x Blank + 2 x SAV + UYVY, all in 10bits
    // I can confirm EAV & SAV needs to be doubled, not sure if blank also needs that.
    //
    uint32_t stride = get_stride(width);
    uint8_t *outputBuf = (uint8_t *) calloc(1, stride * height);

    printf("stride=%d output frame size=%d\n", stride, stride * height);

    fprintf(stderr, "Processing: ");

    while (1)
    {
        rd_sz = read(ifd, img, wxh * 3 / 2);
        if (rd_sz == wxh * 3 / 2)
        {
            convert_yuv420_1080_to_SDI(outputBuf, img);

            write(ofd, outputBuf, stride * height);

            frame_cnt++;
        }
        else
        {
            break;
        }
        fputc('.', stderr);
        fflush(stderr);
    }
#endif

    close(ifd);
    close(ofd);

    fprintf(stderr, "Done\n");
    fprintf(stderr, "Output file: %s\n", output);

    return 0;
}

