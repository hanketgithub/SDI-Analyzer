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
#include <vector>


#include "parser.h"

//#define SDI_BUF_SIZE    1000000

// With blanking lines
//#define SDI_BUF_SIZE    (8800 * 1125)
// Without blanking lines
#define SDI_BUF_SIZE    (8800 * 1080)


using namespace std;

typedef long long LL;


int main(int argc, char *argv[])
{
    int ifd;

    ssize_t rd_sz;


    if (argc < 2)
    {
        fprintf(stderr, "useage: %s [input_file]\n", argv[0]);
        return -1;
    }

    rd_sz       = 0;

    // get input file name from comand line
    ifd = open(argv[1], O_RDONLY);
    if (ifd < 0)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    uint8_t *sdi_buf = (uint8_t *) calloc(1, SDI_BUF_SIZE);

    fprintf(stderr, "Processing: ");

    vector< SAV_T > savs;
    vector< EAV_T > eavs;
    do
    {
        rd_sz = read(ifd, sdi_buf, SDI_BUF_SIZE);
        if (rd_sz == SDI_BUF_SIZE)
        {
            parse(sdi_buf, SDI_BUF_SIZE, savs, eavs);

            for (auto sav : savs) {
                
            }
        }
        else
        {
            break;
        }
    } while (0);

    close(ifd);

    fprintf(stderr, "Done\n");

    return 0;
}

