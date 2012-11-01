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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <entities.h>
#include <xml.h>
#include "gltoolkit.h"





/**
 * @return Opened file inside a directory or 0, iff file cannot be opened
 */
static FILE* open_in_directory(
			uint8_t const* directory,
			uint8_t const* file,
			uint8_t const* mode
		) {

	uint8_t const* separator = "/";

	size_t path_length = strlen(directory) + strlen(separator) + strlen(file) + 1;
	uint8_t* path = alloca(path_length * sizeof(uint8_t));

	snprintf(path, path_length, "%s%s%s", directory, separator, file);
	path[path_length - 1] = 0;

	return fopen(path, mode);
}



/**
 * Prints all IANA language codes into a file, needed by `gettext'
 *
 * @see http://www.gnu.org/software/gettext/manual/html_node/po_002fLINGUAS.html
 */
static void print_linguas(
			struct gl_languages* languages,
			uint8_t const* directory, uint8_t const* file
		) {

	FILE* linguas = open_in_directory(directory, file, "wb");

	if (!linguas) {
		fprintf(stderr, "Cannot open %s in %s\n", file, directory);
		exit(EXIT_FAILURE);
	}

	size_t i = 0; for (; i < gl_get_languages_count(languages); ++i) {
		struct gl_language* language = gl_get_language(languages, i);
		fprintf(linguas, "%s ", gl_get_language_code(language));
	}
	fclose(linguas);
}



/**
 * Read a translation configuration
 *
 * ---( Example configuration )---
 * <Translation>
 *     <Original-Translator>Adrián Chaves Fernández</Original-Translator>
 *     <Plural-Forms>nplurals=2; plural=(n != 1);</Plural-Forms>
 * </Translation>
 * ---
 */
static struct xml_document* open_configuration(
			struct gl_language* language,
			uint8_t const* directory
		) {

	/* Configuration file is named <iana-language-code>.xml
	 */
	uint8_t const* language_iana = gl_get_language_code(language);

	size_t file_length = strlen(language_iana) + strlen(".xml") + 1;
	uint8_t* file = alloca(file_length * sizeof(uint8_t));

	snprintf(file, file_length, "%s.xml", language_iana);
	file[file_length - 1] = 0;

	/* Open and return file contents
	 */
	FILE* configuration = open_in_directory(directory, file, "rb");

	if (!configuration) {
		return 0;
	}
	return xml_open_document(configuration);
}



/**
 * Configuration helper
 */
static uint8_t* configuration_value(
			struct xml_node* configuration,
			uint8_t const* key
		) {

	if (!configuration) {
		return 0;
	}

	uint8_t* value = xml_easy_content(xml_easy_child(
		configuration, key, 0
	));
	if (!value) {
		return 0;
	}

	decode_html_entities_utf8(value, 0);
	return value;
}



/**
 * Prints all translations of a language into a po-file
 */
