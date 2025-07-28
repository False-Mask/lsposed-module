//
// Created by rose on 2025/1/31.
//

#ifndef LSPOSED_TOOLS_MOCK_LISTENER_H
#define LSPOSED_TOOLS_MOCK_LISTENER_H

#include "virlistener.h"
#include "art_method.h"
#include "atrace.h"

namespace art {

    namespace mirror {
        class Class;
        class Object;
        class Throwable;
    }  // namespace mirror

    namespace instrumentation {

        struct MListener final : public InstrumentationListener {


        public:

            MListener() {

            }

            ~MListener() override = default;

            void MethodEntered(art::Thread *thread, art::ArtMethod *method) REQUIRES_SHARED(Locks::mutator_lock_) override {
                auto x = MyArtMethod::PrettyMethod(method, false);
                LOGE("MethodEnter:%s", x.c_str());
                ATrace::TraceBegin(x.c_str());
            }

            void MethodExited(Thread *thread,
                              ArtMethod *method,
                              OptionalFrame frame,
                              art::Handle<mirror::Object> field_value) REQUIRES_SHARED(Locks::mutator_lock_) override {
                auto x = MyArtMethod::PrettyMethod(method, false);
                LOGE("MethodExited1:%s",x.c_str());
                ATrace::TraceEnd();
            }

            void MethodExited(Thread *thread,
                              art::ArtMethod *method,
                              OptionalFrame frame,
                              JValue &return_value) REQUIRES_SHARED(Locks::mutator_lock_) override  {
                auto x = MyArtMethod::PrettyMethod(method, false);
                LOGE("MethodExited2:%s",x.c_str());
                ATrace::TraceEnd();
            }

            void MethodUnwind(Thread *thread,
                              Handle<mirror::Object> this_object,
                              art::ArtMethod *method,
                              uint32_t dex_pc) REQUIRES_SHARED(Locks::mutator_lock_) override {

            }

            void DexPcMoved(Thread *thread,
                            Handle<mirror::Object> this_object,
                            art::ArtMethod *method,
                            uint32_t new_dex_pc)  REQUIRES_SHARED(Locks::mutator_lock_) override {

            }

            void FieldRead(Thread *thread,
                           Handle<mirror::Object> this_object,
                           art::ArtMethod *method,
                           uint32_t dex_pc,
                           ArtField *field) REQUIRES_SHARED(Locks::mutator_lock_) override {

            }

            void FieldWritten(Thread *thread,
                              Handle<mirror::Object> this_object,
                              art::ArtMethod *method,
                              uint32_t dex_pc,
                              ArtField *field,
                              Handle<mirror::Object> field_value)  REQUIRES_SHARED(Locks::mutator_lock_) override {

            }

            void FieldWritten(Thread *thread,
                              Handle<mirror::Object> this_object,
                              art::ArtMethod *method,
                              uint32_t dex_pc,
                              ArtField *field,
                              const JValue &field_value) REQUIRES_SHARED(Locks::mutator_lock_) override {

            }


            void ExceptionThrown(Thread *thread,
                                 Handle<mirror::Throwable> exception_object) REQUIRES_SHARED(
                    Locks::mutator_lock_) override {

            }

            void ExceptionHandled(Thread *thread,
                                  Handle<mirror::Throwable> exception_object) REQUIRES_SHARED(
                    Locks::mutator_lock_) override {

            }

            void Branch(Thread *thread, art::ArtMethod *method, uint32_t dex_pc,
                        int32_t dex_pc_offset) REQUIRES_SHARED(Locks::mutator_lock_) override {

            }

            void WatchedFramePop(Thread *thread, const ShadowFrame &frame) REQUIRES_SHARED(
                    Locks::mutator_lock_) override {
                LOGE("WatchedFramePop");

            }

        };



    }
}

#endif //LSPOSED_TOOLS_MOCK_LISTENER_H
