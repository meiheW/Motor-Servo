#!/bin/sh

# command to run
COMMAND=/home/root/motor-servo
# holding process name
PRO_NAME=motor-servo

while true ; do

  NUM=`ps | grep ${PRO_NAME} | grep -v grep | wc -l`
 
  # the running process less than 1
  if [ "$NUM" -lt "1" ]; then
    echo "${PRO_NAME} was killed"
    # run the command
    ${COMMAND}
  elif ["$NUM" -gt "1"]; then
    # kill all of the running process
    killall -9 ${PRO_NAME}
    ${COMMAND}
  fi

  # kill the zombie process
  NUM_STAT=`ps | grep ${PRO_NAME} | grep T | grep -v grep | wc -l`

  if [ "${NUM_STAT}" -gt "0" ]; then
    killall -9 ${PRO_NAME}
    ${COMMAND}
  fi
  
done

exit 0
