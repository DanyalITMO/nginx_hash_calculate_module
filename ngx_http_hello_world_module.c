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

#include "utils.h"


static char *ngx_http_hello_world(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r);

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

struct mempool pool;

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


ngx_int_t init_module(ngx_cycle_t *cycle)
{
    pool.size = 5000;
    pool.p = (u_char*) malloc(pool.size);
    fprintf(stderr, "*******************INIT MODULE\n");
    return 0;
}


void exit_master(ngx_cycle_t *cycle)
{
    fprintf(stderr, "*******************EXIT MASTER\n");
}


/*
 * Configuration directive handlers are called as they appear in configuration files in the context of the master process.
After the configuration is parsed successfully, init_module handler is called in the context of the master process. The init_module handler is called in the master process each time a configuration is loaded.
The master process creates one or more worker processes and the init_process handler is called in each of them.
When a worker process receives the shutdown or terminate command from the master, it invokes the exit_process handler.
The master process calls the exit_master handler before exiting.
Because threads are used in nginx only as a supplementary I/O facility with its own API, init_thread and exit_thread handlers are not currently called. There is also no init_master handler, because it would be unnecessary overhead.
 * */
/* Module definition. */
ngx_module_t ngx_http_hello_world_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_world_module_ctx, /* module context */
    ngx_http_hello_world_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    init_module, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    exit_master, /* exit master */
    NGX_MODULE_V1_PADDING
};



static u_char hash_string[300];

static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t *r)
{
    ngx_buf_t *b;
    ngx_chain_t out;


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
    r->headers_out.content_length_n = sizeof(hash_string);
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
