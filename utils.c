//
// Created by root on 25.02.19.
//

#include "utils.h"
#include "logger.h"

//static char str[200];// = NULL;

hash_t calculate(ngx_http_request_t *r)
{
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!in calculate");


    FILE *fp;

    ///usr/local/man/man1/scapy.1

    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!! args\n");

    char buf_for_args[400];
    strcpy(buf_for_args, (char*)r->args_start);

    char * file = strtok (buf_for_args,"=");//skip first ("path" word)
    file = strtok (NULL, "= ");
    fp = fopen(file, "rb");

    if(fp == NULL) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!fp == NULL");
        perror("fopen for Users.txt for read/write failed");
    }
    long size = fsize(fp);

    fprintf(stderr, "!!!!!SIZE file =%lu %s", size, "!!!");
    fprintf(stderr, "!!!!!POOL SIZE file =%lu %s", pool.size, "!!!");

    clock_t start = clock();
    if((size_t)size > pool.size)
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!have not need memory");
        return 0;
    }

    size_t n = fread(pool.p, 1, size, fp);
    if(n == 0) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!can not read data");
        return 0;
    }

    pool.last += n;

    clock_t end = clock();
    uint16_t cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    fprintf(stderr, "%d msec used time for read \n", cpu_time_used);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!read data");


    //calculate
    start = clock();
    hash_t hash_value = size < 100? hash(pool.p, size): hash(pool.p, 100);
    end = clock();
    cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    fprintf(stderr, "hash = %lu , %d msec used time for calculate \n", (long unsigned)hash_value, cpu_time_used);

    return hash_value;

}


hash_t
hash(uint8_t *str, size_t size)
{
    unsigned long hash = 5381;
    int c;

//    while (c = *str++)
    for(size_t i  = 0; i < size; i++) {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int to_string(long number, char* str)
{
    const int n = snprintf(NULL, 0, "%lu", number );
//    char hash_string[n+1];
    str = malloc(n+1);
    int c = snprintf((char*)str, n+1, "%lu", number );
    return c;
}
