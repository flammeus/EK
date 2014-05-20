#ifndef _VERTEX_H
#define _VERTEX_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "basic.h"

vertex_t vertex_create(u32 id);
vertex_t vertex_destroy(vertex_t vertex);
bool vertex_compare(vertex_t vertex1, vertex_t vertex2);
u32 vertex_id(vertex_t vertex);

#endif
