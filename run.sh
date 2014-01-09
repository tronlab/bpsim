#!/bin/bash

TRACE_DIR="./traces"
OUT_DIR="./output"

make

for file in `ls $TRACE_DIR`
do
	trace=`echo $file | sed 's/.trace//'`
	echo ./cbp3 -t $TRACE_DIR/$file \> $OUT_DIR/$trace.out
	./cbp3 -t $TRACE_DIR/$file > $OUT_DIR/$trace.out
    grep Mispred_penalty_cond_br: $OUT_DIR/$trace.out
    grep Conditional_MR: $OUT_DIR/$trace.out
	echo " "
done
