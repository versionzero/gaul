#!/usr/bin/csh
# Creates a Postscript file summerizing an evolution.
# Requires Daylight stuff so should be used on hgu129 or phu050!
# Use setdaylight or equivalent first.
# Usage: makeps.csh whatever.log >whatever.ps
# Stewart. 30/06/97 

tail +31 $* | awk '{print $3" Generation: "$1"  Score: "$2"%";}' | prado

# The end.
