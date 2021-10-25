#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_executable(revng-abi-convert
  Main.cpp)

target_link_libraries(revng-abi-convert
  revngModel
  revngSupport
  ${LLVM_LIBRARIES})
