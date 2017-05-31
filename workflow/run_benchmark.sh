# $1 = number of 1MB chunks
# $2 = number of files
./producer $1 $2
sleep 1
time sudo sync
sleep 1
time echo 3 | sudo tee /proc/sys/vm/drop_caches
sleep 1
./consumer $1 $2
