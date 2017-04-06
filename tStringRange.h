//
// You received this file as part of RRLib
// Robotics Research Library
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    rrlib/uri/tStringRange.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-29
 *
 * \brief   Contains tStringRange
 *
 * \b tStringRange
 *
 * Efficiently stores a substring from another string.
 * As it references the original string, it is only valid as long the original string is not modified.
 * Notably string ranges only provide read-only access to the referenced string.
 */
//----------------------------------------------------------------------
#ifndef __rrlib__uri__tStringRange_h__
#define __rrlib__uri__tStringRange_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace rrlib
{
namespace uri
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! String Range
/*!
 * Efficiently stores a substring from another string.
 * As it references the original string, it is only valid as long the original string is not modified.
 * Notably string ranges only provide read-only access to the referenced string.
 */
class tStringRange
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tStringRange() : begin_char(nullptr), length(0)
  {}

  tStringRange(const char* null_terminated_string) : begin_char(null_terminated_string), length(strlen(null_terminated_string))
  {}

  tStringRange(const char* begin_char, size_t length) : begin_char(begin_char), length(length)
  {}

  tStringRange(const std::string& s) : begin_char(s.c_str()), length(s.length())
  {}

  /*!
   * \return Pointer to beginning of string
   */
  const char* CharPointer() const
  {
    return begin_char;
  }

  /*!
   * \return Length of string
   */
  size_t Length() const
  {
    return length;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  const char* begin_char;  //!< Offset to first character of string representation
  size_t length;           //!< Length of string
};

inline bool operator==(const tStringRange& lhs, const tStringRange& rhs)
{
  return lhs.Length() == rhs.Length() && memcmp(lhs.CharPointer(), rhs.CharPointer(), lhs.Length()) == 0;
}

inline bool operator!=(const tStringRange& lhs, const tStringRange& rhs)
{
  return !(lhs == rhs);
}

inline serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tStringRange& string_range)
{
  stream.Write(string_range.CharPointer(), string_range.Length());
  stream.WriteByte(0);
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
