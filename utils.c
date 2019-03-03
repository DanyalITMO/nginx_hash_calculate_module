//
// Created by root on 25.02.19.
//

#include "utils.h"
#include "logger.h"

//static char str[200];// = NULL;
list_t *list;

list_node_t *find(list_t *list, const char *file) {
    list_node_t *node = NULL;
    list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
    while ((node = list_iterator_next(it))) {
        if (strcmp(node->file, file) == 0)
            return node;
    }
    return NULL;
}

char *getPath(u_char *args, char *buf) {
    strcpy(buf, (char *) args);

    char *file = strtok(buf, "=");//skip first ("path" word)
    file = strtok(NULL, "= ");
    return file;
}

size_t getTotalMemory()
{
    return pool.size;
}
size_t getTotalMemoryUsage() {
    size_t total = 0;
    list_node_t *node;
    list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
    while ((node = list_iterator_next(it))) {
        total += node->size;
    }
    return total;
}

void releaseMemory(size_t need_memory)
{
    list_node_t *node = NULL;
    size_t free_memory = getTotalMemory() - getTotalMemoryUsage();
    while (need_memory > free_memory) {
        node = list_lpop(list);
        free_memory += node->size;
    }
}

uint8_t checkFreeMemoryEnough(size_t file_size) {
    return file_size > (getTotalMemory() - getTotalMemoryUsage()) ? 0: 1;
}

hash_t calculate(ngx_http_request_t *r) {
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!in calculate");

    ///usr/local/man/man1/scapy.1

    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!! args\n");

    char buf_for_args[400];
    char *file = getPath(r->args_start, buf_for_args);
    list_node_t *node = find(list, file);

    if (node != NULL) {
        fprintf(stderr, "######hit the cache\n");
        fprintf(stderr, "size = %lu, hash = %lu, filename = %s\n", node->size, node->hash, node->file);
        return node->hash;
    }
    fprintf(stderr, "######cache miss\n");

    FILE *fp;
    fp = fopen(file, "rb");

    if (fp == NULL) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!fp == NULL");
        perror("fopen for Users.txt for read/write failed");
    }
    long size = fsize(fp);

    fprintf(stderr, "!!!!!SIZE file =%lu %s\n", size, "!!!");
    fprintf(stderr, "!!!!!POOL SIZE  =%lu %s\n", pool.size, "!!!");

    clock_t start = clock();

    if(getTotalMemory() < (size_t) size) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!have not need memory");
        return 0;
    }

    if(0 == checkFreeMemoryEnough((size_t) size))
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!have not need free memory");
        releaseMemory(size);
    }

//    char* buf = (char*) malloc(size);


    size_t n = fread(pool.p, 1, size, fp);
    if (n == 0) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!can not read data");
        return 0;
    }
    fprintf(stderr, "real read byte %lu \n", n);

    clock_t end = clock();
    uint16_t cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    fprintf(stderr, "%d msec used time for read \n", cpu_time_used);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!read data");


    //calculate
    start = clock();
    hash_t hash_value = size < 100 ? hash(pool.p, size) : hash(pool.p, 100);
    end = clock();
    cpu_time_used = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    fprintf(stderr, "hash = %lu , %d msec used time for calculate \n", (long unsigned) hash_value, cpu_time_used);

//    free(buf);
    list_node_t *a = list_node_new(size, hash_value, file);
    list_rpush(list, a);


    return hash_value;

}


hash_t
hash(uint8_t *str, size_t size) {
    unsigned long hash = 5381;
    int c;

//    while (c = *str++)
    for (size_t i = 0; i < size; i++) {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

int to_string(long number, char *str) {
    const int n = snprintf(NULL, 0, "%lu", number);
//    char hash_string[n+1];
    str = malloc(n + 1);
    int c = snprintf((char *) str, n + 1, "%lu", number);
    return c;
}
