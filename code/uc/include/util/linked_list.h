/*!
 * @file linked_list.h
 * @author Alex Murray
 * 
 * Created on 14 November 2015, 03:35
 */

#ifndef LEDS_H
#define	LEDS_H

#ifdef	__cplusplus
extern "C" {
#endif
    
/*!
 * @brief List node.
 */
struct linked_list_node_t
{
    struct linked_list_node_t* next;
};

/*!
 * @brief Linked list object.
 */
struct linked_list_t
{
    struct linked_list_node_t* head; /* first item in list */
    struct linked_list_node_t* tail; /* last item in list */
    unsigned short             size;
};

/*!
 * @brief Allocates a new linked list object.
 * @return A pointer to the linked list object in memory. Returns NULL on
 * failure.
 */
struct linked_list_t* linked_list_create(void);

/*!
 * @brief Destroys a linked list object.
 * @warning This will iterate and call free() on all inserted items.
 * @param[in] list The list to destroy.
 */
void linked_list_destroy(struct linked_list_t* list);

/*!
 * @brief Inserts a new item at the end of the list.
 * @param[in] list The list object to insert the item into.
 * @param[in] node The node to be linked in. This is an intrusive linked list
 * implementation, which means the item being inserted must have a "next"
 * member as the first item in its struct when casted to linked_list_node_t.
 */
void linked_list_push(struct linked_list_t* list,
                      struct linked_list_node_t* node);

#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */
