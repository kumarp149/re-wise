#ifndef __CONSTANTS__
#define __CONSTANTS__

#define JVC_BASE ".jvc/"

#define JVC_PGM_NAME "rw"

/*JVC_BLOBS contain all the blobs named with their checksum*/
#define JVC_BLOBS "blobs/"

/*JVC_VERSIONS contain all the versions of the jar, named with a unique checksum. Each file will have a name, parent version (i.e. previous version)
and a pointer to a tree file*/
#define JVC_VERSIONS "versions/"

/*JVC_INDEX contains a list of all files and their checksums*/
#define JVC_INDEX "index"

/*JVC_HEAD contains the name of the current version of the jar*/
#define JVC_HEAD "HEAD"

/*JVC_TREES contains the trees, named with a unique checksum. Each file will have the map from file path to object*/
#define JVC_TREES "trees/"

#define JVC_BUFFER 8192

#define JVC_NOT_A_VALID_ARCHIVE "Not a valid archive"

#define JVC_UNABLE_TO_OPEN_ARCHIVE "Unable to open the archive"

#define JVC_UNABLE_TO_OPEN_FILE "Unable to open the file in archive"

#define JVC_UNABLE_TO_READ_FILE "Unable to read the file content"

#define JVC_HASH_GENERATOR_SIZE 20000

#define JVC_STRING_BUFFER 20000

#define JVC_HASH_GENERATOR_DELIMITER "ABC"

#define JVC_HASH_GENERATOR_PREFIX "HEADER"

#define JVC_HASH_GENERATOR_SUFFIX "TRAILER"

#endif