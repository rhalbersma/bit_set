#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# x64-linux, but built with the pinned GCC trunk snapshot instead of
# whatever ABI-stable compiler built vcpkg's ordinary prebuilt binaries.
# Used by the trunk leg of the GCC workflow so every dependency shares one
# (trunk) libstdc++ with the bit_set tests themselves.
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../toolchains/gcc-trunk.cmake)
