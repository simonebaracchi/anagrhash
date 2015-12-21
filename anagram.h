/*
    anagrhash v0.1 - an anagram generator/hash reverser
    Copyright (C) 2008 Simone Baracchi

    See LICENSE.
*/

#ifndef __ANAGRAM_H__
#define __ANAGRAM_H__

extern char* separators;
extern int token_limit;
extern int max_key_length;

int search( GSList* tokens, GSList* xtokens );
char* print( GSList *list );

#endif
