#ifndef _LLIST_H_
#define _LLIST_H_

typedef struct LListEntry{
  void *value;
  struct LListEntry *next;
  struct LListEntry *prev;
}LListEntry;

#endif
