#include <stdio.h>
#include <stdlib.h>

#include "list.h"

GSList *list_new() {
	GSList *newitem = (GSList*)malloc( sizeof( GSList ) );
	if( newitem == NULL ) {
		fprintf(stderr, "ERROR - LIST.H - Cannot malloc()\n");
		exit( 1 );
	}
	return newitem;
}

GSList *list_prepend( GSList *list, void *data ) {
		GSList *newitem = list_new();
                newitem->data = data;
                newitem->next = list;
                return newitem;
}

GSList *list_nth( GSList *list, unsigned long int index ) {
	int i;
        GSList *result = list;
        for( i=0; i<index && result != NULL; i++ ) {
                result = result->next;
        }
	return result;
}

void list_free( GSList *list ) {
	GSList *p, *nextp;
	p = list;
	do{
		nextp = p->next;
		free( p->data );
		free( p );
	}while( (p=nextp) != NULL );
}

void list_free_1( GSList *list ) {
	free( list );
}
