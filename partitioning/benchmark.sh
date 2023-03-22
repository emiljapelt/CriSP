./compile.sh
now=$(date +'%d-%m-%Y_%H-%M')
cd automated_perf
npm start $now
cd ..
./partitioning.exe $now