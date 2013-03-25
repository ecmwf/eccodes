# SYNOPSIS
#
#   AX_LINUX_DISTRIBUTION
#
# DESCRIPTION
#
#	Try to detect the name and version of the Linux distribution where the software is being run
#
#   In configure.in:
#
#     dnl For my rpm.m4 macros
#     AC_SUBST(LINUX_DISTRIBUTION_NAME)
#     AC_SUBST(LINUX_DISTRIBUTION_VERSION)
#
#  	Daniel Varela <dvarela@ecmwf.int> 2011
#   Version 1.1 (2011-11-11)
 

dnl AX_LINUX_DISTRIBUTION
dnl Figure out the Linux distribution where the software is being built
dnl automake target

AC_DEFUN([AX_LINUX_DISTRIBUTION],
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
HOST_CPU=${host_cpu}
HOST_VENDOR=${host_vendor}
HOST_OS=${host_os}

if test x$HOST_OS = "xlinux-gnu"
then
	AC_MSG_CHECKING(for Linux distribution )

	# This works for Fedora, RedHat and Slackware
	for f in /etc/fedora-release /etc/redhat-release /etc/slackware-release
	do
		if test -f $f; then
			distro=`cat $f`	
			break
		fi
	done	

	# This works in Ubuntu (11 at least)
	if test -f /etc/lsb-release; then
		distro=`cat /etc/lsb-release | grep DISTRIB_ID | awk -F= '{print $2}' `
		distro_version=`cat /etc/lsb-release | grep DISTRIB_RELEASE | awk -F= '{print $2}' `
	fi

	# For SuSE
	if test -f /etc/SuSE-release; then
		distro=`cat /etc/SuSE-release | head -1`
		#distro_version=`cat /etc/SuSE-release | tail -1 | awk -F= '{print $2}' `
	fi

	# At least Debian has this
	if test -f /etc/issue.net -a "x$distro" = x; then
		distro=`cat /etc/issue.net | head -1`
	fi

	# Everything else
	if test "x$distro" = x; then
		distro="Unknown Linux"
	fi
	
	LINUX_DISTRIBUTION_NAME=$distro
	LINUX_DISTRIBUTION_VERSION=$distro_version
    AC_MSG_RESULT($LINUX_DISTRIBUTION_NAME $LINUX_DISTRIBUTION_VERSION)

else
	LINUX_DISTRIBUTION_NAME=$HOST_OS
	LINUX_DISTRIBUTION_VERSION=""
	AC_MSG_NOTICE(OS is non-Linux UNIX $HOST_OS.)
fi

AC_SUBST(LINUX_DISTRIBUTION_NAME)
AC_SUBST(LINUX_DISTRIBUTION_VERSION)
])
