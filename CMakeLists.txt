cmake_minimum_required(VERSION 3.22)
project(reef-src LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(CheckPIESupported)
check_pie_supported()
add_executable(login src/login/login.c)
add_executable(reef-authhelper src/authhelper/authhelper.c)
# The loader of Mac programs and its Mach broker (`reef --mach-broker`).
file(GLOB loader src/loader/*.cpp)
add_executable(reef ${loader})
# hdiutil: HFS+, ISO 9660, Joliet, UDF and UDIF images; LZFSE from upstream/.
find_package(ICU REQUIRED COMPONENTS uc)
find_package(LibLZMA REQUIRED)
find_package(ZLIB REQUIRED)
file(GLOB hdiutil src/hdiutil/*.cpp)
add_executable(reef-hdiutil ${hdiutil} upstream/lzfse/lzfse_encode.c upstream/lzfse/lzfse_encode_base.c upstream/lzfse/lzfse_fse.c
    upstream/lzfse/lzvn_decode_base.c upstream/lzfse/lzvn_encode_base.c upstream/lzfse/lzfse_decode.c upstream/lzfse/lzfse_decode_base.c)
target_link_libraries(reef-hdiutil PRIVATE ICU::uc LibLZMA::LibLZMA ZLIB::ZLIB)

# Needs the toolkit's headers (commit 7e25ddc) and libraries; left out without them.
set(TOOLKIT_SRC "" CACHE PATH "checkout of cutefish-toolkit")
set(TOOLKIT_LIB "" CACHE PATH "directory of libcutefish-toolkit*.so.1")
if(TOOLKIT_SRC AND TOOLKIT_LIB)
    find_package(OpenSSL REQUIRED)
    add_executable(reef-open src/open/main.cpp src/open/signature.cpp)
    target_include_directories(reef-open PRIVATE ${TOOLKIT_SRC}/src)
    target_link_libraries(reef-open PRIVATE ${TOOLKIT_LIB}/libcutefish-toolkit.so.1
        ${TOOLKIT_LIB}/libcutefish-toolkit-base.so.1 OpenSSL::Crypto)
    target_link_options(reef-open PRIVATE -Wl,--unresolved-symbols=ignore-in-shared-libs)
endif()
if(TOOLKIT_SRC AND TOOLKIT_LIB)
    add_executable(reef-unsupported src/unsupported/main.cpp)
    target_include_directories(reef-unsupported PRIVATE ${TOOLKIT_SRC}/src)
    target_link_libraries(reef-unsupported PRIVATE ${TOOLKIT_LIB}/libcutefish-toolkit.so.1
        ${TOOLKIT_LIB}/libcutefish-toolkit-base.so.1)
    target_link_options(reef-unsupported PRIVATE -Wl,--unresolved-symbols=ignore-in-shared-libs)
    find_package(LibXml2 REQUIRED)
    find_package(LibLZMA REQUIRED)
    find_package(ZLIB REQUIRED)
    add_executable(reef-install-package src/install-package/main.cpp src/install-package/package.cpp)
    target_include_directories(reef-install-package PRIVATE ${TOOLKIT_SRC}/src)
    target_link_libraries(reef-install-package PRIVATE ${TOOLKIT_LIB}/libcutefish-toolkit.so.1
        ${TOOLKIT_LIB}/libcutefish-toolkit-base.so.1 LibXml2::LibXml2 LibLZMA::LibLZMA ZLIB::ZLIB)
    target_link_options(reef-install-package PRIVATE -Wl,--unresolved-symbols=ignore-in-shared-libs)
    # The report of a machine's graphics; reef-gpu-probe, which it runs, is a program of the platform.
    file(GLOB diagnose src/diagnose/*.cpp)
    add_executable(reef-diagnose ${diagnose})
    target_compile_definitions(reef-diagnose PRIVATE REEF_VERSION="0.1")
    target_include_directories(reef-diagnose PRIVATE ${TOOLKIT_SRC}/src)
    target_link_libraries(reef-diagnose PRIVATE ${TOOLKIT_LIB}/libcutefish-toolkit.so.1
        ${TOOLKIT_LIB}/libcutefish-toolkit-base.so.1 EGL GLESv2)
    target_link_options(reef-diagnose PRIVATE -Wl,--unresolved-symbols=ignore-in-shared-libs)
    add_library(uishim SHARED tests/uishim.cpp)
    target_include_directories(uishim PRIVATE ${TOOLKIT_SRC}/src)
endif()

# The WebKit host is incomplete (no function table yet): an object to compare, not a library to use.
set(WEBKIT_INCLUDE "" CACHE PATH "include directory of cutefish-webkit")
if(WEBKIT_INCLUDE AND TOOLKIT_LIB)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Glib REQUIRED IMPORTED_TARGET glib-2.0)
    file(GLOB webkit_host src/webkit-host/*.cpp)
    add_library(reef-webkit-host SHARED ${webkit_host})
    set_target_properties(reef-webkit-host PROPERTIES CXX_VISIBILITY_PRESET hidden)
    target_include_directories(reef-webkit-host PRIVATE ${WEBKIT_INCLUDE})
    target_link_libraries(reef-webkit-host PRIVATE ${TOOLKIT_LIB}/libcutefish-webkit.so.2 gbm PkgConfig::Glib)
    target_link_options(reef-webkit-host PRIVATE -Wl,--unresolved-symbols=ignore-all)
endif()

# The AppKit host: everything AppKit asks for, through the table of reef_appkit_host_interface.
if(TOOLKIT_SRC AND TOOLKIT_LIB)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(AppkitHost REQUIRED IMPORTED_TARGET harfbuzz-subset harfbuzz fontconfig freetype2 zlib egl glesv2)
    file(GLOB appkit_host src/appkit-host/*.cpp)
    add_library(reef-appkit-host SHARED ${appkit_host})
    set_target_properties(reef-appkit-host PROPERTIES CXX_VISIBILITY_PRESET hidden)
    target_include_directories(reef-appkit-host PRIVATE ${TOOLKIT_SRC}/src)
    target_link_libraries(reef-appkit-host PRIVATE ${TOOLKIT_LIB}/libcutefish-toolkit.so.1
        ${TOOLKIT_LIB}/libcutefish-toolkit-base.so.1 PkgConfig::AppkitHost)
    target_link_options(reef-appkit-host PRIVATE -Wl,--no-undefined -Wl,--unresolved-symbols=ignore-in-shared-libs)
endif()

# The Metal translator: metallibs and AIR to GLSL, with LLVM 19 (Debian's libLLVM.so.19.1).
find_package(LLVM 19.1 CONFIG HINTS /usr/lib/llvm-19/lib/cmake/llvm)
if(LLVM_FOUND)
    file(GLOB metal_translator src/metal-translator/*.cpp)
    add_library(reef-metal-translator SHARED ${metal_translator})
    set_target_properties(reef-metal-translator PROPERTIES CXX_VISIBILITY_PRESET hidden)
    target_include_directories(reef-metal-translator SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    separate_arguments(llvm_definitions NATIVE_COMMAND "${LLVM_DEFINITIONS}")
    target_compile_definitions(reef-metal-translator PRIVATE ${llvm_definitions})
    target_link_libraries(reef-metal-translator PRIVATE ${LLVM_LIBRARY_DIR}/libLLVM.so.19.1)
    target_link_options(reef-metal-translator PRIVATE -Wl,--no-undefined)

    # The Metal Shading Language front end: source to AIR, with clang 19 (Debian's libclang-cpp.so.19.1).
    set(metal_msl ${CMAKE_CURRENT_SOURCE_DIR}/src/metal-msl)
    file(GLOB_RECURSE metal_msl_include CONFIGURE_DEPENDS ${metal_msl}/include/*)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/metal-msl/headers.cpp
        COMMAND ${CMAKE_COMMAND} -DINPUT=${metal_msl}/include -DOUTPUT=${CMAKE_CURRENT_BINARY_DIR}/metal-msl/headers.cpp
                -P ${metal_msl}/embed.cmake
        DEPENDS ${metal_msl}/embed.cmake ${metal_msl_include} VERBATIM)
    # In the order of the published library: the headers link between lower.cpp and source.cpp.
    add_library(reef-metal-msl SHARED ${metal_msl}/attributes.cpp ${metal_msl}/compile.cpp ${metal_msl}/entry_points.cpp
        ${metal_msl}/types.cpp ${metal_msl}/argument_buffers.cpp ${metal_msl}/lower.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/metal-msl/headers.cpp ${metal_msl}/source.cpp)
    set_target_properties(reef-metal-msl PROPERTIES CXX_STANDARD 20 CXX_VISIBILITY_PRESET hidden)
    target_include_directories(reef-metal-msl PRIVATE ${metal_msl})
    target_include_directories(reef-metal-msl SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
    target_compile_definitions(reef-metal-msl PRIVATE ${llvm_definitions})
    target_link_libraries(reef-metal-msl PRIVATE ${LLVM_LIBRARY_DIR}/libclang-cpp.so.19.1 ${LLVM_LIBRARY_DIR}/libLLVM.so.19.1)
    target_link_options(reef-metal-msl PRIVATE -Wl,--no-undefined)
endif()
