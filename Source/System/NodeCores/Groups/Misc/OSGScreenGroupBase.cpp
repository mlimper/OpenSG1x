/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 **     Do not change this file, changes should be done in the derived      **
 **     class ScreenGroup!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILESCREENGROUPINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGScreenGroupBase.h"
#include "OSGScreenGroup.h"


OSG_BEGIN_NAMESPACE

const OSG::BitVector ScreenGroupBase::MTInfluenceMask = 
    (Inherited::MTInfluenceMask) | 
    (static_cast<BitVector>(0x0) << Inherited::NextFieldId); 



FieldContainerType ScreenGroupBase::_type(
    "ScreenGroup",
    "Group",
    NULL,
    (PrototypeCreateF) &ScreenGroupBase::createEmpty,
    ScreenGroup::initMethod,
    NULL,
    0);

//OSG_FIELD_CONTAINER_DEF(ScreenGroupBase, ScreenGroupPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &ScreenGroupBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &ScreenGroupBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr ScreenGroupBase::shallowCopy(void) const 
{ 
    ScreenGroupPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const ScreenGroup *>(this)); 

    return returnValue; 
}

UInt32 ScreenGroupBase::getContainerSize(void) const 
{ 
    return sizeof(ScreenGroup); 
}


#if !defined(OSG_FIXED_MFIELDSYNC)
void ScreenGroupBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((ScreenGroupBase *) &other, whichField);
}
#else
void ScreenGroupBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField,                                    const SyncInfo       &sInfo     )
{
    this->executeSyncImpl((ScreenGroupBase *) &other, whichField, sInfo);
}
void ScreenGroupBase::execBeginEdit(const BitVector &whichField, 
                                            UInt32     uiAspect,
                                            UInt32     uiContainerSize) 
{
    this->execBeginEditImpl(whichField, uiAspect, uiContainerSize);
}

void ScreenGroupBase::onDestroyAspect(UInt32 uiId, UInt32 uiAspect)
{
    Inherited::onDestroyAspect(uiId, uiAspect);

}
#endif

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

ScreenGroupBase::ScreenGroupBase(void) :
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

ScreenGroupBase::ScreenGroupBase(const ScreenGroupBase &source) :
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

ScreenGroupBase::~ScreenGroupBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 ScreenGroupBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);


    return returnValue;
}

void ScreenGroupBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);


}

void ScreenGroupBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);


}

#if !defined(OSG_FIXED_MFIELDSYNC)
void ScreenGroupBase::executeSyncImpl(      ScreenGroupBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);


}
#else
void ScreenGroupBase::executeSyncImpl(      ScreenGroupBase *pOther,
                                        const BitVector         &whichField,
                                        const SyncInfo          &sInfo      )
{

    Inherited::executeSyncImpl(pOther, whichField, sInfo);



}

void ScreenGroupBase::execBeginEditImpl (const BitVector &whichField, 
                                                 UInt32     uiAspect,
                                                 UInt32     uiContainerSize)
{
    Inherited::execBeginEditImpl(whichField, uiAspect, uiContainerSize);

}
#endif



OSG_END_NAMESPACE

#include <OSGSFieldTypeDef.inl>
#include <OSGMFieldTypeDef.inl>

OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
DataType FieldDataTraits<ScreenGroupPtr>::_type("ScreenGroupPtr", "GroupPtr");
#endif

OSG_DLLEXPORT_SFIELD_DEF1(ScreenGroupPtr, OSG_SYSTEMLIB_DLLTMPLMAPPING);
OSG_DLLEXPORT_MFIELD_DEF1(ScreenGroupPtr, OSG_SYSTEMLIB_DLLTMPLMAPPING);


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGScreenGroupBase.cpp,v 1.1 2007/05/07 11:50:36 pdaehne Exp $";
    static Char8 cvsid_hpp       [] = OSGSCREENGROUPBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGSCREENGROUPBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGSCREENGROUPFIELDS_HEADER_CVSID;
}

OSG_END_NAMESPACE
