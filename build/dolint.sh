#!/bin/bash

echo "############################# CPP LINT ##################################"

files=`find $1 -name "*.c" -or -name "*.cc" -or -name "*.h" | grep -v picojson.h`

ret=0
python $(dirname $0)/cpplint.py $files
if [ $? -ne 0 ]; then
	ret=1
fi

echo "############################# LINT DONE ##################################"

exit $ret

