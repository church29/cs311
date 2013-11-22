#!/bin/bash
#
# This shell script is only an example of how the same type of
# processing as is required for Homweork #5 can be done in a bash
# script.  It does not mean that you need to have these same
# functions in your C code.  This simple shell script does not
# allow multiple sorts, as your C code will allow.
#
# Jesse

function removeNonAlpha {
    # convert all the non-alphabetic characters into spaces
    sed 's/[^a-zA-Z]/ /g'
}

function lowerCase {
    # convert all upper-case characters to lower case
    tr '[A-Z]' '[a-z]'
}

function splitLines {
    # split all the words on a single line into a bunch of lines
    awk '{ for(i = 1; i <= NF; i++) { print $i; } }'
}


removeNonAlpha | lowerCase | splitLines | sort | uniq -c
