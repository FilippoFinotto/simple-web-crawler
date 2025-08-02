#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

typedef struct {
    char* memory;
    size_t size;
    size_t capacity;
} HttpData;

HttpData* download_page(const char* url);
void free_http_data(HttpData* data); 

#endif



































































