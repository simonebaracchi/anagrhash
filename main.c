/*
    anagrhash v0.1 - an anagram generator/hash reverser
    Copyright (C) 2008 Simone Baracchi

    See LICENSE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "main.h"
#include "result.h"
#include "anagram.h"

/* Global variables */
char* program_name = "anagrhash";
char* program_version = "0.1";
char* separators = NULL;
int verbose = FALSE;
int token_limit = 8;
/* end of global variables */

#define MAX_TOKEN_LENGTH 1024

struct command_line_arg {
	char name;
	int arguments;
	char *alias;
};
#define NUM_OPTS 9
struct command_line_arg args[] = {
	{'h', 1, "hash" },
	{'a', 1, "anagram" },
	{'s', 1, "sep" },
	{'l', 1, "limit" },
	{'i', 1, "input" },
	{'e', 1, "ex-input" },
	{'o', 1, "output" },
	{'v', 0, "verbose" },
	{'?', 0, "help" }	
};


void usage() {
	printf("%s v%s, an anagram generator/hash reverser\n", program_name, program_version);
	printf("Usage: %s [OPTIONS] [token1 [token2 ...]] { xtoken1 xtoken2 [...] }\n", program_name );
	printf("Available options:\n");
	printf("	-h hash		: Stops when hash is matched\n");
	printf("	-a word		: Use every letter of word as token (makes anagrams of word)\n");
	printf("	-s separators	: Use separators between tokens\n");
	printf("	-l n		: Use at most n tokens (default: %d)\n", token_limit);
	printf("	-i filename	: Load every line of file as a token\n");
	printf("	-e filename	: Load every line of file as a group of exclusive tokens\n");
	printf("	-o filename	: Writes anagrams to file\n");
	printf("	[any word]	: Use word as token\n");
	printf("	{ [any word] }	: Add the words specified between curly brackets to an exclusive-tokens group\n");
	printf("	-v		: Verbose mode.\n");
	printf("	-vv		: Also prints every generated key.\n");
	printf("	-?		: Prints this help.\n");
}

int parse_xtokens( int* index, int argc, char* argv[], GSList** xtokens ) {
	GSList* newlist = NULL;
	for( ; *index<argc; (*index) ++ ) {
		if( argv[*index][0] != '}' ) {
			newlist = g_slist_prepend( newlist, argv[*index] );
			/*
			GSList* newitem = g_slist_alloc();
			newitem->data = argv[*index];
			newitem->next = newlist;
			newlist = newitem;
			*/
		} else
			break;
	}
	if( newlist != NULL )
		*xtokens = g_slist_prepend( *xtokens, newlist );
	return SUCCESS;
}

