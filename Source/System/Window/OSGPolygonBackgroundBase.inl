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
 **     class PolygonBackground!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &PolygonBackgroundBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 PolygonBackgroundBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
PolygonBackgroundPtr PolygonBackgroundBase::create(void) 
{
    PolygonBackgroundPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = PolygonBackgroundPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
PolygonBackgroundPtr PolygonBackgroundBase::createEmpty(void) 
{ 
    PolygonBackgroundPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the PolygonBackground::_sfMaterial field.
inline
SFMaterialPtr *PolygonBackgroundBase::getSFMaterial(void)
{
    return &_sfMaterial;
}

//! Get the PolygonBackground::_mfTexCoords field.
inline
MFVec3f *PolygonBackgroundBase::getMFTexCoords(void)
{
    return &_mfTexCoords;
}

//! Get the PolygonBackground::_mfPositions field.
inline
MFPnt2f *PolygonBackgroundBase::getMFPositions(void)
{
    return &_mfPositions;
}

//! Get the PolygonBackground::_sfNormalizedX field.
inline
SFBool *PolygonBackgroundBase::getSFNormalizedX(void)
{
    return &_sfNormalizedX;
}

//! Get the PolygonBackground::_sfNormalizedY field.
inline
SFBool *PolygonBackgroundBase::getSFNormalizedY(void)
{
    return &_sfNormalizedY;
}


//! Get the value of the PolygonBackground::_sfMaterial field.
inline
MaterialPtr &PolygonBackgroundBase::getMaterial(void)
{
    return _sfMaterial.getValue();
}

//! Get the value of the PolygonBackground::_sfMaterial field.
inline
const MaterialPtr &PolygonBackgroundBase::getMaterial(void) const
{
    return _sfMaterial.getValue();
}

//! Set the value of the PolygonBackground::_sfMaterial field.
inline
void PolygonBackgroundBase::setMaterial(const MaterialPtr &value)
{
    _sfMaterial.setValue(value);
}

//! Get the value of the PolygonBackground::_sfNormalizedX field.
inline
bool &PolygonBackgroundBase::getNormalizedX(void)
{
    return _sfNormalizedX.getValue();
}

//! Get the value of the PolygonBackground::_sfNormalizedX field.
inline
const bool &PolygonBackgroundBase::getNormalizedX(void) const
{
    return _sfNormalizedX.getValue();
}

//! Set the value of the PolygonBackground::_sfNormalizedX field.
inline
void PolygonBackgroundBase::setNormalizedX(const bool &value)
{
    _sfNormalizedX.setValue(value);
}

//! Get the value of the PolygonBackground::_sfNormalizedY field.
inline
bool &PolygonBackgroundBase::getNormalizedY(void)
{
    return _sfNormalizedY.getValue();
}

//! Get the value of the PolygonBackground::_sfNormalizedY field.
inline
const bool &PolygonBackgroundBase::getNormalizedY(void) const
{
    return _sfNormalizedY.getValue();
}

//! Set the value of the PolygonBackground::_sfNormalizedY field.
inline
void PolygonBackgroundBase::setNormalizedY(const bool &value)
{
    _sfNormalizedY.setValue(value);
}


//! Get the value of the \a index element the PolygonBackground::_mfTexCoords field.
inline
Vec3f &PolygonBackgroundBase::getTexCoords(const UInt32 index)
{
    return _mfTexCoords[index];
}

//! Get the PolygonBackground::_mfTexCoords field.
inline
MFVec3f &PolygonBackgroundBase::getTexCoords(void)
{
    return _mfTexCoords;
}

//! Get the PolygonBackground::_mfTexCoords field.
inline
const MFVec3f &PolygonBackgroundBase::getTexCoords(void) const
{
    return _mfTexCoords;
}

//! Get the value of the \a index element the PolygonBackground::_mfPositions field.
inline
Pnt2f &PolygonBackgroundBase::getPositions(const UInt32 index)
{
    return _mfPositions[index];
}

//! Get the PolygonBackground::_mfPositions field.
inline
MFPnt2f &PolygonBackgroundBase::getPositions(void)
{
    return _mfPositions;
}

//! Get the PolygonBackground::_mfPositions field.
inline
const MFPnt2f &PolygonBackgroundBase::getPositions(void) const
{
    return _mfPositions;
}

OSG_END_NAMESPACE

#define OSGPOLYGONBACKGROUNDBASE_INLINE_CVSID "@(#)$Id: OSGPolygonBackgroundBase.inl,v 1.1 2005/06/06 17:16:01 yjung Exp $"
