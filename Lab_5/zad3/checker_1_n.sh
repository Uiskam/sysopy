#!/bin/bash
# else example
diff -b result.out good_1_n.out 
if [ $? -eq 0 ]
then
    echo -e "Correct answer\n"
else
    echo -e "ERROR\n"
fi
