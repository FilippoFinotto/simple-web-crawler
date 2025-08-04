#ifndef PARSER_H
#define PARSER_H

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gumbo.h>

Queue* find_links(const char *html_buffer, const char *base_url);

#endif
