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

OSG_BEGIN_NAMESPACE


inline void StatIntOnceElem::add(UInt32 id, Int32 v)
{
    if(_ids.find(id) == _ids.end())
    {
        _value += v;
        _ids.insert(id);
    }
}

inline void StatIntOnceElem::sub(UInt32 id, Int32 v)
{
    if(_ids.find(id) == _ids.end())
    {
        _value -= v;
        _ids.insert(id);
    }
}

inline void StatIntOnceElem::inc(UInt32 id)
{
    if(_ids.find(id) == _ids.end())
    {
        _value ++;
        _ids.insert(id);
    }
}

inline void StatIntOnceElem::dec(UInt32 id)
{
    if(_ids.find(id) == _ids.end())
    {
        _value --;
        _ids.insert(id);
    }
}

inline Int32 StatIntOnceElem::get() const
{
    return _value;
}


OSG_END_NAMESPACE

#define OSGSTATINTONCEELEM_INLINE_CVSID "@(#)$Id: OSGStatIntOnceElem.inl,v 1.1 2005/07/26 19:12:07 dirk Exp $"
