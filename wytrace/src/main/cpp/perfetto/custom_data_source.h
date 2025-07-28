//
// Created by rose on 2025/8/3.
//

#ifndef LSPOSED_TOOLS_CUSTOM_DATA_SOURCE_H
#define LSPOSED_TOOLS_CUSTOM_DATA_SOURCE_H

#include "perfetto.h"

class CustomDataSource : public perfetto::DataSource<CustomDataSource> {
public:
    void OnSetup(const SetupArgs&) override {
        // Use this callback to apply any custom configuration to your data source
        // based on the TraceConfig in SetupArgs.
    }

    void OnStart(const StartArgs&) override {
        // This notification can be used to initialize the GPU driver, enable
        // counters, etc. StartArgs will contains the DataSourceDescriptor,
        // which can be extended.
    }

    void OnStop(const StopArgs&) override {
        // Undo any initialization done in OnStart.
    }

    // Data sources can also have per-instance state.
    int my_custom_state = 0;
};

PERFETTO_DECLARE_DATA_SOURCE_STATIC_MEMBERS(CustomDataSource);


#endif //LSPOSED_TOOLS_CUSTOM_DATA_SOURCE_H
