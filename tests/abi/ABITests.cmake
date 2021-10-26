#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

macro(add_abi_tests ARCHITECTURE ABI_NAME)
  add_test(NAME "test_conversion_from_${ABI_NAME}"
           COMMAND sh -c "./bin/revng-abi-convert \
           --op=ToRaw --abi=${ABI_NAME} \
           ${CMAKE_SOURCE_DIR}/tests/abi/input/${ARCHITECTURE}.yml \
           | ./bin/revng-compare-models \
           ${CMAKE_SOURCE_DIR}/tests/abi/output/from/${ABI_NAME}.yml")
  set_tests_properties("test_conversion_from_${ABI_NAME}" PROPERTIES LABELS "abi")

  add_test(NAME "test_conversion_to_${ABI_NAME}"
           COMMAND sh -c "./bin/revng-abi-convert \
           --op=ToCABI --abi=${ABI_NAME} \
           ${CMAKE_SOURCE_DIR}/tests/abi/input/${ARCHITECTURE}.yml \
           | ./bin/revng-compare-models \
           ${CMAKE_SOURCE_DIR}/tests/abi/output/to/${ABI_NAME}.yml")
  set_tests_properties("test_conversion_to_${ABI_NAME}" PROPERTIES LABELS "abi")
endmacro()

add_abi_tests("x64" "SystemV_x86_64")
add_abi_tests("x64" "Microsoft_x64")
add_abi_tests("x86" "SystemV_x86")
