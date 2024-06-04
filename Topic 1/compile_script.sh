#!/bin/bash

# Step a: Open the 'compile.sh' script in the text editor
gedit compile.sh &

# Step b: Check that the script arguments are not empty
if [ -z "$1" ]; then
    echo "Error: No program name provided."
    exit 1
fi

# Step c: Save the program to compile script that is passed as a script argument in a variable
program_to_compile="$1"

# Step d: Substitute the variable where you currently have the 'HelloWorld' program name text
echo "#!/bin/sh" > compile.sh
echo "echo \"Compiling...\"" >> compile.sh

# Step e: Check the return value from 'gcc'
if ! gcc -o "$program_to_compile" "$program_to_compile.c"; then
    echo "Error: Compilation failed."
    exit 1
fi

echo "echo \"Running...\"" >> compile.sh
echo "./$program_to_compile" >> compile.sh

# Step f: Wait for the text editor to be closed
wait

# Step g: Run your script 'compile.sh' from a Terminal session
./compile.sh

# Step h: Give the script permission
chmod 777 compile.sh

# Step i: Run the script again
./compile.sh

