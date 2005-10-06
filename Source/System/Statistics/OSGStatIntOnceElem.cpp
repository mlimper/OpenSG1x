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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGSysFieldDataType.h"

#include "OSGStatIntOnceElem.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::StatIntOnceElem
    \ingroup GrpSystemStatistics

    The StatIntOnceElem is similar to the osg::StatIntElem, but it keeps
    track of who has contributed already and only allows each ID to
    contribute once. \ref PageSystemStatistics for details. 
*/

/***************************************************************************\
 *                         Instance methods                                *
\***************************************************************************/

/*------------- constructors & destructors --------------------------------*/

StatIntOnceElem::StatIntOnceElem(StatElemDescBase *desc)
  : StatElem(desc), 
    _value(0),
    _ids()
{
}

StatElem *StatIntOnceElem::create(StatElemDescBase *desc)
{
    return new StatIntOnceElem(desc);
}


StatIntOnceElem::~StatIntOnceElem(void)
{
}

/*------------------------------ access -----------------------------------*/

void StatIntOnceElem::putToString(std::string &str, const char *format) const
{
    if(!format)
    {
        FieldDataTraits<Int32>::putToString(_value, str);
    }
    else
    {
        char *temp = new char [strlen(format) + 40];
        sprintf(temp, format, _value);
        str = temp;
        delete [] temp;
    }
}

bool StatIntOnceElem::getFromString(const Char8 *&inVal)
{
    return FieldDataTraits<Int32>::getFromString(_value, inVal);
}

Real64 StatIntOnceElem::getValue(void) const
{
    return static_cast<Real64>(get());
}

void StatIntOnceElem::reset(void) 
{ 
    _value = 0; 
    _ids.clear();
}

/*-------------------------- assignment -----------------------------------*/

StatIntOnceElem& StatIntOnceElem::operator = (const StatIntOnceElem &source)
{
    if (this == &source)
        return *this;

    _value = source._value;
    _ids = source._ids;
    
    return *this;
}

/*-------------------------- comparison -----------------------------------*/

bool StatIntOnceElem::operator < (const StatIntOnceElem &other) const
{
    return this->get() < other.get();
}

/*--------------------------- creation ------------------------------------*/

StatElem *StatIntOnceElem::clone(void) const
{
    StatIntOnceElem *e = new StatIntOnceElem(getDesc());
    
    *e = *this;
    
    return e;
}

/*--------------------------- operators ------------------------------------*/

StatElem &StatIntOnceElem::operator += (const StatElem &other)
{
    const StatIntOnceElem *o = dynamic_cast<const StatIntOnceElem *>(&other);
    
    _value += o->_value;

    IdHash::const_iterator it = o->_ids.begin();

    while (it != o->_ids.end())
    {
        _ids.insert(*it);
        ++it;
    }

    return *this;
}



/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */

#ifdef __sgi
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGStatIntOnceElem.cpp,v 1.4 2005/10/06 14:48:46 a-m-z Exp $";
    static Char8 cvsid_hpp[] = OSGSTATINTONCEELEM_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGSTATINTONCEELEM_INLINE_CVSID;
}
