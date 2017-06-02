Copyright (c) 2017 The University of Edinburgh.
 
This software was developed as part of the  

EC H2020 funded project NEXTGenIO (Project ID: 671951)
    http://www.nextgenio.eu

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


# Introduction

This respository contains a number of benchmarks we have created to evaluate the performance of non-volatile memory, NVMe hardware, pmem.io, and other aspects of the hardware we will use to build the NEXTGenIO prototype.

We are interested in evaluating both hardware performance, and software overheads, and therefore have benchmarks that are purely hardware focussed, and benchmarks that investigate the software performance aspects of I/O and memory movements.

# NEXTGenIO Kernel Benchmarks - Serial

The ngio-kernel-benchmarks directory contains level1 kernel benchmarks (build on ones originally developed for the Adept project (https://github.com/EPCCed/adept-kernel-openmp)).  Of particular interest for I/O is the file parse benchmark, which does file operations either using linux posix functionality or the pmem.io library.


# NEXTGenIO Micro Benchmarks - Serial

The ngio-micro-benchmarks directory contains level0 micro benchmarks (build on
ones originally developed for the Adept project
(https://github.com/EPCCed/adept-micro-openmp)).  Of particular interest for
I/O are the file benchmarks, which evaluate the costs of file open and
writing/reading in various different modes, using linux posix functionality or
the pmem.io library.

The memory benchmarks have also been extended to use the vmem functionality
from pmem.io to use memory on a non-volatile device rather than standard main memory.

