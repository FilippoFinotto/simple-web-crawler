/* This Module is responsible for dowloading the content of a given url using the library libcurl */


#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

/* This is the struct for keeping the downloaded data*/

typedef struct {
    char* memory; // pointer to the memory block where we store the data
    size_t size; // bites accumulated
    size_t capacity; // current capacity of our memory block
} HttpData;

HttpData* download_page(const char* url);
void free_http_data(HttpData* data); 

#endif



































































