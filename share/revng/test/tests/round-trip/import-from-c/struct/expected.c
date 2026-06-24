//
// This file is distributed under the MIT License. See LICENSE.md for details.
//


/// This is a struct
struct _PACKED _SIZE(40) my_struct {
  
  /// First field with a comment.
  uint64_t first_field _STARTS_AT(0);
  uint64_t second_field _STARTS_AT(8);
  
  /// Third field with a comment.
  uint32_t third_field _STARTS_AT(16);
  uint64_t fourth_field _STARTS_AT(24);
  
  /// Fifth field with a comment.
  uint32_t fifth_field _STARTS_AT(32);
};


