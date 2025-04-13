#ifndef __JVC__
#define __JVC__

#include "structures.h"

struct jvc_index* loadIndex();
void saveIndex(struct jvc_index* index);

struct jvc_blob* loadBlob(char* id);
void saveBlob(struct jvc_blob* blob);

struct jvc_tree* loadTree(char* id);
void saveTree(struct jvc_tree* tree);

struct jvc_version* loadVersion(char* id);
void saveVersion(struct jvc_version* version);

struct jvc_head* loadHead();
void saveHead(struct jvc_head* head);


#endif