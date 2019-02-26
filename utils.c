//
// Created by root on 25.02.19.
//

#include "utils.h"

hash_t calculate(ngx_http_request_t *r)
{
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!in calculate");

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

    fprintf(stderr, "%lu !!!!!SIZE file =", size);

    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!convert to str");
    char * str = NULL;
    int rc = to_string(size, str);
    if(rc < 0)     ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "rc < 0");
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!SIZE");
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, str); // dont work
    uint8_t* fcontent = ngx_pcalloc(r->pool, size);// malloc(size);

    if(fcontent == NULL)
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!can not allocate");
    } else
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!CAN allocate");
    }
    //read
    clock_t start = clock();
    size_t n = fread(fcontent, 1, size, fp);
    if(n == 0)
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!can not read data");
    clock_t end = clock();
    uint16_t cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    fprintf(stderr, "%d msec used time for read \n", cpu_time_used);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!read data");


    //calculate
    start = clock();
    hash_t hash_value = size < 100? hash(fcontent, size): hash(fcontent, 100);
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
