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

data_sources {
  config {
    name: "track_event"
    track_event_config {
        enabled_categories: "perfetto-tracer"
        disabled_categories: "*"
    }
  }
}


duration_ms: 5000

EOF
