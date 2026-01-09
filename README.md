# AdventOfCode
My personal repo for the advent of code challenge

## How this repo is structured
There is a generic main source file `single_input_main.c` and header file `puzzle_solver.h` in the root directory.
It opens a file passed to the executable as first commandline argument.
If the file was opened successfully, it is passed as file pointer `FILE *` to a function `solve_puzzle(...)` which shall contain the algorithm to solve the puzzle for the specific day.

Each puzzle's solution is placed in the subdirectory named `YYYY/dayDD` where the file `dayDD.c` contains an implementation of the `solve_puzzle(...)` function for the specific day.
The executable is build by `cmake` by compiling and linking all source files in the specific `YYYY/dayDD` folder and the `single_input_main.c` of the root folder.

It is intended to have the puzzle input stored as `input.txt` in the same folder as the dedicated solving algorithm.
The `CMakeLists.txt` files of each folder create a `build` folder containing the same folder structure `YYYY/dayDD` for each puzzle with the executables and puzzle inputs in their specific folder.
The `CMakeLists.txt` are structured in a way, that for each day's puzzle, the part 1 and part 2 executables are built using the exact same executables and defining the preprocessor constant `BUILD_SOLUTION` with the value `1` or `2`.
The executables are named by the following scheme:
```
YYYY_dayDD_p(1|2)[.exe]
```

To run the solution of a specific day navigate to the directory containing the executable and call it with the input file, e.g. for year 2025 day 1 part 2:
```
cd <projectroot>
cd ./build/2025/day1
.\2025_day1_p2.exe input.txt        -- for windows
./2025_day1_p2 input.txt            -- for linux
```
