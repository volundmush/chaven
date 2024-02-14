#!/bin/bash

# Set the memory usage threshold in percentage
THRESHOLD=90

while true; do
    while IFS= read -r line; do
        if [[ -n $line ]]; then
            PID=$(echo "$line" | awk '{print $1}')
            MEM=$(echo "$line" | awk '{print $4}')
            MEM=${MEM%.*}  # Remove the decimal part
            if [[ -n $MEM && "$MEM" -gt "$THRESHOLD" ]]; then
                kill -9 "$PID"
                echo "Process with PID $PID killed due to high memory usage."
            fi
        fi
    done < <(ps ax -o pid=,pmem= --sort=-pmem | sed 's/^ *//' | sed '/^$/d')
    sleep 120  # Check every 120 seconds, you can adjust this as needed
done
