# Partitioning

Tools used to benchmark the performance of partitioning methods.

## Automatic data collection

Run `benchmark.sh`. This will compile the project, run the node app to measure metrics with `perf`, run the timing benchmarks from `partitioning.c`, and write the data to csv-files in the folder `../benchmark_data/<DATE_AND_TIME_OF_RUN>`

## Generating charts

Install dependencies with `pip install matplotlib, pandas`

To generate charts from the data for a partitioning method, run `python3 generateMetricCharts.py ../benchmark_data/<DATE_AND_TIME_OF_RUN>/<METHOD_NAME>`. Charts will then be put in the `charts` folder in the given directory. This means that to generate charts for both methods, run the command twice, once for each method.

To generate charts comparing the two methods, run `python3 generateComparativeCharts.py ../benchmark_data/<DATE_AND_TIME_OF_RUN>`. This will generate charts comparing the two methods at 32 threads, and put them in the `comparative_charts` folder in the given directory.

If you run either of the chart generators with data that does not go up to 32 threads, they will break, and you will have to change the code.
