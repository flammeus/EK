#include "API.h"

#define MAX_SIZE 33
#define FORWARD 1
#define BACKWARD -1
#define NOT_SET 0

/*Estructura principal. Guarda todas las informaciones importantes del "grafo".
 *Contiene : El nodo source (0) , el sink (1), una cola de todos los nodos 
 *presentes en el grafo (util para limpiar directamente al final :) ) y otra 
 *estructura (info) que vamos a detallar mas adelante.
 */
struct DragonSt {
    node_t source, sink;
    queue_t queue;
    u64 flow;
    info_t info;
};

/*Estructura auxiliar. Permite guardar las informaciones generales como :
 *una cola de nodos que reprensenta el corte (los nodos por donde pasamos en 
 *ECAML) , una cola path que representa el camino de sink hasta source. un 
 *numero (number) que se incrementa cada vez que encontramos un camino
 *aumentante y informacion auxiliar (sobre todo para la funcion donde estamos 
 *parados).
 */
struct _info_t {
    queue_t cut;
    queue_t path;
    u32 number; //count of aumenting path found
    u32 flow_path; //temp flow in edge
    int is_flow_max;
    int last_call_ECAML;
    int ECAML_data_has_been_used;
};

/*Funcion para inicializar la estructura info. Alloca memoria , inicializa los
 *campos y devuelve un puntero a ella.
 */
info_t info_init(void) {
    info_t info = calloc(1, sizeof(struct _info_t));
    assert(info != NULL);
    
    info->cut = NULL;
    info->path = NULL;
    info->number = 0;
    info->flow_path = 0;
    info->is_flow_max = false;
    info->last_call_ECAML = -1;
    info->ECAML_data_has_been_used = 0;    
    
    return info;
}

/*Funcion para destruir la estructura info.
 *
 */
info_t info_destroy(info_t info) {
    assert(info != NULL);
    if(info->cut != NULL) {
        info->cut = queue_destroy(info->cut);
    }
    if(info->path != NULL) {
        info->path = queue_destroy(info->path);
    }
    free(info);
    return NULL;
}

/*La funcion aloca memoria e inicializa una estructura DragonSt y devuelve
 *un puntero a esta.
 *En caso de error , la funcion devolvera un puntero a NULL
 */
DragonP NuevoDragon(void) {
    
     DragonP drag_pointer = calloc(1, sizeof(struct DragonSt));
     if(drag_pointer != NULL) {
        drag_pointer->queue = queue_create("node_main");
        drag_pointer->info = info_init(); 
        drag_pointer->source = NULL;
        drag_pointer->sink = NULL;
        drag_pointer->flow = 0;
     }
     return drag_pointer;
}

/*Destruye N y libera la memoria asociada.
 *Retorna 1 si todo anduvo bien y 0 si no.
 */
int DestruirDragon(DragonP N) {
    
    N->queue = queue_destroy(N->queue);
    N->info = info_destroy(N->info);
    free(N);
    N = NULL;
    return 1;
}

/*La funcion carga el lado xy con capacidad c en N. La funcion retorna 1 si no
 *hubo problemas y 0 en caso contrario.
 *Informacion extra : El valor 0 representa la fuente y 1 el resumidero. Si 
 *se agrega el lado xy entonces NO hay el lado yx. Tambien se asume que no hay 
 *lados repetidos.
 */
int CargarUnLado(DragonP N, u32 x, u32 y, u32 c) {
    assert(N != NULL);

    node_t node_x = node_create(x);
    node_t node_y = node_create(y);
    
    member_t member_x = member_create(node_x, "node_main");
    member_t member_y = member_create(node_y, "node_main");
    
    void *search = NULL;
    
    search = queue_search(N->queue, member_x);
    
    if(search == NULL) {
        N->queue = enqueue(N->queue, member_x);
        if(x == 0) {
            N->source = node_x;
        }
    }
    else {
        member_x = member_destroy(member_x, "node_main");
        node_x = (node_t)(get_content(search));
    }
    
    search = queue_search(N->queue, member_y);
    if(search == NULL) {
        N->queue = enqueue(N->queue, member_y);
        if(y == 1) {
            N->sink = node_y;
        }
    }
    else {
        member_y = member_destroy(member_y, "node_main");
        node_y = (node_t)(get_content(search));
    }
    
    node_add_neighboor(node_x, node_y, c);
    return 1;
}

/*La funcion lee todos los datos del network donde se correra el algoritmo desde
 *stdin y lo carga en N usando la funcion CargarUnLado(). La funcion retorna 1
 *si no hubo problemas y 0 en caso contrario.
 *Informacion extra : Los datos ingresados deben tener la forma siguiente :
 *"vertex vertex capacity\n". Ademas por seguridad se agrego un MAX_SIZE que
 *no permite leer mas que MAX_SIZE caracteres por stdin (33 por defecto pues 
 *que 2^32 tiene 10 digitos y tenemos 3 espacios entonces 30 digitos 
 *+ 3 espacios). No hay checkeo de overflow (si si , lo pueden romper facil).
 */
