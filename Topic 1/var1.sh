#!/bin/bash

# Step h: Modify 'var1.sh' script and prefix the variable with the word export
export my_var="Hello from var1.sh"

# Step c: Call the printenv command to print the environment variables out
echo "Environment variables in var1.sh:"
printenv

# Step d: Call var2.sh from this script
echo "Calling var2.sh from var1.sh..."
./var2.sh

