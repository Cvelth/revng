#
# This file is distributed under the MIT License. See LICENSE.md for details.
#

revng_add_library_internal(revngModelImporterDebugInfo SHARED DwarfImporter.cpp
                           PDBImporter.cpp)

set(DISASSEMBLER_ARCHITECTURE_SUPPORT "")
foreach(CURRENT_ARCHITECTURE AArch64 ARM Mips SystemZ X86)
  set(DISASSEMBLER_ARCHITECTURE_SUPPORT
      ${DISASSEMBLER_ARCHITECTURE_SUPPORT} "${CURRENT_ARCHITECTURE}Info"
      "${CURRENT_ARCHITECTURE}Desc")
endforeach(CURRENT_ARCHITECTURE)

llvm_map_components_to_libnames(
  LLVM_LIBRARIES
  Object
  Support
  DebugInfoDWARF
  DebugInfoCodeView
  DebugInfoMSF
  DebugInfoPDB
  BinaryFormat
  ${DISASSEMBLER_ARCHITECTURE_SUPPORT})

target_link_libraries(revngModelImporterDebugInfo revngSupport revngModel
                      revngModelPasses revngABI ${LLVM_LIBRARIES})
