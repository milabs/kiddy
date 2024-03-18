#pragma once

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>

// compat-layer

#ifndef GLOBAL_ROOT_UID
# define GLOBAL_ROOT_UID (0)
# define uid_eq(a, b) ((a) == (b))
#endif

// kernels >= 3.4
extern void *vm_mmap_proto(void *, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
extern long vm_munmap_proto(void *, long);

// kernels <= 3.3
extern void *sys_mmap_pgoff_proto(unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long);
extern int sys_munmap(void *, unsigned long);
