# Copyright (c) 2017 The University of Edinburgh.
# 
# This software was developed as part of the                       
# EC H2020 funded project NEXTGenIO (Project ID: 671951)                 
#   www.nextgenio.eu           
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CC = gcc
CFLAGS += -O3
LDFLAGS += -L/usr/local/lib64 -lvmem -lpmem -lm -lpthread -lrt
DMACROS += 

OS := $(shell uname -s)

ifeq ($(OS),Linux)
DMACROS += -D__linux__
endif

DMACROS += -DPMEM
