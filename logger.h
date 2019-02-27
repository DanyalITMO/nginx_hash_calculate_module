//
// Created by root on 26.02.19.
//

#ifndef NGINX_MODULE_LOGGER_H
#define NGINX_MODULE_LOGGER_H

#include <ngx_config.h>

ngx_int_t init_logger();
void write_log(const char*);
#endif //NGINX_MODULE_LOGGER_H
