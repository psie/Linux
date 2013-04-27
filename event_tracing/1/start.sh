#!/bin/bash

echo 1 > /sys/kernel/debug/tracing/events/sched/sched_process_fork/enable
