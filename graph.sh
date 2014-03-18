#!/bin/sh

if [ $# -lt 1 ] ; then
  echo "Usage : $0 file.dot"
  exit 1
fi

dot -Tpng $1 -o image.png

if [ ! $? -eq 0 ]; then
    exit 1
fi

shotwell image.png 2> /dev/null
