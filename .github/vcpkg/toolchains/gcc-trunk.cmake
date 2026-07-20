#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# Chainloaded by the x64-linux-gcc-trunk triplet: builds vcpkg ports with
# the same GCC trunk snapshot the GCC workflow's trunk leg uses for bit_set
# itself. The trunk snapshot bundles its own private libstdc++ (unlike
# apt-installed GCCs, which all share the system one), so anything vcpkg
# prebuilt against the system libstdc++ won't link against it - this
# triplet forces every dependency to be rebuilt from source with the same
# compiler instead.
set(CMAKE_C_COMPILER gcc-17)
set(CMAKE_CXX_COMPILER g++-17)
