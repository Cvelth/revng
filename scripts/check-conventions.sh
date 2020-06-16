#!/bin/bash

#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

SCRIPT_PATH="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

FORMAT="0"

set -e

while [[ $# > 0 ]]; do
    key="$1"
    case $key in
        --force-format)
            FORMAT="1"
            shift # past argument
            ;;
        --*)
            echo "Unexpected option $key" > /dev/stderr
            echo > /dev/stderr
            echo "Usage: $0 [--force-format] [FILE...]" > /dev/stderr
            exit 1
            shift
            ;;
        *)
            break
            ;;
    esac
done

if [[ $# -eq 0 ]]; then
    FILES="$(git ls-files | grep -E '\.(c|cc|cpp|h|hpp)$')"
else
    FILES="$@"
fi

GREP="git grep -n --color=always"

# Run clang-format on FILES
function run_clang_format() {
    if test "$FORMAT" -gt 0; then
        clang-format -style=file -i $FILES
    else
        clang-format --dry-run -style=file -i $FILES
    fi
}

# Run revng-specific checks on files
function run_revng_checks() {
    # Check for lines longer than 80 columns
    $GREP -E '^.{81,}$' $FILES | cat

    # Things should never match
    for REGEXP in '\(--> 0\)' ';;' '^\s*->.*;$' 'Twine [^&]'; do
        $GREP "$REGEXP" $FILES | cat
    done

    # Things should never match (except in support.c)
    FILTERED_FILES="$(echo $FILES | sed 's|\bruntime/support\.c\b||g; s|\blib/Support/Assert\.cpp\b||g;')"
    for REGEXP in '\babort(' '\bassert(' 'assert(false' 'llvm_unreachable'; do
        $GREP "$REGEXP" $FILTERED_FILES | cat
    done

    # Things should never be at the end of a line
    for REGEXP in '::' '<' 'RegisterPass.*>' '} else' '\bopt\b.*>'; do
        $GREP "$REGEXP\$" $FILES | cat
    done

    # Parenthesis at the end of line (except for raw strings)
    $GREP "(\$" $FILES | grep -v 'R"LLVM.*(' | cat

    # Things should never be at the beginning of a line
    for REGEXP in '\.[^\.]' '\*>' '/[^/\*]' ':[^:\(]*)' '==' '\!=' '<[^<]' '>' '>=' '<=' '//\s*WIP' '#if\s*[01]'; do
        $GREP "^\s*$REGEXP" $FILES | cat
    done

    # Check there are no static functions in header files
    for FILE in $FILES; do
        if [[ $FILE == *h ]]; then
            $GREP -H '^static\b[^=]*$' "$FILE" | cat
            $GREP -HE '#ifndef\s*_.*_H\s*$' "$FILE" | cat
        fi
    done


}

# First, run the checks and output warnings in a human readable format.
run_clang_format
run_revng_checks | sort -u

# Second, re-run the check commands, and fail if any of them prints anything.
# This ensures that that user sees all the errors on the command line, while at
# the same time setting a proper exit value that can be checked by other scripts
# or by git hooks.
if [[ $( run_clang_format 2>&1 | head -c 1 | wc -c ) -ne 0 ]]; then
  exit 1
fi
if [[ $( run_revng_checks 2>&1 | head -c 1 | wc -c ) -ne 0 ]]; then
  exit 1
fi
