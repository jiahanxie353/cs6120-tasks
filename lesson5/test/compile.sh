#!/bin/bash

read -p "Enter your choice: " choice

case $choice in
    "post")
        g++ -g post-dominator-test.cpp utils.cpp ../../cfg.cpp ../../utils.cpp ../../block.cpp -o post-dom
        ;;
    "dom")
        g++ -g dominator-test.cpp utils.cpp ../../cfg.cpp ../../utils.cpp ../../block.cpp -o dom
        ;;
    *)
        echo "Invalid input"
        ;;
esac
