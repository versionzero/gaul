#!/bin/sh
#
#######################################################################
# gaul-devel/tests/run_tests.sh
#######################################################################
#
# GAUL - Genetic Algorithm Utility Library
# Copyright ©2005, Stewart Adcock <stewart@linux-domain.com>
# All rights reserved.
#
# The latest version of this program should be available at:
# http://gaul.sourceforge.net/
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.  Alternatively, if your project
# is incompatible with the GPL, I will probably agree to requests
# for permission to use the terms of any other license.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY WHATSOEVER.
#
# A full copy of the GNU General Public License should be in the file
# "COPYING" provided with this distribution; if not, see:
# http://www.gnu.org/
#
#######################################################################
#
# Synopsis:	Run GAUL test codes.
#

echo "Running GAUL tests:"

#
# Simple tests for programs with deterministic output.
# We look for files named "(1).out" in the testout subdirectory,
# where (1) is the name of a test program, and run them all.
#

for f in testout/*.out
  do
    BASENAME=`basename $f .out`
    echo -n "$BASENAME... "
    ./$BASENAME > /tmp/$$.$BASENAME.output
    if [ `expr 1 + \`diff $f /tmp/$$.$BASENAME.output | wc -l\`` -gt 1 ]; then
      echo "FAILED - please check on the output of the $BASENAME executable."
    else
      echo "PASSED"
    fi
    rm /tmp/$$.$BASENAME.output
  done

#
# S-Lang test script special case.
#

echo -n "test_slang... "
./test_slang --script test_slang.sl > /tmp/$$.test_slang.output
if [ `expr 1 + \`diff testout/test_slang.log /tmp/$$.test_slang.output | wc -l\`` -gt 1 ]; then
  echo "FAILED - please check on the output of './test_slang --script test_slang.sl'."
else
  echo "PASSED"
fi
rm /tmp/$$.test_slang.output

#
# Random number generator test script special case.
#

echo -n "test_prng... "
if [ `expr 1 + \`./test_prng | grep FAILED | wc -l\`` -gt 31 ]; then
  echo "FAILED - please check on the chi squared tests from './test_prng'."
else
  echo "PASSED"
fi



