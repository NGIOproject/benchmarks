#!/bin/bash

size=1000000
nvme_location=/media/nvme1

 ./micro -b memory -s ${size} -o calloc
 ./micro -b memory -s ${size} -o pvmem_calloc -l ${nvme_location}
 ./micro -b memory -s ${size} -o read_ram
 ./micro -b memory -s ${size} -o read_vram -l ${nvme_location}
 ./micro -b memory -s ${size} -o read_lat
 ./micro -b memory -s ${size} -o vmem_read_lat -l ${nvme_location}
 ./micro -b memory -s ${size} -o write_contig
 ./micro -b memory -s ${size} -o write_vmem_contig -l ${nvme_location}
 ./micro -b memory -s ${size} -o write_strided
 ./micro -b memory -s ${size} -o write_vmem_strided -l ${nvme_location}
 ./micro -b memory -s ${size} -o write_random
 ./micro -b memory -s ${size} -o write_vmem_random -l ${nvme_location}
