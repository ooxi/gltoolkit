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
#ifndef GLTOOLKIT_HTTP
#define GLTOOLKIT_HTTP





/**
 * Includes
 */
#include <stdint.h>

/**
 * Opaque structures
 */
struct gl_http_response;





/**
 * Downloads the contents of an url into a dynamic buffer
 */
struct gl_http_response* gl_download(uint8_t const* url);

/**
 * @return Response data
 */
uint8_t* gl_get_response_data(struct gl_http_response* response);

/**
 * @return Response data length
 */
size_t gl_get_response_length(struct gl_http_response* response);

/**
 * Frees all resources allocated by struct
 */
void gl_free_response(struct gl_http_response* response);





#endif

