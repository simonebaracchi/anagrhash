/*
    anagrhash v0.1 - an anagram generator/hash reverser
    Copyright (C) 2008 Simone Baracchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __RESULT_H__
#define __RESULT_H__

int init_outfile( char *output_file );
int handle_result( char *string );
int initialize_hash_method( char *hash );
void deinit_hash();

#endif
