./compile.sh
now=$(date +'%d-%m-%Y_%H-%M')
node run_perf_benchmarks.js $now
./partitioning.exe $now