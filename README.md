# CellularAutomaton2d
My 2d Cellular Automaton implementation using C and Raylib

## Requirements
raylib (https://www.raylib.com/index.html)

gcc compiler on linux

## Compiling

### Linux
``` bash
$ chmod +x build.sh
$ ./build.sh
$ ./run
```

### Windows
I dont know even if it works there, so good luck 😜

## Pre-configuring starting positions
sadly, at the moment there is no way of using user friendly editing start positions, so there are two ways of configuring start positions
1. edit `clibs/startpos.c` file
2. edit int `pos[][]` variable in `main.c`
default start pos defined in `main.c` in the middle of field

## Usage Examples
### Running app
``` bash
$ ./run -fps 30 -square 450 -scale 2 -rules b13/s24/9
$ ./run -square 401 -scale 2 -rules b12345/s34568/2 -fps 10
```
### Controls
point with mouse pointer at place to zoom into, and with *W, S* keys zoom in and/or out

---
### Additional info
developed using Gentoo linux, GNU nano text editor and GCC compiler
