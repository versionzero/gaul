#!/bin/csh -f
#
#######################################################################
# gaul/examples/run_examples.csh
#######################################################################
#
# GAUL - Genetic Algorithm Utility library.
# Copyright ©2000-2002, Stewart Adcock <stewart@linux-domain.com>
#
# The latest version of this program should be available at:
# http://www.stewart-adcock.co.uk/
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
# Synopsis:	Run the series of GAUL tests.
#
# Usage:	After compilation, adjust the specified paths, then
#		type either:
#			./run_examples.csh
#		or (on systems with PBS):
#			qsub ./run_examples.csh
#
# This is PBS stuff:
#
#PBS -N TestingGaul
#PBS -l walltime=24:0:0
#PBS -M ${EMAIL} -m ea
#

#
# Adjust this path:
#
#setenv GAUL_TEST_DIR /net/home/adcock/Code_development/gaul-devel/examples
setenv GAUL_TEST_DIR /u1/gaul-devel/examples

#
# Comment this to squash the core dumps:
#
limit coredumpsize 0

#
# Run tests.
#
setenv unique $$

setenv WKDIR /tmp/gaul-tests/${unique}
mkdir -p $WKDIR >& /dev/null
cd $WKDIR

#if (-e examples.log ) rm -f examples.log

echo ">>> Running GAUL example programs." >! examples.log
foreach f ( diagnostics struggle struggle2 struggle3 struggle4 struggle5 struggle_ss royalroad goldberg1 goldberg2 pingpong nnevolve wildfire )
  echo ">> Running ${f}" >>& examples.log
  time $GAUL_TEST_DIR/$f >>& examples.log
end

echo ">>> All tests completed."
