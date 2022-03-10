#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_executable(revng-linkage-problem-reproducer
  ABIArtifactParser.cpp
  Main.cpp)

target_link_libraries(revng-linkage-problem-reproducer
  revngModelPasses
  ${LLVM_LIBRARIES})
