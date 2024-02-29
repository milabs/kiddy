# 0

`Kiddy` - Linux Kernel identify spoofer

# Purpose

`Kiddy` module performs runtime Linux kernel modification so its futher **identification** will give **irrelevant** result.

Such modification serves the purpose of **defeating** kernel **exploitation** attempts as most of kernel exploits require the version of the kernel to be known to use the proper offsets[^1].

In other words, `Kiddy` changes current kernel identification and so implements `security through obscurity`.

[^1]: [CVE-20217-1000112](https://github.com/xairy/kernel-exploits/blob/master/CVE-2017-1000112/poc.c#L91)

# Challenges

## Kernel log (dmesg)

It's obvious that kernel log has a lot of identification information. Exposing the information usually happen via `dmesg` or its underlying interfaces. Although, some modern systems already restrict non-privileged kernel log access, `Kiddy` enforces same restriction as well.

STATUS: `IMPLEMENTED` ([source](src/mod_syslog.c))

## UTS namespace (uname)

UTS namespace provide isolation of `hostname` and `domainname` via `set/gethostname()` and `set/getdomainname()` system call services. Besides that, UTS contains the following information: machine architecture, OS type and release information, kernel version:

~~~
enum uts_proc {
	UTS_PROC_ARCH,
	UTS_PROC_OSTYPE,
	UTS_PROC_OSRELEASE,
	UTS_PROC_VERSION,
	UTS_PROC_HOSTNAME,
	UTS_PROC_DOMAINNAME,
};
~~~

While is's possible to alter `hostname` and `domainname` values, other parameters are not changable but provide sensitive identification information via the following interfaces:

* procfs
  - /proc/sys/kernel/{version,osrelease}
* syscalls
  - sys\_newuname(2)
  - sys\_uname(2)
  - sys\_olduname(2)
* hostnamectl
  - calls systemd-hostnamed service via D-Bus remote API

NOTE: Current implementation takes care of kernel version and OS release information.

STATUS: `IMPLEMENTED` ([source](src/mod_uname.c))

## vDSO (virtual dynamic shared object)

The `vDSO` (virtual dynamic shared object) is a small shared library that the kernel automatically maps into the address space of all user-space applications.

`vDSO` exposes `LINUX_VERSION_CODE` via `.note` section so must be handled.

STATUS: `IMPLEMENTED` ([source](src/mod_vdso.c))

## Filesystem artifacts

Kernel version information:

~~~
/proc/cmdline
/proc/version
~~~

Some other files/directories to protect:

~~~
/boot
/lib/modules
/proc/modules
/proc/kallsyms
/proc/config.gz
/usr/src/kernels
... see default preset for more details
~~~

STATUS: `IMPLEMENTED` ([source](src/mod_fs.c))

# Implementation details

`Kiddy` is based on [KHOOK](https://github.com/milabs/khook) which is kernel hooking library.

Hooking (or intercepting) kernel function allows to alter its behaviour thus it becomes possible to filter out data which goes from the kernel to processes via system calls.

The filtering decision is made as follows: filter data for all non-root processes OR root processes without tty. The later condition allows to handle `hostnamectl` cases by filtering out root services (which do not have a terminal).

# Usage

~~~
[PRESET=<preset>] make
sudo insmod kiddy.ko
~~~

# Limitations

Doesn't help with `dirtycow`-like exploits.

Doesn't help with `SUID`-like user-mode exploits (sudo, pkexec & friends).

# License

This software is licensed under the GPL.

# Author

[Ilya V. Matveychikov](https://github.com/milabs)

2023, 2024
