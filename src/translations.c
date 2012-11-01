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
#ifndef GET_LOCALIZATION_TRANSLATIONS_PATTERN
#define GET_LOCALIZATION_TRANSLATIONS_PATTERN "http://www.getlocalization.com/api/localized_strings/%s/%s/"
#endif





/**
 * [OPAQUE API]
 *
 * Holds one translation (all strings are 0-terminated)
 */
struct gl_translation {
	uint8_t* master_string;
	uint8_t* logical_string;
	uint8_t* context_info;
	uint8_t* translation;
};



/**
 * [OPAQUE API]
 *
 * Holds all translations in one language
 */
struct gl_translations {
	struct gl_translation** translations;
	size_t translations_count;
};





/**
 * [PUBLIC API]
 */
struct gl_translations* gl_get_translations(uint8_t const* project, uint8_t const* language) {
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
	uint8_t* language_escaped = curl_easy_escape(curl, language, 0);

	size_t url_length = 1
		+ strlen(GET_LOCALIZATION_TRANSLATIONS_PATTERN)
		+ strlen(project_escaped)
		+ strlen(language_escaped)
	;

	uint8_t* url = alloca(url_length * sizeof(uint8_t));
	snprintf(
		url, url_length - 1,
		GET_LOCALIZATION_TRANSLATIONS_PATTERN,
		project_escaped, language_escaped
	);
	url[url_length - 1] = 0;

	curl_free(project_escaped);
	curl_free(language_escaped);
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


	/* Build language list ... the hard way :D
	 */
	struct xml_node* root = xml_document_root(document);

	struct gl_translations* translations = malloc(sizeof(struct gl_translations));
	translations->translations_count = xml_node_children(root) - 1;
	translations->translations = calloc(translations->translations_count + 1, sizeof(struct gl_translation*));


	/* Skip first child, since it contains the project name
	 */
	size_t i = 0; for (; i < translations->translations_count; ++i) {
		struct xml_node* node = xml_node_child(root, i + 1);

		struct xml_string* xml_master = xml_node_content(xml_node_child(node, 0));
		struct xml_string* xml_logical = xml_node_content(xml_node_child(node, 1));
		struct xml_string* xml_context = xml_node_content(xml_node_child(node, 2));
		struct xml_string* xml_translation = xml_node_content(xml_node_child(node, 3));

		struct gl_translation* translation = malloc(sizeof(struct gl_translation));
		translation->master_string = calloc(xml_string_length(xml_master) + 1, sizeof(uint8_t));
		translation->logical_string = calloc(xml_string_length(xml_logical) + 1, sizeof(uint8_t));
		translation->context_info = calloc(xml_string_length(xml_context) + 1, sizeof(uint8_t));
		translation->translation = calloc(xml_string_length(xml_translation) + 1, sizeof(uint8_t));

		xml_string_copy(xml_master, translation->master_string, xml_string_length(xml_master));
		xml_string_copy(xml_logical, translation->logical_string, xml_string_length(xml_logical));
		xml_string_copy(xml_context, translation->context_info, xml_string_length(xml_context));
		xml_string_copy(xml_translation, translation->translation, xml_string_length(xml_translation));

		translations->translations[i] = translation;
	}


	/* Return translations
	 */
	return translations;	


	/* Free resources and return with failure
	 */
exit_failure:
	if (document) {
		xml_document_free(document, false);
	}
	if (response) {
		gl_free_response(response);
	}
	return 0;
}



/**
 * [PUBLIC API]
 */
size_t gl_get_translations_count(struct gl_translations* translations) {
	return translations->translations_count;
}



/**
 * [PUBLIC API]
 */
struct gl_translation* gl_get_translation(struct gl_translations* translations, size_t n) {
	if (n >= translations->translations_count) {
		return 0;
	}

	return translations->translations[n];
}



/**
 * [PUBLIC API]
 */
uint8_t const* gl_get_translation_master_string(struct gl_translation* translation) {
	return translation->master_string;
}



/**
 * [PUBLIC API]
 */
uint8_t const* gl_get_translation_logical_string(struct gl_translation* translation) {
	return translation->logical_string;
}



/**
 * [PUBLIC API]
 */
uint8_t const* gl_get_translation_context_info(struct gl_translation* translation) {
	return translation->context_info;
}



/**
 * [PUBLIC API]
 */
uint8_t const* gl_get_translation_string(struct gl_translation* translation) {
	return translation->translation;
}



/**
 * [PUBLIC API]
 */
void gl_free_translations(struct gl_translations* translations) {

	size_t i = 0; for (; i < translations->translations_count; ++i) {
		struct gl_translation* translation = translations->translations[i];

		free(translation->master_string);
		free(translation->logical_string);
		free(translation->context_info);
		free(translation->translation);
		free(translation);
	}

	free(translations->translations);
	free(translations);
}











