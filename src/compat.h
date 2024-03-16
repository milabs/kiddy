#pragma once

#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/sched.h>

// compat-layer

#ifndef GLOBAL_ROOT_UID
# define GLOBAL_ROOT_UID (0)
# define uid_eq(a, b) ((a) == (b))
#endif
