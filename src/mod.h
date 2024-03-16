#pragma once

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/cred.h>

#include "compat.h"

#include <khook/engine.h>

#ifndef KIDDY_UTS_VERSION
# ifdef CONFIG_SMP
#  define KIDDY_UTS_VERSION "#1-kiddy SMP"
# else
#  define KIDDY_UTS_VERSION "#1-kiddy"
# endif
#endif

#ifndef KIDDY_UTS_RELEASE
# define KIDDY_UTS_RELEASE __stringify(LINUX_VERSION_A) "." __stringify(LINUX_VERSION_B) "-generic"
#endif

#ifndef KIDDY_PROC_CMDLINE
# define KIDDY_PROC_CMDLINE "kiddy quiet ro"
#endif

#ifndef KIDDY_PROC_VERSION
# define KIDDY_PROC_VERSION KIDDY_UTS_RELEASE " " KIDDY_UTS_VERSION
#endif

#ifndef KIDDY_LINUX_VERSION_CODE
# define KIDDY_LINUX_VERSION_CODE KERNEL_VERSION(LINUX_VERSION_A, LINUX_VERSION_B, 0)
#endif

#ifndef KIDDY_RESTRICT_INODES
# define KIDDY_RESTRICT_INODES "/boot:/lib/modules:/proc/modules:/proc/kallsyms:/proc/config.gz:/var/log"
#endif

static inline int kiddy_do_filter(void) {
	return !uid_eq(current_cred()->uid, GLOBAL_ROOT_UID);
}

static inline int kiddy_do_filter_tty(void) {
	const void *tty = current->signal->tty; // no need to lock
	return !uid_eq(current_cred()->uid, GLOBAL_ROOT_UID) || !tty;
}

long kiddy_init_fs(void);
void kiddy_cleanup_fs(void);

long kiddy_init_vdso(void);
void kiddy_cleanup_vdso(void);

long kiddy_init_uname(void);
void kiddy_cleanup_uname(void);

long kiddy_init_syslog(void);
void kiddy_cleanup_syslog(void);
