#!/bin/bash

if [ $# -eq 0 ]; then
    echo "No arguments provided"
    exit 1
fi

number=$1

if [ $number -lt 50 ]; then
    echo "The number is less than 50"
elif [ $number -eq 50 ]; then
    echo "The number is equal to 50"
else
    echo "The number is greater than 50"
fi

# ALL CODE DONE BY JAKIN ROGEL