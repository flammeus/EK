#include "tuple.h"

/*Estructura que permite guardar los datos de una arista (ej : vecino, capacidad
 *y flujo).
 */
struct _tuple_t {
    node_t neighboor;
    u32 capacity, flow;
};

/*Como siempre , funcion para allocar memoria a la struct tuple.
 *
 */
tuple_t tuple_create(node_t node, u32 capacity) {
    assert(node != NULL);

    tuple_t tuple = calloc(1, sizeof(struct _tuple_t));
    assert(tuple != NULL);
    
    tuple->neighboor = node;
    tuple->capacity = capacity;
    tuple->flow = 0;

    return tuple;
}

/*Funcion para destruir la struct tuple.
 *
 */
tuple_t tuple_destroy(tuple_t tuple) {
    assert(tuple != NULL);
    
    //Se asume que todos los nodos se destruyen en el main...
    free(tuple);

    return NULL;
}

/*Devuelve el nodo contenido en la estructura.
 *
 */
node_t tuple_get_node(tuple_t tuple) {
    assert(tuple != NULL);
    
    return tuple->neighboor;
}

/*Devuelve el flujo de la tupla.
 *
 */
u32 tuple_get_flow(tuple_t tuple) {
    assert(tuple != NULL);
    
    return tuple->flow;
}

/*Devuelve la capacidad de la tupla.
 *
 */
u32 tuple_capacity_available(tuple_t tuple) {
    assert(tuple != NULL);
    
    return (tuple->capacity);
}

/*Deja pasar un flujo en una tupla.(incrementa el flujo y disminuye la 
 *capacidad).
 */
tuple_t let_pass_flow(tuple_t tuple, u32 capacity) {
    assert(tuple != NULL);
    assert(tuple->capacity >= capacity);
    
    tuple->capacity = tuple->capacity - capacity;
    
    tuple->flow = tuple->flow + capacity;
    
    return tuple;
}

/*Devuelve el flujo (decrementa el flujo y aumenta la capacidad).
 *
 */
tuple_t get_back_flow(tuple_t tuple, u32 flow) {
    assert(tuple != NULL);
    assert(tuple->flow >= flow);
    
    tuple->flow = tuple->flow - flow;
    tuple->capacity = tuple->capacity + flow;
    
    return tuple;
}

/*Compara dos tuplas (en realidad simple compara los nodos , no mira la 
 *capacidad y tampoco el flujo).
 */
 bool tuple_compare(tuple_t tuple1, tuple_t tuple2) {
    assert(tuple1 != NULL);
    assert(tuple2 != NULL);
    
    return node_compare(tuple1->neighboor, tuple2->neighboor);
}
