#include "mod.h"

#include <linux/utsname.h>
#include <linux/sysctl.h>
#include <linux/mman.h>

static inline void copymem(char *dst, int dstlen, const char *src, int srclen) {
	memset(dst, 0, dstlen);
	memcpy(dst, src, min(dstlen - 1, srclen - 1));
}

#define copymem_pv(p, v)				\
	copymem((p), sizeof(p), (v), sizeof(v))

#define fixup_utsname(p) do {				\
	copymem_pv((p)->version, KIDDY_UTS_VERSION);	\
	copymem_pv((p)->release, KIDDY_UTS_RELEASE);	\
} while (0)

////////////////////////////////////////////////////////////////////////////////
// UTS namespace (uname) support
////////////////////////////////////////////////////////////////////////////////

KHOOK_EXT(int, __do_sys_newuname, struct new_utsname __user *);
static int khook___do_sys_newuname(struct new_utsname __user *name) {
	int ignored = 0;

	if (kiddy_do_filter()) {
		struct new_utsname tmp = { 0 };
		const size_t len = roundup(sizeof(tmp), PAGE_SIZE);
		void *map = (void *)vm_mmap(NULL, 0, len,
					    PROT_READ|PROT_WRITE,
					    MAP_ANONYMOUS|MAP_PRIVATE, 0);
		if (map) {
			int res = KHOOK_ORIGIN(__do_sys_newuname, map);

			if (!res) {
				ignored = copy_from_user(&tmp, map, sizeof(tmp));
				fixup_utsname(&tmp);
				if (copy_to_user(name, &tmp, sizeof(tmp))) {
					res = -EFAULT;
				}
			}

			vm_munmap((unsigned long)map, len);

			return res;
		}
	}

	return KHOOK_ORIGIN(__do_sys_newuname, name);
}


#ifdef __ARCH_WANT_SYS_OLD_UNAME

KHOOK_EXT(int, __do_sys_uname, struct old_utsname __user *);
static int khook___do_sys_uname(struct old_utsname __user *name) {
	int ignored = 0;

	if (kiddy_do_filter()) {
		struct old_utsname tmp = { 0 };
		const size_t len = roundup(sizeof(tmp), PAGE_SIZE);
		void *map = (void *)vm_mmap(NULL, 0, len,
					    PROT_READ|PROT_WRITE,
					    MAP_ANONYMOUS|MAP_PRIVATE, 0);
		if (map) {
			int res = KHOOK_ORIGIN(__do_sys_uname, map);

			if (!res) {
				ignored = copy_from_user(&tmp, map, sizeof(tmp));
				fixup_utsname(&tmp);
				if (copy_to_user(name, &tmp, sizeof(tmp))) {
					res = -EFAULT;
				}
			}

			vm_munmap((unsigned long)map, len);

			return res;
		}
	}

	return KHOOK_ORIGIN(__do_sys_uname, name);
}

KHOOK_EXT(int, __do_sys_olduname, struct oldold_utsname __user *);
static int khook___do_sys_olduname(struct oldold_utsname __user *name) {
	int ignored = 0;

	if (kiddy_do_filter()) {
		struct oldold_utsname tmp = { 0 };
		const size_t len = roundup(sizeof(tmp), PAGE_SIZE);
		void *map = (void *)vm_mmap(NULL, 0, len,
					    PROT_READ|PROT_WRITE,
					    MAP_ANONYMOUS|MAP_PRIVATE, 0);
		if (map) {
			int res = KHOOK_ORIGIN(__do_sys_olduname, map);

			if (!res) {
				ignored = copy_from_user(&tmp, map, sizeof(tmp));
				fixup_utsname(&tmp);
				if (copy_to_user(name, &tmp, sizeof(tmp))) {
					res = -EFAULT;
				}
			}

			vm_munmap((unsigned long)map, len);

			return res;
		}
	}

	return KHOOK_ORIGIN(__do_sys_olduname, name);
}

#endif // __ARCH_WANT_SYS_OLD_UNAME

//
// cat /proc/sys/kernel/{version,release,...}
//
// REFERENCE: https://elixir.bootlin.com/linux/latest/source/kernel/utsname_sysctl.c
//

KHOOK_EXT(int, proc_do_uts_string, struct ctl_table *, int, void *, size_t *, loff_t *);
static int khook_proc_do_uts_string(struct ctl_table *table, int write, void *buffer, size_t *lenp, loff_t *ppos) {
	const char *procname = table->procname ?: "-";

	if (kiddy_do_filter()) {
		struct ctl_table uts_table;
		char tmp_data[__NEW_UTS_LEN + 1];

		memcpy(&uts_table, table, sizeof(uts_table));
		uts_table.data = tmp_data;

		if (!strcmp(procname, "version")) {
			copymem_pv(tmp_data, KIDDY_UTS_VERSION);
			return proc_dostring(&uts_table, write, buffer, lenp, ppos);
		} else if (!strcmp(procname, "osrelease")) {
			copymem_pv(tmp_data, KIDDY_UTS_RELEASE);
			return proc_dostring(&uts_table, write, buffer, lenp, ppos);
		}
	}

	return KHOOK_ORIGIN(proc_do_uts_string, table, write, buffer, lenp, ppos);
}

////////////////////////////////////////////////////////////////////////////////

long kiddy_init_uname(void) {
	return 0;
}

void kiddy_cleanup_uname(void) {
	// nothing
}