#include "node.h"
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))

/*La segunda estructura mas importante (despues del Dragon).
 *En vertex contiene el numero del vertice
 *El is_visited permite correr el BFS ( y no checkear cada vez que sea en el 
 *corte o no )...
 *Las dos colas (back y front neighboor) contienen el conjunto de todos los
 *vecinos (backward como forward).
 *BFS_ancestor sirve para la funcion backward y forward search para recoger
 *el camino original ej : si de A visitamos B entonces el ancestro de B sera A.
 *Forward_or_backward tambien permite saber si usamos la funcion backward o 
 *forward_search para encontrar el camino aumentante
 *temp_flow es el minimo flujo que pudo pasar hasta el nodo (es decir que cuando
 *llegamos a sink tendramos el flujo maximo que puede soportar el camino).
 */
struct _node_t {
    vertex_t vertex;
    queue_t back_neighboor;//Queue of tuple
    bool is_visited;
    queue_t front_neighboor;//Queue of tuple
    node_t BFS_ancestor;
    int forward_or_backward;//forward is 1 and backward -1, 0 is it's not set
    u32 temp_flow;
};

/*
 *Funcion para inicializar el nodo (y allocar la memoria)
 */
node_t node_create(u32 id) {

    vertex_t vertex = vertex_create(id);

    node_t new_node = calloc(1, sizeof(struct _node_t));
    assert(new_node != NULL);
    
    new_node->vertex = vertex;
    new_node->is_visited = false;
    new_node->back_neighboor = queue_create("tuple");
    new_node->front_neighboor = queue_create("tuple");
    new_node->BFS_ancestor = NULL;
    new_node->forward_or_backward = 0;
    
    return new_node;
}

/*
 *Devuelve temp_flow del nodo
 */
u32 get_temp_flow(node_t node) {
    assert(node != NULL);
    
    return node->temp_flow;
}

/*
 *Devuelve un int para ver si fue forward o no
 */
int get_way(node_t node) {
    assert(node != NULL);
    assert(node->forward_or_backward != 0);
    
    return node->forward_or_backward;
}

/*
 *Pone un flujo en el nodo (en temp_flow)
 */
node_t set_temp_flow(node_t node, u32 flow) {
    assert(node != NULL);
    
    node->temp_flow = flow;
    return node;
}

/*
 *Las 2 funciones siguientes permiten poner el nodo como "backward" o "forward"
 */
node_t set_forward(node_t node) {
    assert(node != NULL);
    
    node->forward_or_backward = 1;
    return node;
}

node_t set_backward(node_t node) {
    assert(node != NULL);
    
    node->forward_or_backward = -1;
    return node;
}

/*
 *Funcion para resetear el forward o backward
 */
node_t unset_forward_or_backward(node_t node) {
    assert(node != NULL);
    
    node->forward_or_backward = 0;
    return node;
}

/*
 *Funcion para asignar un ancestro a un nodo
 */
node_t set_ancestor(node_t node, node_t ancestor) {
    assert(node != NULL);
    assert(ancestor != NULL);
    
    node->BFS_ancestor = ancestor;
    return node;
}

/*
 *Funcion para resetear el ancestro del nodo
 */
node_t unset_ancestor(node_t node) {
    assert(node != NULL);
    
    node->BFS_ancestor = NULL;
    
    return node;
}

/*
 *Funcion para recuperar el ancestro de un nodo
 */
node_t get_ancestor(node_t node) {
    assert(node != NULL);
    assert(node->BFS_ancestor != NULL);
    
    return node->BFS_ancestor;
}

/*
 *Funcion para destruir un nodo
 */
node_t node_destroy(node_t node) {
    assert(node != NULL);
    
    node->vertex = vertex_destroy(node->vertex);
    queue_destroy(node->back_neighboor);
    queue_destroy(node->front_neighboor);
    free(node);
    return NULL;
}

/*
 *Funcion que usamos al final pero podria usarse (no la usamos pues que
 *queue_destroy maneja todo :) )
 */
