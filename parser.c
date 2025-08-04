#include "parser.h"

/*This function extracts the base url from a complete url, it outputs (es. https://units.it from http://units.it/path
 * it returns an allocated string that must be freed by the caller or NULL if there's an error */

static const char* get_base_domain(const char *base_url){
	if(!base_url) return NULL;

	const char *protocol_end = strstr(base_url, "://");
    	if (!protocol_end) return NULL;

	const char *path_start = strchr(protocol_end + 3, '/');

	size_t domain_len;
	if(path_start != NULL){
		domain_len = path_start - base_url;
	}else {
		domain_len = strlen(base_url);
	}

	const char* domain = malloc(domain_len +1);
	domain[domain_len] = "\0";
	return domain;
}

/*This funtion checks if the href value of the element is a valid url or a relative url:
 * if its relative it adds the relative part to the absolute url
 * it doesn't check for all edge cases. */

const char* rel_to_abs_url(const char *initial_url, const char *base_url){
	if ((strncmp(initial_url, "http://", 7) == 0) || (strncmp(initial_url, "https://", 8) == 0)){
		return strdup(initial_url);

	}
	if (strncmp(initial_url, "/", 1) == 0){
		const char *domain = get_base_domain(base_url);
		if(!domain) return NULL;
		size_t total_len = strlen(base_url) + strlen(initial_url) +1;
		const char *abs_url = malloc(total_len);
		if(!abs_url){
			free(abs_url);
			return NULL;
		}

		strcpy(abs_url, domain);
		strcat(abs_url, initial_url);
		
		free(domain);
		return abs_url;
	}
	return NULL;
}

static int is_valid(const char *url){
	if(!url) return NULL;
	if ((strncmp(initial_url, "http://", 7) == 0) || (strncmp(initial_url, "https://", 8) == 0)){
                return 0;
        }

	const char* extensions_to_avoid[] = {
        ".jpg", ".jpeg", ".png", ".gif", ".pdf", ".zip", ".css", ".js",
        ".mp3", ".mp4", ".avi", ".mov", ".docx", ".xlsx", ".pptx", NULL
	};

	const char *last_dot = strrchr(absolute_url, '.');
	if (last_dot != NULL) {
		if (strchr(last_dot, '/')) {
			return 1;
		}
		for (int i = 0; extensions_to_avoid[i] != NULL; ++i) {
			if (strcmp(last_dot, extensions_to_avoid[i]) == 0) {
				return 0;
			}
		}
	}
	return 1;
}



static void recursive_tree_navigation(GumboNode *node, Queue *link_queue, const char *url base_url) {
	if(!node || node->type != GUMBO_NODE_ELEMENT) return;
	if (node->v.element.tag == GUMBO_TAG_A) {
		GumboAttribute* href_attr = gumbo_get_attribute(&node->v.element.attributes, "href");
		if (href_attr) {
			const char* href_value = href_attr->value;

			char* absolute_url = rel_to_abs_url(href_value, base_url);
			if (absolute_url != NULL) {
				if (is_valid(absolute_url)) {
					enqueue(link_queue, absolute_url);
				}
				free(absolute_url);
			}
		}
	}
	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i) {
		recursive_tree_navigation(children->data[i], link_queue, base_url);
	}


}

Queue* find_links(const char *html_buffer, const char *base_url){
	Queue *link_queue = create_queue();
	if(!link_queue) return NULL;

	GumboOutput *output = gumbo_parse(html_buffer);
	recursive_tree_navigation(output->root, link_queue, base_url);

	gumbo_destroy_output(&kGumboDefaultOptions, output);

	return link_queue;

}



