
#!/bin/bash
# else example
diff -b result.out good_1_n.out 
if [ $? -eq 0 ]
then
    echo Correct answer
else
    echo ERROR
fi