node_t simple_node_destroy(node_t node) {
    assert(node != NULL);
    
    return NULL;
}

/*
 *Devuelve true si los dos nodos dados tienen el mismo vertex (numero)
 */
bool node_compare(node_t node1, node_t node2) {
    assert(node1 != NULL);
    assert(node2 != NULL);

    return vertex_compare(node1->vertex, node2->vertex);
}

/*
 *fneighboor para forward neighboor , agrega un nodo neighboor en la cola 
 *forward de "node" con capacidad "capacity"
 */
static node_t node_add_fneighboor(node_t node, node_t neighboor, u32 capacity) {
    assert(node != NULL);
    assert(neighboor != NULL);
    assert((vertex_id(node->vertex)) != 1);
    
    tuple_t tuple = tuple_create(neighboor, capacity);
    member_t new_member = member_create(tuple, "tuple");
    node->front_neighboor = enqueue(node->front_neighboor, new_member);
    return node;
}

/*
 *Devuelve el i esimo vecino forward de node (empieza en 1 y no en 0)
 */
tuple_t get_neighboor(node_t node, u32 i) {
    assert(node != NULL);
    
    member_t member = get_member(node->front_neighboor, i);
    assert(strcmp(get_queue_type(node->front_neighboor), "tuple") == 0);
    
    tuple_t tuple = (tuple_t)(get_content(member));
    return tuple;
}

/*
 *bneighboor para backward neighboor , perimite agregar neighboor en la cola
 *backward de node
 */
static node_t node_add_bneighboor(node_t node, node_t neighboor, u32 capacity) {
    assert(node != NULL);
    assert(neighboor != NULL);
    assert((vertex_id(node->vertex)) != 0);
    
    tuple_t tuple = tuple_create(neighboor, capacity);
    member_t new_member = member_create(tuple, "tuple");
    node->back_neighboor = enqueue(node->back_neighboor, new_member);
    
    return node;
}

/*
 *Funcion general , agrega node1 en la cola forward de node y node en la cola
 *backward de node1
 */
void node_add_neighboor(node_t node, node_t node1, u32 capacity) {
    assert(node != NULL);
    assert(node1 != NULL);
    
    node = node_add_fneighboor(node, node1, capacity);
    node1 = node_add_bneighboor(node1, node, capacity);
}

/*
 *Devuelve el vertice del nodo
 */
vertex_t get_node_vertex(node_t node) {
    assert(node != NULL);
    
    return node->vertex;
}

/*
 *Devuelve el booleano del vertice
 */
bool is_visited(node_t node) {
    assert(node != NULL);
    
    return node->is_visited;
}

/*
 *Asigna true al booleano is_visited del nodo
 */
node_t visit(node_t node) {
    assert(node != NULL);
    
    node->is_visited = true;
    return node;
}

/*
 *Resetea is_visited (ponemos false)
 */
node_t unvisit(node_t node) {
    assert(node != NULL);
    
    node->is_visited = false;
    return node;
}

/*
 *Devuelve la cola backward del nodo node
 */
queue_t get_backward_queue(node_t node) {
    assert(node != NULL);
    
    return node->back_neighboor;
}

/*
 *Devuelve la cola forward del nodo node
 */
queue_t get_forward_queue(node_t node) {
    assert(node != NULL);
    
    return node->front_neighboor;
}

/*
 *Se sigio el libro para hacer esta funcion. Mas o menos como BFS :
 *Si sos vecino forward y si no estas visitado entonces te pongo como visitado
 *te agrego al corte y te encolo a la cola queue (tambien se asigna el ancestro
 *y el forward)
 */
