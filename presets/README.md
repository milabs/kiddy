# Kiddy presets

It's possible to define `Kiddy` behavior using so-called presets.

Let's use `misc/id.sh` script to identidy a system with no `Kiddy` loaded:

~~~
** UNAME identidty leaks
 - uname -r
   6.1.69
 - uname -v
   #1-NixOS SMP PREEMPT_DYNAMIC Wed Dec 20 16:00:29 UTC 2023
 - uname -a
   Linux nixos 6.1.69 #1-NixOS SMP PREEMPT_DYNAMIC Wed Dec 20 16:00:29 UTC 2023 x86_64 GNU/Linux
** PROCFS identidty leaks
 - /proc/cmdline
   initrd=\efi\nixos\jm89ii8s22r7iqmnqj54p39wlr3b9f5i-initrd-linux-6.1.69-initrd.efi init=/nix/store/8v0lhcwkb5g4sxwa5p08xl0rzhgiq5jg-nixos-system-nixos-23.11.2413.32f63574c85f/init splash loglevel=4
 - /proc/version
   Linux version 6.1.69 (nixbld@localhost) (gcc (GCC) 12.3.0, GNU ld (GNU Binutils) 2.40) #1-NixOS SMP PREEMPT_DYNAMIC Wed Dec 20 16:00:29 UTC 2023
 - /proc/sys/kernel/version
   #1-NixOS SMP PREEMPT_DYNAMIC Wed Dec 20 16:00:29 UTC 2023
 - /proc/sys/kernel/osrelease
   6.1.69
** HOSTNAMECTL identidty leaks
 - hostnamectl (kernel)
   Kernel: Linux 6.1.69
~~~

## default

System identidty **with** `Kiddy` loaded:

~~~
** UNAME identidty leaks
 - uname -r
   6.1-kiddy
 - uname -v
   #1-Linux Thu Mar 14 10:41:57 PM UTC 2024
 - uname -a
   Linux nixos 6.1-kiddy #1-Linux Thu Mar 14 10:41:57 PM UTC 2024 x86_64 GNU/Linux
** PROCFS identidty leaks
 - /proc/cmdline
   kiddy quiet ro
 - /proc/version
   Linux version 6.1-kiddy #1-Linux Thu Mar 14 10:41:57 PM UTC 2024
 - /proc/sys/kernel/version
   #1-Linux Thu Mar 14 10:41:57 PM UTC 2024
 - /proc/sys/kernel/osrelease
   6.1-kiddy
** HOSTNAMECTL identidty leaks
 - hostnamectl (kernel)
   Kernel: Linux 6.1-kiddy
~~~

## windows (tm)

System identidty **with** `Kiddy` loaded:

~~~
** UNAME identidty leaks
 - uname -r
   Windows
 - uname -v
   NT 4.0
 - uname -a
   Linux nixos Windows NT 4.0 x86_64 GNU/Linux
** PROCFS identidty leaks
 - /proc/cmdline
   \EFI\Microsoft\Boot\bootmgfw.efi
 - /proc/version
   Windows NT 4.0
 - /proc/sys/kernel/version
   NT 4.0
 - /proc/sys/kernel/osrelease
   Windows
** HOSTNAMECTL identidty leaks
 - hostnamectl (kernel)
   Kernel: Linux Windows
~~~
