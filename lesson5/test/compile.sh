#!/bin/bash

# Compile the test executable
g++ -g post-dominator-test.cpp ../../cfg.cpp ../../utils.cpp ../../block.cpp -o post-dom

g++ -g dominator-test.cpp ../../cfg.cpp ../../utils.cpp ../../block.cpp -o dom