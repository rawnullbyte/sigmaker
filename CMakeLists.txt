cmake_minimum_required(VERSION 3.20)
project(SigMaker VERSION 1.0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# IDA SDK path - override with -DIDA_SDK_PATH=/path/to/sdk
if(NOT DEFINED IDA_SDK_PATH)
    set(IDA_SDK_PATH "${CMAKE_SOURCE_DIR}/../SDK" CACHE PATH "Path to IDA Pro SDK")
endif()

# Source files
set(SOURCES
    "IDA Pro SigMaker/Main.cpp"
    "IDA Pro SigMaker/Plugin.cpp"
    "IDA Pro SigMaker/SignatureUtils.cpp"
    "IDA Pro SigMaker/Utils.cpp"
)

set(HEADERS
    "IDA Pro SigMaker/Main.h"
    "IDA Pro SigMaker/Plugin.h"
    "IDA Pro SigMaker/SignatureUtils.h"
    "IDA Pro SigMaker/Utils.h"
    "IDA Pro SigMaker/Version.h"
)

# Shared library (plugin)
add_library(sigmaker SHARED ${SOURCES} ${HEADERS})

target_include_directories(sigmaker PRIVATE
    "${IDA_SDK_PATH}/src/include"
    "${CMAKE_SOURCE_DIR}/IDA Pro SigMaker"
)

target_compile_definitions(sigmaker PRIVATE
    NOMINMAX
)

if(WIN32)
    target_compile_definitions(sigmaker PRIVATE __NT__)
else()
    target_compile_definitions(sigmaker PRIVATE __LINUX__)
endif()

target_compile_options(sigmaker PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W3 /MP /GS- /guard:cf- /Zc:__cplusplus>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wno-unused-parameter>
)

# 64-bit build: add __EA64__
option(IDA_EA64 "Build for IDA 64-bit (EA64)" ON)
if(IDA_EA64)
    target_compile_definitions(sigmaker PRIVATE __EA64__)
endif()

# Linker settings
if(MSVC)
    set_target_properties(sigmaker PROPERTIES
        OUTPUT_NAME "SigMaker64"
        SUFFIX ".dll"
        LINK_FLAGS "/EXPORT:PLUGIN"
    )

    if(IDA_EA64)
        target_link_directories(sigmaker PRIVATE "${IDA_SDK_PATH}/src/lib/x64_win_64")
    else()
        target_link_directories(sigmaker PRIVATE "${IDA_SDK_PATH}/src/lib/x64_win_32")
    endif()

    target_link_libraries(sigmaker PRIVATE ida)
else()
    set_target_properties(sigmaker PROPERTIES
        OUTPUT_NAME "SigMaker64"
        SUFFIX ".so"
        PREFIX ""
    )

    # Linux: link against IDA shared library and SDK static libs
    target_link_directories(sigmaker PRIVATE "${IDA_SDK_PATH}/src/lib/x64_linux_64")
    target_link_libraries(sigmaker PRIVATE ida pthread dl m)

    target_link_options(sigmaker PRIVATE "-Wl,--export-dynamic")
endif()
