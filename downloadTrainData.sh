#!/bin/bash
if [ ! -f $2 ];then
  source $(pwd)/env.sh
  alien.py cp alien://$1 file:$2   
else
  echo "File already downloaded."
fi