int getopts( int argc, char *argv[], GSList** tokens, GSList** xtokens ) {
	int i;
	for( i=0; i<argc; i++ ) {
		if( argv[i][0] == '-' ) {
			int j;
			int found = FALSE;
			/* Let's see if it's a known option */
			for(j=0; j<NUM_OPTS; j++ ) {
				if( args[j].name == argv[i][1] ) {
					if( i + args[j].arguments >= argc ) {
						printf("Error: option '%c' missing required argument.\n", argv[i][1]);
						usage();
						return ERROR;
					} else
						found = TRUE;
				}
			}
			if( !found ) {
				printf("Error: unrecognized option %c. See %s -? for help.\n", argv[i][1], program_name );
				return ERROR;
			}

			switch( argv[i][1] ) {
				case 'h':
					i++;
					if( initialize_hash_method( argv[i] ) != SUCCESS )
						return ERROR;
					break;
				case 'a':
					i++;
					int length = strlen( argv[i] );
					int j;
					for( j=0; j<length; j++ ) {
						char *data = malloc(2);
						if( data == NULL )
							return ERROR;
						data[0] = argv[i][j];
						data[1] = '\0';
						*tokens = g_slist_prepend( *tokens, data );
					}
					break;
				case 's':
					i++;
					separators = argv[i];
					printf("Using separators: \"%s\"\n", separators);
					break;
				case 'l':
					i++;
					sscanf( argv[i], "%d", &token_limit );
					printf("Using limit: %d\n", token_limit);
					break;
				case 'i': {
					i++;
					int token_counter = 0;
					/* Load a file as list of tokens */
					FILE* tokfile = fopen( argv[i], "r" );
					if( tokfile == NULL ) {
						printf("File %s not found.\n", argv[i] );
						return ERROR;
					}
					char* buf = malloc( MAX_TOKEN_LENGTH );
					if( buf == NULL ) return ERROR;
					while( !feof( tokfile ) ) {
						char *ret = fgets( buf, MAX_TOKEN_LENGTH-1, tokfile );
						if( ret == NULL ) continue;
						int length = strlen( buf );
						/* strip the newline character */
						if( buf[length-1] == '\n' ) {
							buf[length-1] = '\0';
							length--;
						}
						/* allocate and store */
						char* data = malloc( length+1 );
						if( data == NULL ) return ERROR;
						strcpy( data, buf );
						*tokens = g_slist_prepend( *tokens, data );
						token_counter ++;
					}
					free( buf );
					printf("Loaded %d tokens from %s.\n", token_counter, argv[i] );
					break;
				}
				case 'e': {
					i++;
					int token_counter = 0;
					/* Load a file as list of tokens */
					FILE* tokfile = fopen( argv[i], "r" );
					if( tokfile == NULL ) {
						printf("File %s not found.\n", argv[i] );
						return ERROR;
					}
					char* buf = malloc( MAX_TOKEN_LENGTH );
					if( buf == NULL ) return ERROR;
					GSList* new_xtoken = NULL;
					while( !feof( tokfile ) ) {
						char *ret = fgets( buf, MAX_TOKEN_LENGTH-1, tokfile );
						if( ret == NULL ) continue;
						int length = strlen( buf );
						/* strip the newline character */
						if( buf[length-1] == '\n' ) {
							buf[length-1] = '\0';
							length--;
						}
						/* allocate and store */
						char* data = malloc( length+1 );
						if( data == NULL ) return ERROR;
						strcpy( data, buf );
						new_xtoken = g_slist_prepend( new_xtoken, data );
						token_counter ++;
					}
					*xtokens = g_slist_prepend( *xtokens, new_xtoken );
					free( buf );
					printf("Loaded %d exclusive tokens from %s.\n", token_counter, argv[i] );
					break;
				} 
				case 'o':
					i++;
					if( init_outfile( argv[i] ) != SUCCESS )
						return ERROR;
					break;
				case 'v':
					verbose = strlen(argv[i])-1;
					break;
				case '?':	
					usage();
					return ERROR;
					break;
				default:
					break;
			}
		} else if( argv[i][0] == '{' ) {
			// parse xtokens list
			i++;
			parse_xtokens( &i, argc, argv, xtokens );
			//printf("xtokens parsed: %s\n", print((*xtokens)->data) );
		} else {
			// populate tokens list
			*tokens = g_slist_prepend( *tokens, argv[i] );
		}
	}
	if( *tokens == NULL && *xtokens == NULL ) {
		printf("No tokens specified.\n");
		usage();
		return ERROR;
	}

	return SUCCESS;
}

int main( int argc, char *argv[] ) {
	GSList *tokens = NULL, *xtokens = NULL;
	/* Load options */
	if( argc < 2 ) {
		usage();
		return ERROR;
	}
	if( getopts( argc-1, &(argv[1]), &tokens, &xtokens ) != SUCCESS )
		return ERROR;		
	
	/* Start searching */
	int result = search( tokens, xtokens );

	/* Deinit... */
	deinit_hash();
	/* free() tokens and xtokens */
	g_slist_free( tokens );
	if( xtokens != NULL ) {
		GSList *p = xtokens, *nextp;
		do {
			nextp = p->next;
			g_slist_free_1( p );
		} while( (p = nextp) != NULL );
	}

	/* Return status and quit */
	if( result != SUCCESS ) {
		if( verbose )
			printf("Nothing found.\n");
		exit(result);
	} else
		exit(0);
}
