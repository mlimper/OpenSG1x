/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000-2002 by the OpenSG Forum                   *
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

#ifndef _DGRAM_INL_
#define _DGRAM_INL_

// Application declarations
// to get ntons/ntohs
#ifdef WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif

OSG_BEGIN_NAMESPACE

/*---------------------------------------------------------------------*/
/*                      get                                            */

UInt32 Dgram::getCapacity(void) const
{
    return sizeof(_buffer) - sizeof(_buffer._id);
}

UInt16 Dgram::getId(void) const
{
    return ntohs(_buffer._id);
}

UInt32 Dgram::getSize(void) const
{
    return _size;
}

UChar8 *Dgram::getData(void)
{
    return _buffer._data;
}

const UChar8 *Dgram::getData(void) const
{
    return _buffer._data;
}

UChar8 *Dgram::getBuffer(void)
{
    return (UChar8 *)&_buffer;
}

UInt32 Dgram::getBufferSize(void) const
{
    return _size + sizeof(_buffer._id);
}

UInt32 Dgram::getBufferCapacity(void) const
{
    return sizeof(_buffer);
}

bool Dgram::getResponseAck(void) const
{
    return _buffer._data[0];
}

bool Dgram::getEarlySend(void) const
{
    return _earlySend;
}

/*---------------------------------------------------------------------*/
/*                      set                                            */

inline void Dgram::setSize(UInt32 size)
{
    _size = size;
}

inline void Dgram::setResponseSize(void)
{
    _size = sizeof(UChar8);
}

inline void Dgram::setId(UInt16 id)
{
    _buffer._id = htons(id);
}

inline void Dgram::setBufferSize(UInt32 size)
{
    _size = size - sizeof(_buffer._id);
}

inline void Dgram::setResponseAck(bool value)
{
    _buffer._data[0] = value;
}

inline void Dgram::setEarlySend(bool value)
{
    _earlySend = value;
}

/*---------------------------------------------------------------------*/
/*                      compare                                        */

/*! Compare IDs. Use Window.
 */
inline bool Dgram::operator <(const Dgram &other) const
{
    return less(this->getId(),other.getId());
}

/*! Compare IDs. Use Window.
 */
inline bool Dgram::less(UInt16 a,UInt16 b)
{
    a = a - b;

    if(a & 0x8000)
        return true;
    else
        return false;
}

OSG_END_NAMESPACE
 
#define OSG_DGRAMINLINE_CVSID "@(#)$Id:$"

#endif







