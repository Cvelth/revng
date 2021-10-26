#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_executable(revng-abi-convert
  Convert.cpp
  Main.cpp)

target_link_libraries(revng-abi-convert
  revngModel
  revngStackAnalysis
  revngSupport
  ${LLVM_LIBRARIES})
