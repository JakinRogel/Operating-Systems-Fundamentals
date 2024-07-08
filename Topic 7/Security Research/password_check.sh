#!/bin/bash

password=$1

if [[ ${#password} -lt 8 ]]; then
  echo "Password must be at least 8 characters long."
fi

if ! [[ "$password" =~ [0-9] ]]; then
  echo "Password must contain at least one numeric character."
fi

if ! [[ "$password" =~ [@\#\$%\*\+=] ]]; then
  echo "Password must contain at least one special character (@, #, $, %, *, +, =)."
fi

if [[ ${#password} -ge 8 ]] && [[ "$password" =~ [0-9] ]] && [[ "$password" =~ [@\#\$%\*\+=] ]]; then
  echo "Password is strong."
fi
