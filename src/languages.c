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
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <curl/curl.h>
#include <xml.h>

#include "gltoolkit.h"
#include "http.h"





/**
 * If not overwritten use default REST API URL
 */
#ifndef GET_LOCALIZATION_LANGUAGES_PATTERN
#define GET_LOCALIZATION_LANGUAGES_PATTERN "http://www.getlocalization.com/api/languages/?product=%s&type=xml"
#endif





/**
 * [OPAQUE API]
 *
 * Holds the language's name and iana code as 0-terminated UTF-8 character
 * sequences
 *
 * @see http://www.getlocalization.com/api/languages/?product=<product>&type=xml
 * @see http://www.gnu.org/software/gettext/manual/html_node/Usual-Language-Codes.html
 */
struct gl_language {
	uint8_t* name;
	uint8_t* iana;
};



/**
 * [OPAQUE API]
 *
 * 0-terminated array with all languages provided by project
 */
struct gl_languages {
	struct gl_language** languages;
	size_t languages_count;
};





/**
 * [PUBLIC API]
 */
struct gl_languages* gl_get_languages(uint8_t const* project) {
	struct gl_http_response* response = 0;
	struct xml_document* document = 0;
	struct gl_languages* languages = 0;


	/* Initialize cURL
	 */
	CURL* curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "Initializing cURL failed\n");
		goto exit_failure;
	}


	/* Create URL
	 */
	uint8_t* project_escaped = curl_easy_escape(curl, project, 0);

	size_t url_length = strlen(GET_LOCALIZATION_LANGUAGES_PATTERN) + strlen(project_escaped) + 1;
	uint8_t* url = alloca(url_length * sizeof(uint8_t));
	snprintf(url, url_length - 1, GET_LOCALIZATION_LANGUAGES_PATTERN, project_escaped);
	url[url_length - 1] = 0;

	curl_free(project_escaped);
	curl_easy_cleanup(curl);


	/* Download content
	 */
	response = gl_download(url);
	if (!response) {
		fprintf(stderr, "Failed downloading %s\n", url);
		goto exit_failure;
	}


	/* Parse contents
	 */
	document = xml_parse_document(
		gl_get_response_data(response),
		gl_get_response_length(response)
	);

	if (!document) {
		size_t length = gl_get_response_length(response);
		uint8_t* buffer = alloca(length + 1);
		memcpy(buffer, gl_get_response_data(response), length);
		buffer[length] = 0;

		fprintf(stderr, "Failed parsing response from %s: %s\n", url, buffer);
		goto exit_failure;
	}


	/* Build language list
	 */
	struct xml_node* xml_languages = xml_document_root(document);
	
	languages = malloc(sizeof(struct gl_languages));
	languages->languages_count = xml_node_children(xml_languages);
	languages->languages = calloc(languages->languages_count + 1, sizeof(struct gl_language*));

	size_t i = 0; for (; i < languages->languages_count; ++i) {
		struct gl_language* language = malloc(sizeof(struct gl_language));

		struct xml_node* xml_language = xml_node_child(xml_languages, i);
		struct xml_string* xml_language_name = xml_node_content(xml_node_child(xml_language, 0));
		struct xml_string* xml_language_iana = xml_node_content(xml_node_child(xml_language, 1));

		language->name = calloc(xml_string_length(xml_language_name) + 1, sizeof(uint8_t));
		language->iana = calloc(xml_string_length(xml_language_iana) + 1, sizeof(uint8_t));

		xml_string_copy(xml_language_name, language->name, xml_string_length(xml_language_name));
		xml_string_copy(xml_language_iana, language->iana, xml_string_length(xml_language_iana));

		languages->languages[i] = language;
	}


	/* Free temporary data and return compiled list
	 */
	xml_document_free(document, false);
	gl_free_response(response);
	return languages;


	/* Free all resources and exit with failure
	 */
exit_failure:
	if (document) {
		xml_document_free(document, false);
	}
	if (response) {
		gl_free_response(response);
	}
	if (languages) {
		gl_free_languages(languages);
	}
	return 0;
}



/**
 * [PUBLIC API]
 */
size_t gl_get_languages_count(struct gl_languages* languages) {
	return languages->languages_count;
}



/**
 * [PUBLIC API]
 */
struct gl_language* gl_get_language(struct gl_languages* languages, size_t n) {
	if (n >= languages->languages_count) {
		return 0;
	}

	return languages->languages[n];
}



/**
 * [PUBLIC API]
 */
uint8_t const* gl_get_language_name(struct gl_language* language) {
	return language->name;
}



/**
 * [PUBLIC API]
 */
uint8_t const* gl_get_language_code(struct gl_language* language) {
	return language->iana;
}



/**
 * [PUBLIC API]
 */
void gl_free_languages(struct gl_languages* languages) {
	size_t i = 0; for (; i < languages->languages_count; ++i) {
		struct gl_language* language = languages->languages[i];
		free(language->name);
		free(language->iana);
		free(language);
	}

	free(languages->languages);
	free(languages);
}

