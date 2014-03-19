#!/bin/sh

ext="png"

if [ $# -lt 1 ] ; then
  echo "Usage : $0 file.dot"
  exit 1
fi

dot -T$ext "$1" -o image.$ext

if [ ! $? -eq 0 ]; then
    exit 1
fi

shotwell image.$ext 2> /dev/null