int LlenarDragon(DragonP N) {
    u32 x, y, c;
    char *buffer = calloc(1, MAX_SIZE);
    int check = 0;
    int result = 1;
    bool stop = false;
    while(!stop && fgets(buffer, MAX_SIZE, stdin) != NULL) {
        check = sscanf(buffer, "%lu %lu %lu", &x, &y, &c);
        if(check == 3) {
            CargarUnLado(N, x, y, c);
        }
        else {
            if(ferror(stdin)) {
                clearerr(stdin);
                result = 0;
            }
            stop = true;
        }
    }
    if(buffer != NULL) {
        free(buffer);
    }
    return result;
}

/*Funcion auxiliar para ECAML. Se llama si y solo si ECAML llega a t.
 *La funcion "construye" (en realidad solo mira una informacion en cada node 
 *para saber cual fue el ancestro) un camino de s a t y lo guarda en 
 *N->info->path, ademas guarda el flujo del camino aumentante.
 */
void construct_original_path(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    
    queue_t final_queue = queue_create("node");
    
    node_t ancestor = NULL;
    member_t new_member = NULL;
    bool stop = false;
    
    N->info->flow_path = get_temp_flow(N->sink);
    new_member = member_create(N->sink, "node");
    final_queue = enqueue(final_queue, new_member);
    
    ancestor = get_ancestor(N->sink);
    if(ancestor != NULL) {
        while(!stop && !node_compare(N->source, ancestor)) {
            new_member = member_create(ancestor, "node");
            final_queue = enqueue(final_queue, new_member);
            ancestor = get_ancestor(ancestor);
            if(ancestor == NULL) {
                //Stop the loop , something is goind bad
                stop = true;
                final_queue = queue_destroy(final_queue);
            }
        }
        if(!stop && (final_queue != NULL)) {
            new_member = member_create(N->source, "node");
            final_queue = enqueue(final_queue, new_member);
        }
    }
    else {
        //Something is going bad ... go gdb ...
        final_queue = queue_destroy(final_queue);
    }
    if(N->info->path != NULL) {
        N->info->path = queue_destroy(N->info->path);
    }
    N->info->path = final_queue;
}

/*La funcion hace una busqueda de un camino aumentante de menor longitud. Debe 
 *actualizar en N las etiquetas o datos que uds hayan decidido crear asociadas
 *a la busqueda al llegar a t (si llegan) o continuarla hasta que la cola se
 *vacie. Devuelve 1 si llega a t, 0 si no, -1 si hay error.
 */
int ECAML(DragonP N) {
    assert(N->source != NULL);
    assert(N->sink != NULL);
    
    
    bool stop = false;
    int result = 1;
    member_t new_member = NULL;
    
    if(N->info->cut != NULL) {
        N->info->cut = queue_destroy(N->info->cut);
    }
    N->info->ECAML_data_has_been_used = 0;//reset datas
    
    N->source = visit(N->source);
    
    queue_t cut = queue_create("node");
    new_member = member_create(N->source, "node");
    cut = enqueue(cut, new_member);
    
    queue_t queue = queue_create("node");
    new_member = member_create(N->source, "node");
    queue = enqueue(queue, new_member);
    
    node_t node = NULL;
    
    member_t queue_member = pop(queue);
    //Mientras no llegamos a s && no hay "error" entonces seguimos buscando...
    while(!stop && !node_compare(N->sink, (node_t)(get_content(queue_member)))){
        node = (node_t)(get_content(queue_member));
        forward_search(node, queue, cut);
        backward_search(node, queue, cut);
        queue_member = member_destroy(queue_member, "node");
        if(!queue_empty(queue)) {
            queue_member = pop(queue);
        }
        else {
            stop = true;
            result = 0;
            N->info->last_call_ECAML = 0;
            N->info->is_flow_max = 1;
        }
    }
    if(queue_member != NULL) {
        queue_member = member_destroy(queue_member, "node");
    }
    if(!stop) {
        N->info->number = N->info->number + 1; 
        N->info->last_call_ECAML = 1;
        construct_original_path(N);
        if(N->info->path == NULL) {
            result = -1;
        }
    }
    queue = queue_destroy(queue);
    N->info->cut = cut;
    return result;
}


/*Funcion rara ... Permite debugear supongo. En todo caso devuelve informacion
 *sobre el grafo (alpha = 1 si el flujo es maximal, beta = 1 si se llego a t, 
 *gamma = 1 si los resultados de la ultima busqueda ECAML han sido usados).
 */
