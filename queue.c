#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *q = malloc(sizeof(struct list_head));
    if (!q)
        return NULL;
    INIT_LIST_HEAD(q);
    return q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l) {
    if (!l)
        return;

    struct list_head *node, *safe;
    list_for_each_safe(node, safe, l) {
        element_t *ele = list_entry(node, element_t, list);
        list_del(&ele->list);
        q_release_element(ele);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *newnode = malloc(sizeof(element_t));
    if (!newnode)
        return false;
    int s_len = strlen(s) + 1;
    newnode -> value = malloc(sizeof(char) * (s_len));
    if (!newnode -> value) {
        free(newnode);
        return false;
    }
    memcpy(newnode -> value, s, s_len);
    list_add(&newnode -> list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *newnode = malloc(sizeof(element_t));
    if (!newnode)
        return false;
    int s_len = strlen(s) + 1;
    newnode -> value = malloc(sizeof(char) * (s_len));
    if (!newnode -> value) {
        free(newnode);
        return false;
    }
    memcpy(newnode -> value, s, s_len);
    list_add_tail(&newnode -> list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *ele = list_first_entry(head, element_t, list);
    list_del_init(&ele->list);
    if (sp) {
        // memcpy(sp, ele->value, bufsize);
        strncpy(sp, ele->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return ele;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
    return NULL;
    element_t *ele = list_last_entry(head, element_t, list);
    list_del_init(&ele->list);
    if (sp) {
        // memcpy(sp, ele->value, bufsize);
        strncpy(sp, ele->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return ele;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int cnt = 0;
    struct list_head *temp;
    list_for_each(temp, head)
        cnt += 1;
    return cnt;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }
    element_t *del = list_entry(slow, element_t, list);
    list_del(slow);
    q_release_element(del);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    struct list_head *node, *safe;
    bool dup = false;
    list_for_each_safe(node, safe, head) {
        if (node->next != head &&
            !strcmp(list_entry(node, element_t, list)->value,
                    list_entry(node->next, element_t, list)->value))
        {
            dup = true;
            element_t *del = list_entry(node, element_t, list);
            list_del(node);
            q_release_element(del);
        }
        else if (dup) {
            dup = false;
            element_t *del = list_entry(node, element_t, list);
            list_del(node);
            q_release_element(del);
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    q_reverseK(head, 2);
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {

    if (!head)
        return;
    struct list_head *cur = head;
    struct list_head *next = head -> next;
    do {
        cur -> next = cur -> prev;
        cur -> prev = next;
        cur = next;
        next = cur -> next;
    }while (cur != head);
}

int length(struct list_head *start, struct list_head *head) {
    int cnt = 0;
    while (start != head) {
        cnt += 1;
        start = start->next;
    }
    return cnt;
}
/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (k <= 1 || list_empty(head))
        return;
    int cnt = 0;
    struct list_head *cur = head->next, *start = head, *next;
    LIST_HEAD(rev);
    while (cur != head) {
        while (cnt < k - 1  && cur != head && cur->next != head) {
            cur = cur->next;
            cnt += 1;
        }
        cnt = 0;
        next = cur->next;
        if (length(start, cur) >= k) {
            list_cut_position(&rev, start, cur);
            q_reverse(&rev);
            list_splice_init(&rev, start);
        }
        cur = next;
        start = next->prev;
    }
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

struct list_head *merge(struct list_head *left, struct list_head *right)
{
    struct list_head *head = NULL, **ptr = &head, **node;
    for (node = NULL; left && right; *node = (*node)->next) {
        element_t *ele1 = list_entry(left, element_t, list);
        element_t *ele2 = list_entry(right, element_t, list);
        node = strcmp(ele1->value, ele2->value) <= 0 ? &left : &right;
        *ptr = *node;
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head*)((uintptr_t) left | (uintptr_t) right);
    return head;
}
struct list_head* mergeSortList(struct list_head *head)
{
    if (!head || !head->next) 
        return head;
    
    struct list_head *slow = head;
    struct list_head *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *mid = slow->next;
    slow->next = NULL;
    struct list_head *left = mergeSortList(head);
    struct list_head *right = mergeSortList(mid);
    return merge(left, right);

}
/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {
    
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    
    head->prev->next = NULL;
    head->next = mergeSortList(head->next);
    struct list_head *first = head;
    struct list_head *second = head->next;
    while (second) {
        second->prev = first;
        first = first->next;
        second = second->next;
    }
    first->next = head;
    head->prev = first;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}