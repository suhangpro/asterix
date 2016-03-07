#!/bin/bash

# get all valid ip addresses (including local loopback)
ip_addrs=`ifconfig  | \
grep -Eo "inet addr:([0-9]{1,3}[\.]){3}[0-9]{1,3}" | \
grep -Eo "([0-9]{1,3}[\.]){3}[0-9]{1,3}"`

# log folders
if [ ! -d logs ]; then
    mkdir logs
fi
exp_id=$((`ls logs | egrep -o '[0-9]{3}' | sort -n | tail -n 1`+1))
log_dir=`printf 'logs/log.%03d' $exp_id`
mkdir $log_dir

# start a process for each peer in background
n_peers=0
n_sellers=0
n_buyers=0
peer_ids=
peer_types=
for ip in $ip_addrs; do
    curr_peer_ids=`grep $ip $1 | cut -d " " -f 1`
    for peer in $curr_peer_ids; do
        is_seller=$(($RANDOM % 2))
        ./dummy_work.sh $peer $1 $is_seller > `printf '%s/peer.%03d' $log_dir $peer` &
        peer_types[$n_peers]=$is_seller
        peer_ids[$n_peers]=$peer
        let n_peers=n_peers+1
        let n_sellers=n_sellers+is_seller
        let n_buyers=n_buyers+1-is_seller
    done
done

# trap ctrl-c and call kill_peers()
trap kill_peers INT
peer_pids=`pgrep -P$$`
function kill_peers() {
    # kill all child processes (peers) 
    for pid in $peer_pids; do
        kill $pid
        wait $pid
    done
    cat $log_dir/peer.* > $log_dir/summary
    exit
}

# wait for command (q -- quit)
while [ -z "$cmd" ] || [ "$cmd" != q ]; do 
    read -p "Command (q--quit,i--info,l--list): " cmd
    case $cmd in
        quit|exit|q)
            cmd=q
            ;;
        info|i)
            echo "$n_peers peers ($n_sellers sellers & $n_buyers buyers) are running on this machine."
            ;;
        list|l)
            echo -e "PEER_ID\tPEER_TYPE"
            for idx in `seq 0 $((n_peers-1))`; do
                echo -ne "${peer_ids[$idx]} \t"
                if [[ ${peer_types[$idx]} -gt 0 ]]; then
                    echo "seller"
                else
                    echo "buyer"
                fi
            done
            ;;
    esac
done

# stop all peers
kill_peers

