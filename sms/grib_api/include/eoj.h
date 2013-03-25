
if [[ `hostname|cut -c1-3` = hpc ]]
then
  JID=`/usr/local/bin/jid`
  rsh %HOST% -n /usr/local/bin/eoj -v $JID
  export NO_EPILOG='eoj'

  rcp %LOGDIR%%SMSNAME%.%SMSTRYNO% \
      %SMSNODE%:%SMSHOME%/%SMSNAME%.%SMSTRYNO%

  . /etc/epilog
fi

if [[ "x${ARCH}x" = "xhpia64x" ]] 
then

  rcp /var/spool/PBS/spool/${PBS_JOBID}.OU \
	  %SMSNODE%:%SMSJOBOUT%

fi

