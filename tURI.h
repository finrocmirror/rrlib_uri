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
/*!\file    rrlib/uri/tURI.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-29
 *
 * \brief   Contains tURI
 *
 * \b tURI
 *
 * This class wraps a RFC 3986 URI.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__uri__tURI_h__
#define __rrlib__uri__tURI_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/uri/tURIElements.h"

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
//! URI
/*!
 * This class wraps a RFC 3986 URI.
 */
class tURI
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  static const char* cUNENCODED_RESERVED_CHARACTERS_PATH;  // !$&'()*+,;=:@

  /*! URI string */
  tURI(const std::string uri = std::string()) :
    uri(uri)
  {}

  /*! Creates local URI from path */
  tURI(const tPath& path, const char* unencoded_reserved_characters = cUNENCODED_RESERVED_CHARACTERS_PATH);

  /*!
   * Converts percent-encoded string to decoded string
   *
   * \param decode_buffer Buffer for decoded string. As no character could be percent-encoded, should have a size == encoded.Length()
   * \param encoded Percent-encoded string
   * \return Pointer to character after the last character written in decode_buffer (notably string in decode_buffer is not null-terminated)
   * \throws std::invalid_argument if string cannot be decoded
   */
  static char* Decode(char* decode_buffer, const tStringRange& encoded);

  /*!
   * Converts decoded string to percent-encoded string
   *
   * \param encode_buffer Buffer for percent-encoded string. As all characters could be percent-encoded, should have a size >= 3 * decoded.Length()
   * \param decoded String to encode
   * \param unencoded_reserved_characters Reserved characters not to encode (see constants above)
   * \return Pointer to character after the last character written in encode_buffer (notably string in encode_buffer is not null-terminated)
   */
  static char* Encode(char* encode_buffer, const tStringRange& decoded, const char* unencoded_reserved_characters);

  /*!
   * Parses URI
   *
   * \param result Object to store results in. If many URI are parsed it makes sense to reuse the object - as this avoid reallocation of memory if its fields are sufficiently large.
   * \throw Throws std::invalid_argument if URI could not be parsed
   */
  void Parse(tURIElements& result) const;

  /*!
   * \return URI string
   */
  const std::string& ToString() const
  {
    return uri;
  }

  friend inline serialization::tInputStream& operator >> (serialization::tInputStream& stream, tURI& uri)
  {
    stream.ReadString(uri.uri);
    return stream;
  }
  friend inline serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tURI& uri)
  {
    stream >> uri.uri;
    return stream;
  }
//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! URI string */
  std::string uri;

};

inline bool operator==(const tURI& lhs, const tURI& rhs)
{
  return lhs.ToString() == rhs.ToString();
}

inline bool operator<(const tURI& lhs, const tURI& rhs)
{
  return lhs.ToString() < rhs.ToString();
}

inline serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tURI& uri)
{
  stream << uri.ToString();
  return stream;
}

inline serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tURI& uri)
{
  stream << uri.ToString();
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
