adb shell perfetto \
  -c - --txt \
  -o /data/misc/perfetto-traces/trace \
<<EOF

buffers: {
    size_kb: 634880
    fill_policy: DISCARD
}
buffers: {
    size_kb: 2048
    fill_policy: DISCARD
}
data_sources: {
    config {
        name: "linux.process_stats"
        target_buffer: 1
        process_stats_config {
            scan_all_processes_on_start: true
        }
    }
}
data_sources: {
    config {
        name: "linux.sys_stats"
        sys_stats_config {
            stat_period_ms: 1000
            stat_counters: STAT_CPU_TIMES
            stat_counters: STAT_FORK_COUNT
        }
    }
}
data_sources: {
    config {
        name: "linux.ftrace"
        ftrace_config {
            ftrace_events: "sched/sched_switch"
            ftrace_events: "power/suspend_resume"
            ftrace_events: "sched/sched_wakeup"
            ftrace_events: "sched/sched_wakeup_new"
            ftrace_events: "sched/sched_waking"
            ftrace_events: "power/cpu_frequency"
            ftrace_events: "power/cpu_idle"
            ftrace_events: "power/gpu_frequency"
            ftrace_events: "raw_syscalls/sys_enter"
            ftrace_events: "raw_syscalls/sys_exit"
            ftrace_events: "sched/sched_process_exit"
            ftrace_events: "sched/sched_process_free"
            ftrace_events: "task/task_newtask"
            ftrace_events: "task/task_rename"
            ftrace_events: "mm_event/mm_event_record"
            ftrace_events: "kmem/rss_stat"
            ftrace_events: "ion/ion_stat"
            ftrace_events: "dmabuf_heap/dma_heap_stat"
            ftrace_events: "kmem/ion_heap_grow"
            ftrace_events: "kmem/ion_heap_shrink"
            ftrace_events: "ftrace/print"
            atrace_categories: "gfx"
            atrace_categories: "input"
            atrace_categories: "view"
            atrace_categories: "wm"
            atrace_categories: "am"
            atrace_categories: "res"
            atrace_categories: "dalvik"
            atrace_categories: "bionic"
            atrace_apps: "com.example.hooktester"
        }
    }
}


duration_ms: 15000

EOF
