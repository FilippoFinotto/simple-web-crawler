#ifndef PARSER_H
#define PARSER_H

#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gumbo.h>

/* This module will parse the downloaded html documents and find the new links to add to the frontier.*/

Queue* find_links(char *html_buffer, char *base_url);

#endif
