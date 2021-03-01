#!/bin/bash

set -e


SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


$EXEPATH/bin/e-write-buf.elf > e-write-buf.log

retval=$?

if [ $retval -ne 0 ]
then
    echo "$SCRIPT FAILED"
else
    echo "$SCRIPT PASSED"
fi

exit $retval

