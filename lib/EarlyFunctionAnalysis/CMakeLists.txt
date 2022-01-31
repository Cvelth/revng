#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

add_subdirectory(ABIAnalyses)

# Header files containing efa YAML must be added here
set(EFA_HEADERS_DIR "${CMAKE_SOURCE_DIR}/include/revng/EarlyFunctionAnalysis")
set(EFA_HEADERS
  "${EFA_HEADERS_DIR}/BasicBlock.h"
  "${EFA_HEADERS_DIR}/CallEdge.h"
  "${EFA_HEADERS_DIR}/FunctionEdge.h"
  "${EFA_HEADERS_DIR}/FunctionEdgeBase.h"
  "${EFA_HEADERS_DIR}/FunctionEdgeType.h"
  "${EFA_HEADERS_DIR}/FunctionMetadata.h"
)

# Generate C++ headers from the collected YAML
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/include/revng/EarlyFunctionAnalysis/Generated")
set(SCHEMA_PATH "${CMAKE_BINARY_DIR}/efa-schema.yml")
tuple_tree_generator(
  generate-efa-tuple-tree-code
  "${EFA_HEADERS}"
  TUPLE-TREE-YAML
  efa
  "${SCHEMA_PATH}"
  "${OUTPUT_DIR}"
  "revng/EarlyFunctionAnalysis"
  GENERATED_HEADERS
  GENERATED_IMPLS
  ""
  ""
  ""
  "MetaAddress"
)

install(
    DIRECTORY ${OUTPUT_DIR}
    DESTINATION include/revng/EarlyFunctionAnalysis
)

# Define revngEarlyFunctionAnalysis library
revng_add_analyses_library_internal(revngEarlyFunctionAnalysis
  AAWriterPass.cpp
  ABIDetectionPass.cpp
  IndirectBranchInfoPrinterPass.cpp
  PromoteGlobalToLocalVars.cpp
  SegregateDirectStackAccesses.cpp
  EarlyFunctionAnalysis.cpp
  CollectFunctionsFromCalleesPass.cpp
  CollectFunctionsFromUnusedAddressesPass.cpp
  FunctionMetadata.cpp
  ${GENERATED_IMPLS}
)

llvm_map_components_to_libnames(LLVM_LIBRARIES Analysis)

target_link_libraries(revngEarlyFunctionAnalysis
  revngBasicAnalyses
  revngABI
  revngABIAnalyses
  revngSupport
  revngModel
  ${LLVM_LIBRARIES})

add_dependencies(revngEarlyFunctionAnalysis generate-efa-tuple-tree-code)
