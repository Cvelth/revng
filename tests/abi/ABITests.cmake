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

add_abi_tests("x86_64" "SystemV_x86_64")
add_abi_tests("x86_64" "Microsoft_x64")
add_abi_tests("x86_64" "Microsoft_x64_vectorcall")
add_abi_tests("x86_64" "Microsoft_x64_clrcall")

add_abi_tests("x86" "SystemV_x86")
add_abi_tests("x86" "SystemV_x86_regparm_3")
add_abi_tests("x86" "SystemV_x86_regparm_2")
add_abi_tests("x86" "SystemV_x86_regparm_1")
add_abi_tests("x86" "Microsoft_x86_cdecl")
add_abi_tests("x86" "Microsoft_x86_stdcall")
add_abi_tests("x86" "Microsoft_x86_thiscall")
add_abi_tests("x86" "Microsoft_x86_fastcall")
add_abi_tests("x86" "Microsoft_x86_clrcall")
add_abi_tests("x86" "Microsoft_x86_vectorcall")

add_abi_tests("aarch64" "Aarch64")
add_abi_tests("arm" "Aarch32")

add_abi_tests("mips" "MIPS_o32")

add_abi_tests("systemz" "SystemZ_s390x")

# More architectures and ABIs are to come.
