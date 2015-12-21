/*
    anagrhash v0.1 - an anagram generator/hash reverser
    Copyright (C) 2008 Simone Baracchi

    See LICENSE.
*/

#ifndef __RESULT_H__
#define __RESULT_H__

int init_outfile( char *output_file );
int handle_result( char *string );
int initialize_hash_method( char *hash );
int initialize_regex( char *regex );
void deinit_hash();

#endif
