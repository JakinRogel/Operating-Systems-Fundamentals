#!/bin/bash
# Script to find logins during non-office hours (before 8 AM and after 6 PM)

log_file="/var/log/auth.log"
grep -E 'session opened for user' $log_file | awk '{print $1, $2, $3, $11}' | while read date time user; do
    hour=$(echo $time | cut -d: -f1)
    if [ $hour -lt 8 ] || [ $hour -gt 18 ]; then
        echo "User $user logged in during non-office hours on $date at $time"
    fi
done