node_t forward_search(node_t node, queue_t queue, queue_t cut) {
    assert(node != NULL);
    assert(queue != NULL);
    assert(strcmp(get_queue_type(queue), "node") == 0 || 
                             (strcmp(get_queue_type(queue), "node_main") == 0));
    assert(strcmp(get_queue_type(cut), "node") == 0 ||
                             (strcmp(get_queue_type(queue), "node_main") == 0));
    
    u32 i = 1;
    tuple_t tup_neighboor = NULL;
    node_t nod_neighboor = NULL;
    member_t member = NULL;
    u32 available = 0;
    member_t new_member = NULL;
    
    while(i <= queue_size(node->front_neighboor)) {
        available = 0;
        member = get_member(node->front_neighboor, i);
        i = i + 1;
        tup_neighboor = (tuple_t)(get_content(member));
        nod_neighboor = tuple_get_node(tup_neighboor);
        
        available = tuple_capacity_available(tup_neighboor);
        if(!is_visited(nod_neighboor) && (available > 0)) {
            visit(nod_neighboor);
            new_member = member_create(nod_neighboor, "node");
            queue = enqueue(queue, new_member);
            new_member = member_create(nod_neighboor, "node");
            cut = enqueue(cut, new_member);
            set_ancestor(nod_neighboor, node);
            set_forward(nod_neighboor);
            if((node->temp_flow) == 0) {
                nod_neighboor = set_temp_flow(nod_neighboor, available);
            }
            else {
                nod_neighboor = set_temp_flow(nod_neighboor, MIN(node->temp_flow, available));
            }
        }
    }
    return node;
}

/*
 *Muy parecida a forward search : si hay flujo y no fuiste visitado en esta
 *llamada ECAML entonces te agrego al corte y a la cola y seteo el ancestro y 
 *el backward
 */
node_t backward_search(node_t node, queue_t queue, queue_t cut) {
    assert(node != NULL);
    assert(queue != NULL);
    assert(strcmp(get_queue_type(queue), "node") == 0 || 
                             (strcmp(get_queue_type(queue), "node_main") == 0));
    assert(strcmp(get_queue_type(cut), "node") == 0 ||
                             (strcmp(get_queue_type(queue), "node_main") == 0));
    
    u32 i = 1;
    tuple_t tup_neighboor = NULL;
    node_t nod_neighboor = NULL;
    member_t member = NULL;
    
    member_t new_member = NULL;
    u32 node_temp_flow = node->temp_flow;
    u32 available = 0;
    
    while(i <= queue_size(node->back_neighboor)) {
        available = 0;
        member = get_member(node->back_neighboor, i);
        i = i + 1;
        tup_neighboor = (tuple_t)(get_content(member));
        nod_neighboor = tuple_get_node(tup_neighboor);
        
        available = tuple_get_flow(tup_neighboor);
        if(!is_visited(nod_neighboor) && (available > 0)) {
            visit(nod_neighboor);
            new_member = member_create(nod_neighboor, "node");
            queue = enqueue(queue, new_member);
            new_member = member_create(nod_neighboor, "node");
            cut = enqueue(cut, new_member);
            set_ancestor(nod_neighboor, node);
            set_backward(nod_neighboor);
            if(node_temp_flow == 0) {
                node_temp_flow = available;
            }
            else {
                node_temp_flow = MIN(node_temp_flow, available);
            }
            nod_neighboor = set_temp_flow(nod_neighboor, node_temp_flow);
        }
    }
    return node;
}

/*
 *Otra funcion muy muy util para el debugeo con GDB :3 ...
 *Muestro por stdout las informaciones del nodo
 */
void display_info_node(node_t node) {
    printf("\n---Displaying node info---\n");
    printf("My vertex id is : %lu\n", vertex_id(node->vertex));
    if(node->is_visited) {
        printf("I have already been visited.\n");
    }
    else {
        printf("Nobody visited me for the moment.\n");
    }
    printf("I have %lu backward neighboors.\n"
    , queue_size(node->back_neighboor));
    printf("They are :\n");
    info_queue_tuple(node->back_neighboor);
    printf("I have %lu forward neighboors.\n"
    , queue_size(node->front_neighboor));
    printf("They are :\n");
    info_queue_tuple(node->front_neighboor);
}