int DondeEstamosParados(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    assert(N->info->last_call_ECAML != -1);
    
    int alpha = N->info->is_flow_max;
    int beta = N->info->last_call_ECAML;
    int gamma = N->info->ECAML_data_has_been_used;
    
    return (100 * alpha + 10 * beta + gamma);
}

/*Cambia el flujo a lo largo del camino aumentante (que se construyo en la func
 *construct_original_path().
 */
u32 AumentarFlujo(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    assert(N->info->path != NULL);
    assert(strcmp(get_queue_type(N->info->path), "node") == 0);
    assert(!queue_empty(N->info->path));
    
    
    u32 increase_flow = 0;
    int way = 0;
    
    if(N->info->last_call_ECAML == 1 && N->info->ECAML_data_has_been_used == 0){
    
        N->info->ECAML_data_has_been_used = 1;
        
        increase_flow = N->info->flow_path;
        assert(increase_flow > 0);
        N->info->flow_path = 0;
        
        queue_t path = N->info->path;//contain all the node of the aumenting path
        //first node must be sink and last source
        assert(node_compare(N->sink, (node_t)(get_content(get_member(path, 1)))));
        assert(node_compare(N->source, (node_t)(get_content(get_last(path)))));
        
        node_t node1 = NULL;
        node_t node2 = NULL;
        
        member_t member = NULL;
        member_t member1 = NULL;
        member_t member2 = NULL;
        member_t member_search = NULL;
        tuple_t tuple = NULL;
        
        queue_t tuple_queue1 = NULL;
        queue_t tuple_queue2 = NULL;
        
        u32 i = 1;
        u32 len = queue_size(path);
        
        member1 = get_member(path, i);
        node1 = (node_t)(get_content(member1));
        
        while(i < len) {
            member2 = get_member(path, i + 1);
            node2 = (node_t)(get_content(member2));
            if(node_compare(N->source, node2)) {
                way = FORWARD;
            }
            else {
                way = get_way(node1);
            }
            if(way == FORWARD) {
                tuple_queue2 = get_forward_queue(node2);
                tuple_queue1 = get_backward_queue(node1);
            }
            else {
                tuple_queue2 = get_backward_queue(node2);
                tuple_queue1 = get_forward_queue(node1);
            }
            member_search = member_create(tuple_create(node1, 0), "tuple");//tuple contain fake
            member = queue_search(tuple_queue2, member_search);//tuple but don't contain fake so just free him
            assert(member != NULL);
            
            tuple = (tuple_t)(get_content(member));
            if(way == FORWARD) {
                tuple = let_pass_flow(tuple, increase_flow);
            }
            else {
                tuple = get_back_flow(tuple, increase_flow);
            }
            member_search = member_destroy(member_search, "tuple");
            
            member_search = member_create(tuple_create(node2, 0), "tuple");
            member = queue_search(tuple_queue1, member_search);
            assert(member != NULL);
            
            tuple = (tuple_t)(get_content(member));
            if(way == FORWARD) {
                tuple = let_pass_flow(tuple, increase_flow);
            }
            else {
                tuple = get_back_flow(tuple, increase_flow);
            }
            member_search = member_destroy(member_search, "tuple");
            node1 = node2;
            member1 = member2;
            i = i + 1;
        }
        assert(N->info->cut !=  NULL);
        clear_all_node(N->info->cut);
    }
    N->flow = Sumar64(N->flow, increase_flow);
    return increase_flow;
}

/*Hace lo mismo que AumentarFlujo() pero ademas imprime el camino aumentante y 
 *el aumento a lo largo de el.
 */
u32 AumentarFlujoYtambienImprimirCamino(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    assert(N->info->path != NULL);
    assert(strcmp(get_queue_type(N->info->path), "node") == 0);
    
    
    u32 flow_path = AumentarFlujo(N);
    if(flow_path != 0) {
        queue_t path = N->info->path;
        assert(!queue_empty(path));
        
        int wich_way = 0;
        u32 i = 1;
        u32 number = N->info->number;
        printf("Camino aumentante %lu:\n", number);
        u32 len = queue_size(path);
        u32 id_vertex = 0;
        member_t member = NULL;
        node_t node = NULL;
        
        while(i <= len) {
            member = get_member(path, i);
            node = (node_t)(get_content(member));
            assert(node != NULL);
            id_vertex = vertex_id(get_node_vertex(node));
            if(id_vertex == 0) {
                printf("s");
                fflush(stdout);
            }
            else if(id_vertex == 1) {
                printf("t");
                fflush(stdout);
            }
            else {
                printf("%lu", id_vertex);
                fflush(stdout);
            }
            if(node_compare(node, N->source)) {
                wich_way = FORWARD;
            }
            else {
                wich_way = get_way(node);
            }
            if(i != len) {
                if(wich_way == FORWARD) {
                    printf(":");
                }
                else {
                    printf("|");
                }
            }
        i = i + 1;
        }
        printf(" <%lu>\n\n", flow_path);
    }
    return flow_path;
}

