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
# Simple test programs with deterministic output.
#

for f in *.out
  do
    BASENAME=`basename $f .out`
    echo -n "$BASENAME... "
    ./$BASENAME > $BASENAME.output
    if [ `expr 1 + \`diff $f $BASENAME.output | wc -l\`` -gt 1 ]; then
      echo "FAILED - please check on the output of the $BASENAME executable."
    else
      echo "PASSED"
    fi
  done

#
# S-Lang test script.
#

echo -n "test_slang... "
./test_slang --script test_slang.sl > test_slang.output
if [ `expr 1 + \`diff test_slang.log test_slang.output | wc -l\`` -gt 1 ]; then
  echo "FAILED - please check on the output of './test_slang --script test_slang.sl'."
else
  echo "PASSED"
fi

#
# Random number generator test.
#

echo -n "test_prng... "
if [ `expr 1 + \`./test_prng | grep FAILED | wc -l\`` -gt 31 ]; then
  echo "FAILED - please check on the chi squared tests from './test_prng'."
else
  echo "PASSED"
fi



