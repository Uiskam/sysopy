#!/bin/bash
# else example
diff -b result.out good_solve.out >/dev/null
if [ $? -eq 0 ]
then
    echo -e "Correct answer\n"
else
    echo -e "ERROR\n"
fi
