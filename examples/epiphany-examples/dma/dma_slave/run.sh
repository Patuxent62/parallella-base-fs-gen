#!/bin/bash

set -e

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")


cd $EXEPATH/bin

./dma_slave_test.elf

