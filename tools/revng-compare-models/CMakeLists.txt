#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_executable(revng-compare-models
  Main.cpp)

target_link_libraries(revng-compare-models
  revngModel
  revngSupport
  ${LLVM_LIBRARIES})
