/*
    anagrhash v0.1 - an anagram generator/hash reverser
    Copyright (C) 2008 Simone Baracchi

    See LICENSE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>

#include "anagram.h"
#include "main.h"
#include "result.h"

int max_key_length;

char* print( GSList *list ) {
	int i;
	char* buf = malloc(max_key_length);
	buf[0] = '\0';
	for( i=0; i<g_slist_length(list); i++ ) {
		sprintf(buf, "%s %s", buf, (char*)g_slist_nth_data( list, i ) );
	}
	return buf;
}

int generate_combination( int depth, GSList **fixed, GSList **tokens, int t_size, GSList **xtokens, int x_size, int canbenull ) {
	// Should we check our results and stop recursion?
	if( depth == token_limit || t_size+x_size == 0 ) {
		if( *fixed == NULL ) return ERROR;
		int result = ERROR;
		if( separators == NULL ) {
			char *buf = malloc(max_key_length);
			if( buf == NULL )
				return ERROR;
			buf[0] = '\0';
			GSList *list_it = *fixed;
			do{
				sprintf(buf, "%s%s", buf, (char*)list_it->data );
			}while( (list_it = g_slist_next(list_it)) != NULL );
			result = handle_result( buf );
		} else {
			int j;
			// TODO this is inefficient
			int seplength = strlen( separators );
			for(j=0; j<seplength; j++) {
				int first = TRUE;
				char *buf = malloc(max_key_length);
				if( buf == NULL )
					return ERROR;
				buf[0] = '\0';
				GSList *list_it = *fixed;
				do{
					//strcat(buf, list_it->data );
					if( first ) {
						sprintf(buf, "%s", (char*)list_it->data );
						first = FALSE;
					} else {
						sprintf(buf, "%s%c%s", buf, separators[j], (char*)list_it->data );
					}
				}while( (list_it = g_slist_next(list_it)) != NULL );
				if( handle_result(buf) == SUCCESS ) {
					result = SUCCESS;
					break;
				}
			}
		}
		// we let handle_result free the memory when he's done
		//free( buf );
		return result;
	} else {
		int i=0;
		/* Try a null token first, so that we first check shorter keys */
		if( canbenull ) {
			if( generate_combination( depth+1, fixed, tokens, t_size, xtokens, x_size, TRUE ) == SUCCESS )
				return SUCCESS;
		}
		/* tokens */
		for( i=0; i<t_size; i++ ) {
			// move an element from tokens to fixed 
			// TODO nth, then remove_link ... it's inefficient for big lists.
			GSList *moving = g_slist_nth( *tokens, i );
			char *data = moving->data;
			*tokens = g_slist_remove_link( *tokens, moving );
			g_slist_free_1(moving);
			*fixed = g_slist_prepend( *fixed, data );
			// call this method again 
			if( generate_combination( depth+1, fixed, tokens, t_size-1, xtokens, x_size, FALSE ) == SUCCESS )
				return SUCCESS;
			// remove the head, but leave the data!
			moving = *fixed;
			*fixed = g_slist_remove_link( *fixed, *fixed );
			g_slist_free_1( moving );
			// place everything back 
			*tokens = g_slist_insert( *tokens, data, i );
		}
		/* x-tokens */
		for( i=0; i<x_size; i++ ) {
			int j;
			GSList* xtok = g_slist_nth_data( *xtokens, i );
			int xtok_size = g_slist_length( xtok );
			// remove the xtoken-list from the big list
			*xtokens = g_slist_remove( *xtokens, xtok );
			for( j=0; j<xtok_size; j++ ) {
				GSList* moving = g_slist_alloc();
				// make a copy of the data
				moving->data = g_slist_nth_data( xtok, j );
				// place the new token in the string
				//moving->next = *fixed; *fixed = moving;
				*fixed = g_slist_prepend( *fixed, moving->data );
				if( generate_combination( depth+1, fixed, tokens, t_size, xtokens, x_size-1, FALSE ) == SUCCESS )
					return SUCCESS;
				// remove the token from the string
				*fixed = g_slist_delete_link( *fixed, *fixed );
				g_slist_free_1(moving);
			}
			// place the list back
			*xtokens = g_slist_insert( *xtokens, xtok, i );
		}
		return ERROR;
	}
}


int search( GSList *tokens, GSList *xtokens ) {
	int t_size = g_slist_length( tokens );
	int x_size = g_slist_length( xtokens );
	token_limit = (token_limit < t_size + x_size ? token_limit : t_size + x_size);

	/* TODO Find the maximum length of the key */
	max_key_length = 1024;

	GSList* fixed = NULL;
	return generate_combination( 0, &fixed, &tokens, t_size, &xtokens, x_size, TRUE );
}
