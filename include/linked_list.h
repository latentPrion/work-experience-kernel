#ifndef _WEK_LINKED_LIST_H
#define _WEK_LINKED_LIST_H

#include <stddef.h>

struct list_element_;
typedef struct list_element_ list_element_t;
struct list_element_ {
    list_element_t *next;
};

struct linked_list_;
typedef struct linked_list_ linked_list_t;
struct linked_list_ {
    list_element_t *head;
};

static inline int list_init(linked_list_t *list)
{
    list->head = NULL;
    return 0;
}

static inline void list_prepend(linked_list_t *list, list_element_t *item)
{
    if (list == NULL || item == NULL) { return; }

    item->next = list->head;
    list->head = item;
}

static inline void list_append(linked_list_t *list, list_element_t *item)
{
    list_element_t *curr;

    if (list == NULL || item == NULL) { return; }

    item->next = NULL;

    if (list->head == NULL)
    {
        list->head = item;
        return;
    }

    for (curr = list->head; curr != NULL; curr = curr->next)
    {
        if (curr->next == NULL)
        {
            curr->next = item;
            return;
        }
    }
}

static inline list_element_t *list_pop_head(linked_list_t *list)
{
    list_element_t *ret;

    ret = list->head;
    if (list->head != NULL) {
        list->head = list->head->next;
    }

    return ret;
}

static inline int list_remove(linked_list_t *list, list_element_t *item)
{
    list_element_t *curr, *prev=NULL;

    if (list->head == item) {
        list->head = list->head->next;
        return 0;
    }

    for (curr = list->head; curr != NULL; prev = curr, curr = curr->next)
    {
        if (curr != item) { continue; }

        prev->next = curr->next;
        return 0;
    }

    // Not found
    return -1;
}

#endif
