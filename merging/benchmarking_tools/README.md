# Benchmarking tools

## Compiling

Run `./bench_compile.sh`. This will put executables for all compilers in /executables/out

## Running benchmarks

To run a set of benchmarks run `node benchmark.js`. This will compile run all combinations multiple times, and output data to csv-files in benchmarking_data in a folder named as the current time. Two folders will be made in that folder, one for averages and one for standard deviations.

In the file max_depth, data_size, repetitions, and arity can be set.

To run a single executable, run the one of choice in /executables/out like so `merge-<COMPILER>-bench.exe <DEPTH> <ARITY> <DATASIZE>`.

## Generating charts

Install dependencies with `pip3 install matplotlib pandas`.

Run `python3 /path/to/data` for example `python3 ../benchmark_data/5-5-2023_18-18_arity-2_laptop`. This will generate charts for the averages, and put them in the `/averages/charts`-folder.

To generate charts comparing the gcc-default-results of two folders, run `python3 generateComparativeCharts.py /path/to/data1 /path/to/data2` for example `python3 generateComparativeCharts.py ../benchmark_data/5-5-2023_18-18_arity-2_laptop ../benchmark_data/5-5-2023_22-33_arity-2_server`. This will put the charts in `comparative_charts`-folder of the directory given as the first argument.

## Generate object dumps

Run `node create_obj_dumps.js /path/to/executables` for example `node create_obj_dumps.js ../executables/out_laptop`. This will put the object dumps in an obj-folder in the given directory.
