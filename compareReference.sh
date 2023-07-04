#! /bin/sh

# Purpose: Compare our um with the reference um

make clean
clear
make
if [ -f $executable ] ; then
        clear
        echo "Hello"
        ./um umbin/hello.um > "out"
        um umbin/hello.um > "outReference"
        diffOutput1=$(diff "out" "outReference")
        if [[ $diffOutput1 != "" ]] ; then
                echo -e "\nREFERENCE OUTPUT IS DIFFERENT:\n$diffOutput1\n"
        else
                echo -e "Success!\n"
        fi
        
        echo "Midmark"
        ./um umbin/midmark.um > "out"
        um umbin/midmark.um > "outReference"
        diffOutput1=$(diff "out" "outReference")
        if [[ $diffOutput1 != "" ]] ; then
                echo -e "\nREFERENCE OUTPUT IS DIFFERENT:\n$diffOutput1"
        else
                echo -e "Success!\n"
        fi

        echo "Sandmark"
        ./um umbin/sandmark.umz > "out"
        um umbin/sandmark.umz > "outReference"
        diffOutput1=$(diff "out" "outReference")
        if [[ $diffOutput1 != "" ]] ; then
                echo -e "\nREFERENCE OUTPUT IS DIFFERENT:\n$diffOutput1"
        else
                echo -e "Success!\n"
        fi
fi
