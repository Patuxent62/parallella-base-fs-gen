#!/bin/bash

set -e

apt-get install $(cat $1 | grep -v "^#" | sed 's,\s\s*,\n,g' | grep -v "^\s*$" | xargs echo )
