/* Copyright (c) 2015 The University of Edinburgh. */

/* 
* This software was developed as part of the                       
* EC FP7 funded project Adept (Project ID: 610490)                 
* www.adept-project.eu                                            
*/

/* Licensed under the Apache License, Version 2.0 (the "License"); */
/* you may not use this file except in compliance with the License. */
/* You may obtain a copy of the License at */

/*     http://www.apache.org/licenses/LICENSE-2.0 */

/* Unless required by applicable law or agreed to in writing, software */
/* distributed under the License is distributed on an "AS IS" BASIS, */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/* See the License for the specific language governing permissions and */
/* limitations under the License. */

/* ************************************************************* */
/* Linux kernel module to disable/enable caches on Intel x86_64  */
/* ************************************************************* */
/*                                                               */
/* The Intel® 64 and IA-32 Architectures Software Developer's    */
/* Manual Volume 3A: System Programming Guide, Part 1 says:      */
/*                                                               */
/* To disable the L1, L2, and L3 caches after they have been     */
/* enabled and have received cache fills, perform the            */
/* following steps:                                              */
/*                                                               */
/* 1. Enter the no-fill cache mode. (Set the CD flag in control  */
/*    register CR0 to 1 and the NW flag to 0.                    */
/* 2. Flush all caches using the WBINVD instruction.             */
/* 3. Disable the MTRRs and set the default memory type to       */
/*    uncached or set all MTRRs for the uncached memory type.    */
/*                                                               */
/* The caches must be flushed (step 2) after the CD flag is set  */
/* to insure system memory coherency. If the caches are not      */
/* flushed, cache hits on reads will still occur and data will   */
/* be read from valid cache lines.                               */
/*                                                               */
/* The intent of the three separate steps listed above address   */
/* three distinct requirements:                                  */
/*                                                               */
/* (i)   discontinue new data replacing existing data            */
/*       in the cache,                                           */
/* (ii)  ensure data already in the cache are evicted            */
/*       to memory,                                              */
/* (iii) ensure subse- quent memory references                   */
/*       observe UC memory type semantics. Different             */
/*       processor implementation of caching control             */
/*       hardware may allow some variation of                    */
/*       software implementation of these three                  */
/*       requirements.                                           */
/*                                                               */
/*                                                               */
/* Use the module by doing:                                      */
/*                                                               */
/* make                                                          */
/* insmod ./disableCache.ko -- insert module and disable cache   */
/* lsmod                    -- to check the module is running    */
/* rmmod disableCache       -- remove moduel and enable cache    */                 
/*                                                               */
/* ************************************************************* */

#include <linux/init.h>
#include <linux/module.h>

/* code below sets CD flag (bit 30), unsets the NW flag (bit 29) */
/* and then flushes the caches using WBINVD.                     */
/* Then disable the MTRRs by unsetting flag E (bit 11) and set   */
/* the default memory type to uncached by unsetting bits 0-2     */

static int disableCache_init(void){
  printk(KERN_ALERT "Disabling CPU caches.\n");
  __asm__ __volatile__("push   %rax\n\t"
	  "movq    %cr0,%rax;\n\t"
	  "or     $(1 << 30),%rax;\n\t"          
	  "and     $~(1 << 29),%rax;\n\t"
	  "movq    %rax,%cr0;\n\t"
	  "wbinvd\n\t"
	  "movq $0x000002FF, %rcx;\n\t"
	  "rdmsr\n\t"
          "and    $~(1 << 11),%rax;\n\t"         // Unset bit 11
          "and    $0xFFFFFFFFFFFFFFF8,%rax;\n\t" // Unset bottom 3 bits 
	  "wrmsr\n\t"
	  "pop    %rax"
	  );
  return 0;
}

/* code below unsets CD flag (bit 30), sets the NW flag (bit 29) */
/* and then flushes the caches using WBINVD.                     */
/* Then enable the MTRRs by setting flag E (bit 11) and set      */
/* the default memory type to cached by setting bits 0-2         */

static void disableCache_exit(void){
  printk(KERN_ALERT "Enabling CPU caches.\n");
  __asm__ __volatile__("push   %rax\n\t"
	  "movq    %cr0,%rax;\n\t"
	  "and    $~(1 << 30),%rax;\n\t"
	  "or     $(1 << 29),%rax;\n\t"
	  "movq    %rax,%cr0;\n\t"
	  "wbinvd\n\t"
          "movq $0x000002FF, %rcx;\n\t"
          "rdmsr\n\t"
	  "or    $(1 << 11),%rax;\n\t"           // Set bit 11
	  "or    $~0xFFFFFFFFFFFFFFF8,%rax;\n\t" // Set bottom 3 bits 
	  "wrmsr\n\t"
	  "pop    %rax"
	  );
}

module_init(disableCache_init);
module_exit(disableCache_exit);
