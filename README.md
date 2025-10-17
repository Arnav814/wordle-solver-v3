Wordle Solver v3
================

This is a simple wordle solver. It's the third version I've written (v2 and v1
aren't published anywhere).

Usage
-----

Run the `wordlebot3` binary. Flags can be viewed with `wordlebot3 --help`.

This program follows the xdg base dir spec for cache dirs.

Paths to search for wordlists can be set using the WORDLIST\_PATH environment
variable. Multiple paths can be specified if they're seperated with `:`.
Directories will be recursively searched in reverse order (last entry to first
entry). If a path on the command line start with `./`, it will always be treated
as a plain path and this logic doesn't apply.

Building
--------

Install libncurses (`libncurses-dev` on Debian). Then run these commands.

```bash
mkdir build
cd build
cmake -S .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j $(nproc)
```

Bugs
----

Initial creation of the cache fails, but re-running the program works.

Invalid wordlists aren't handled very well.

The tests don't really test much.

