#!/bin/bash

checkLog() {
    if grep -q "no leaks are possible" valgrind.log; then
        printf "\tNo memory leaks\n"
    else
        printf "\tMemory leaks!\n"
        exit 1
    fi
    if grep -q "ERROR SUMMARY: 0 errors" valgrind.log; then
        printf "\tNo errors\n"
    else
        printf "\tErrors found!\n"
        exit 2
    fi
}

echo "Testing with generics"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test_with_generics.out > valgrind.log 2>&1
checkLog

echo "Testing with specilised"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test_with_specialised.out > valgrind.log 2>&1
checkLog

rm valgrind.log