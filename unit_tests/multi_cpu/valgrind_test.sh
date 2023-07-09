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

echo "Testing with no arguments"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./sched_sim.out > valgrind.log 2>&1
checkLog

echo ""

echo "Testing passing only cpu number"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./sched_sim.out 2 > valgrind.log 2>&1
checkLog

echo ""

echo "Testing without passing cpu number"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./sched_sim.out ./example/p{1..4}.txt > valgrind.log 2>&1
checkLog

echo ""

echo "Testing normal"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./sched_sim.out 2 ./example/p{1..4}.txt > valgrind.log 2>&1
checkLog

rm valgrind.log