#!/bin/ksh

set -e # stop the shell on first error
set -u # fail when using an undefined variable

# Defines the three variables that are needed for any
# communication with SMS

export SMS_PROG=%SMS_PROG%  # SMS Remote Procedure Call number
export SMSNODE=%SMSNODE%    # The name sms that issued this task
export SMSNAME=%SMSNAME%    # The name of this current task
export SMSPASS=%SMSPASS%    # A unique password
export SMSTRYNO=%SMSTRYNO%  # Current try number of the task

# Tell SMS we have stated
# The SMS variable SMSRID will be set to parameter of smsinit
# Here we give the current PID.

smsinit $$ 

# Defined a error hanlder

ERROR() {
        set +e        # Clear -e flag, so we don't fail
        smsabort      # Notify SMS that something went wrong
        trap 0        # Remove the trap
		. /usr/local/share/ecmwf/share/.epilog
        exit 0        # End the script
}

# Trap any calls to exit and errors caught by the -e flag

trap ERROR 0

# Trap any signal that may cause the script to fail

trap '{ echo "Killed by a signal"; ERROR ; }' 1 2 3 4 5 6 7 8 10 12 13 15

ulimit -s
ulimit -d
ulimit -m
ulimit -a

export SMSHOME=%SMSHOME%

workdir=%WORKDIR%/%FAMILY1%

[[ -d $workdir ]] || mkdir -p $workdir

export MARS_MM_FIRSTOFMONTH=1

[[ %MARSMS% -eq 1 ]] && export MARS_HOME=/home/ma/max/dwd/$ARCH

# hostname >> $SMSHOME/hosts

set -x # echo script lines as they are executed
cd $workdir

export MARSCMD='mars -t'

#banner `hostname`

#export LOCAL_DEFINITION_TEMPLATES=/