static void print_po(	uint8_t const* project,
			struct gl_language* language,
			struct gl_translations* translations,
			struct xml_node* configuration,
			uint8_t const* directory
		) {

	/* Extrat configuration values (will be empty strings, if missing)
	 */
	uint8_t* license = configuration_value(configuration, "License");
	uint8_t* original_author = configuration_value(configuration, "Original-Translator");
	uint8_t* report_msgid_bugs_to = configuration_value(configuration, "Report-Msgid-Bugs-To");
	uint8_t* language_team = configuration_value(configuration, "Language-Team");
	uint8_t* plural_forms = configuration_value(configuration, "Plural-Forms");


	/* Open po file
	 */
	uint8_t const* language_iana = gl_get_language_code(language);
	size_t po_name_length = strlen(language_iana) + strlen(".po") + 1;
	uint8_t* po_name = alloca(po_name_length * sizeof(uint8_t));

	snprintf(po_name, po_name_length, "%s.po", language_iana);
	po_name[po_name_length - 1] = 0;

	FILE* po = open_in_directory(directory, po_name, "wb");
	if (!po) {
		fprintf(stderr, "Cannot open %s\n", po_name);
		goto exit;
	}


	/* Write po header
	 *
	 * @warning Currently there is no way to get information about
	 *     `Last-Translator' and `POT-Creation-Date' from
	 *     GetLocalization.com
	 *
	 *     Moreover the version in `Project-Id-Version' does not depend on
	 *     GetLocalization.com so it can not be extracted from there
	 */
	fprintf(po, "# This file was auto generated by %s. DO NOT EDIT\n", GLTOOLKIT_NAME);
	fprintf(po, "#\n");

	if (license) {
		fprintf(po, "# Licensed under %s\n", license);
	}
	if (original_author) {
		fprintf(po, "# Original author: %s\n", original_author);
	}
	fprintf(po, "\n");

	fprintf(po, "msgid \"\"\n");
	fprintf(po, "msgstr \"\"\n");
	fprintf(po, "\"Project-Id-Version: %s\\n\"\n", project);
	if (report_msgid_bugs_to) {
		fprintf(po, "\"Report-Msgid-Bugs-To: %s\\n\"\n", report_msgid_bugs_to);
	}
//	fprintf(po, "\"POT-Creation-Date: 2011-03-29 22:06+0200\\n\"\n");

	time_t now = time(0);
	struct tm* lt = localtime(&now);

	fprintf(po, "\"PO-Revision-Date: %04d-%02d-%02d %02d:%02d %s\\n\"\n",
		1900 + lt->tm_year,
		1 + lt->tm_mon,
		lt->tm_mday,
		lt->tm_hour,
		lt->tm_min,
		lt->tm_zone
	);

//	fprintf(po, "\"Last-Translator: Nikita M. Makarov <5253450@gmail.com>\\n\"\n");
	if (language_team) {
		fprintf(po, "\"Language-Team: %s\\n\"\n", language_team);
	}
	fprintf(po, "\"MIME-Version: 1.0\\n\"\n");
	fprintf(po, "\"Content-Type: text/plain; charset=UTF-8\\n\"\n");
	fprintf(po, "\"Content-Transfer-Encoding: 8bit\\n\"\n");
	fprintf(po, "\"Language: %s\\n\"\n", language_iana);
	fprintf(po, "\"X-Generator: %s\\n\"\n", GLTOOLKIT_NAME);
	if (plural_forms) {
		fprintf(po, "\"Plural-Forms: %s\\n\"\n", plural_forms);
	}
	fprintf(po, "\n");


	/* Write translations
	 */
	size_t j = 0; for (; j < gl_get_translations_count(translations); ++j) {
		struct gl_translation* translation = gl_get_translation(translations, j);

		fprintf(po, "# %s\n", gl_get_translation_context_info(translation));
		fprintf(po, "msgid \"%s\"\n", gl_get_translation_master_string(translation));
		fprintf(po, "msgstr \"%s\"\n", gl_get_translation_string(translation));
		fprintf(po, "\n");
	}


	/* Free allocated resources
	 */
exit:
	if (license)			free(license);
	if (original_author)		free(original_author);
	if (report_msgid_bugs_to)	free(report_msgid_bugs_to);
	if (language_team)		free(language_team);
	if (plural_forms)		free(plural_forms);
}





/**
 * GetLocalization.com Toolkit
 * ===========================
 *
 * Downloads translations from GetLocalization.com and converts them into
 * gettext sources
 *
 * @param argv[1] GetLocalization.com project name
 * @param argv[2] Working directory
 *
 * Currently this toolkit does several actions at once and is optimized for the
 * Violetland project. A future version might be better generalized and runtime
 * configurable:
 *
 *  0. Validate arguments
 *  1. Fetch all available languages and write them to LINGUAS
 *  2. All translations and write po translation file
 */
int main(int argc, char** argv) {

	/* 0. Validate arguments
	 */
	if (3 != argc) {
		fprintf(stderr, "Usage: gltoolkit <project> <working-directory>\n");
		return EXIT_FAILURE;
	}
	uint8_t const* project = argv[1];
	uint8_t const* working_directory = argv[2];


	/* 1. Fetch all available languages and write them to LINGUAS
	 */
	struct gl_languages* languages = gl_get_languages(project);
	print_linguas(languages, working_directory, "LINGUAS");


	/* 2. For every language do
	 */
	size_t i = 0; for (; i < gl_get_languages_count(languages); ++i) {
		struct gl_language* language = gl_get_language(languages, i);
		uint8_t const* language_code = gl_get_language_code(language);
		fprintf(stdout, "Fetching %s/%s\n", project, language_code);
		
		/* 2.a Fetch all translations
		 */
		struct gl_translations* translations = gl_get_translations(
			project, language_code
		);

		/* 2.b Open translation configuration
		 */
		struct xml_document* configuration = open_configuration(
			language, working_directory
		);
		struct xml_node* config = configuration
			? xml_document_root(configuration) : 0
		;

		/* 2.c Write po file
		 */
		print_po(project, language, translations, config, working_directory);

		/* Free resources
		 */
		if (configuration) {
			xml_document_free(configuration, true);
		}
		gl_free_translations(translations);
	}


	/* Free resources and exit
	 */
	gl_free_languages(languages);
	return EXIT_SUCCESS;
}

