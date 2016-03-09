#!/bin/bash

# Usage
if [ $# -lt 1 ]; then
    echo Usage: asterix.sh GRAPH 
    exit
fi

# Get all valid ip addresses (including local loopback)
# ip address can be put in ip.conf, otherwise will be looked up with ifconfig
# This script will pick the peers that have ip addresses associated with 
# the current machine and deploy them
# Note: 127.0.0.1 will be picked up by ifconfig
if [ -f ip.conf ]; then
    ip_addrs=`cat ip.conf`
else
    ip_addrs=`ifconfig  | \
    grep -Eo "inet addr:([0-9]{1,3}[\.]){3}[0-9]{1,3}" | \
    grep -Eo "([0-9]{1,3}[\.]){3}[0-9]{1,3}"`
fi

# logs are saved in logs/log.xxx/peer.yyy
# xxx is an incremental index for sessions, yyy indicates peer id
# summary including all logs of a specific session will be saved at logs/log.xxx/summary
if [ ! -d logs ]; then
    mkdir logs
fi
exp_id=$((`ls logs | egrep -o '[0-9]{3}' | sort -n | tail -n 1 | sed 's/^0*//'`+1))
log_dir=`printf 'logs/log.%03d' $exp_id`
mkdir $log_dir

# Start a process for each peer in background
n_peers=0
n_sellers=0
n_buyers=0
peer_ids=
peer_types=
for ip in $ip_addrs; do
    # parse peer id from configuration file
    curr_peer_ids=`grep $ip $1 | cut -d " " -f 1`
    for peer in $curr_peer_ids; do
        # randomly assigns type: seller/buyer
        is_seller=$(($RANDOM % 2))
#        ./dummy_work.sh $peer $1 $is_seller > `printf '%s/peer.%03d' $log_dir $peer` &
        ./run_peer $peer $1 $is_seller > `printf '%s/peer.stdout.%03d' $log_dir $peer` \
            2> `printf '%s/peer.stderr.%03d' $log_dir $peer`&
        peer_types[$n_peers]=$is_seller
        peer_ids[$n_peers]=$peer
        let n_peers=n_peers+1
        let n_sellers=n_sellers+is_seller
        let n_buyers=n_buyers+1-is_seller
    done
done

# trap EXIT and call kill_peers()
trap kill_peers EXIT
peer_pids=`pgrep -P$$`
function kill_peers() {
    # kill and wait for all child processes (peers) 
    for pid in $peer_pids; do
        kill $pid
        wait $pid
    done
    # create summary log by concatenating all peer logs
    cat $log_dir/peer.stdout.* > $log_dir/summary
    cat $log_dir/peer.stderr.* > $log_dir/summary.err
    cat logs/peer_time_* > $log_dir/summary.time
    rm -r logs/peer_time_*
    exit
}

# wait for command (q -- quit,i--info,l--list)
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
            # display a list of all peers
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

