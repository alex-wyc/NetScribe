#include "gap_buffer.h"
#include "text_buffer.h"

#ifndef _EDITOR_BACKEND_H
#define _EDITOR_BACKEND_H
bool is_tbuf(tbuf tb);
bool tbuf_empty(tbuf tb);
tbuf new_tbuf();
void split_point(tbuf tb, int user);
void forward_char(tbuf tb, int user);
void backward_char(tbuf tb, int user);
void insert_char(tbuf tb, char c, int user);
void delete_char(tbuf tb, int user);
#endif // _EDITOR_BACKEND_H
