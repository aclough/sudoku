#!/bin/sh

# Turn off some kernel security to run flamegraph

set -e

echo 0 | sudo tee /proc/sys/kernel/kptr_restrict
echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid
