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
/*!\file    rrlib/uri/tURIElements.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-29
 *
 * \brief   Contains tURIElements
 *
 * \b tURIElements
 *
 * This struct contains the top-level elements of a URI.
 * It is the result of parsing a URI - and can be used to create one.
 *
 */
//----------------------------------------------------------------------
#ifndef __rrlib__uri__tURIElements_h__
#define __rrlib__uri__tURIElements_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/uri/tPath.h"

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
//! URI elements
/*!
 * This struct contains the top-level elements of a URI.
 * It is the result of parsing a URI - and can be used to create one.
 */
struct tURIElements
{
  std::string scheme;    //!< Scheme in URI (empty string if no scheme)
  std::string authority; //!< Authority in URI (percent-encoded; empty string if no authority)
  tPath path;            //!< Path in URI (decoded; empty path if no path)
  std::string query;     //!< Query in URI (percent-encoded; empty string if no query)
  std::string fragment;  //!< Fragment in URI (percent-encoded; empty string if no fragment)
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
