#! /bin/sh

# Purpose: Automate unit testing process for Universal Machine

testFiles=*.um
valgrind=$1
executable="um"
success=true
curr_success=true

make clean
clear
make
if [ -f $executable ] ; then
        echo "" > "failedTests.txt"
        ./writetests
        clear
        for testFile in $testFiles ; do
                curr_success=true
                testName=$(echo $testFile | sed -E 's/(.*).um/\1/')
                echo -e "\n---------------------------------------------\n$testName:"
                if [[ $valgrind == "val" ]] ; then
                        valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./um $testFile > "$testName-out"
                else
                        ./um $testFile 2> "$testName-err" 1> "$testName-out"
                        if [ -s "$testName-err" ] ; then
                                echo -e "$testName\n" >> "failedTests.txt"
                                cat "$testName-err"
                                echo -e ""
                                curr_success=false
                                success=false
                        fi
                fi

                um $testFile > "$testName-out-reference"
                diffOutput=$(diff "$testName-out" "$testName-out-reference")
                if [[ $diffOutput != "" ]] ; then
                        echo -e "\nREFERENCE OUTPUT IS DIFFERENT:\n$diffOutput"
                        if $curr_success ; then
                                echo -e "$testName\n" >> "failedTests.txt"
                        fi
                        curr_success=false
                        success=false
                fi

                if [ -f $testName.1 ] ; then
                        diffOutput=$(diff $testName.1 "$testName-out")
                        if [[ $diffOutput != "" ]] ; then
                                echo -e "\nDIFF TEST FAILED:\n$diffOutput"
                                if $curr_success ; then
                                        echo -e "$testName\n" >> "failedTests.txt"
                                fi
                                success=false
                        fi
                elif [ -s "$testName-out" ] ; then
                        echo -e "\nOUTPUT GIVEN WHEN NOT EXPECTED:"
                        cat "$testName-out"
                        echo ""
                        if $curr_success ; then
                                echo -e "$testName\n" >> "failedTests.txt"
                        fi
                        success=false
                fi
                echo -e "---------------------------------------------"
        done
        if $success ; then
                echo -e "\nCongratualtions! All tests passed! (DOES NOT ACCOUNT FOR VALGRIND ERRORS)\n"
        fi
fi