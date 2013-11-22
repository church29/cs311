#!/bin/bash
#
# This shell script is only an example of how the same type of
# processing as is required for Homweork #5 Problem 2 can be done 
# in a bash script. 
#
# pipeline.bash -f outfile2 < infile > outfile1
#
# rev < simple1.txt | sort | uniq -c | tee outfile2 | wc > outfile1
#
# another way to represent the same pipeline is this:
# (rev | sort | uniq -c | tee outfile2| wc) < simple1.txt > outfile1
#
# Jesse
#
# the script does not really parse fhe command line, it just puts $2 as the
# argument for tee in the pipeline.  In bash programming, $2 is just the second
# argument on the command line.

rev | sort | uniq -c  | tee $2 | wc

