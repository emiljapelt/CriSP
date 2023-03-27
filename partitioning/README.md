# Partitioning

Benchmarking two different partitioning algorithms for course Computer Systems Performance, spring 2023.

## Automatic data collection

Run `benchmark.sh`. This will compile the project, run the node app to measure metrics with `perf`, run the timing benchmarks from `partitioning.c`, and write the data to csv-files in the folder `/benchmark_data/<DATE_AND_TIME_OF_RUN>`

`index.ts` in `automated_perf` requires `tsc` to transpile. `benchmark.sh` does not transpile, so this must be done manually.

## Generating charts

To generate charts from the data for a partitioning method, run `python3 generateMetricCharts.py ./benchmark_data/<DATE_AND_TIME_OF_RUN>/<METHOD_NAME>`. Charts will then be put in the `charts` folder in the given directory. This means that to generate charts for both methods, run the command twice, once for each method.

To generate charts comparing the two methods, run `python3 generateComparativeCharts.py ./benchmark_data/<DATE_AND_TIME_OF_RUN>`. This will generate charts comparing the two methods at 32 threads, and put them in the `comparative_charts` folder in the given directory.

If you run either of the chart generators with data that does not go up to 32 threads, they will break, and you will have to change the code.
