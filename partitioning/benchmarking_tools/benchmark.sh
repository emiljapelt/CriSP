cd ../utilities
./compile.sh
cd ../benchmarking_tools
now=$(date +'%d-%m-%Y_%H-%M')
node run_perf_benchmarks.js $now
./partitioning.exe $now