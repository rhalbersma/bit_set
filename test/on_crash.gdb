#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

# TEMPORARY, part of the MinGW Debug segfault investigation.
#
# Run the test, and dump state only if it did not exit normally. The guard is
# the whole point: gdb -batch exits non-zero when any command in its script
# errors, and that beats --return-child-result. An unguarded "bt" after a
# clean exit errors with "No stack.", so every passing test was reported to
# ctest as a failure - 18 of 27 on the first run, where only a handful are
# real. $_exitcode is void exactly when the inferior did not exit normally,
# which is the condition we want.

set confirm off
set pagination off

# Bound the unwind so a runaway recursion cannot make "bt -20" walk a hundred
# thousand frames before printing anything.
set backtrace limit 5000

run

if $_isvoid($_exitcode)
        printf "=== did not exit normally ===\n"

        # The faulting instruction plus the general registers. Between them
        # they give the address that was dereferenced, which says what kind of
        # corruption this is: a null base, a small offset off null, a poisoned
        # pointer, or an address that is merely unmapped.
        printf "=== faulting instruction ===\n"
        x/i $rip
        printf "=== registers ===\n"
        info registers
        printf "=== frame ===\n"
        info frame

        printf "=== innermost frames ===\n"
        bt 60
        printf "=== outermost frames ===\n"
        bt -20
end
