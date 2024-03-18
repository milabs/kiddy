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

// kernels >= 3.4
static typeof(vm_mmap_proto) *p_vm_mmap = NULL;
static typeof(vm_munmap_proto) *p_vm_munmmap = NULL;

// kernels <= 3.3
static typeof(sys_mmap_pgoff_proto) *p_sys_mmap_pgoff = NULL;
static typeof(sys_munmap) *p_sys_munmap = NULL;

static int init_kernel_apis(void) {
	p_vm_mmap = (void *)khook_lookup_name("vm_mmap");
	p_vm_munmmap = (void *)khook_lookup_name("vm_munmap");

	if (p_vm_mmap && p_vm_munmmap) {
		return 0;
	} else {
		pr_warn("no vm_mmap/vm_munmap found\n");
	}

	p_sys_mmap_pgoff = (void *)khook_lookup_name("sys_mmap_pgoff");
	p_sys_munmap = (void *)khook_lookup_name("sys_munmap");

	if (p_sys_mmap_pgoff && p_sys_munmap) {
		return 0;
	} else {
		pr_warn("no sys_mmap_pgoff/sys_munmap found\n");
	}

	return -EINVAL;
}

void *kiddy_mmap(int len) {
	void *res = ERR_PTR(-EINVAL);

	if (p_vm_mmap) {
		res = p_vm_mmap(NULL, 0, len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0);
	} else if (p_sys_mmap_pgoff) {
		res = p_sys_mmap_pgoff(0, len, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	} else {
		WARN_ONCE(1, "kiddy_mmap was not initialized properly");
	}

	return res;
}

int kiddy_munmap(void *addr, size_t size) {
	int res = -EINVAL;

	if (p_vm_munmmap) {
		res = p_vm_munmmap(addr, size);
	} else if (p_sys_munmap) {
		res = p_sys_munmap(addr, size);
	} else {
		WARN_ONCE(1, "kiddy_munmap was not initialized properly");
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////////

int init_module() {
	long res = 0;

	if (init_kernel_apis()) {
		pr_err("unable to initialize kernel apis\n");
		return -EINVAL;
	}

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
