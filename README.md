# anagrhash

Anagrhash is a tool which can generate anagrams, and test them against a hash.

It is a tool I wrote many years ago for educative purposes, to try out some different concurrency models and inter-thread communication methods in C, and get some basics of programming in the GNU/Linux environment.

It serves as an anagram generator, and can shuffle letters, but can also be set to shuffle whole words in a sentence, use tokens from a list (words which you don't want to appear simultaneously), and specify separators between tokens (like spaces or commas).

It can also test anagrams which satisfies an exact target hash, or a hash specified with a regular expression.

The home page for this project is http://simonebaracchi.eu/posts/anagrhash

### Building

\# sudo apt-get install libmhash-dev libglib2.0-dev

\# make

The repo contains a binary which might work for you, if you are on an x86 Debian or similar.

### Using

<pre>
anagrhash v0.2, an anagram generator/hash reverser
Usage: anagrhash [OPTIONS] [token1 [token2 ...]] { xtoken1 xtoken2 [...] }
Available options:
        -h hash         : Stops when hash is matched
        -r regex        : Stops when hash matches regex
        -a word         : Use every letter of word as token (makes anagrams of word)
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

