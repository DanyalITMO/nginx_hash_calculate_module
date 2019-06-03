//
// Created by root on 25.02.19.
//

#include "../include/utils.h"

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

size_t getTotalMemory() {
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

void releaseMemory(size_t need_memory) {
    list_node_t *node = NULL;
    size_t free_memory = getTotalMemory() - getTotalMemoryUsage();
    while (need_memory > free_memory) {
        node = list_lpop(list);
        free_memory += node->size;
    }
}

uint8_t checkFreeMemoryEnough(size_t file_size) {
    return file_size > (getTotalMemory() - getTotalMemoryUsage()) ? 0 : 1;
}


long loadFile(char *file) {
    FILE *fp;
    fp = fopen(file, "rb");

    if (fp == NULL) {
        fprintf(stderr, "can not read %s\n", file);
        perror("system error");
    }
    long size = fsize(fp);

//    fprintf(stderr, "!SIZE file =%lu %s\n", size, "!!!");
//    fprintf(stderr, "!POOL SIZE  =%lu %s\n", pool.size, "!!!");


    if (getTotalMemory() < (size_t) size) {
        fprintf(stderr, "have not need memory \n");
        return -1;
    }

    if (0 == checkFreeMemoryEnough((size_t) size)) {
        fprintf(stderr, "!have not need free memory\n");
        releaseMemory(size);
    }

    size_t n = fread(pool.p, 1, size, fp);
    if (n == 0) {
        fprintf(stderr, "!can not read data\n");
        return 0;
    }
//    fprintf(stderr, "real read byte %lu \n", n);
    fclose(fp);
    return size;
}

void printMemoryState() {
    fprintf(stderr, "Memory state");
    list_node_t *node = NULL;
    list_iterator_t *it = list_iterator_new(list, LIST_HEAD);
    while ((node = list_iterator_next(it))) {
        fprintf(stderr, "%s, %lu\n", node->file, node->size);
    }
    fprintf(stderr, "Free memory %lu\n", getTotalMemory() - getTotalMemoryUsage());

}

int getProcessTime(ngx_http_request_t *r) {


    char buf_for_args[400];
    char *file = getPath(r->args_start, buf_for_args);

    list_node_t *node = find(list, file);

    if (node != NULL) {
        fprintf(stderr, "######hit the cache\n");
        fprintf(stderr, "size = %lu, hash = %lu, filename = %s, calculate_time = %d\n", node->size, node->hash,
                node->file, node->calculta_time);
        usleep(node->calculta_time * 1000);
        return node->calculta_time;
//        return node->hash;
    }

    fprintf(stderr, "######cache miss\n");

    clock_t start = clock();
    long size = loadFile(file);
    clock_t end = clock();

    if (size < 0)
        return -1;
    uint16_t load_time = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);

    fprintf(stderr, "%d msec used time for load \n", load_time);

    start = clock();
    hash_t hash_value = hash(pool.p, (size_t) size * 0.04);
    end = clock();
    int calculta_time = ((double) (end - start)) / (CLOCKS_PER_SEC / 1000);
    fprintf(stderr, "hash = %lu , %d msec used time for calculate \n", (long unsigned) hash_value, calculta_time);

    list_node_t *a = list_node_new(size, hash_value, file, calculta_time);
    list_rpush(list, a);

    printMemoryState();

    return calculta_time + load_time;
}


hash_t
hash(uint8_t *str, size_t size) {
    unsigned long hash = 5381;
    int c;

    for (size_t i = 0; i < size; i++) {
        c = *str++;
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

int to_string(long number, char *str) {
    const int n = snprintf(NULL, 0, "%lu", number);
    str = malloc(n + 1);
    int c = snprintf((char *) str, n + 1, "%lu", number);
    return c;
}
