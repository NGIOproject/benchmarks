# $1 = number of 1MB chunks
# $2 = number of files
# $3 = path for files
./producer $1 $2 $3
sleep 1
sudo sync
sleep 1
echo 3 | sudo tee /proc/sys/vm/drop_caches
sleep 1
./consumer $1 $2 $3
