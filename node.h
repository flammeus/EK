#ifndef _NODE_H
#define _NODE_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic.h"
#include "queue.h"
#include "tuple.h"
#include "vertex.h"

node_t node_create(u32 id);
node_t node_destroy(node_t node);
bool node_compare(node_t node1, node_t node2);
void node_add_neighboor(node_t node, node_t node1, u32 capacity);
vertex_t get_node_vertex(node_t node);
bool is_visited(node_t node);
node_t visit(node_t node);
node_t unvisit(node_t node);
queue_t get_backward_queue(node_t node);
queue_t get_forward_queue(node_t node);
void display_info_node(node_t node);
tuple_t get_neighboor(node_t node, u32 i);
node_t set_ancestor(node_t node, node_t ancestor);
node_t get_ancestor(node_t node);
node_t unset_ancestor(node_t node);
u32 get_temp_flow(node_t node);
node_t set_temp_flow(node_t node, u32 flow);
node_t forward_search(node_t node, queue_t queue, queue_t cut);
node_t backward_search(node_t node, queue_t queue, queue_t cut);
int get_way(node_t node);
#endif
