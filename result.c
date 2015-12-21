/*
    anagrhash v0.1 - an anagram generator/hash reverser
    Copyright (C) 2008 Simone Baracchi

    See LICENSE.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
//#include <semaphore.h>

#include <glib.h>
#include <mhash.h>

#include "result.h"
#include "main.h"
#include "anagram.h"
#include "list.h"

/* Comment/uncomment to enable/disable multithreading */
//#define SINGLE_THREAD_MODE
/* Comment/uncomment to enable/disable standard GLib primitives for
 * dealing with lists */
/*
#define g_slist_alloc list_new
#define g_slist_prepend list_prepend
#define g_slist_nth list_nth
#define g_slist_free list_free
#define g_slist_free_1 list_free_1

#ifdef g_slist_free1
#undef g_slist_free1
#define g_slist_free1 list_free_1
#endif
*/

/* Which hash method are we using? (References the "methods" array) */
int method_index;
/* Hash we are trying to match */
char *target_hash = NULL;
/* File we should write keys to */
FILE *f_output = NULL;
/* String that matches target_hash */
char *solution_found = NULL;
/* Should we terminate everything? */
int termination_flag = FALSE;

/* How many consumer threads? */
#define MAX_THREADS 2
/* Each thread has its own buffer of this number of candidates */
#define THREAD_BUF_SIZE 5000
/* Thread handles */
pthread_t tid[MAX_THREADS];
int thread_started = FALSE;

/* mutexes for "candidates" access */
pthread_mutex_t list_access;

GSList *candidates = NULL;
int candidates_count = 0;
/* We stop the generator thread if we already have more than MAX_CANDIDATES */
#define MAX_CANDIDATES MAX_THREADS * THREAD_BUF_SIZE * 40

void *hashing_thread( void* );

struct method { 
	/* Hash length, in bytes */
	int length; 
	char* name; 
	hashid id;
};

#define N_METHODS 5
struct method methods[] = {
	{ 32, "md5", MHASH_MD5 },
	{ 40, "sha1", MHASH_SHA1 },
	{ 56, "sha224", MHASH_SHA224 },
	{ 64, "sha256", MHASH_SHA256 },
	{ 128, "sha512", MHASH_SHA512 }
};

int init_outfile( char *output_file ) {
	f_output = fopen( output_file, "w" );
	if( f_output == NULL )
		return ERROR;
	else
		return SUCCESS;
}

int initialize_hash_method( char* hash ) {
	/* Store the target hash */
	target_hash = hash;
	/* Find a "plausible" hash algorithm for string "hash" */
	int i=0;
	int found=FALSE;
	int length = strlen( hash );
	for( i=0; i<N_METHODS; i++) {	
		if( methods[i].length == length ) {
			if( verbose )
				printf("Method \"%s\" matches hash length of %d bytes.\n", methods[i].name, length );
			found=TRUE;
			method_index = i;
			break;
		}
	}
	if( !found ) {
		printf("No hash method found that matches the target hash.\n");
		return ERROR;
	}
	// TODO initialize the hashing algorithm?
	// initialize the thread server
	pthread_mutex_init( &list_access, NULL );
#ifndef SINGLE_THREAD_MODE
	g_thread_init(NULL);
	for( i=0; i<MAX_THREADS; i++ ) {
		int result = pthread_create( &tid[i], NULL, hashing_thread, NULL ); 
		if( result != 0 ) {
			printf("Error: cannot create thread!\n");
			return ERROR;
		}
	}
	thread_started = TRUE;
#endif
	return SUCCESS;
}

void deinit_hash() {
	termination_flag = TRUE;
#ifndef SINGLE_THREAD_MODE
	int i;
	if( thread_started == TRUE ) {
		for( i=0; i<MAX_THREADS; i++ ) {
			pthread_join( tid[i], NULL );
		}
	}
#endif
}

