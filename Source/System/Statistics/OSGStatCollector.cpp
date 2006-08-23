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

#define OSG_COMPILESTATCOLLECTORINST

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include "OSGStatElemDesc.h"
#include "OSGStatElem.h"

#include "OSGStatCollector.h"

OSG_USING_NAMESPACE



/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::StatCollector
    \ingroup GrpSystemStatistics

    The StatCollector is responsible for managing a set of osg::StatElem
    elements, see \ref PageSystemStatistics for details.
*/

/***************************************************************************\
 *                         Instance methods                                *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                -
\*-------------------------------------------------------------------------*/

StatCollector::StatCollector(void)
{
    refitElemNum();
}

StatCollector::StatCollector(const StatCollector &source)
{
    _elemVec.resize(source._elemVec.size());
    
    for(UInt32 i = 0; i < source._elemVec.size(); ++i)
    {
        if(source._elemVec[i])
        {
            _elemVec[i] = source._elemVec[i]->clone();
        }
        else
        {
            _elemVec[i] = NULL;
        }
    }
}

StatCollector *StatCollector::create(void)
{
    return new StatCollector();
}

StatCollector::~StatCollector(void)
{
	for(UInt32 i = 0; i < _elemVec.size(); ++i)
		delete _elemVec[i];
}

/*-------------------------- your_category---------------------------------*/

/*! Increase the size of the StatCollector's data array. This is called during
    construction and will only be needed later, when a new StatElem has been added
    after the StatCollector was instanced. This will very rarely be the case.
*/
void StatCollector::refitElemNum(void)
{
  unsigned eN = _elemVec.size(), dN = StatElemDescBase::getNumOfDescs();

  if(eN != dN)
  {
    _elemVec.resize(dN,0);
  }
}

/*! Convert the current contents into a string. This
    string can be used as a compact representation of the data, and as input
    for StatCollector::getFromString.
*/
void StatCollector::putToString(std::string &str) const
{
    std::vector<StatElem*>::const_iterator it;
    bool first = true;
     
    str = "{";
    
    for(it = _elemVec.begin(); it != _elemVec.end(); ++it)
    {
        if(*it != NULL)
        {
            std::string elem;
            
            if(!first)
                str.append("|");
            str.append((*it)->getDesc()->getName().str());
            str.append("=");
            (*it)->putToString(elem);
            str.append(elem);
            first = false;
        }
    }
    str.append("}");
}
     

/*! Set the contents from a string. The string has to have the format that is
    used by StatCollector::putToString.
*/
bool StatCollector::getFromString(const Char8 *&inVal)
{
    const Char8 *c = inVal;
    
    if(*c++ != '{')
        return false;

    StatElemDescBase *desc;
    StatElem *elem;
    
    clearElems();
    
    while(*c && *c != '}')
    {
        const Char8 *end = c;
        
        while(*end != 0 && *end != '=' && *end != '}' && *end != '|')
            end++;
            
        if(*end == 0 || *end == '}' || *end == '|')
            return false;
        
        std::string name(c, end - c);
        desc = StatElemDescBase::findDescByName(name.c_str());
        
        if(!desc)
            return false;
        
        elem = getElem(*desc);

        c = end = end + 1;       
        while(*end != 0 && *end != '}' && *end != '|')
            end++;
            
        if(*end == 0)
            return false;

        std::string val(c, end - c);
        const Char8 *valp = val.c_str();
        if(!elem->getFromString(valp))
            return false;
 
        c = end + 1;
    }
    return true;
}

/*! Get the value of the named element, if it exists, return false if not.
*/
bool StatCollector::getValue(std::string &name, Real64 &val)
{
    StatElemDescBase *desc = StatElemDescBase::findDescByName(name.c_str());
    
    if(!desc)
        return false;
    
    StatElem *el = getElem(*desc, false);
    
    if(!el)
        return false;
    
    val = el->getValue();
    
    return true;
}

/*! Remove all elements from the collector.
*/
void StatCollector::clearElems(void)
{
    for(std::vector<StatElem*>::iterator i = _elemVec.begin(); 
        i != _elemVec.end();
        ++i)
    {
        if(*i != NULL)
        {
            delete *i;
            *i = NULL;
        }
    }
	
	_elemVec.clear();
}


/*! Reset all elements to the start value.
*/
void StatCollector::reset(void)
{
    for(std::vector<StatElem*>::iterator i = _elemVec.begin(); 
        i != _elemVec.end();
        ++i)
    {
        if(*i != NULL)
        {
            (*i)->reset();
        }
    }
}

/*-------------------------- assignment -----------------------------------*/

const StatCollector& StatCollector::operator = (const StatCollector &source)
{
    if (this == &source)
        return *this;

    _elemVec = source._elemVec;
 
    return *this;
}

/*-------------------------- comparison -----------------------------------*/

/*! The comparison is only done on the addresses, as a real comparison is not
    well defined on a StatCollector.
*/
bool StatCollector::operator < (const StatCollector &other) const
{
    return this < &other;
}


StatCollector StatCollector::operator + (const StatCollector &other)
{
    StatCollector res(*this);
    
    res += other;
    
    return res;
}

StatCollector &StatCollector::operator += (const StatCollector &other)
{
    if(other._elemVec.size() > _elemVec.size())
    {
        _elemVec.resize(other._elemVec.size());
    }
    
    for(UInt32 i = 0; i < _elemVec.size(); ++i)
    {
        if(_elemVec[i])
        {
            *_elemVec[i] += *other._elemVec[i];
        }
    }
    
    return *this;
}



#include <OSGMFieldTypeDef.inl>
#include <OSGSFieldTypeDef.inl>

OSG_BEGIN_NAMESPACE

#ifndef OSG_DO_DOC

/*-------------------------- field instantiations -------------------------*/

DataType FieldDataTraits<StatCollector>::_type("StatCollector", 
                                               "None");

#endif

OSG_DLLEXPORT_SFIELD_DEF1(StatCollector, OSG_SYSTEMLIB_DLLTMPLMAPPING);

OSG_END_NAMESPACE

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
    static Char8 cvsid_hpp[] = OSGSTATCOLLECTOR_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGSTATCOLLECTOR_INLINE_CVSID;
}

