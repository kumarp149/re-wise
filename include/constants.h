#ifndef __CONSTANTS__
#define __CONSTANTS__

#define __CONSTANTS_RW_BASE__ ".jvc/"

#define __CONSTANTS_RW_PGMNAME__ "rw"

/*__CONSTANTS_RW_BLOBS__ contain all the blobs named with their checksum*/
#define __CONSTANTS_RW_BLOBS__ "blobs/"

/*JVC_VERSIONS contain all the versions of the jar, named with a unique checksum. Each file will have a name, parent version (i.e. previous version)
and a pointer to a tree file*/
#define JVC_VERSIONS "versions/"

/*__CONSTANTS_RW_INDEX__ contains a list of all files and their checksums*/
#define __CONSTANTS_RW_INDEX__ "index"

/*__CONSTANTS_RW_HEAD__ contains the name of the current version of the jar*/
#define __CONSTANTS_RW_HEAD__ "HEAD"

/*__CONSTANTS_RW_TREES__ contains the trees, named with a unique checksum. Each file will have the map from file path to object*/
#define __CONSTANTS_RW_TREES__ "trees/"

#define JVC_BUFFER 8192

#define __CONSTANTS_HASH_GENERATOR_SIZE__ 20000

#define JVC_STRING_BUFFER 20000

#define __CONSTANTS_RW_HASH_GENERATOR_DELIMITER__ "ABC"

#define __CONSTANTS_RW_HASH_GENERATOR_PREFIX__ "HEADER"

#define __CONSTANTS_RW_HASH_GENERATOR_SUFFIX__ "TRAILER"

#define __CONSTANTS_RW_COLOR_GREEN__ "[0;32m"

#define __CONSTANTS_RW_COLOR_START__ "\033"

#define __CONSTANTS_RW_COLOR_END__ "\033[0m"

#define __CONSTANTS_RW_COLOR_YELLOW__ "[0;33m"

#define __CONSTANTS_RW_COLOR_RED__ "[0;31m"

#endif