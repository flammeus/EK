#ifndef _QUEUE_H
#define _QUEUE_H

#include <string.h>

#include "basic.h"
#include "node.h"
#include "tuple.h"

queue_t queue_create(char *type);
queue_t queue_destroy(queue_t queue);
member_t member_create(void *node, char *type);
member_t member_destroy(member_t member, char *type);
queue_t enqueue(queue_t queue, member_t member);
member_t pop(queue_t queue);
char *get_queue_type(queue_t queue);
unsigned long queue_size(queue_t queue);
bool queue_empty(queue_t queue);
member_t queue_search(queue_t queue, member_t member);
void info_queue_node(queue_t queue);
member_t get_member(queue_t queue, u32 i);
void *get_content(member_t member);
member_t reverse_pop(queue_t queue);
void info_queue_tuple(queue_t queue);
member_t get_last(queue_t queue);
void clear_all_node(queue_t queue);
void display_info_queue(queue_t queue);
#endif
