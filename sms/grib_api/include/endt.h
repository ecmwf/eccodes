#==========================================================================
#  End task.
#  Clean up and exit
#==========================================================================

wait
/usr/local/bin/smscomplete
trap 0

set +e

%include <eoj.h>

exit 0

# SMSCMD is %SMSCMD%
# SMSNODE is %SMSNODE%
#
