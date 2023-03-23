./compile.sh
now=$(date +'%d-%m-%Y_%H-%M')
node perf-run.js $now
./partitioning.exe $now