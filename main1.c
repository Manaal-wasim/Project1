#include <stdio.h>
#include <stdlib.h>
#include "source3.h"  
#include <string.h>
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
int main() {
    // Retrieve environmental data (temperature)
    EnvData env = get_environmental_data();

    // Check if the temperature was successfully retrieved
    if (env.temperature != 0.0) {
        // Print the temperature data
        printf("Current temperature: %.2f°C\n", env.temperature);
    }
    else {
        // In case of failure to retrieve data
        printf("Failed to retrieve temperature data.\n");
    }

    return 0;
}
