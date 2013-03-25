# QSUB -p 70
#=======================================================================
# step1.h
# Define second step for copying the logfile when on the ibm-cluster 
#=======================================================================
# @ step_name=step_1
# @ queue
# @ dependency=(step_1==0 || step_1!=0) 
# @ class=%QUEUE_EPILOG:emos-exp%
# @ user_priority=100
# @ output = /dev/null
# @ error =  /dev/null
# @ job_name = $(jobid)
# @ step_name = step_2

export LOADL_STEP_NAME=${LOADL_STEP_NAME:-step_1}
case $LOADL_STEP_NAME in
step_1)
