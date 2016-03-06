#!/bin/bash

# get all valid ip addresses (including local loopback)
ip_addrs=`ifconfig  | \
grep -Eo "inet addr:([0-9]{1,3}[\.]){3}[0-9]{1,3}" | \
grep -Eo "([0-9]{1,3}[\.]){3}[0-9]{1,3}"`

# start a process for each peer in background
n_peers=0
for ip in $ip_addrs; do
    peer_ids=`grep $ip $1 | cut -d " " -f 1`
    for peer in $peer_ids; do
        ./dummy_work.sh $peer $1 &
        let n_peers=n_peers+1
    done
done

# wait for command (q -- quit)
while [ -z "$cmd" ] || [ "$cmd" != q ]; do 
    read -p "Command (q--quit,i--info): " cmd
    case $cmd in
        quit|exit|q)
            cmd=q
            ;;
        info|i)
            echo "$n_peers peers in running on this machine!"
            ;;
    esac
done

# kill all child processes (peers) 
for pid in `pgrep -P$$`; do
    kill $pid
done
