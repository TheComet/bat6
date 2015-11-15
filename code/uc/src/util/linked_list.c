/*!
 * @file linked_list.c
 * @author Alex Murray
 * 
 * Created on 15 November 2015, 03:35
 */

#include "util/linked_list.h"
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
struct linked_list_t* linked_list_create(void)
{
    struct linked_list_t* list;
    if(!(list = (struct linked_list_t*)malloc(sizeof *list)))
        return NULL;
    
    memset(list, 0, sizeof *list);
    return list;
}

/* -------------------------------------------------------------------------- */
void linked_list_destroy(struct linked_list_t* list)
{
    struct linked_list_node_t* node = list->head;
    struct linked_list_node_t* next;
    while(node)
    {
        next = node->next;
        free(node);
        node = next;
    }
    free(list);
}

/* -------------------------------------------------------------------------- */
void linked_list_push(struct linked_list_t* list,
                      struct linked_list_node_t* node)
{
    /* link new node as next item to current head node */
    if(list->tail)
        list->tail->next = node;
    else
        list->head = node; /* if this is the first node, set as head */
    
    /* update tail */
    list->tail = node;
    
    /* update size */
    ++(list->size);
}
