Copyright (c) 2017 The University of Edinburgh.
 
This software was developed as part of the  

EC H2020 funded project NEXTGenIO (Project ID: 671951)
    http://www.nextgenio.eu

It builds on benchmarks originally developed as part of the 
                     
EC FP7 funded project Adept (Project ID: 610490)                 
    http://www.adept-project.eu                                            

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

These benchmarks were created to test hardware performance associated with non-volatile memory, including pmem.io functionality to write 
data to and from NVMe enabled storage devices.  The majority of the code in this package comes from the Adept kernel benchmarks, which we have 
extended for non-volatile memory.

# NEXTGenIO Kernel Benchmarks - Serial

We have extended the fileparse benchmark to include a pmem enabled fileparse that opens a file on an NVMe device and does all the fileparse operations to 
that file.  You need to specify the location of the NVMe filesystem, or where you want the file to be created, when running the benchmark.


# Adept Kernel Benchmarks - Serial

The serial kernel benchmarks we built on from the Adept project came from https://github.com/EPCCed/adept-kernel-openmp. They are implemented in C.

## Citation & Further Information
If you would like to cite this work, please cite:
Nick Johnson et al., "Adept Deliverable D2.3 - Updated Report on Adept Benchmarks", September 2015.
available at http://www.adept-project.eu/images/Deliverables/Adept%20D2.3.pdf


## BLAS-type benchmarks

In our BLAS-type benchmarks we implement a few of the most common linear algebra computations.

#### AXPY
This benchmark takes two vectors `x` and `y`, and the scalar `a`, and computes:
``` 
  y = a * x + y
```
The user can choose the length (number of elements) of the vectors, as well as their data type (int, float or double).

#### Dot product 
The dot product benchmark multiplies two vectors x and y of length n and returns a scalar:
```
  result = x_0 y0 + x_1 y_1 + ... x_n y_n
```
The user can choose the length (number of elements) of the vectors, as well as their data type (int, float or double).

#### Scalar multiplication
Thise benchmark scales the vector x by a fixed scalar a:
```
  x = a * x
```
The user can choose the length (number of elements) of the vectors, as well as their data type (int, float or double).

#### Euclidean norm
This benchmarks computes for Euclidean norm of vector x:
```
  || x || = sqrt ( |x_1|^2 + |x_2|^2 + ... |x_n|^2 )
```
The user can choose the length (number of elements) of the vectors, as well as their data type (int, float or double).
  
#### Dense matrix-vector multiplication
This benchmarks multiplies a square dense matrix A with a vector x to compute vector y:
```
y = A * x
```
Both A and x are randomly generated. The user can choose the size of the data structures (where size*size equals the number of elements in the matrix), as well as their data type (int, float or double).

#### Sparse matrix-vector multiplication
This benchmarks multiplies a square sparse matrix A with a vector x to compute vector y:
```
y = A * x
```
A is represented in CSR format and read from an input file. The vector x is randomly generated. The size of the matrix is fixed by the input file (which the user can substitute for a different matrix). The user can choose the data type to be used (float or double).

#### Sparse matrix-matrix multiplication
This benchmarks multiplies two square sparse matrices A and B to compute matrix C:
```
C = A * B
```
A and B are both represented in CSR format and read from an input file. The size of the matrices is fixed by the input file (which the user can substitute for a different matrix). The user can choose the data type to be used (float or double).
  
## Stencil computation

The stencil benchmarks compute values for each element in a 2D or 3D grid based on the values of their nearest neighbours.
 
#### 2D grid: 5-point and 9-point Stencil
On a 2D grid, the 5-point stencil computes the value of A[i][j] by taking the values from left, right, up and down from the current position, and scale them with a constant. The 9-point stencil is similar, but also includes the diagonals.
The user can choose the data type to be used in the grid (int, float or double).
 
#### 3D grid: 19-point and 27-point Stencil 
The 19-point and 27-point stencils are analogous to the 5 and 9 point stencil, but they operate in a 3D space. 
The user can choose the data type to be used in the grid (int, float or double).

## File parsing
The file parsing benchmark creates a file filled with sequences of random characters, as well as a fixed search phrase (here: "AdeptProject"). The benchmark then searches through the file and counts the occurences of the search phrase. 
The user can determine the size of the file by passing the number of lines to be created (using size).
 
## Conjugate Gradient solver
 This benchmark implements a simple CG solver, with a random matrix A of (user defined) size s. The CG computation includes BLAS computations (AXPY, AYPX and dot product) which are part of the slover loop. Only the solver loop is measured, the setup time is discarded.
