#include "mod.h"

static struct {
	const char *name;
	long (* init)(void);
	void (* cleanup)(void);
} modules[] = {
	{ "fs", kiddy_init_fs, kiddy_cleanup_fs },
	{ "vdso", kiddy_init_vdso, kiddy_cleanup_vdso },
	{ "uname", kiddy_init_uname, kiddy_cleanup_uname },
	{ "syslog", kiddy_init_syslog, kiddy_cleanup_syslog },
};

////////////////////////////////////////////////////////////////////////////////

static unsigned long kiddy_lookup(const char *name) {
	unsigned long addr = khook_lookup_name(name);
	if (!addr && (strstr(name, "__do_sys") == name)) {
		addr = khook_lookup_name(name + 5); // __do_sys_xxx -> sys_xxx
	}
	return addr;
}

////////////////////////////////////////////////////////////////////////////////

int init_module() {
	long res = 0;

	for (int i = 0; i < ARRAY_SIZE(modules); i++) {
		if ((res = modules[i].init()) != 0) {
			pr_err("failed to initialize module %s\n", modules[i].name);
			while (--i >= 0) modules[i].cleanup();
			return res;
		}
	}

	return khook_init(kiddy_lookup);
}

void cleanup_module() {
	khook_cleanup();

	for (int i = 0; i < ARRAY_SIZE(modules); i++) {
		modules[ARRAY_SIZE(modules) - i - 1].cleanup();
	}
}

MODULE_LICENSE("GPL\0but who really cares?");
