#include "mod.h"

#include <linux/namei.h>
#include <linux/seq_file.h>

////////////////////////////////////////////////////////////////////////////////

KHOOK_EXT(int, cmdline_proc_show, struct seq_file *, void *);
static int khook_cmdline_proc_show(struct seq_file *m, void *v) {
	if (kiddy_do_filter()) {
		return seq_printf(m, "%s\n", KIDDY_PROC_CMDLINE), 0;
	} else {
		return KHOOK_ORIGIN(cmdline_proc_show, m, v);
	}
}

KHOOK_EXT(int, version_proc_show, struct seq_file *, void *);
static int khook_version_proc_show(struct seq_file *m, void *v) {
	if (kiddy_do_filter()) {
		return seq_printf(m, "%s\n", KIDDY_PROC_VERSION), 0;
	} else {
		return KHOOK_ORIGIN(cmdline_proc_show, m, v);
	}
}

////////////////////////////////////////////////////////////////////////////////

static int nb_restrict_inodes = 0;
static struct inode_restriction {
	const char *	name;
	struct path	path;
	umode_t		mode;
} *restrict_inodes = NULL;

static int do_restrict_inodes(struct inode *inode) {
	for (int i = 0; i < nb_restrict_inodes; i++) {
		if (!restrict_inodes[i].name)
			continue;
		if (restrict_inodes[i].path.dentry->d_inode == inode) {
			return -EPERM;
		}
	}

	return 0;
}

KHOOK_EXT(int, security_inode_permission, struct inode *, int);
static int khook_security_inode_permission(struct inode *inode, int mask) {
	if (kiddy_do_filter()) {
		if (S_ISREG(inode->i_mode) || S_ISDIR(inode->i_mode)) {
			int res = do_restrict_inodes(inode);
			if (res) return res;
		}
	}
	return KHOOK_ORIGIN(security_inode_permission, inode, mask);
}

////////////////////////////////////////////////////////////////////////////////

long kiddy_init_fs(void) {
	static char rpathstr[] = KIDDY_RESTRICT_INODES, *rpaths = rpathstr;

	for (const char *p = rpaths; p && p[0]; p = strchr(p, ':')) {
		nb_restrict_inodes++, p++;
	}

	if (!nb_restrict_inodes) {
		pr_warn("no inode restrictions");
		return 0;
	}

	restrict_inodes = kcalloc(nb_restrict_inodes, sizeof(struct inode_restriction), GFP_KERNEL);
	if (!restrict_inodes) {
		pr_err("unable to allocate memory");
		return -ENOMEM;
	}

	for (int i = 0; i < nb_restrict_inodes; i++) {
		restrict_inodes[i].name = rpaths;
		if ((rpaths = strchr(rpaths, ':')) != NULL) {
			*rpaths++ = 0;
		}
		if (kern_path(restrict_inodes[i].name, LOOKUP_FOLLOW, &restrict_inodes[i].path)) {
			pr_warn("unable to restrict %s\n", restrict_inodes[i].name);
			restrict_inodes[i].name = NULL;
		} else {
			restrict_inodes[i].mode = restrict_inodes[i].path.dentry->d_inode->i_mode;
			restrict_inodes[i].path.dentry->d_inode->i_mode &= 0777700;
			pr_info("restricting user access to %s\n", restrict_inodes[i].name);
		}
	}

	return 0;
}

void kiddy_cleanup_fs(void) {
	for (int i = 0; i < nb_restrict_inodes; i++) {
		if (!restrict_inodes[i].name)
			continue;
		restrict_inodes[i].path.dentry->d_inode->i_mode = restrict_inodes[i].mode;
		path_put(&restrict_inodes[i].path);
	}
	kfree(restrict_inodes), restrict_inodes = NULL;
}
