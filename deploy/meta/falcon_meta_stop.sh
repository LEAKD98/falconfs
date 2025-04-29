#!/bin/bash
set -euo pipefail

DIR=$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")
source "$DIR/falcon_meta_config.sh"

# Fast shutdown function
stop_falcon_cluster() {
    local path=$1

    # 1. Stop all Falcon background processes
    local falcon_pids=$(pgrep -f "postgres: falcon_.*_process" 2>/dev/null || true)
    if [ -n "$falcon_pids" ]; then
        echo "Stopping Falcon background processes (PIDs: $falcon_pids)"
        kill -9 $falcon_pids 2>/dev/null || true
    fi

    # 2. Stop PostgreSQL
    if [ -f "$path/postmaster.pid" ]; then
        echo "Stopping PostgreSQL: $path"
        pg_ctl stop -D "$path" -m immediate 2>/dev/null || true
    fi

    if [ -f "$path/postmaster.pid" ]; then
        echo "Force stopping PostgreSQL: $path"
        pkill -9 -f "postgres.*-D.*$path" 2>/dev/null || true
        rm -f "$path/postmaster.pid"
    fi

    # 3. Verify all processes are terminated
    sleep 0.5
    if pgrep -f "postgres.*-D.*$path" >/dev/null; then
        echo "Warning: Remaining processes detected, performing secondary cleanup..."
        pkill -9 -f "postgres.*-D.*$path" 2>/dev/null || true
    fi
}

# Main shutdown logic (parallel execution)
if [[ "$cnIp" == "$localIp" ]]; then
    stop_falcon_cluster "${cnPathPrefix}0" &
    rm -f "$DIR/cnlogfile0.log" &
fi

for ((n = 0; n < ${#workerIpList[@]}; n++)); do
    workerIp=${workerIpList[$n]}
    if [[ "$workerIp" == "$localIp" ]]; then
        for ((i = 0; i < ${workerNumList[$n]}; i++)); do
            stop_falcon_cluster "${workerPathPrefix}$i" &
            rm -f "$DIR/workerlogfile$i.log" &
        done
    fi
done

wait # Wait for all background tasks

# Final cleanup
clean_metadata_dirs
ipcs -m | awk '/^0x/{print $2}' | xargs -I {} ipcrm -m {} 2>/dev/null || true

echo "All Falcon services have been forcefully stopped and cleaned up"
exit 0
