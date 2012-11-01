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
#ifndef HEADER_GLTOOLKIT
#define HEADER_GLTOOLKIT





/**
 * Includes
 */
#include <stdint.h>
#include <string.h>

/**
 * Opaque structures
 */
struct gl_language;
struct gl_languages;
struct gl_translation;
struct gl_translations;





/**
 * @return All languages used by `project'
 */
struct gl_languages* gl_get_languages(uint8_t const* project);

/**
 * @return Number of languages in project
 */
size_t gl_get_languages_count(struct gl_languages* languages);

/**
 * @return n-th language or 0 if out of range
 */
struct gl_language* gl_get_language(struct gl_languages* languages, size_t n);

/**
 * @return 0-terminated language name
 */
uint8_t const* gl_get_language_name(struct gl_language* language);

/**
 * @return 0-terminated iana language code
 */
uint8_t const* gl_get_language_code(struct gl_language* language);

/**
 * Frees all resources allocated by the structure
 */
void gl_free_languages(struct gl_languages* languages);



/**
 * @param project GetLocalization.com project name
 * @param language iana language code of interest
 *
 * @return All translations of of `project' in `language'
 */
struct gl_translations* gl_get_translations(uint8_t const* project, uint8_t const* language);

/**
 * @return Number of translations
 */
size_t gl_get_translations_count(struct gl_translations* translations);

/**
 * @return n-th translation or 0 if out of range
 */
struct gl_translation* gl_get_translation(struct gl_translations* translations, size_t n);

/**
 * @return 0-terminated master string of translation
 */
uint8_t const* gl_get_translation_master_string(struct gl_translation* translation);

/**
 * @return 0-terminated logical string of translation
 */
uint8_t const* gl_get_translation_logical_string(struct gl_translation* translation);

/**
 * @return 0-terminated context info of translation
 */
uint8_t const* gl_get_translation_context_info(struct gl_translation* translation);

/**
 * @return 0-terminated translated string
 */
uint8_t const* gl_get_translation_string(struct gl_translation* translation);

/**
 * Frees all resources allocated by struct
 */
void gl_free_translations(struct gl_translations* translations);





#endif

