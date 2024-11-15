#include <stdio.h>
#include <stdlib.h>
#define CURL_STATICLIB
#include "curl/curl.h" 
#ifdef _DEBUG
#pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#pragma comment (lib, "curl/libcurl_a.lib")
#endif
#pragma comment (lib, "Normaliz.lib")
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wldap32.lib")
#pragma comment (lib, "advapi32.lib")
#include "cJSON.h"
#include "source3.h"

// Structure to hold API response as a string
struct string {
    char* ptr;
    size_t len;
};

// Initialize the string structure
void init_string(struct string* s) {
    s->len = 0;
    s->ptr = malloc(1);
    if (s->ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
    s->ptr[0] = '\0';
}

// Callback function for cURL to write data into the response string
size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s) {
    size_t new_len = s->len + size * nmemb;
    char* temp = realloc(s->ptr, new_len + 1);
    if (temp == NULL) {
        fprintf(stderr, "Failed to reallocate memory.\n");
        free(s->ptr);
        return 0;
    }
    s->ptr = temp;
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

// Function to fetch temperature data from API using cJSON
EnvData get_environmental_data() {
    CURL* curl;
    CURLcode res;
    struct string s;
    EnvData envData = { 0.0 }; // Only storing temperature

    init_string(&s);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://api.openweathermap.org/data/2.5/weather?q=London&appid=731524b22f07919f29af17af4f147e56&units=metric");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        else {
            // Parse JSON response using cJSON
            cJSON* root = cJSON_Parse(s.ptr);
            if (root) {
                cJSON* main = cJSON_GetObjectItem(root, "main");
                if (main) {
                    cJSON* temp = cJSON_GetObjectItem(main, "temp");
                    if (temp) {
                        envData.temperature = (float)temp->valuedouble;
                    }
                    else {
                        fprintf(stderr, "Temperature data not found in JSON.\n");
                    }
                }
                else {
                    fprintf(stderr, "Main data not found in JSON.\n");
                }
                cJSON_Delete(root);
            }
            else {
                fprintf(stderr, "Failed to parse JSON response.\n");
            }
        }

        free(s.ptr);
        curl_easy_cleanup(curl);
    }
    else {
        fprintf(stderr, "Failed to initialize cURL.\n");
    }

    return envData;
}
