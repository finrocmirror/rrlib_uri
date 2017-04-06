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
/*!\file    rrlib/uri/tPath.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-29
 *
 */
//----------------------------------------------------------------------
#include "rrlib/uri/tPath.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/uri/tURI.h"

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

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
static size_t Normalize(tStringRange* buffer, size_t size)
{
  tStringRange* current_write_element = buffer;
  for (size_t i = 0; i < size; i++)
  {
    if (buffer[i] == ".")
    {
      continue;
    }
    else if (buffer[i] == "..")
    {
      if (current_write_element > buffer)
      {
        current_write_element--;
      }
    }
    else
    {
      (*current_write_element) = buffer[i];
      current_write_element++;
    }
  }
  return current_write_element - buffer;
}


tPath tPath::Append(const tPath& append) const
{
  tStringRange buffer[Size() + append.Size()];
  for (size_t i = 0; i < Size(); i++)
  {
    buffer[i] == (*this)[i];
  }
  for (size_t i = 0; i < append.Size(); i++)
  {
    buffer[Size() + i] == append[i];
  }
  size_t size = Normalize(buffer, Size() + append.Size());
  return tPath(IsAbsolute(), &buffer[0], &buffer[size]);
}

void tPath::Set(const tStringRange& path_string, char separator)
{
  bool absolute = path_string.CharPointer()[0] == separator;
  size_t start_index = absolute ? 1 : 0;
  size_t end_index = path_string.Length() - (path_string.CharPointer()[path_string.Length() - 1] == separator ? 1 : 0);

  // Create element table
  element_count = 1;
  uint elements[path_string.Length() + 1];
  elements[0] = start_index;

  for (size_t i = 1; i < end_index; i++)
  {
    if (path_string.CharPointer()[i] == separator)
    {
      elements[element_count] = i + 1;
      element_count++;
    }
  }
  elements[element_count] = end_index + 1;

  // Allocate and fill memory
  size_t table_size = (element_count + 1) * sizeof(uint);
  size_t required_memory = table_size + end_index + 1; // +1 for terminating null character
  memory.resize(required_memory);
  memcpy(&memory[0], elements, table_size);
  memcpy(&memory[table_size], path_string.CharPointer(), end_index);
  if (separator != '/')
  {
    if (absolute)
    {
      memory[table_size] = '/';
    }
    for (size_t i = 1; i < element_count; i++)
    {
      memory[table_size + elements[i] - 1] = '/';
    }
  }
  memory[required_memory - 1] = 0; // Null-terminator
}


serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tPath& path)
{
  size_t size = path.Size();
  stream.WriteInt(path.TotalCharacters());
  if (path.IsAbsolute())
  {
    stream.WriteByte(0);
  }
  for (size_t i = 0; i < size; i++)
  {
    stream << path[i];
  }
  return stream;
}

serialization::tInputStream& operator >> (serialization::tInputStream& stream, tPath& path)
{
  size_t size = stream.ReadInt();
  char buffer[size];
  stream.ReadFully(buffer, size);
  path.Set(tStringRange(buffer, size), 0);
  return stream;
}

serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tPath& path)
{
  stream << tURI(path);
  return stream;
}

serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tPath& path)
{
  tURIElements elements;
  tURI uri;
  stream >> uri;
  uri.Parse(elements);
  path = elements.path;
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
