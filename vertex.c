#include "vertex.h"

/*Estructura que simplemente guarda el numero del vertice (tipo 0 para source).
 *
 */
struct _vertex_t {
    u32 id;
};

/*Crea la estructura, alloca memoria y devuelve un puntero a ella.
 *
 */
vertex_t vertex_create(u32 id) {
    vertex_t new_vertex = calloc(1, sizeof(struct _vertex_t));
    assert(new_vertex != NULL);

    new_vertex->id = id;

    return new_vertex;
}

/*Quien dice allocar dice free... Funcion para libera la estructura vertex.
 *
 */
vertex_t vertex_destroy(vertex_t vertex) {
    assert(vertex != NULL);

    free(vertex);
    return NULL;
}

/*Compara dos vertices; si son iguales (si tienen el mismo numero) devuelve 
 *true si no false.
 */
bool vertex_compare(vertex_t vertex1, vertex_t vertex2) {
    return (vertex1->id == vertex2->id);
}

/*Funcion que simplemente devuelve el numero del vertice.
 *
 */
u32 vertex_id(vertex_t vertex) {
    assert(vertex != NULL);
    
    return vertex->id;
}
