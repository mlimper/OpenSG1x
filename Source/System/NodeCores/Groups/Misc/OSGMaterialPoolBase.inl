/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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
 **     class MaterialPool!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &MaterialPoolBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 MaterialPoolBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
MaterialPoolPtr MaterialPoolBase::create(void) 
{
    MaterialPoolPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = MaterialPoolPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
MaterialPoolPtr MaterialPoolBase::createEmpty(void) 
{ 
    MaterialPoolPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the MaterialPool::_mfMaterials field.
inline
MFMaterialPtr *MaterialPoolBase::getMFMaterials(void)
{
    return &_mfMaterials;
}



//! Get the value of the \a index element the MaterialPool::_mfMaterials field.
inline
MaterialPtr &MaterialPoolBase::getMaterials(const UInt32 index)
{
    return _mfMaterials[index];
}

//! Get the MaterialPool::_mfMaterials field.
inline
MFMaterialPtr &MaterialPoolBase::getMaterials(void)
{
    return _mfMaterials;
}

//! Get the MaterialPool::_mfMaterials field.
inline
const MFMaterialPtr &MaterialPoolBase::getMaterials(void) const
{
    return _mfMaterials;
}

OSG_END_NAMESPACE

#define OSGMATERIALPOOLBASE_INLINE_CVSID "@(#)$Id: OSGMaterialPoolBase.inl,v 1.1 2005/04/30 15:03:20 a-m-z Exp $"
