#!/bin/bash
cd ~
mkdir mycode
cd mycode
cp ~/path_to_c_files/*.c .  # Adjust path_to_c_files accordingly
cd ..
mkdir mycode2
cp mycode/* mycode2/
mv mycode deadcode
rm -r deadcode
