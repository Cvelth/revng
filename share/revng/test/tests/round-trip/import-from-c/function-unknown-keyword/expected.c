//
// This file is distributed under the MIT License. See LICENSE.md for details.
//

/// We should silently ignore most doxygen keywords, like \ref whatever.
///
/// \c code_text \b bold_text \e em_text
///
/// <b>bold</b> <i>italic</i> <a href="http://example.com">link</a>
///
/// \note This is a note.
/// \warning This is a warning.
/// \since 1.0
/// \deprecated Use new_function instead.
/// \see related_function
/// \todo Fix this.
///
/// \code
/// int foo() {
///     return 42;
/// }
/// \endcode
///
/// \page a
/// \tableofcontents
/// \section a1
/// \section a2
/// \subsection a2.1
_ABI(SystemV_x86_64) int64_t doxygen_test(int64_t arg1);

