/**
 * Copyright (c) 2012 ooxi/gltoolkit
 *     https://github.com/ooxi/gltoolkit
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the
 * use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software in a
 *     product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 * 
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 */
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <curl/curl.h>

#include "http.h"





/**
 * [OPAQUE API]
 */
struct gl_http_response {
	uint8_t* data;

	size_t response_length;
	size_t buffer_length;
};





/**
 * [PRIVATE]
 *
 * cURL write function callback
 */
static size_t write_response(void* src, size_t size, size_t nmemb, void* dest) {
	struct gl_http_response* response = dest;
	size_t required_length = response->response_length + size * nmemb;

	/* Reallocate buffer iff necessary
	 */
	if (required_length > response->buffer_length) {
		response->data = realloc(response->data, required_length);
		response->buffer_length = required_length;
	}

	/* Copy data
	 */
	memcpy(&response->data[response->response_length], src, size * nmemb);
	response->response_length += size * nmemb;

	return size * nmemb;
}





/**
 * [PUBLIC API]
 */
struct gl_http_response* gl_download(uint8_t const* url) {

	/* Initialize cURL
	 */
	CURL* curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "curl_init() failed\n");
		return 0;
	}


	/* Prepare response
	 */
	struct gl_http_response* response = malloc(sizeof(struct gl_http_response));
	response->response_length = 0;
	response->buffer_length = 1;
	response->data = malloc(response->buffer_length * sizeof(uint8_t));


	/* Configure cURL
	 */
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);


	/* Download contents
	 */
	CURLcode code = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (CURLE_OK != code) {
		fprintf(stderr, "curl_easy_perform() failed %s\n", curl_easy_strerror(code));
		gl_free_response(response);
		return 0;
	}


	/* Return response :-)
	 */
	return response;
}



/**
 * [PUBLIC API]
 */
uint8_t* gl_get_response_data(struct gl_http_response* response) {
	return response->data;
}



/**
 * @return Response data length
 */
size_t gl_get_response_length(struct gl_http_response* response) {
	return response->response_length;
}



/**
 * Frees all resources allocated by struct
 */
void gl_free_response(struct gl_http_response* response) {
	free(response->data);
	free(response);
}













