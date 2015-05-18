#ifndef __LIST_H__
#define __LIST_H__

#include <glib.h>

GSList *list_new();
GSList *list_prepend( GSList *list, void *data );
GSList *list_nth( GSList *list, unsigned long int index );
void list_free( GSList *list );
void list_free_1( GSList *list );

#endif
