/**
 * @file   ngx_http_hello_world_module.c
 * @author António P. P. Almeida <appa@perusio.net>
 * @date   Wed Aug 17 12:06:52 2011
 *
 * @brief  A hello world module for Nginx.
 *
 * @section LICENSE
 *
 * Copyright (C) 2011 by Dominic Fallows, António P. P. Almeida <appa@perusio.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#define make_str( bar ) # bar
#define HELLO_WORLD(x) "<!DOCTYPE html>                \
<html>                          \
<head>                          \
<title> \
Basic Web Page</title>                           \
</head>                         \
<body>"                          \
        make_str(x) \
                            \
"</body>                         \
</html>"

//#define HELLO_WORLD  "hello world"

static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r);

/**
 * This module provided directive: hello world.
 *
 */
static ngx_command_t ngx_http_hello_world_commands[] = {

    { ngx_string("hello_world"), /* directive */
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS, /* location context and takes
                                            no arguments*/
      ngx_http_hello_world, /* configuration setup function */
      0, /* No offset. Only one context is supported. */
      0, /* No offset when storing the module configuration on struct. */
      NULL},

    ngx_null_command /* command termination */
};

/* The hello world string. */
static u_char ngx_hello_world[300] = HELLO_WORLD(100);

/* The module context. */
static ngx_http_module_t ngx_http_hello_world_module_ctx = {
    NULL, /* preconfiguration */
    NULL, /* postconfiguration */

    NULL, /* create main configuration */
    NULL, /* init main configuration */

    NULL, /* create server configuration */
    NULL, /* merge server configuration */

    NULL, /* create location configuration */
    NULL /* merge location configuration */
};

/* Module definition. */
ngx_module_t ngx_http_hello_world_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_world_module_ctx, /* module context */
    ngx_http_hello_world_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    NULL, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

typedef unsigned long hash_t;

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
long fsize(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}
/**
 * Content handler.
 *
 * @param r
 *   Pointer to the request structure. See http_request.h.
 * @return
 *   The status of the response generation.
 */


int to_string(long number, char* str)
{
    const int n = snprintf(NULL, 0, "%lu", number );
//    char hash_string[n+1];
    str = malloc(n+1);
    int c = snprintf((char*)str, n+1, "%lu", number );
    return c;
}

hash_t calculate(ngx_http_request_t *r)
{
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!in calculate");

    FILE *fp;

    ///usr/local/man/man1/scapy.1
    char* file = "/home/mugutdinov/Anaconda3-5.1.0-Linux-x86_64.sh";
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!open file");
//    ngx_file_s file;
//    ngx_read_file()
    fp = fopen(file, "rb");

//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, file);

    if(fp == NULL) {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!fp == NULL");
//        perror("fopen for Users.txt for read/write failed");
    }
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!calculate size");
    long size = fsize(fp);

    fprintf(stderr, "%lu/////////////////////////////", size);

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

static u_char hash_string[300];

static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r)
{
    ngx_buf_t *b;
    ngx_chain_t out;

    char buf[300];
    ngx_str_set(&r->args, buf);

    char buf_for_args[400];
    strcpy(buf_for_args, (char*)r->args_start);
    char * pch = strtok (buf_for_args,"=");
    while (pch != NULL)                         // пока есть лексемы
    {
        ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, pch);
        pch = strtok (NULL, "=");
    }
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!! args");
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, buf);
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, (char*)r->args_start);
    /* Set the Content-Type header. */
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";

    /* Allocate a new buffer for sending out the reply. */
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    /* Insertion in the buffer chain. */
    out.buf = b;
    out.next = NULL; /* just one buffer */

    hash_t hash = calculate(r);

//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "!!!!!!!!!!!convert to char str");

    const int n = snprintf(NULL, 0, "%lu", hash );
//    char hash_string[n+1];
    int c = snprintf((char*)hash_string, n+1, "%lu", hash );
    fprintf(stderr, "c = %d \n", c);
//    fprintf(stderr, "2hash = %lu \n", (long unsigned)hash);
//    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, (char*)&hash_string);

    b->pos = hash_string; /* first position in memory of the data */
    b->last = hash_string + sizeof(hash_string); /* last position in memory of the data */
    b->memory = 1; /* content is in read-only memory */
    b->last_buf = 1; /* there will be no more buffers in the request */

    /* Sending the headers for the reply. */
    r->headers_out.status = NGX_HTTP_OK; /* 200 status code */
    /* Get the content length of the body. */
    r->headers_out.content_length_n = sizeof(ngx_hello_world);
    ngx_http_send_header(r); /* Send the headers */

    /* Send the body, and return the status code of the output filter chain. */
    return ngx_http_output_filter(r, &out);
} /* ngx_http_hello_world_handler */

/**
 * Configuration setup function that installs the content handler.
 *
 * @param cf
 *   Module configuration structure pointer.
 * @param cmd
 *   Module directives structure pointer.
 * @param conf
 *   Module configuration structure pointer.
 * @return string
 *   Status of the configuration setup.
 */
static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *clcf; /* pointer to core location configuration */
    /* Install the hello world handler. */
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_hello_world_handler;


    ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                       "!!!!!init success");
    return NGX_CONF_OK;
} /* ngx_http_hello_world */
