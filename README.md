# anagrhash

An anagram generator / reverse-hash resolver. 

I wrote this mostly for educative purposes. 
Uses C and is written for Linux systems.

### Building

\# sudo apt-get install libmhash-dev libglib2.0-dev

\# make

### Using

<pre>
anagrhash v0.1, an anagram generator/hash reverser
Usage: anagrhash [OPTIONS] [token1 [token2 ...]] { xtoken1 xtoken2 [...] }
Available options:
        -h hash         : Stops when hash is matched
        -a word         : Use every letter of word as token (makes anagrams of word)
        -r regex        : Stops when hash matches regex
        -s separators   : Use separators between tokens
        -l n            : Use at most n tokens (default: 8)
        -i filename     : Load every line of file as a token
        -e filename     : Load every line of file as a group of exclusive tokens
        -o filename     : Writes anagrams to file
        [any word]      : Use word as token
        { [any word] }  : Add the words specified between curly brackets to an exclusive-tokens group
        -n              : Terminates every token with a newline before generating a hash.
        -v              : Verbose mode.
        -vv             : Also prints every generated key.
        -?              : Prints this help.
</pre>

### Examples

By default anagrams are not printed to screen. Use either -vv or -o to print to file.

<pre>
# anagrhash -a hello
-> generates all anagrams of "hello"

# anagrhash hello world 
-> generates all combinations of the tokens "hello" and "world"
e.g. hello
     world
     helloworld
     worldhello

# anagrhash -s , hello world
-> generates all combinations of the tokens "hello" and "world", separating the tokens with ","
e.g. hello
     world
     hello,world
     world,hello

# anagrhash hello { world earth }
-> Uses only one of "world" or "earth"
e.g. hello
     earth
     world
     earthhello
     worldhello
     helloearth
     helloworld

# anagrhash -l 2 hello world earth
-> Uses at most 2 out of "hello", "world", "earth"
e.g. earth
     world
     hello
     worldearth
     helloearth
     earthworld
     helloworld
     earthhello
     worldhello

# anagrhash -l 2 hello world earth -h fc5e038d38a57032085441e7fe7010b0
-> String 'helloworld' matches hash fc5e038d38a57032085441e7fe7010b0, success!

# anagrhash -a abcdefghijklmnopqrstuvwxyz -n -r 'baddcafe'
-> Tries to find a combination of "[a-z]{8}\n" which hashes to "baddcafe"

</pre>

