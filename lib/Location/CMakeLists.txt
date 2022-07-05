#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_analyses_library_internal(revngLocation SHARED Empty.cpp)

target_link_libraries(revngLocation revngModel revngSupport)

set(INTERNAL_ASSEMBLY_HEADERS "${CMAKE_SOURCE_DIR}/include/revng/Location")
target_tuple_tree_generator(
  revngLocation
  INSTALL
  HEADER_DIRECTORY
  "Location"
  NAMESPACE
  "revng::location"
  SCHEMA_PATH
  "${CMAKE_BINARY_DIR}/location-internal-schema.yml"
  SEPARATE_STRING_TYPES
  "MetaAddress"
  "model::Type::Key"
  HEADERS
  "${INTERNAL_ASSEMBLY_HEADERS}/Kind.h")
