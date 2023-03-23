./compile.sh
now=$(date +'%d-%m-%Y_%H-%M')
cd automated_perf
node index.js $now
cd ..
./partitioning.exe $now