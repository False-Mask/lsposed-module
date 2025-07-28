//
// Created by rose on 2025/1/30.
//

#ifndef LSPOSED_TOOLS_VIRLISTENER_H
#define LSPOSED_TOOLS_VIRLISTENER_H
#include "utils.h"
#include <stdint.h>
#include <functional>
#include <atomic>

namespace art {
    class SHARED_LOCKABLE MutatorMutex;
    class Mutex;

    class Locks {
    public:
        static Mutex* user_code_suspension_lock_;
        static Mutex* instrument_entrypoints_lock_ ACQUIRED_AFTER(user_code_suspension_lock_);
        static MutatorMutex* mutator_lock_ ACQUIRED_AFTER(instrument_entrypoints_lock_);
    };
    namespace mirror {
        class Class;

        class Object;

        class Throwable;
    }  // namespace mirror


    class ArtField;

    class ArtMethod;

    struct NthCallerVisitor;
    union JValue;

    class SHARED_LOCKABLE ReaderWriterMutex;

    class ShadowFrame;

    class Thread;

    enum class DeoptimizationMethodType;

    template <typename T>
    class Handle{};

    template <typename T>
    class MutableHandle;

    namespace instrumentation {

        using OptionalFrame = std::optional<std::reference_wrapper<const ShadowFrame>>;

        struct InstrumentationListener {
            InstrumentationListener() = default;

            virtual ~InstrumentationListener() = default;

            // Call-back for when a method is entered.
            virtual void MethodEntered(Thread *thread, ArtMethod *method)REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            virtual void MethodExited(Thread *thread,
                                      ArtMethod *method,
                                      OptionalFrame frame,
                                      Handle<mirror::Object> field_value) REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back for when a method is exited. The implementor should either handler-ize the return
            // value (if appropriate) or use the alternate MethodExited callback instead if they need to
            // go through a suspend point.
            virtual void MethodExited(Thread *thread,
                                      ArtMethod *method,
                                      OptionalFrame frame,
                                      JValue &return_value)
            REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back for when a method is popped due to an exception throw. A method will either cause a
            // MethodExited call-back or a MethodUnwind call-back when its activation is removed.
            virtual void MethodUnwind(Thread *thread,
                                      Handle<mirror::Object> this_object,
                                      ArtMethod *method,
                                      uint32_t dex_pc)
                                      REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back for when the dex pc moves in a method.
            virtual void DexPcMoved(Thread *thread,
                                    Handle<mirror::Object> this_object,
                                    ArtMethod *method,
                                    uint32_t new_dex_pc)
            REQUIRES_SHARED(Locks::mutator_lock_) = 0;

//            // Call-back for when we read from a field.
            virtual void FieldRead(Thread *thread,
                                   Handle<mirror::Object> this_object,
                                   ArtMethod *method,
                                   uint32_t dex_pc,
                                   ArtField *field) REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            virtual void FieldWritten(Thread *thread,
                                      Handle<mirror::Object> this_object,
                                      ArtMethod *method,
                                      uint32_t dex_pc,
                                      ArtField *field,
                                      Handle<mirror::Object> field_value) REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back for when we write into a field.
            virtual void FieldWritten(Thread *thread,
                                      Handle<mirror::Object> this_object,
                                      ArtMethod *method,
                                      uint32_t dex_pc,
                                      ArtField *field,
                                      const JValue &field_value) REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back when an exception is thrown.
            virtual void ExceptionThrown(Thread *thread,
                                         Handle<mirror::Throwable> exception_object)
            REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back when an exception is caught/handled by java code.
            virtual void
            ExceptionHandled(Thread *thread, Handle<mirror::Throwable> exception_object)
            REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back for when we execute a branch.
            virtual void Branch(Thread *thread,
                                ArtMethod *method,
                                uint32_t dex_pc,
                                int32_t dex_pc_offset)
            REQUIRES_SHARED(Locks::mutator_lock_) = 0;

            // Call-back when a shadow_frame with the needs_notify_pop_ boolean set is popped off the stack by
            // either return or exceptions. Normally instrumentation listeners should ensure that there are
            // shadow-frames by deoptimizing stacks.
            virtual void WatchedFramePop(Thread *thread ATTRIBUTE_UNUSED,
                                         const ShadowFrame &frame ATTRIBUTE_UNUSED)

            REQUIRES_SHARED(Locks::mutator_lock_) = 0;
        };
    }


}

#endif //LSPOSED_TOOLS_VIRLISTENER_H
