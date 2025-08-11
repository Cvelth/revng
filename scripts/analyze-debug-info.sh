#!/usr/bin/env bash

#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

set -euo pipefail

# example invocation: ./scripts/analyze-debug-info.sh \
#  emit-c \
#  ../../root/share/revng/test/tests/runtime \
# "calc-" \
#  ../log/debug-info

ARTIFACT="$1"
INPUT_PATH="$2"
FILTER="$3"
OUTPUT_PATH="$4"

rm -rf "$OUTPUT_PATH"
mkdir "$OUTPUT_PATH"

for FILE in "$INPUT_PATH"/**; do
  if [[ "$FILE" != *"-stripped-"* && "$FILE" != *"-with-debug-info-"* ]]; then
    continue
  fi

  if [[ -n $FILTER ]]; then
    if [[ "$FILE" != *"$FILTER"* ]]; then
      continue
    fi
  fi

  CURRENT_MODULE_PATH="$OUTPUT_PATH/modules/$(basename "$FILE")/"
  CURRENT_LOG_PATH="$OUTPUT_PATH/logs/$(basename "$FILE").log"
  CURRENT_MODEL_PATH="$OUTPUT_PATH/models/$(basename "$FILE").yml"
  CURRENT_OUTPUT_PATH="$OUTPUT_PATH/output/$(basename "$FILE").yml"
  CURRENT_LOST_PATH="$OUTPUT_PATH/lost/$(basename "$FILE").yml"
  CURRENT_VARIABLE_PATH="$OUTPUT_PATH/variables/$(basename "$FILE").yml"
  CURRENT_DISASSEMBLED_PATH="$OUTPUT_PATH/disassembled/$(basename "$FILE").S.tar.gz"
  CURRENT_DECOMPILED_PTML_PATH="$OUTPUT_PATH/decompiled/$(basename "$FILE").c.ptml"
  CURRENT_DECOMPILED_C_PATH="$OUTPUT_PATH/decompiled/$(basename "$FILE").c"

  mkdir -p "$CURRENT_MODULE_PATH"
  mkdir -p "$(dirname "$CURRENT_LOG_PATH")"
  mkdir -p "$(dirname "$CURRENT_MODEL_PATH")"
  mkdir -p "$(dirname "$CURRENT_OUTPUT_PATH")"
  mkdir -p "$(dirname "$CURRENT_LOST_PATH")"
  mkdir -p "$(dirname "$CURRENT_VARIABLE_PATH")"
  mkdir -p "$(dirname "$CURRENT_DISASSEMBLED_PATH")"
  mkdir -p "$(dirname "$CURRENT_DECOMPILED_PTML_PATH")"
  mkdir -p "$(dirname "$CURRENT_DECOMPILED_C_PATH")"

  cp -r ./share/revng/style "$OUTPUT_PATH/disassembled"

  {
    {
      echo
      echo ">>> Auto-analysis <<<"
      echo
      echo orc shell revng analyze revng-initial-auto-analysis "$FILE" \
        -o="$CURRENT_MODEL_PATH"
      echo
      orc shell revng analyze revng-initial-auto-analysis "$FILE" \
        -o="$CURRENT_MODEL_PATH"

      echo
      echo ">>> Dump modules <<<"
      echo
      echo orc shell revng artifact "$ARTIFACT" "$FILE" \
        --debug-info-instrumentation="$CURRENT_MODULE_PATH" \
        -m="$CURRENT_MODEL_PATH" \
        -o=/dev/null
      echo
      orc shell revng artifact "$ARTIFACT" "$FILE" \
        --debug-info-instrumentation="$CURRENT_MODULE_PATH" \
        -m="$CURRENT_MODEL_PATH" \
        -o=/dev/null

      echo
      echo ">>> Analyze modules <<<"
      echo
      echo orc shell revng analyze-debug-info "$CURRENT_MODULE_PATH" \
        --output="$CURRENT_OUTPUT_PATH" \
        --lost="$CURRENT_LOST_PATH" \
        --variables="$CURRENT_VARIABLE_PATH" || true
      echo
      orc shell revng analyze-debug-info "$CURRENT_MODULE_PATH" \
        --output="$CURRENT_OUTPUT_PATH" \
        --lost="$CURRENT_LOST_PATH" \
        --variables="$CURRENT_VARIABLE_PATH" || true

      echo
      echo ">>> Produce decompiled code <<<"
      echo
      echo orc shell revng artifact decompile-to-single-file "$FILE" \
        -m="$CURRENT_MODEL_PATH" \
        -o="$CURRENT_DECOMPILED_PTML_PATH"
      echo
      orc shell revng artifact decompile-to-single-file "$FILE" \
        -m="$CURRENT_MODEL_PATH" \
        -o="$CURRENT_DECOMPILED_PTML_PATH"

      echo orc shell revng ptml \
        "$CURRENT_DECOMPILED_PTML_PATH" \
        -o="$CURRENT_DECOMPILED_C_PATH"
      echo
      orc shell revng ptml \
        "$CURRENT_DECOMPILED_PTML_PATH" \
        -o="$CURRENT_DECOMPILED_C_PATH"

      echo
      echo ">>> Produce marked up assembly <<<"
      echo
      echo orc shell revng artifact disassemble "$FILE" \
        -m="$CURRENT_MODEL_PATH" \
        --lost-addresses="$CURRENT_LOST_PATH" \
        -o="$CURRENT_DISASSEMBLED_PATH"
      echo
      orc shell revng artifact disassemble "$FILE" \
        -m="$CURRENT_MODEL_PATH" \
        --lost-addresses="$CURRENT_LOST_PATH" \
        -o="$CURRENT_DISASSEMBLED_PATH"

      {
        cat <<'EOF'
<!DOCTYPE html>
<html lang="en-US"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"><title>Assembly emitted by revng for 
EOF
        basename "$FILE"
        cat <<'EOF'
</title><link rel="apple-touch-icon" sizes="180x180" href="https://rev.ng/favicon/apple-touch-icon.png"><link rel="icon" type="image/png" sizes="32x32" href="https://rev.ng/favicon/favicon-32x32.png"><link rel="icon" type="image/png" sizes="16x16" href="https://rev.ng/favicon/favicon-16x16.png"><link href="./style/assembly.css" rel="stylesheet"></head><body xmlns="http://www.w3.org/1999/xhtml">
EOF

        tar --use-compress-program=unzstd -xO -f "$CURRENT_DISASSEMBLED_PATH"

        cat <<'EOF'
</body></html>
EOF
      } > "$CURRENT_DISASSEMBLED_PATH".html

    echo
    echo ">>> Agglomerate data <<<"
    echo
    echo ./scripts/agglomerate-debug-info.py "$OUTPUT_PATH"
    echo
    ./scripts/agglomerate-debug-info.py "$OUTPUT_PATH"

    } &>> "$CURRENT_LOG_PATH"

    echo "$FILE"

  } &

done

wait
