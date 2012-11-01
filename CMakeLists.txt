# Project setup
PROJECT(gltoolkit)
SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR) 


# Compiler setup
SET(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
SET(CMAKE_C_FLAGS_RELEASE "-O2")


# Build submodules
#
# @warning cmake must be called with -DCURL_STATICLIB=ON
ADD_SUBDIRECTORY(lib/curl)
ADD_SUBDIRECTORY(lib/xml.c)


# curl/curl.h has to be copied into binary directory
SET(CURL_INCLUDE_DIRECTORY ${PROJECT_BINARY_DIR}/lib/curl/include)
FILE(COPY ${CURL_SOURCE_DIR}/include/curl DESTINATION ${CURL_INCLUDE_DIRECTORY})

# xml.c includes
SET(XML_LIBRARY_DIRECTORY ${PROJECT_BINARY_DIR}/lib/xml.c)
SET(XML_INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/xml.c/src)


# Sources
SET(SOURCE_DIRECTORY src)
SET(TEST_SOURCE_DIRECTORY test)

SET(SOURCE_FILES
	${SOURCE_DIRECTORY}/http.c
	${SOURCE_DIRECTORY}/languages.c
	${SOURCE_DIRECTORY}/main.c
	${SOURCE_DIRECTORY}/translations.c
)
SET(TEST_SOURCE_FILES
	${SOURCE_DIRECTORY}/http.c
	${SOURCE_DIRECTORY}/languages.c
	${SOURCE_DIRECTORY}/translations.c
	${TEST_SOURCE_DIRECTORY}/test-gltoolkit.c
)


# Headers
INCLUDE_DIRECTORIES(${SOURCE_DIRECTORY} ${CURL_INCLUDE_DIRECTORY} ${XML_INCLUDE_DIRECTORY})


# Test executable
ADD_EXECUTABLE(test-gltoolkit
	${TEST_SOURCE_FILES}
)
TARGET_LINK_LIBRARIES(test-gltoolkit libcurl xml)


# Target executable
ADD_EXECUTABLE(gltoolkit
	${SOURCE_FILES}
)
TARGET_LINK_LIBRARIES(gltoolkit libcurl xml)

