#include "http.h"

#define INITIAL_CAPACITY 4096

/* This is the callback function that is going to be called everytime we recieve a packet. */

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t piece_size = size * nmemb;
        HttpData *buffer = (HttpData *)userp;
	if (buffer->size + piece_size >= buffer->capacity) {
		size_t new_capacity = buffer->capacity;
		while (new_capacity < buffer->size + piece_size) {
			new_capacity *= 2;
		}

        	char* temp_ptr = realloc(buffer->memory, new_capacity);
        
        	if (temp_ptr == NULL) {
            		free(buffer->memory); 
			printf("Errore: riallocazione della memoria fallita\n");
            		return 0;
        	}

        	buffer->memory = temp_ptr;
        	buffer->capacity = new_capacity;
	}
    	memcpy(buffer->memory + buffer->size, contents, piece_size);
    	buffer->size += piece_size;

    	return piece_size;
}

HttpData* download_page(const char *url) {
    	CURL *my_handle = curl_easy_init(); // Obtaining the handle to configure.
    	if (my_handle == NULL) {
        	printf("Errore di inizializzazione dell'handle di CURL\n");
        	return NULL;
    	}

    	HttpData *data = malloc(sizeof(HttpData)); // Allocating the memory for the struct.
    	if (data == NULL) {
        	printf("Errore nell'allocazione della memoria per il puntatore ai dati\n");
        	curl_easy_cleanup(my_handle); 
        	return NULL;
    	}

    	data->memory = malloc(INITIAL_CAPACITY); // Allocating the memory for the data.
    	if (data->memory == NULL) {
		printf("Errore nell'allocazione della memoria per i dati\n");
		free(data); 
		curl_easy_cleanup(my_handle); 
		return NULL;
    	}

	data->size = 0;
	data->capacity = INITIAL_CAPACITY;
	CURLcode res;

#define CLEANUP_AND_FAIL() \
    	do { \
        	free(data->memory); \
        	free(data); \
        	curl_easy_cleanup(my_handle); \
        	return NULL; \
    	} while(0)

    	res = curl_easy_setopt(my_handle, CURLOPT_URL, url);
    	if (res != CURLE_OK) {
        	fprintf(stderr, "curl_easy_setopt(CURLOPT_URL) failed: %s\n", curl_easy_strerror(res));
        	CLEANUP_AND_FAIL();
    	}

    	res = curl_easy_setopt(my_handle, CURLOPT_WRITEFUNCTION, write_callback);
    	if (res != CURLE_OK) {
        	fprintf(stderr, "curl_easy_setopt(CURLOPT_WRITEFUNCTION) failed: %s\n", curl_easy_strerror(res));
        	CLEANUP_AND_FAIL();
    	}

    	res = curl_easy_setopt(my_handle, CURLOPT_WRITEDATA, (void *)data);
    	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_setopt(CURLOPT_WRITEDATA) failed: %s\n", curl_easy_strerror(res));
		CLEANUP_AND_FAIL();
    	}
    
    	const char* user_agent = "MioCrawler/1.0";
    	res = curl_easy_setopt(my_handle, CURLOPT_USERAGENT, user_agent);
    	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_setopt(CURLOPT_USERAGENT) failed: %s\n", curl_easy_strerror(res));
		CLEANUP_AND_FAIL();
    	}

	res = curl_easy_setopt(my_handle, CURLOPT_FOLLOWLOCATION, 1L);
	if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_setopt(CURLOPT_FOLLOWLOCATION) failed: %s\n", curl_easy_strerror(res));
		CLEANUP_AND_FAIL();
	}

    	res = curl_easy_setopt(my_handle, CURLOPT_TIMEOUT, 15L); 
    	if (res != CURLE_OK) {
        	fprintf(stderr, "curl_easy_setopt(CURLOPT_TIMEOUT) failed: %s\n", curl_easy_strerror(res));
        	CLEANUP_AND_FAIL();
    	}

    	res = curl_easy_perform(my_handle);
    	if (res != CURLE_OK) {
        	fprintf(stderr, "Errore durante curl_easy_perform: %s\n", curl_easy_strerror(res));
        	CLEANUP_AND_FAIL();
    	} else {
        char *new_memory = realloc(data->memory, data->size + 1);
        if (new_memory == NULL) {
            	fprintf(stderr, "Errore nella riallocazione della memoria per l'aggiunta del carattere di terminazione\n");
            	CLEANUP_AND_FAIL();
        }
        data->memory = new_memory;
        data->capacity = data->size + 1;
        data->memory[data->size] = '\0';
        
        curl_easy_cleanup(my_handle);
        return data;
    	}

    	CLEANUP_AND_FAIL();
}

void free_http_data(HttpData *data) {
	if (data == NULL) {
		printf("Errore: nessun dato\n");
		return;
	}
	free(data->memory);
	free(data);
}


































