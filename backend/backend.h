#ifndef _BACKEND_H_
#define _BACKEND_H_

#include "tree/tree.h"

void TranslateToAsm(const tree_t* tree, FILE* out_stream);

#endif
