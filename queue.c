#include "queue.h"

/*Una vieja cola como las conocemos... Lo unico que vale la peno detallar es el 
 *campo type; en realidad es por cuestion de seguridad y sobre todo no 
 *confundirnos. Como podemos tener colas de tuplas y de nodos le pusimos un tipo
 *para no equivocarnos. Es node_main cuando es la cola principal (la que esta en
 *N) esto sinifica que puede destruir completamente los nodos, si es node 
 *entonces libera solamente el member_t (cuando hay que liberar) y si es tuple
 *libera la tupla.
 */
struct _queue_t {
    member_t first;  
    member_t last;
    unsigned long length;
    char *type;//Can be node, tuple and node_main
};

/*Miembro de la cola. Como intentemos hacer una cola general pusimos un void*
 *pero asi es mal facil equivocarse...
 */
struct _member_t {
    void *member;//node or tuple
    member_t next;
    member_t previous;
    char *type;
};

/*Alloca memoria y devuelve un puntero a la estructura. El tipo puede ser
 *cualquier cosa (pero para que se puedan usar todas las funciones tienen que 
 *ser node_main , node o tuple).
 */
queue_t queue_create(char *type) {
    queue_t queue = calloc(1, sizeof(struct _queue_t));
    assert(queue != NULL);
    
    queue->first = NULL;
    queue->last = NULL;
    queue->length = 0;
    queue->type = type;

    return queue;
}

/*Destructor de la cola. Se adapta en funcion del tipo de la cola.
 *
 */
queue_t queue_destroy(queue_t queue) {
    assert(queue != NULL);
    
    member_t current = queue->first;
    member_t temp = current;
    
    while(current != NULL) {
        temp = temp->next;
        current = member_destroy(current, queue->type);
        
        current = temp;
    }
    free(queue);
    return NULL;
}

/*Alloca memoria para un miembro y devuelve un puntero a este.
 *
 */
member_t member_create(void *object, char *type) {
    assert(object != NULL);
    
    member_t new_member = NULL;
    new_member = calloc(1, sizeof(struct _member_t));
    assert(new_member != NULL);
    
    new_member->member = object;
    new_member->next = NULL;
    new_member->previous = NULL;
    new_member->type = type;
    
    return new_member;
}

/*Destructor del miembro , se adapta en funcion del tipo.
 *
 */
member_t member_destroy(member_t member, char *type) {
    assert(member != NULL);
    
    if(strcmp(type, "node_main") == 0) {
        node_destroy(member->member);
    }
    else if(strcmp(type, "tuple") == 0) {
        tuple_destroy(member->member);
    }
    else if(strcmp(type, "node") != 0){
        printf("Warning : Unknown type. Internal member object ins't destroyed.\n");
    }
    
    free(member);
    
    return NULL;
}

/*Funcion encolar para poner el miembro dentro de una queue.
 *
 */
queue_t enqueue(queue_t queue, member_t member) {
    assert(queue != NULL);
    assert(member != NULL);
    assert(strcmp(queue->type, member->type) == 0);
    
    if(queue->first == NULL) {
        queue->first = member;
        queue->last = member;
    }
    else {
        member->previous = queue->last;
        queue->last->next = member;
        queue->last = member;
    }
    queue->length = queue->length + 1;
    
    return queue;
}

/*Saca el primer miembro de una queue , el llamador es responsable para liberar
 *el miembro.
 */
member_t pop(queue_t queue) {
    assert(queue != NULL);
    assert(queue->length > 0);
    
    member_t tmp = queue->first;
    queue->first = queue->first->next;
    if(queue->first == NULL) {
        queue->last = NULL;
    }
    else {
        queue->first->previous = NULL;
    }
    queue->length = queue->length - 1;
    
    return tmp;
}

/*Funcion opcional (que no se usa al final pero la dejamos) hace lo mismo que 
 *pop pero saca el ultimo al lugar del primero.
 */
member_t reverse_pop(queue_t queue) {
    assert(queue != NULL);
    assert(queue->length > 0);
    
    member_t result = queue->last;
    queue->last = queue->last->previous;
    queue->last->next = NULL;
    
    queue->length = queue->length - 1;
    
    return result;
}   

/*Devuelve un puntero al ultimo miembro de la cola pero NO lo saca de la cola.
 *
 */
member_t get_last(queue_t queue) {
    assert(queue != NULL);
    
    return queue->last;
}

/*Devuelve el contenido de un miembro.
 *
 */
void *get_content(member_t member) {
    assert(member != NULL);
    assert(member->member != NULL);
    
    void *result = member->member;
    assert(result != NULL);
    
    return result;
}

/*Devuelve un miembro a la posicion i dentro de una cola.
 *
 */
member_t get_member(queue_t queue, u32 i) {
    assert(queue != NULL);
    assert(i <= queue->length);
    
    member_t current_member = queue->first;
    
    u32 j = 1;
    while(j != i) {
        j = j + 1;
        current_member = current_member->next;
    }
    return current_member;
}

/*Si la cola es vacia entonces devuelve true , si no false.
 *
 */
bool queue_empty(queue_t queue) {
    assert(queue != NULL);
    
    return (queue->length == 0);
}
/*Devuelve el tamano de la cola.
 *
 */
unsigned long queue_size(queue_t queue) {
    assert(queue != NULL);
    
    return queue->length;
}

