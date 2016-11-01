# HDAStar
The solver for sliding tile problem(15-puzzle) using HDA*  algorithm([Kishimoto, Fukunaga and Botea 2009](http://metahack.org/Kishimoto-Fukunaga-Botea-icaps2009.pdf)).

## Requirement
```
g++
OpenMPI >= 2.0
boost >= 15.5
```

## Build
```
$ git clone https://github.com/Kurorororo/hdastar
$ cd hdastar
```
### HDAStar solver
```
$ make hdastar
$ cat data/korf79.dat |  mpirun -n <nodesize> ./hdastar
```

### AStar solver
```
$ make hdastar
$ cat data/korf79.dat | ./astar
```

## Data Format
The Solver recieves an input from stdin.   
There is some example input files in `data/` directory. These inputs are from [Korf 1985](https://cse.sc.edu/~mgv/csce580f11/gradPres/korf_IDAStar_1985.pdf).
```
example: korf79.dat

0
0
1
9
7
11
13
5
3
14
12
4
2
8
6
10
15
```
line1: the position of the blank(0-15).  
line2-16: tiles in each position(0-15). 0 represents the blank.
```
Korf 79
 0  1  9  7
11 13  5  3
14 12  4  2
 8  6 10 15
```
```
GOAL STATE
 0  1  2  3
 4  5  6  7
 8  9 10 11
12 13 14 15
```
