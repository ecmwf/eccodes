#=======================================================================
# step2.h
# Define second step for copying the logfile when on the ibm-cluster 
#=======================================================================

#sleep for a few seconds to allow next task to get into queue

;;
step_2)

if [[ "x${ARCH}x" = "xibm_power4x" ]]; then
  rcp %SMSJOBOUT% %SMSNODE%:%SMSJOBOUT%
fi

exit
;;
esac
