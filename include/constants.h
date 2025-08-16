#ifndef __CONSTANTS__
#define __CONSTANTS__

#define __CONSTANTS_RW_BASE__ ".jvc/"

#define __CONSTANTS_RW_PGMNAME__ "rw"

/*__CONSTANTS_RW_BLOBS__ contain all the blobs named with their checksum*/
#define __CONSTANTS_RW_BLOBS__ "blobs/"

/*JVC_VERSIONS contain all the versions of the jar, named with a unique checksum. Each file will have a name, parent version (i.e. previous version)
and a pointer to a tree file*/
#define __CONSTANTS_RW_TAGS__ "versions/"

/*__CONSTANTS_RW_INDEX__ contains a list of all files and their checksums*/
#define __CONSTANTS_RW_INDEX__ "index"

/*__CONSTANTS_RW_HEAD__ contains the name of the current version of the jar*/
#define __CONSTANTS_RW_HEAD__ "HEAD"

/*__CONSTANTS_RW_TREES__ contains the trees, named with a unique checksum. Each file will have the map from file path to object*/
#define __CONSTANTS_RW_TREES__ "trees/"

#define __CONSTANTS_RW_BLOBTYPE_IDENTIFIER__ ".type"

#define __CONSTANTS_RW_COMMIT_IDENTIFIER__ "COMMIT"

#define __CONSTANTS_RW_TREE_IDENTIFIER__ "TREE"

#define __CONSTANTS_RW_BLOB_IDENTIFIER__ "BLOB"

#define __CONSTANTS_RW_HEAD_IDENTIFIER_ "HEAD"


#define JVC_BUFFER 8192

#define __CONSTANTS_HASH_GENERATOR_SIZE__ 200000

#define __CONSTANTS_RW_STRING_BUFFER 50000

#define __CONSTANT_RW_STRING_BUFFER_S 20
#define __CONSTANT_RW_STRING_BUFFER_M 100

#define __CONSTANTS_RW_HASH_GENERATOR_DELIMITER__ "ABC"

#define __CONSTANTS_RW_HASH_GENERATOR_PREFIX__ "HEADER"

#define __CONSTANTS_RW_HASH_GENERATOR_SUFFIX__ "TRAILER"

#define __CONSTANTS_RW_COLOR_GREEN__ "[0;32m"

#define __CONSTANTS_RW_COLOR_GREEN_BRIGHT__ "[1;32m"

#define __CONSTANTS_RW_COLOR_START__ "\033"

#define __CONSTANTS_RW_COLOR_END__ "\033[0m"

#define __CONSTANTS_RW_COLOR_YELLOW__ "[0;33m"

#define __CONSTANTS_RW_COLOR_YELLOW_BRIGHT__ "[1;33m"

#define __CONSTANTS_RW_COLOR_RED__ "[0;31m"

#define __CONSTANTS_RW_COLOR_RED_BRIGHT__ "[1;31m"

#define __CONSTANTS_RW_VERSION__ "1.01"

#endif