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

#include "OSGStatRealElem.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::StatRealElem
    \ingroup GrpSystemStatistics

    The StatIntElem keeps an Real32 value, see \ref PageSystemStatistics for
    details. 
*/

/***************************************************************************\
 *                         Instance methods                                *
\***************************************************************************/

/*------------- constructors & destructors --------------------------------*/

StatRealElem::StatRealElem(StatElemDescBase *desc)
  : StatElem(desc), _value(0)
{
}

StatElem *StatRealElem::create ( StatElemDescBase *desc)
{
    return new StatRealElem(desc);
}

StatRealElem::~StatRealElem(void)
{
}

/*------------------------------ access -----------------------------------*/

void StatRealElem::putToString(std::string &str, const char *format) const
{
    if(!format)
    {
        FieldDataTraits<Real32>::putToString(_value, str);
    }
    else
    {
        char *temp = new char [strlen(format) + 40];
        sprintf(temp, format, _value);
        str = temp;
        delete [] temp;
    }
}

bool StatRealElem::getFromString(const Char8 *&inVal)
{
    return FieldDataTraits<Real32>::getFromString(_value, inVal);
}

Real64 StatRealElem::getValue(void) const
{
    return static_cast<Real64>(get());
}


void StatRealElem::reset(void) 
{ 
    _value = 0; 
}

/*-------------------------- assignment -----------------------------------*/

StatRealElem& StatRealElem::operator = (const StatRealElem &source)
{
    if (this == &source)
        return *this;

    set(source.get());
    
    return *this;
}

/*-------------------------- comparison -----------------------------------*/

bool StatRealElem::operator < (const StatRealElem &other) const
{
    return this->get() < other.get();
}

/*--------------------------- creation ------------------------------------*/

StatElem *StatRealElem::clone(void) const
{
    StatRealElem *e = new StatRealElem(getDesc());
    
    *e = *this;
    
    return e;
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
    static Char8 cvsid_hpp[] = OSGSTATREALELEM_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGSTATREALELEM_INLINE_CVSID;
}
