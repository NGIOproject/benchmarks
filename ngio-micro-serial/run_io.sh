#!/bin/bash

size=1000000
nvme_location=/media/nvme1

 ./micro -b io -s ${size} -o mk_rm_dir
 ./micro -b io -s ${size} -o file_write
 ./micro -b io -s ${size} -o file_pmem_write -l ${nvme_location}
 ./micro -b io -s ${size} -o file_read
 ./micro -b io -s ${size} -o file_pmem_read -l ${nvme_location}
 ./micro -b io -s ${size} -o file_write_random
 ./micro -b io -s ${size} -o file_pmem_write_random -l ${nvme_location}
 ./micro -b io -s ${size} -o file_read_random
 ./micro -b io -s ${size} -o file_pmem_read_random -l ${nvme_location}
 ./micro -b io -s ${size} -o file_read_direct
 ./micro -b io -s ${size} -o file_read_random_direct
