#!/bin/bash

echo "peer #$1 initiated (graph read from $2)"

while :
do
    if [[ $3 -gt 0 ]]; then
        echo "peer #$1 is running as a seller (pid $$) ..."
    else
        echo "peer #$1 is running as a buyer (pid $$) ..."
    fi
    sleep 1
done
