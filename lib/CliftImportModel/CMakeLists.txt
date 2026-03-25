#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_library_internal(revngCliftImportModel SHARED ImportModelInfo.cpp
                           ImportTypes.cpp ModelVerify.cpp)

target_link_libraries(revngCliftImportModel revngClift revngModel)
