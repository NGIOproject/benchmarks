#!/bin/bash

size=1000000


 ./micro -b memory -s ${size} -o calloc
 ./micro -b memory -s ${size} -o pvmem_calloc -l /media/nvme1
 ./micro -b memory -s ${size} -o read_ram
 ./micro -b memory -s ${size} -o read_vram -l /media/nvme1
 ./micro -b memory -s ${size} -o read_lat
 ./micro -b memory -s ${size} -o vmem_read_lat -l /media/nvme1
 ./micro -b memory -s ${size} -o write_contig
 ./micro -b memory -s ${size} -o write_vmem_contig -l /media/nvme1
 ./micro -b memory -s ${size} -o write_strided
 ./micro -b memory -s ${size} -o write_random
