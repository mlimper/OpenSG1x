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

#include <OSGConfig.h>
#include <OSGSysFieldDataType.h>

#include "OSGStatIntElem.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::StatIntElem
    \ingroup GrpSystemStatistics

    The StatIntElem keeps an Int32 counter, see \ref PageSystemStatistics for
    details. 
*/

/***************************************************************************\
 *                         Instance methods                                *
\***************************************************************************/

/*------------- constructors & destructors --------------------------------*/

StatIntElem::StatIntElem(StatElemDescBase *desc)
  : StatElem(desc), _value(0)
{
}

StatElem *StatIntElem::create ( StatElemDescBase *desc)
{
  return new StatIntElem(desc);
}

StatIntElem::~StatIntElem(void)
{
}

/*------------------------------ access -----------------------------------*/

void StatIntElem::putToString(std::string &str, const char *format) const
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

bool StatIntElem::getFromString(const Char8 *&inVal)
{
    return FieldDataTraits<Int32>::getFromString(_value, inVal);
}

Real64 StatIntElem::getValue(void) const
{
    return static_cast<Real64>(get());
}

/*-------------------------- assignment -----------------------------------*/

StatIntElem& StatIntElem::operator = (const StatIntElem &source)
{
    if (this == &source)
        return *this;

    set(source.get());

    return *this;
}

/*-------------------------- comparison -----------------------------------*/

bool StatIntElem::operator < (const StatIntElem &other) const
{
    return this->get() < other.get();
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
    static Char8 cvsid_cpp[] = "@(#)$Id: $";
    static Char8 cvsid_hpp[] = OSGSTATINTELEM_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGSTATINTELEM_INLINE_CVSID;
}
