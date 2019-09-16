# Vectorization Meets Compilation
This repository contains source code of initital attempts to combine two traditional query processing engines: vectorizing and compilation-based engines.

## How To Build
A configuration file is provided to build this project with CMake. 
To create a release build, in the project directory run:
```
mkdir -p build/release
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make
```

To create a debug build, in the project directory run:
```
mkdir -p build/release
cd build/release
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make
```

This creates among others the main binary run\_hybrid.

## Running Experiments
To run the experiments, you need to generate TPC-H tables using TPC-H dbgen tool. Once the benchmark dataset is created, use the following command to run the experiments:
./build/release/run_hybrid nrRepetitions tpchPath nrThreads verbose

## How To Extend Code
Check the corresponding Wiki page for more information about the code structure, and possible entry points.