/*Devuelve el tipo de la cola.
 *
 */
char *get_queue_type(queue_t queue) {
    assert(queue != NULL);
    
    return queue->type;
}

/*Compara dos miembros , si son "iguales" devuelve true , si no false.
 *
 */
bool member_search(member_t member1, member_t member2, char *type) {
    assert(member1 != NULL);
    assert(member2 != NULL);
    assert(strcmp(member1->type, member2->type) == 0);
    
    void *content1 = member1->member;
    void *content2 = member2->member;
    
    assert(content1 != NULL);
    assert(content2 != NULL);
    
    bool result = false;
    
    if(strcmp(type, "tuple") == 0) {
        tuple_t tuple1 = (tuple_t)(content1);
        tuple_t tuple2 = (tuple_t)(content2);
        
        result = tuple_compare(tuple1, tuple2);
    }
    else if((strcmp(type, "node") == 0) || (strcmp(type, "node_main") == 0)) {
        node_t node1 = (node_t)(content1);
        node_t node2 = (node_t)(content2);
        
        result = node_compare(node1, node2);
    }
    else {
        printf("Unknow member type : %s\n", type);
    }
    return result;
}

/*Busca un miembro dentro de una cola. Se adapta en funcion del tipo de la cola.
 *Si se encuentra el miembro entonces la funcion devuelve un puntero a este, 
 *si no devuelve NULL.
 */
member_t queue_search(queue_t queue, member_t member) {
    assert(queue != NULL);
    assert(member != NULL);
    assert(strcmp(queue->type, member->type) == 0);

    bool found = false;
    void *result = NULL;
    if(!queue_empty(queue)) {
        member_t queue_member = queue->first;
        assert(queue_member != NULL);
        
        
        if((strcmp(queue->type, "node") == 0) || 
                                      (strcmp(queue->type, "node_main") == 0)) {
            while(!found && (queue_member != NULL)) {
                if(member_search(queue_member, member, "node")) {
                    found = true;
                    result = queue_member;
                }
                else {
                    queue_member = queue_member->next;
                }
            }
        }
        else if(strcmp(queue->type, "tuple") == 0) {
            while(!found && (queue_member != NULL)) {
                if(member_search(queue_member, member, "tuple")) {
                    found = true;
                    result = queue_member;
                }
                else {
                    queue_member = queue_member->next;
                }
            }
        }
        else {
            printf("Warning : Queue type isn't defined. Unable to compare\n");
        }
    }
    return result;
}

/*Funcion que permite borrar toda la informacion que se puso en cada node
 *despues de la llamada a ECAML.
 */
void clear_all_node(queue_t queue) {
    assert(queue != NULL);
    assert(strcmp(queue->type, "node") == 0);
    
    member_t member = NULL;
    node_t node = NULL;
    u32 i = 1;
    u32 len = queue_size(queue);
    while(i <= len) {
        member = get_member(queue, i);
        node = (node_t)(get_content(member));
        unvisit(node);
        set_temp_flow(node, 0);
        unset_ancestor(node);
        i = i + 1;
    }
}

/*Funcion muy util que me permitio encontrar muchos bugs y memory corruption
 *con GDB y Valgrind.
 *Muestra informacion sobre la cola , lo que contiene por ejemplo (los nombres 
 *de los vertices y la cantidad de vertice que tiene).
 */
void info_queue_node(queue_t queue) {
    assert(queue != NULL);
    assert((strcmp(queue->type, "node") == 0) || 
                                       (strcmp(queue->type, "node_main") == 0));
    
    u32 i = 1;
    node_t node = NULL;
    member_t member = NULL;
    while(i <= queue->length) {
        member = get_member(queue, i);
        i = i + 1;
        node = (node_t)(member->member);
        printf("%lu:", vertex_id(get_node_vertex(node)));
    }
}
/*Muestra informacion de todas las tuplas en una queue.
 *
 */
void info_queue_tuple(queue_t queue) {
    assert(queue != NULL);
    assert(strcmp(queue->type, "tuple") == 0);
    
    u32 i = 1;
    tuple_t tuple = NULL;
    member_t member = NULL;
    while(i <= queue->length) {
        member = get_member(queue, i);
        i = i + 1;
        tuple = (tuple_t)(member->member);
        printf("%lu ", vertex_id(get_node_vertex(tuple_get_node(tuple))));
        fflush(stdout);//If buffer stdout is full you can disable it : 
        //setbuf(stdout, NULL);
    }
    printf("\n");
}

/*Tambien funcion MUY UTIL para el debugging. Muestra informacion de todos los
 *nodos en la cola.
 */
void display_info_queue(queue_t queue) {
    assert(queue != NULL);
    assert(strcmp(queue->type, "node") == 0);
    
    u32 i = 1;
    u32 j = queue_size(queue);
    member_t member = NULL;
    node_t node = NULL;
    vertex_t vertex = NULL;
    u32 id = 0;
       
    printf("\n---Displaying node queue---\n");
    printf("My queue type is %s\n", get_queue_type(queue));
    printf("I contain %lu members\n", queue_size(queue));
    while(i <= j) {
        member = get_member(queue, i);
        node = (node_t)(get_content(member));
        vertex = get_node_vertex(node);
        id = vertex_id(vertex);
        printf("%lu ",id); 
        fflush(stdout);
        i = i + 1;
    }
    printf("\n");
}
