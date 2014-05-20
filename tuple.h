#ifndef _TUPLE_H
#define _TUPLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic.h"
#include "node.h"
#include "vertex.h"

tuple_t tuple_create(node_t node, u32 capacity);
tuple_t tuple_destroy(tuple_t tuple);
node_t tuple_get_node(tuple_t tuple);
u32 tuple_get_capacity(tuple_t tuple);
u32 tuple_get_flow(tuple_t tuple);
u32 tuple_capacity_available(tuple_t tuple);
tuple_t let_pass_flow(tuple_t tuple, u32 capacity);
tuple_t get_back_flow(tuple_t tuple, u32 flow);
bool tuple_compare(tuple_t tuple1, tuple_t tuple2);
#endif
