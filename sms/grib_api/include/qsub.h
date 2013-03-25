:
# QSUB -q %QUEUE%
# QSUB -u %USER%
# QSUB -eo
# QSUB -ro
# QSUB -ko
# QSUB -nr
# QSUB -s /bin/ksh
# QSUB -r %TASK%_%FAMILY1%
# QSUB -o %LOGDIR%%SMSNAME%.%SMSTRYNO%
# QSUB -lT  500
# QSUB -lt  450
# QSUB -lM  200Mb
