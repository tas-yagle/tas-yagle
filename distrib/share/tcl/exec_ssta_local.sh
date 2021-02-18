#!/bin/sh

if [ $# -ne 4 ]
then
  echo "bad call to script.sh"
  echo "script.sh $*"
fi

# slave.tcl (arg 3 of runStatHiTas in script master.tcl)
script=$1

# sample number (arg 1 "id:subid" of runStatHiTas in script master.tcl)
id=$2

# parallel job number (arg 1 "id:subid" of runStatHiTas in script master.tcl)
subid=$3

# hard value - not configurable
avtdata=$4


# source files
curw=`pwd`

# when running in parallel, must create different working directories to avoid conflicts 
workdir="${curw}/SSTA_JOB_${id}"

rm -rf $workdir
mkdir $workdir
cd $workdir
$script $avtdata > stdout.log 2>&1

if [ $? -eq 0 ]
then
  cd /tmp
  rm -rf $workdir
  exit 0
fi

exit 1
