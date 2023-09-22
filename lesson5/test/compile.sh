#!/bin/bash

# Compile the test executable
g++ -g dominator-test.cpp ../../cfg.cpp ../../utils.cpp ../../block.cpp -o test