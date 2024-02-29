#include "mod.h"

////////////////////////////////////////////////////////////////////////////////
// Kernel log (dmesg)
////////////////////////////////////////////////////////////////////////////////

#ifdef CONFIG_SECURITY

KHOOK_EXT(int, security_syslog, int, int);
static int khook_security_syslog(int type, int dummy) {
	if (kiddy_do_filter()) {
		return -EPERM;
	} else {
		return KHOOK_ORIGIN(security_syslog, type, dummy);
	}
}

#else

KHOOK_EXT(int, do_syslog, int, char __user *, int, int);
static int khook_do_syslog(int type, char __user *buf, int len, int source) {
	if (kiddy_do_filter())) {
		return -EPERM;
	} else {
		return KHOOK_ORIGIN(do_syslog, type, buf, len, source);
	}
}

KHOOK_EXT(int, kmsg_open, struct inode *, struct file *);
static int khook_kmsg_open(struct inode *inode, struct file *file) {
	if (kiddy_do_filter())) {
		return -EPERM;
	} else {
		return KHOOK_ORIGIN(kmsg_open, inode, file);
	}
}

KHOOK_EXT(int, devkmsg_open, struct inode *, struct file *);
static int khook_devkmsg_open(struct inode *inode, struct file *file) {
	if (kiddy_do_filter()) {
		return -EPERM;
	} else {
		return KHOOK_ORIGIN(devkmsg_open, inode, file);
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////

long kiddy_init_syslog(void) {
	return 0;
}

void kiddy_cleanup_syslog(void) {
	// nothing
}
