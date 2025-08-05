#ifndef PARSER_H
#define PARSER_H

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gumbo.h>

Queue* find_links(char *html_buffer, char *base_url);

#endif
