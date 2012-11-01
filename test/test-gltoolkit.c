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
#include <stdio.h>
#include <stdlib.h>

#include "gltoolkit.h"




/**
 * Tests gl_language features
 */
static void gl_test_languages(uint8_t const* project) {

	/* Load languages
	 */
	struct gl_languages* languages = gl_get_languages(project);

	if (!languages) {
		fprintf(stderr, "Cannot download languages of project %s\n", project);
		exit(EXIT_FAILURE);
	}


	/* Print languages
	 */
	size_t languages_count = gl_get_languages_count(languages);

	fprintf(stdout,	"Project %s contains %li languages\n",
			project, (unsigned long)languages_count
	);

	if (languages_count) {
		struct gl_language* language = gl_get_language(languages, 0);

		fprintf(stdout, "First language is %s with IANA code %s\n",
			gl_get_language_name(language),
			gl_get_language_code(language)
		);
	}


	/* Free resources
	 */
	gl_free_languages(languages);
	curl_global_cleanup();
}



/**
 * Tests gl_translation features
 */
static gl_test_translations(uint8_t const* project, uint8_t const* language) {

	/* Load translations of project in language
	 */
	struct gl_translations* translations = gl_get_translations(
		project, language
	);

	if (!translations) {
		fprintf(stderr, "Failed loading translations of %s in %s\n", project, language);
		exit(EXIT_FAILURE);
	}


	/* Inspect translation count
	 */
	size_t translations_count = gl_get_translations_count(translations);

	fprintf(stdout, "Project %s contains %li translations in language %s\n",
		project, (unsigned long)translations_count, language
	);


	/* Inspect first translation (if existent)
	 */
	if (!translations_count) {
		goto exit_success;
	}

	struct gl_translation* translation = gl_get_translation(translations, 0);
	if (!translation) {
		fprintf(stderr, "Failed to fetch first translation, must not happen\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "The project's first translation is \"%s\", which translates \"%s\" at \"%s\" (logical string was \"%s\")\n",
		gl_get_translation_string(translation),
		gl_get_translation_master_string(translation),
		gl_get_translation_context_info(translation),
		gl_get_translation_logical_string(translation)
	);


	/* Free all resources
	 */
exit_success:
	gl_free_translations(translations);
}





/**
 * Command line interface, runs all tests
 */
int main(int argc, char** argv) {
	uint8_t const* project = "violetland";

	gl_test_languages(project);
	gl_test_translations(project, "ru");
	gl_test_translations(project, "de");

	fprintf(stdout, "All testes passed :-)\n");
	exit(EXIT_SUCCESS);
}

