#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#include "queue.h"

typedef bool element_cmp_t(element_t const *lhs, element_t const *rhs);

static inline bool element_greater(element_t const *lhs, element_t const *rhs)
{
    return strcmp(lhs->value, rhs->value) > 0;
}

static inline bool element_less(element_t const *lhs, element_t const *rhs)
{
    return strcmp(lhs->value, rhs->value) < 0;
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list)
        q_release_element(entry);
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *element = malloc(sizeof(element_t));
    if (!element)
        return false;
    element->value = NULL;
    if (s) {
        element->value = strdup(s);
        if (!element->value) {
            free(element);
            return false;
        }
    }
    list_add(&element->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *element = list_first_entry(head, element_t, list);
    list_del(&element->list);
    if (sp && element->value) {
        *stpncpy(sp, element->value, bufsize - 1) = '\0';
    }
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head)
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int len = 0;
    struct list_head *node;
    list_for_each(node, head)
        ++len;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;

    bool last_dup = false;
    element_t *curr, *next;
    list_for_each_entry_safe(curr, next, head, list) {
        bool dup = &next->list != head && !strcmp(curr->value, next->value);
        if (last_dup || dup) {
            list_del(&curr->list);
            q_release_element(curr);
        }
        last_dup = dup;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    struct list_head **indir = &head->next;
    while (*indir != head && (*indir)->next != head) {
        struct list_head *curr = *indir;
        *indir = curr->next;
        curr->next = (*indir)->next;
        (*indir)->next = curr;
        (*indir)->prev = curr->prev;
        curr->prev = *indir;
        indir = &curr->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *node, *safe;
    list_for_each_safe(node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head)
        return;

    struct list_head *node;
    list_for_each(node, head) {
        struct list_head *safe, *probe = node;
        int i;
        for (i = 0; i < k && probe != head && probe->next != head; ++i)
            probe = probe->next;
        if (i != k)
            break;
        for (i = 0; i < k; ++i) {
            safe = node->next;
            node->next->prev = node->prev;
            node->prev->next = node->next;
            node->next = probe;
            node->prev = probe->prev;
            probe->prev->next = node;
            probe->prev = node;
            node = safe;
            probe = probe->prev;
        }
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

static inline int ascend_descend_impl(struct list_head *head,
                                      element_cmp_t *cmp)
{
    if (!head)
        return 0;

    int i = 0;
    element_t *pivot = NULL;
    for (element_t *curr = list_last_entry(head, element_t, list),
                   *safe = list_entry(curr->list.prev, element_t, list);
         &curr->list != head;
         curr = safe, safe = list_entry(safe->list.prev, element_t, list)) {
        if (pivot && cmp(curr, pivot)) {
            list_del(&curr->list);
            q_release_element(curr);
        } else {
            ++i;
            pivot = curr;
        }
    }
    return i;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    return ascend_descend_impl(head, element_greater);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    return ascend_descend_impl(head, element_less);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
