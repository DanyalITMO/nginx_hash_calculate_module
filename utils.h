//
// Created by root on 25.02.19.
//

#ifndef NGINX_MODULE_MEM_POOL_H
#define NGINX_MODULE_MEM_POOL_H
#include <ngx_config.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

struct mempool
{
    u_char* p;
    u_char* last;
    size_t  size;
};

//static struct mempool pool;

typedef unsigned long hash_t;

hash_t hash(uint8_t *str, size_t size);

inline long fsize(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

int to_string(long number, char* str);


hash_t calculate(ngx_http_request_t *r);

extern struct mempool pool;
//extern FILE* logger;

#endif //NGINX_MODULE_MEM_POOL_H
