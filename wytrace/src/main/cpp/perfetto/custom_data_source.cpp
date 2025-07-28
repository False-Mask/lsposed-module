//
// Created by rose on 2025/8/3.
//

#include "custom_data_source.h"


int trace() {
    CustomDataSource::TraceWithInstances()
    CustomDataSource::Trace([](CustomDataSource::TraceContext ctx) {
        auto pkg = ctx.NewTracePacket();
        pkg->set_track_event()->set_type();
        pkg.
    });

}