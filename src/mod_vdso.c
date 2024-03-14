#include "mod.h"

#include <linux/elf.h>

struct vdso_image_head {
	void *data;
};

static void *memmem(const void *h, size_t hlen, const void *n, size_t nlen) {
	if (!h || !hlen || !n || !nlen || (nlen > hlen))
		return NULL;

	while (hlen >= nlen) {
		if (!memcmp(h, n, nlen))
			return (void *)h;
		h++, hlen--;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// vdso .note fixup
//
// REFERENCE: https://elixir.bootlin.com/linux/latest/source/arch/x86/entry/vdso/vdso-note.S
//
////////////////////////////////////////////////////////////////////////////////

static struct vdso_patch {
	int is32bit;
	const char *sym;
	const char *symold;
	char *image;
} vdso_patch_tbl[] = {
	{ .is32bit = 1, .sym =  "vdso_image_32", .symold =  "vdso32_start" },
	{ .is32bit = 0, .sym =  "vdso_image_64", .symold =    "vdso_start" },
	{ .is32bit = 1, .sym = "vdso_image_x32", .symold = "vdsox32_start" },
};

typedef struct {
	int *p, old, new;
} patch_ver_t;

static void do_patch_ver(patch_ver_t *v) {
	if (v->p[0] == v->old) v->p[0] = v->new;
}

static inline long do_patch_init(struct vdso_patch *patch) {
	void *image = (void *)khook_lookup_name(patch->sym);
	if (image) {
		patch->image = ((struct vdso_image_head *)image)->data;
	} else if ((image = (void *)khook_lookup_name(patch->symold)) != NULL) {
		patch->image = image;
	}
	return !!patch->image;
}

static void do_patch_vdso32(struct vdso_patch *patch, int old_ver, int new_ver) {
	void *addr = patch->image;
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)addr;
	Elf32_Phdr *phdr = NULL;

	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2 || ehdr->e_ident[EI_MAG3] != ELFMAG3) {
		return;
	}

	phdr = (Elf32_Phdr *)(addr + ehdr->e_phoff);

	for (int i = 0; i < ehdr->e_phnum; ++i) {
		if (phdr[i].p_type == PT_NOTE) {
			int *p = (void *)memmem(addr + phdr[i].p_offset, phdr[i].p_filesz, &old_ver, sizeof(old_ver));
			if (p) {
				patch_ver_t v = { .p = p, .old = old_ver, .new = new_ver };
				khook_write_kernel((void *)do_patch_ver, &v);
				pr_info("replaced %s version %08x with %08x\n", patch->sym, old_ver, new_ver);
			}
			return;
		}
	}
}

static void do_patch_vdso64(struct vdso_patch *patch, int old_ver, int new_ver) {
	void *addr = patch->image;
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)addr;
	Elf64_Phdr *phdr = NULL;

	if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
	    ehdr->e_ident[EI_MAG2] != ELFMAG2 || ehdr->e_ident[EI_MAG3] != ELFMAG3) {
		return;
	}

	phdr = (Elf64_Phdr *)(addr + ehdr->e_phoff);

	for (int i = 0; i < ehdr->e_phnum; ++i) {
		if (phdr[i].p_type == PT_NOTE) {
			int *p = (void *)memmem(addr + phdr[i].p_offset, phdr[i].p_filesz, &old_ver, sizeof(old_ver));
			if (p) {
				patch_ver_t v = { .p = p, .old = old_ver, .new = new_ver };
				khook_write_kernel((void *)do_patch_ver, &v);
				pr_info("replaced %s version %08x with %08x\n", patch->sym, old_ver, new_ver);
			}
			return;
		}
	}
}

static inline void do_patch_vdso(struct vdso_patch *patch, int old_ver, int new_ver) {
	if (patch->is32bit)
		do_patch_vdso32(patch, old_ver, new_ver);
	else
		do_patch_vdso64(patch, old_ver, new_ver);
}

/////////////////////////////////////////////////////////////////////////////////

long kiddy_init_vdso(void) {
	for (int i = 0; i < ARRAY_SIZE(vdso_patch_tbl); i++) {
		struct vdso_patch *p = &vdso_patch_tbl[i];
		if (!do_patch_init(p)) continue;
		do_patch_vdso(p, LINUX_VERSION_CODE, KIDDY_LINUX_VERSION_CODE);
	}

	return 0;
}

void kiddy_cleanup_vdso(void) {
	for (int i = 0; i < ARRAY_SIZE(vdso_patch_tbl); i++) {
		struct vdso_patch *p = &vdso_patch_tbl[i];
		if (!p->image) continue;
		do_patch_vdso(p, KIDDY_LINUX_VERSION_CODE, LINUX_VERSION_CODE);
	}
}