int perform_hash( char* string ) {
	unsigned char *hash;
	MHASH handle = mhash_init( methods[method_index].id );
	if( handle == MHASH_FAILED ) {
		printf("Hashing error.\n");
		exit(FATAL);
	}
	mhash(handle, string, strlen(string) );
	hash = mhash_end(handle);

	// TODO this is inefficient
	char buffer[512];
	buffer[0] = '\0';
	int i = 0;
	for( i=0; i < mhash_get_block_size(methods[method_index].id); i++) {
                sprintf(buffer, "%s%.2x", buffer, hash[i]);
        }
	free( hash );


	/* check if calculated hash is the one we're looking for */
	int result;
	if( strcmp( buffer, target_hash ) == 0 ) {
		printf("String '%s' matches hash %s, success!\n", string, target_hash );
		// I'm setting a global variable within a thread without mutexes -- but
		// this should happen just once
		solution_found = malloc(max_key_length);
		strcpy( solution_found, buffer );
		result = SUCCESS;
	} else {
		result = ERROR;
	}
	return result;
}

void *hashing_thread( void* arg ) {
	while( solution_found == NULL && (termination_flag==FALSE || (candidates_count>0&&candidates!=NULL)) ) {
		GSList *buffer = NULL;
		int buf_size = 0;
		/* ENTERING CRITICAL SECTION */
		pthread_mutex_lock( &list_access );
		if( candidates != NULL ) {
			// How many items are we going to process?
			buf_size = ( THREAD_BUF_SIZE < candidates_count ? THREAD_BUF_SIZE : candidates_count );
			// Splitting candidates at buf_size
			buffer = candidates;
			// TODO this is inefficient, needs some redesign
			GSList *last_element = g_slist_nth( candidates, buf_size-1 );
			candidates = last_element->next;
			last_element->next = NULL;
			candidates_count -= buf_size;
		}
		pthread_mutex_unlock( &list_access );
		/* END OF CRITICAL SECTION */
		
		if( buffer != NULL ) {
			GSList *p = buffer, *nextp;
			do{
				nextp = p->next;
				perform_hash( p->data );
				free( p->data );
				g_slist_free_1( p );
			}while( (p = nextp) != NULL );
		} else
			// queue is empty, wait a little
			sched_yield();
	}
	pthread_exit( NULL );
}


/* 
 * Gets every phrase generated by our anagram algorithm
 * Returns SUCCESS if a matching key has been found in the past, 
 * BUT it could also not be "string".
 * "string" is dynamically allocated and should be freed inside here.
 */
int handle_result( char* string ) {
	/* Print keys to screen, if verbose enough */
	if( verbose >= 2 )
		printf(	"%s\n", string );
	/* Write keys to file, if desired */
	if( f_output != NULL ) {
		int op_result = fprintf( f_output, "%s\n", string );
		if( op_result < 0 ) {
			printf("Error writing to file.\n");
			exit(FATAL);
		}
	}

	/* Don't bother to hash it if we don't know what to look for */
	if( target_hash == NULL ) {
		free(string);
		return ERROR;
	}

#ifdef SINGLE_THREAD_MODE
	/* Single thread mode */
	int result = perform_hash( string );
	free(string);
	return result;
#else
	/* Multi thread mode */
	while( candidates_count > MAX_CANDIDATES-1 ) sched_yield();
	// populate candidates list
	pthread_mutex_lock( &list_access );
	// We don't free() string - we let threads handle it
	candidates = g_slist_prepend( candidates, string );
	candidates_count ++;
	pthread_mutex_unlock( &list_access );
	if( solution_found != NULL )
		return SUCCESS;
	return ERROR;
#endif

}

/* Fake main - used for test */
/*
int verbose = 0;
int max_key_length = 1024;
int main() {
	// Init with a fake md5 hash - will never be matched
	initialize_hash_method( "00000000000000000000000000000000" );

	unsigned long i;
	for( i=0; i < 10000000; i++ ) {
		char *buf = malloc(max_key_length);
		if( buf == NULL ) {
			fprintf(stderr, "Error - cannot malloc().\n");
			return 1;
		}
		sprintf(buf, "%ld", i);
		handle_result(buf);
		if( i % 250000 == 0 )
			printf("%ld...\n", i);
	}

	deinit_hash();

	return 0;
}
*/
