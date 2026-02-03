#!/bin/bash

echo "======================================"
echo "Running VM Test Suite"
echo "======================================"

for asm in tests/*.asm; do
    echo
    echo "--------------------------------------"
    echo "TEST: $asm"
    echo "--------------------------------------"

    ./assemble "$asm"
    if [ $? -ne 0 ]; then
        echo "Assembly failed for $asm"
        continue
    fi

    byc="${asm%.asm}.byc"

    ./vm "$byc"
done

echo
echo "======================================"
echo "All tests executed"
echo "======================================"
