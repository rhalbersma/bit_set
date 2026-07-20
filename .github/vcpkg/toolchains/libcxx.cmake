#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Chainloaded by the x64-linux-libcxx triplet: builds vcpkg ports with the
# same Clang + libc++ combination the Clang workflow's libc++ leg uses for
# bit_set itself. Bump the compiler version together with the clang.yml
# matrix.
set(CMAKE_C_COMPILER clang-23)
set(CMAKE_CXX_COMPILER clang++-23)
set(CMAKE_CXX_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-stdlib=libc++")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-stdlib=libc++")
