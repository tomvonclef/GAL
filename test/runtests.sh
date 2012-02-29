#!/bin/sh

# runtests.sh by Ross Young
# This script will test gal with all files in good/ or bad/
# gal should return a nonzero exit code on bad compilation attempts
# files that are expected to compile should be placed under good/
# known bad files should be placed under bad/

ERROR=error.log
OUTPUT=output.log
headerflag=0
status=0
>$ERROR
>$OUTPUT

if [ ! -d "output" ]; then
    mkdir output
fi

echo "Running tests"
for file in good/*
do
    echo >> $ERROR
    echo $file >> $ERROR
    echo "============" >>$ERROR
    ../bin/gal -d output $1 $file 2>>$ERROR
    if [ "$?" -ne 0 ]; then
        status=1
        if [ $headerflag -eq 0 ]; then
            echo "The following files should have compiled but did not."
            headerflag=1
        fi
        echo "$file: FAILED"
    fi
    echo >> $ERROR
    printf "."
done
headerflag=0

for file in bad/*
do
    echo $file >> $ERROR
    echo "============" >> $ERROR
    ../bin/gal -d output $1 $file 2>>$ERROR
    if [ "$?" -eq 0 ]; then
        status=1 
        if [ $headerflag -eq 0 ]; then
            echo
            echo "The following files have errors and should NOT have compiled."
            headerflag=1
        fi
        echo "$file: FAILED"
    fi
    echo >> $ERROR
    printf "."
done
headerflag=0

for file in output/*
do
    echo $file >> $ERROR
    echo "===========" >> $ERROR
    echo $file >> $OUTPUT
    echo "===========" >> $OUTPUT
    if [ $file != "output/gal_funcs.py" ] && [ $file != "output/gal_funcs.pyc" ]; then
        python -d $file 1>>$OUTPUT 2>>$ERROR
    fi
    
    if [ "$?" -ne 0 ]; then
        status=1
        if [ $headerflag -eq 0 ]; then
            echo "The following files did not run."
            headerflag=1
        fi
        echo "$file: FAILED"
    fi
    echo >> $ERROR
    printf "."
done

if [ $status -eq 0 ]; then
    echo "All tests passed."
fi

echo
echo "See test/error.log for compilation details."
echo "See test/output.log for additional output."
echo "See test/output/ for resulting python code."
echo

exit $status
