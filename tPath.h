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
/*!\file    rrlib/uri/tPath.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-29
 *
 * \brief   Contains tPath
 *
 * \b tPath
 *
 * A path.
 * A path consists of list of path elements (strings).
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__uri__tPath_h__
#define __rrlib__uri__tPath_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <tuple>
#include <cstring>
#include <iostream>
#include "rrlib/serialization/serialization.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/uri/tStringRange.h"

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
//! Path
/*!
 * A path.
 * A path consists of list of path elements.
 *
 * Implementation note: the whole path is efficiently stored in one continuous block of memory
 */
class tPath
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  class tConstIterator;

  /*! Path elements */
  typedef tStringRange tElement;

  /*! Creates empty path */
  tPath() :
    element_count(0)
  {}


  /*!
   * Constructs path from string
   *
   * \param path_string String (e.g. /element1/element2)
   * \param separator Separator of path elements
   */
  tPath(const tStringRange& path_string, char separator = '/') :
    tPath()
  {
    Set(path_string, separator);
  }

  /*!
   * Constructs path from iterator over string elements - e.g.
   *   tPath(string_vector.begin(), string_vector.end())
   * String elements can either be std::string or const char*.
   *
   * \param absolute Whether this is an abolute path
   * \param begin String element begin iterator
   * \param end String element end iterator
   */
  template <typename TStringIterator>
  tPath(bool absolute, TStringIterator begin, TStringIterator end) :
    tPath()
  {
    Set(absolute, begin, end);
  }

  /*!
   * Append path to this path (possibly eliminating '..' and '.' entries)
   *
   * \param append Path to append
   * \return Result
   */
  tPath Append(const tPath& append) const;

  /*!
   * \return Begin iterator for path elements
   */
  tConstIterator Begin() const
  {
    return tConstIterator(*this, 0);
  }

  /*!
   * Clear path
   */
  void Clear()
  {
    element_count = 0;
    memory.clear();
  }

  /*!
   * \return Number of path elements this path and the other path have in common
   */
  size_t CountCommonElements(const tPath& other) const
  {
    size_t max = std::max(other.Size(), this->Size());
    for (size_t i = 0; i < max; i++)
    {
      if ((*this)[i] != other[i])
      {
        return i;
      }
    }
    return max;
  }

  /*!
   * \return End iterator for path elements
   */
  tConstIterator End() const
  {
    return tConstIterator(*this, element_count);
  }

  /*!
   * \return Whether this is an abolute path
   */
  bool IsAbsolute() const
  {
    return GetPathStringBegin()[0] == '/';
  }

  /*!
   * Sets path elements from string
   *
   * \param path_string String (e.g. /element1/element2)
   * \param separator Separator of path elements
   */
  void Set(const tStringRange& path_string, char separator);

  /*!
   * Sets path elements from iterator - e.g.
   *   path.Set(string_vector.begin(), string_vector.end())
   * String elements can either be std::string or const char*.
   *
   * \param absolute Whether this is an abolute path
   * \param begin String element begin iterator
   * \param end String element end iterator
   */
  template <typename TStringIterator>
  void Set(bool absolute, TStringIterator begin, TStringIterator end)
  {
    element_count = end - begin;

    // Create element table
    uint elements[element_count + 1];
    uint lengths[element_count];
    uint current_offset = absolute ? 1 : 0;
    for (auto it = begin; it != end; ++it)
    {
      size_t length = StringLength(*it);
      elements[it - begin] = current_offset;
      current_offset += (length + 1);
      lengths[it - begin] = length;
    }
    elements[element_count] = current_offset;

    // Allocate and fill memory
    size_t table_size = (element_count + 1) * sizeof(uint);
    size_t required_memory = table_size + current_offset;
    memory.resize(required_memory);
    memcpy(&memory[0], elements, table_size);
    char* buffer = &memory[table_size];
    if (absolute)
    {
      (*buffer) = '/';
      buffer++;
    }
    for (auto it = begin; it != end; ++it)
    {
      memcpy(buffer, GetConstCharPointer(*it), lengths[it - begin]);
      buffer += lengths[it - begin];
      (*buffer) = '/';
      buffer++;
    }

    memory[required_memory - 1] = 0; // Null-terminator
  }

  /*!
   * \return Number of elements in path
   */
  size_t Size() const
  {
    return element_count;
  }

  /*!
   * \return Total number of characters in path - including separators and terminator
   */
  size_t TotalCharacters() const
  {
    return memory.size() - (element_count + 1) * sizeof(uint);
  }

  friend bool operator==(const tPath& lhs, const tPath& rhs)
  {
    return lhs.element_count == rhs.element_count && lhs.memory == rhs.memory;
  }
  friend bool operator<(const tPath& lhs, const tPath& rhs)
  {
    return std::tie(lhs.element_count, lhs.memory) < std::tie(rhs.element_count, rhs.memory);
  }
  tElement operator[](size_t index) const
  {
    const uint* table = GetElementOffsetTable();
    const char* chars = GetPathStringBegin();
    return tElement(chars + table[index], table[index + 1] - table[index] - 1);
  }
  friend inline std::ostream& operator << (std::ostream& stream, const tPath& path) // for command line output
  {
    stream.write(path.GetPathStringBegin(), &path.memory.back() - path.GetPathStringBegin());
    return stream;
  }


  /*!
   * Iterator over path elements (tStringRanges)
   */
  class tConstIterator
  {
  public:

    tConstIterator(const tPath& path, size_t element_index) :
      path(path),
      element_index(element_index),
      element()
    {
      UpdateElement();
    }

    friend bool operator==(const tConstIterator& lhs, const tConstIterator& rhs)
    {
      return (&lhs.path == &rhs.path) && lhs.element_index == rhs.element_index;
    }
    friend bool operator!=(const tConstIterator& lhs, const tConstIterator& rhs)
    {
      return !(lhs == rhs);
    }
    const tElement& operator*() const
    {
      return element;
    }
    inline const tElement* operator->() const
    {
      return &(operator*());
    }
    inline tConstIterator& operator++()
    {
      element_index++;
      UpdateElement();
      return *this;
    }
    inline tConstIterator operator ++ (int)
    {
      tConstIterator temp(*this);
      operator++();
      return temp;
    }
    friend size_t operator+(const tConstIterator& lhs, const tConstIterator& rhs)
    {
      return lhs.element_index + rhs.element_index;
    }
    friend size_t operator-(const tConstIterator& lhs, const tConstIterator& rhs)
    {
      return lhs.element_index - rhs.element_index;
    }
    friend tConstIterator operator+(const tConstIterator& lhs, size_t rhs)
    {
      return tConstIterator(lhs.path, lhs.element_index + rhs);
    }
    friend tConstIterator operator-(const tConstIterator& lhs, size_t rhs)
    {
      return tConstIterator(lhs.path, lhs.element_index - rhs);
    }

  private:

    const tPath& path;
    size_t element_index;
    tElement element;

    void UpdateElement()
    {
      element = element_index >= path.element_count ? tStringRange() : path[element_index];
    }
  };

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Number of elements in path */
  size_t element_count;

  /*!
   * Memory allocated to store path:
   * - offsets of elements (uint)
   * - length of below (uint)   (to uniformly be able to determine length of last element in [] operator)
   * - const char* containing whole path separated with slashes and terminated with zero
   */
  std::vector<char> memory;

  /*!
   * \param String to get const char* pointer of
   * \return Const char*
   */
  static const char* GetConstCharPointer(const std::string& string)
  {
    return string.c_str();
  }
  static const char* GetConstCharPointer(const char* string)
  {
    return string;
  }
  static const char* GetConstCharPointer(const tStringRange& string)
  {
    return string.CharPointer();
  }

  /*!
   * \return Pointer to first entry of element offset table
   */
  const uint* GetElementOffsetTable() const
  {
    return reinterpret_cast<const uint*>(&memory[0]);
  }

  /*!
   * \return Pointer to start of path string
   */
  const char* GetPathStringBegin() const
  {
    return memory.size() ? &memory[(element_count + 1) * sizeof(uint)] : "\0";
  }

  /*!
   * \param string String to obtain length from
   * \return String length
   */
  static size_t StringLength(const std::string& string)
  {
    return string.length();
  }
  static size_t StringLength(const char* string)
  {
    return strlen(string);
  }
  static size_t StringLength(const tStringRange& string)
  {
    return string.Length();
  }
};


inline bool operator!=(const tPath& lhs, const tPath& rhs)
{
  return !(lhs == rhs);
}

serialization::tOutputStream& operator << (serialization::tOutputStream& stream, const tPath& path);
serialization::tInputStream& operator >> (serialization::tInputStream& stream, tPath& path);
serialization::tStringOutputStream& operator << (serialization::tStringOutputStream& stream, const tPath& path);
serialization::tStringInputStream& operator >> (serialization::tStringInputStream& stream, tPath& path);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
