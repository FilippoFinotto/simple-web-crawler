#include "http.h"

#define INITIAL_CAPACITY 4096

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
	CURL* my_handle = curl_easy_init();
	if(my_handle == NULL){
		printf("Errore di inizializzazione dell'handle\n");
		return NULL;
	}

	HttpData *data = malloc(sizeof(HttpData));
	if(data == NULL){
                printf("Errore nell'allocazione della memoria per puntatore ai dati");
                return NULL;
        }

	data->memory = malloc(INITIAL_CAPACITY);
	if(data->memory == NULL){
                printf("Errore nell'allocazione della memoria per i dati");
                return NULL;
        }

	data->size = 0;
	data->capacity = INITIAL_CAPACITY;
	
	CURLcode res;

	res = curl_easy_setopt(my_handle, CURLOPT_URL, url);
	if (res != CURLE_OK) {
	    // Gestisci l'errore
	}

	res = curl_easy_setopt(my_handle, CURLOPT_WRITEFUNCTION, write_callback);
	if (res != CURLE_OK) {
	    // Gestisci l'errore
	}
	
	res = curl_easy_setopt(my_handle, CURLOPT_WRITEDATA, (void *)data);
	if (res != CURLE_OK) {
    	// Gestisci l'errore
	}

	const char* user_agent = "MioCrawler/1.0";
	res = curl_easy_setopt(my_handle, CURLOPT_USERAGENT, user_agent);
	if (res != CURLE_OK) {
   	 // Gestisci l'errore
	}

	res = curl_easy_setopt(my_handle, CURLOPT_FOLLOWLOCATION, 1L);
	if (res != CURLE_OK) {
    	// Gestisci l'errore
	}

	res = curl_easy_setopt(my_handle, CURLOPT_TIMEOUT, 15L); // Timeout di 15 secondi
	if (res != CURLE_OK) {
    		// Gestisci l'errore
	}


	res = curl_easy_perform(my_handle);
	if (res != CURLE_OK) {
		printf("Errore\n");
		free(data->memory);
		free(data);
		curl_easy_cleanup(my_handle);
		return NULL;
		
	}else {
		if (data->size == data->capacity) {
			data->capacity++;
			data->memory = realloc(data->memory, data->capacity);
			if (data->memory == NULL) {
				printf("Errore nella riallocazione della memoria per l'aggiunta del carattere di terminazione");
				return NULL;
			}

		}
		data->memory[data->size] = '\0';
		return data;
	}
}



void free_http_data(HttpData *data) {
	if (data == NULL) {
		printf("Errore: nessun dato\n");
		return;
	}
	free(data->memory);
	free(data);
}


































