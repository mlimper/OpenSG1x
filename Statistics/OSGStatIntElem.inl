/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
 *                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

OSG_BEGIN_NAMESPACE

/*-------------------------------------------------------------------------*/
inline void StatIntElem::set (Int32 value) 
{ 
  _value = value; 
}

/*-------------------------------------------------------------------------*/
inline Int32 StatIntElem::get (void) 
{ 
  return _value; 
}

/*-------------------------------------------------------------------------*/
inline void StatIntElem::add (Int32 v) 
{ 
  _value += v; 
}
    
/*-------------------------------------------------------------------------*/
inline void StatIntElem::sub (Int32 v) 
{ 
  _value -= v; 
}

/*-------------------------------------------------------------------------*/
inline void StatIntElem::reset (void) 
{ 
  _value = 0; 
}

/*-------------------------------------------------------------------------*/
inline void StatIntElem::inc (void) 
{ 
  _value++; 
}

/*-------------------------------------------------------------------------*/
inline void StatIntElem::dec (void) 
{ 
  _value--; 
}

OSG_END_NAMESPACE
