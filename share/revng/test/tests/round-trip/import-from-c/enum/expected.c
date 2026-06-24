//
// This file is distributed under the MIT License. See LICENSE.md for details.
//


/// This is an enum!
enum _ENUM_UNDERLYING(uint32_t) _PACKED my_enum {
  
  /// This one has a comment,
  none = 0x0U,
  
  /// So does this one!
  ///
  /// But none of the following ones do!
  positive = 0x1U,
  negative = 0x2U,
  neutral = 0x3U,
  enum_max_value_my_enum = 0xFFFFFFFFU,
};


