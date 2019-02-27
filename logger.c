//
// Created by root on 26.02.19.
//

#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

    static FILE* logger= NULL;

    ngx_int_t init_logger()
    {
        logger = fopen("/home/mugutdinov/my_loger.txt", "w");
        if (logger == NULL)
        {
            printf("Error opening file!\n");
            return 1;
    //        exit(1);
        }
        return 0;
    }

    void write_log(const char* str)
    {
        fprintf(stderr, "%sLOGGGGGGGGGGGGGGGGGGGGGG", str);
        fprintf(logger, "%sLOGGGGGGGGGGGGGGGGGGGGGG", str);
        fprintf(logger, "%s", str);
    }