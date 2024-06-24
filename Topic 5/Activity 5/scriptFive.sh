#!/bin/bash

if [ $# -eq 0 ]; then
    echo "No search name provided"
    exit 1
fi

search_name=$1
sorted_names=$(sort names.txt)

for name in $sorted_names; do
    echo $name
    if [ "$name" == "$search_name" ]; then
        echo "Found $search_name"
    fi
done


# ALL CODE DONE BY JAKIN ROGEL