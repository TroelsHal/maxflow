# MAX FLOW - 3 Algorithms
This program is an implementation of three max flow algorithm:

+ **Edmonds-Karp** implemented with breadth-first search.

+ **Dinics Algorithm**

+ Goldberg and Tarjan's **Push-Relabel** algorithm implemented with relabel-to-front optimisation and gap heuristic.

## Requirements
To build and run this program, you'll need the following:

+ A C++17 compliant compiler.
+ CMake version 3.14 or higher.
+ GoogleTest (only if running the tests), which can be obtained [here](https://github.com/google/googletest).

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
```


## Usage
The `maxflow` program reads a DIMACS max flow problem and solves for the max flow value.

The user must choose between one of the following algorithms:<br>
"ek" - Edmonds-Karp algorithm<br>
"pr" - Push-Relabel algorithm<br>
"dinic" - Dinic's algorithm

The user must also provide a path to a max flow problem in DIMACS format.

From the `build` folder navigate to the `build/src` folder and run the program.<br>
Example:<br>
```bash
cd src
./maxflow pr graphExample.txt
```

For help:<br>
```bash
./maxflow --help
```

## Testing
The program is tested with GoogleTest.<br>
To run the test, navigate from the `build` folder to the `build/test` folder and run `ctest`:<br>
```bash
cd test
ctest
```

## Author

Code written by Troels Halgreen

