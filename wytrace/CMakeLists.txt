# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html
project(perfetto-tracer)
set(TARGET perfetto-tracer)
find_package(Threads)

#启用汇编
enable_language(C ASM)
#add_library(perfetto STATIC perfetto/perfetto.cc)
#include_directories(perfetto)
# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.13)
set(CMAKE_MESSAGE_LOG_LEVEL TRACE)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
#execute_process(COMMAND rm ${CMAKE_CURRENT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/main/libs/${ANDROID_ABI}/)
find_package(shadowhook REQUIRED CONFIG)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -v")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp -O0 -g -fPIC -v")
set(ARCH_LINK_FLAGS "-Wl,-z,max-page-size=16384")
message(WARNING "asm flags: ${CMAKE_ASM_FLAGS}")
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/arch/arm64.S PROPERTIES COMPILE_FLAGS "-fPIC")

# include 其他.h问卷
include_directories(src/main/cpp)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/jvmti.h
)
file(GLOB_RECURSE FASTER_DIRS "${CMAKE_SOURCE_DIR}/src/main/cpp/faster/*.cpp")
# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        ${TARGET}
        # Sets the library as a shared library.
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/arch/arm64.S
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/art_method_trace.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/jvmti_agent.cpp
        ${FASTER_DIRS}
        )

#add_compile_options(-fPIC)
#target_compile_options(${TARGET} PRIVATE
#        $<$<COMPILE_LANGUAGE:ASM>:-Wa,-fPIC>
#        $<$<COMPILE_LANGUAGE:ASM_NASM>:/D_PIC>
#)
#set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
#set_source_files_properties(*.S PROPERTIES
#        COMPILE_FLAGS "-fPIC"
#)



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ${TARGET}
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        shadowhook::shadowhook)

