#!/bin/bash

# Check if script is run as root
if [[ $EUID -ne 0 ]]; then
    echo "This script must be run as root."
    exit 1
fi

# Check for correct number of arguments
if [ $# -ne 3 ]; then
    echo "Usage: $0 <input_file> <group_name> <-a or -r>"
    exit 1
fi

input_file=$1
group_name=$2
operation=$3

# Check if group exists
if ! getent group $group_name >/dev/null; then
    echo "Group $group_name does not exist. Creating..."
    groupadd $group_name
    echo "Group $group_name created."
fi

# Function to add users
function add_users {
    while read -r line; do
        username=$(echo $line | cut -d' ' -f1)
        hashed_password=$(echo $line | cut -d' ' -f2-)
        
        if [[ -n $username && -n $hashed_password ]]; then
            useradd -m -g $group_name $username
            echo "$username:$hashed_password" | chpasswd -e
            echo "User $username added to group $group_name."
        else
            echo "Skipping empty or invalid line in input file."
        fi
    done < "$input_file"
}

# Function to remove users
function remove_users {
    while read -r username _; do
        if [[ -n $username ]]; then
            userdel -r $username
            echo "User $username removed."
        else
            echo "Skipping empty or invalid line in input file."
        fi
    done < "$input_file"
}

# Perform operation based on flag
case "$operation" in
    -a)
        echo "Adding users..."
        add_users
        ;;
    -r)
        echo "Removing users..."
        remove_users
        ;;
    *)
        echo "Invalid operation. Use -a to add users or -r to remove users."
        exit 1
        ;;
esac

exit 0
