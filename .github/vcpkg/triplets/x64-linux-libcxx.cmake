#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# x64-linux, but built with Clang against libc++ instead of the default
# compiler against libstdc++. Used by the libc++ leg of the Clang workflow
# so that vcpkg's Boost.Test and the bit_set tests share one standard
# library.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../toolchains/libcxx.cmake)
