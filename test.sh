#!/bin/bash
g++ --std=c++11 compiler.cc inputbuf.cc lexer.cc ir_debug.cc parser.cc -g

t=$1
file=`ls tests/*.txt | head -${t} | tail -1`
echo "#####################"
echo ${file}
cat -n ${file}
echo ""
echo "#####################"
./a.out < ${file}
