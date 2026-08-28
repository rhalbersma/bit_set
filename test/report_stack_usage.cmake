#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# TEMPORARY diagnostic, part of the MinGW Debug segfault investigation.
# Reports the largest stack frames GCC emitted for one target, by reading the
# .su files that -fstack-usage drops next to each object file.
#
# Windows reserves 2 MiB of stack for the main thread (Linux gives 8 MiB), and
# GCC probes the guard page through ___chkstk_ms on entry to any function whose
# frame exceeds a page. A single frame larger than the reserve therefore faults
# instantly, on entry, before the function body runs - which is what a 0.01 s
# "SEGFAULT" looks like. This script says whether any such frame exists without
# running anything.
#
# Usage:
#   cmake -DSU_DIR=<object dir> -DSU_LABEL=<target> -DSU_TOP=<n> -P report_stack_usage.cmake

if(NOT DEFINED SU_TOP)
    set(SU_TOP 10)
endif()

file(GLOB_RECURSE su_files "${SU_DIR}/*.su")
if(NOT su_files)
    return()
endif()

# Sort on a zero-padded key so that CMake's lexicographic list(SORT) orders by
# magnitude; strip the key again when printing.
set(entries "")
foreach(su ${su_files})
    file(STRINGS "${su}" lines)
    foreach(line ${lines})
        # <file>:<line>:<col>:<signature>\t<bytes>\t<qualifier>
        if(line MATCHES "^(.+)\t([0-9]+)\t([a-z,]+)$")
            set(signature "${CMAKE_MATCH_1}")
            set(bytes     "${CMAKE_MATCH_2}")
            set(qualifier "${CMAKE_MATCH_3}")
            string(LENGTH "${bytes}" width)
            math(EXPR padding "12 - ${width}")
            string(REPEAT "0" ${padding} zeroes)
            list(APPEND entries "${zeroes}${bytes}|${bytes} bytes (${qualifier})  ${signature}")
        endif()
    endforeach()
endforeach()

if(NOT entries)
    return()
endif()

list(SORT entries ORDER DESCENDING)
list(LENGTH entries total)

message(STATUS "stack-usage ${SU_LABEL}: ${total} frames, largest ${SU_TOP}:")
set(shown 0)
foreach(entry ${entries})
    if(shown GREATER_EQUAL SU_TOP)
        break()
    endif()
    string(REGEX REPLACE "^[0-9]+\\|" "" pretty "${entry}")
    message(STATUS "  ${pretty}")
    math(EXPR shown "${shown} + 1")
endforeach()
