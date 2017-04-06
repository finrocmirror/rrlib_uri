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
/*!\file    rrlib/uri/tURI.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-29
 *
 */
//----------------------------------------------------------------------
#include "rrlib/uri/tURI.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <regex>
#include <stdexcept>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
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
// Const values
//----------------------------------------------------------------------
static std::regex cPARSE_REGEX("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");  // from RFC 3986 Appendix B
static char cTO_HEX_TABLE[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
const char* tURI::cUNENCODED_RESERVED_CHARACTERS_PATH = "!$&'()*+,;=:@";


//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

tURI::tURI(const tPath& path, const char* unencoded_reserved_characters) :
  uri()
{
  char buffer[path.TotalCharacters() * 3 + 1];
  char* buffer_pointer = buffer;
  if (path.IsAbsolute())
  {
    (*buffer_pointer) = '/';
    buffer_pointer++;
  }
  for (size_t i = 0; i < path.Size(); i++)
  {
    if (i > 0)
    {
      (*buffer_pointer) = '/';
      buffer_pointer++;
    }
    buffer_pointer = Encode(buffer_pointer, path[i], unencoded_reserved_characters);
  }
  (*buffer_pointer) = 0;
  uri = buffer;
}

char* tURI::Decode(char* decode_buffer, const tStringRange& encoded_string)
{
  const char* encoded = encoded_string.CharPointer();
  for (size_t i = 0; i < encoded_string.Length(); i++)
  {
    if (encoded[i] != '%')
    {
      (*decode_buffer) = encoded[i];
    }
    else if (i + 2 >= encoded_string.Length())
    {
      throw std::invalid_argument("encoded URI string cannot be decoded (invalid percent-encoding)");
    }
    else
    {
      char temp[3] = { encoded[i + 1], encoded[i + 2], 0 };
      char* endptr;
      char result = static_cast<char>(strtol(temp, &endptr, 16));
      if (result == 0)
      {
        throw std::invalid_argument("encoded URI string cannot be decoded (invalid percent-encoding)");
      }
      (*decode_buffer) = result;
      i += 2;
    }
    decode_buffer++;
  }
  return decode_buffer;
}

char* tURI::Encode(char* encode_buffer, const tStringRange& decoded, const char* unencoded_reserved_characters)
{
  for (size_t i = 0; i < decoded.Length(); i++)
  {
    char character = decoded.CharPointer()[i];
    if (std::isalnum(character) || strchr(unencoded_reserved_characters, character) || character == '_' || character == '.' || character == '-' || character == '~')
    {
      (*encode_buffer) = character;
      encode_buffer++;
    }
    else
    {
      unsigned char value = static_cast<unsigned char>(character);
      encode_buffer[0] = '%';
      encode_buffer[1] = cTO_HEX_TABLE[value >> 4];
      encode_buffer[2] = cTO_HEX_TABLE[value & 0xF];
      encode_buffer += 3;
    }
  }
  return encode_buffer;
}

void tURI::Parse(tURIElements& result) const
{
  std::smatch match;
  std::string empty;
  if (!std::regex_match(uri, match, cPARSE_REGEX))
  {
    throw std::invalid_argument("Cannot parse URI " + uri);
  }
  char decoded_buffer[match[5].length() + 1];
  char* post_decoded = Decode(decoded_buffer, tStringRange(&*match[5].first, match[5].second - match[5].first));
  (*post_decoded) = 0;
  result.scheme = (match.size() > 2 && match[2].matched) ? match[2] : empty;
  result.authority = (match.size() > 4 && match[4].matched) ? match[4] : empty;
  result.path = (match.size() > 5 && match[5].matched) ? tPath(decoded_buffer) : tPath();
  result.query = (match.size() > 7 && match[7].matched) ? match[7] : empty;
  result.fragment = (match.size() > 9 && match[9].matched) ? match[9] : empty;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
