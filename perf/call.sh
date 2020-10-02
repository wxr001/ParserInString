#!/bin/bash
sh -c '/usr/bin/time -v g++ -std=c++17 test.cpp -ftemplate-depth=20480 -o test' 2>&1 | grep 'Maximum resident' | sed 's/[^0-9]//g'
