#!/bin/sh
#
# Run GAUL test codes.
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



