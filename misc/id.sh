#!/usr/bin/env bash

echo "** UNAME identidty leaks"
echo " - uname -r"
echo "   $(uname -r)"
echo " - uname -v"
echo "   $(uname -v)"
echo " - uname -a"
echo "   $(uname -a)"

echo "** PROCFS identidty leaks"
echo " - /proc/cmdline"
echo "   $(head -n1 /proc/cmdline)"
echo " - /proc/version"
echo "   $(head -n1 /proc/version)"
echo " - /proc/sys/kernel/version"
echo "   $(head -n1 /proc/sys/kernel/version)"
echo " - /proc/sys/kernel/osrelease"
echo "   $(head -n1 /proc/sys/kernel/osrelease)"

if which hostnamectl >/dev/null; then
    echo "** HOSTNAMECTL identidty leaks"
    echo " - hostnamectl (kernel)"
    echo "   $(hostnamectl | grep -i kernel | xargs)"
fi
