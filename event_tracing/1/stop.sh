#!/bin/bash

echo 0 > /sys/kernel/debug/tracing/events/sched/sched_process_fork/enable