/*Imprime el flujo que se tiene en ese momento (por stdout).
 *
 */
void ImprimirFlujo(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    assert(N->info->path != NULL);
    assert(strcmp(get_queue_type(N->info->path), "node") == 0);
    
    queue_t path = N->info->path;
    assert(!queue_empty(path));
    
    int wich_way = 0;
    u32 i = 1;
    u32 len = queue_size(path);
    
    node_t node1 = NULL;
    node_t node2 = NULL;
    
    vertex_t ver1 = NULL;
    vertex_t ver2 = NULL;
    
    u32 id1 = 0;
    u32 id2 = 0;
    u32 flow = 0;
    queue_t queue = NULL;
    
    member_t search = NULL;
    member_t temp_mem = NULL;
    
    ImprimirValorFlujo(N);
    node1 = (node_t)(get_content(get_member(path, i)));
    
    while(i < len) {
        node2 = (node_t)(get_content(get_member(path, i + 1)));
        i = i + 1;
        
        ver1 = get_node_vertex(node1);
        ver2 = get_node_vertex(node2);
        
        id1 = vertex_id(ver1);
        id2 = vertex_id(ver2);
        if(node_compare(node1, N->source)) {
            wich_way = FORWARD;
        }
        else {
            wich_way = get_way(node1);
        }
        if( wich_way == FORWARD) {
            queue = get_forward_queue(node2);
        }
        else {
            queue = get_backward_queue(node2);
        }
        temp_mem = member_create(tuple_create(node1, 0), "tuple");
        search = queue_search(queue, temp_mem);
        assert(search != NULL);
        temp_mem = member_destroy(temp_mem, "tuple"); 
        flow = tuple_get_flow((tuple_t)(get_content(search)));
        printf("Lado %lu,%lu: <%lu>\n", id1, id2, flow);
        node1 = node2;
    }
    printf("\n");
}

/*Imprime el valor del flujo por stdout.
 *
 */
void ImprimirValorFlujo(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    assert(N->info->path != NULL);
    
    char *max_or_not = NULL;
    u64 flow = 0;
    
    if(N->info->is_flow_max == 1) {
        max_or_not = "maximal";
    }
    else {
        max_or_not = "no maximal";
    }
    flow = N->flow;
    printf("Flujo %s: <%f>\n", max_or_not, flow);
}

/*Imprime el corte y la capacidad de este.
 *
 */
void ImprimirCorte(DragonP N) {
    assert(N != NULL);
    assert(N->info != NULL);
    
    queue_t cut = N->info->cut;
    assert(cut != NULL);
    assert(!queue_empty(cut));
    
    u32 i = 1;
    u32 len = queue_size(cut);
    
    u32 i_neighboor = 1;
    u32 len_neighboor = 0;
    u64 capacity = 0;
    
    member_t member = NULL;
    node_t node = NULL;
    member_t mem_neigh = NULL;
    node_t node_v = NULL;
    member_t temp_mem = NULL;
    tuple_t tuple = NULL;
    vertex_t vertex = NULL;
    u32 id = 0;
    
    queue_t queue_fneighboor = NULL;
    
    printf("S = {");
    fflush(stdout);
    while(i <= len) {
        member = get_member(cut, i);
        node = (node_t)(get_content(member));
        vertex = get_node_vertex(node);
        id = vertex_id(vertex);
        if(id == 0) {
            printf(" s ");
            fflush(stdout);
        }
        else if(id == 1) {
            printf(" t ");
            fflush(stdout);
        }
        else {
            printf(" %lu ", id);
            fflush(stdout);
        }
        queue_fneighboor = get_forward_queue(node);
        len_neighboor = queue_size(queue_fneighboor);
        i_neighboor = 1;
        while(i_neighboor <= len_neighboor) {
            mem_neigh = get_member(queue_fneighboor, i_neighboor);
            tuple = (tuple_t)(get_content(mem_neigh));
            node_v = tuple_get_node(tuple);
            temp_mem = member_create(node_v, "node");
            if(queue_search(cut, temp_mem) == NULL) {
                capacity = Sumar64(capacity, tuple_capacity_available(tuple));
                capacity = Sumar64(capacity, tuple_get_flow(tuple));
            }
            temp_mem = member_destroy(temp_mem, "node");
            i_neighboor = i_neighboor + 1;
        }
        i = i + 1;
    }
    printf("}\n");
    printf("Capacidad: <%f>\n\n", capacity);
}

/*Suma un u32 con un u64 y devuelve el u64.
 *
 */
u64 Sumar64(u64 a, u32 b) {
    a = a + b;
    return a;
}
