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
 **     class TextureBackground!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#define OSG_COMPILETEXTUREBACKGROUNDINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include "OSGTextureBackgroundBase.h"
#include "OSGTextureBackground.h"


OSG_USING_NAMESPACE

const OSG::BitVector  TextureBackgroundBase::ColorFieldMask = 
    (TypeTraits<BitVector>::One << TextureBackgroundBase::ColorFieldId);

const OSG::BitVector  TextureBackgroundBase::TextureFieldMask = 
    (TypeTraits<BitVector>::One << TextureBackgroundBase::TextureFieldId);

const OSG::BitVector  TextureBackgroundBase::TexCoordsFieldMask = 
    (TypeTraits<BitVector>::One << TextureBackgroundBase::TexCoordsFieldId);

const OSG::BitVector TextureBackgroundBase::MTInfluenceMask = 
    (Inherited::MTInfluenceMask) | 
    (static_cast<BitVector>(0x0) << Inherited::NextFieldId); 


// Field descriptions

/*! \var Color4f         TextureBackgroundBase::_sfColor
    
*/
/*! \var TextureChunkPtr TextureBackgroundBase::_sfTexture
    
*/
/*! \var Pnt2f           TextureBackgroundBase::_mfTexCoords
    
*/

//! TextureBackground description

FieldDescription *TextureBackgroundBase::_desc[] = 
{
    new FieldDescription(SFColor4f::getClassType(), 
                     "color", 
                     ColorFieldId, ColorFieldMask,
                     false,
                     (FieldAccessMethod) &TextureBackgroundBase::getSFColor),
    new FieldDescription(SFTextureChunkPtr::getClassType(), 
                     "texture", 
                     TextureFieldId, TextureFieldMask,
                     false,
                     (FieldAccessMethod) &TextureBackgroundBase::getSFTexture),
    new FieldDescription(MFPnt2f::getClassType(), 
                     "texCoords", 
                     TexCoordsFieldId, TexCoordsFieldMask,
                     false,
                     (FieldAccessMethod) &TextureBackgroundBase::getMFTexCoords)
};


FieldContainerType TextureBackgroundBase::_type(
    "TextureBackground",
    "Background",
    NULL,
    (PrototypeCreateF) &TextureBackgroundBase::createEmpty,
    TextureBackground::initMethod,
    _desc,
    sizeof(_desc));

//OSG_FIELD_CONTAINER_DEF(TextureBackgroundBase, TextureBackgroundPtr)

/*------------------------------ get -----------------------------------*/

FieldContainerType &TextureBackgroundBase::getType(void) 
{
    return _type; 
} 

const FieldContainerType &TextureBackgroundBase::getType(void) const 
{
    return _type;
} 


FieldContainerPtr TextureBackgroundBase::shallowCopy(void) const 
{ 
    TextureBackgroundPtr returnValue; 

    newPtr(returnValue, dynamic_cast<const TextureBackground *>(this)); 

    return returnValue; 
}

UInt32 TextureBackgroundBase::getContainerSize(void) const 
{ 
    return sizeof(TextureBackground); 
}


void TextureBackgroundBase::executeSync(      FieldContainer &other,
                                    const BitVector      &whichField)
{
    this->executeSyncImpl((TextureBackgroundBase *) &other, whichField);
}

/*------------------------- constructors ----------------------------------*/

#ifdef OSG_WIN32_ICL
#pragma warning (disable : 383)
#endif

TextureBackgroundBase::TextureBackgroundBase(void) :
    _sfColor                  (), 
    _sfTexture                (), 
    _mfTexCoords              (), 
    Inherited() 
{
}

#ifdef OSG_WIN32_ICL
#pragma warning (default : 383)
#endif

TextureBackgroundBase::TextureBackgroundBase(const TextureBackgroundBase &source) :
    _sfColor                  (source._sfColor                  ), 
    _sfTexture                (source._sfTexture                ), 
    _mfTexCoords              (source._mfTexCoords              ), 
    Inherited                 (source)
{
}

/*-------------------------- destructors ----------------------------------*/

TextureBackgroundBase::~TextureBackgroundBase(void)
{
}

/*------------------------------ access -----------------------------------*/

UInt32 TextureBackgroundBase::getBinSize(const BitVector &whichField)
{
    UInt32 returnValue = Inherited::getBinSize(whichField);

    if(FieldBits::NoField != (ColorFieldMask & whichField))
    {
        returnValue += _sfColor.getBinSize();
    }

    if(FieldBits::NoField != (TextureFieldMask & whichField))
    {
        returnValue += _sfTexture.getBinSize();
    }

    if(FieldBits::NoField != (TexCoordsFieldMask & whichField))
    {
        returnValue += _mfTexCoords.getBinSize();
    }


    return returnValue;
}

void TextureBackgroundBase::copyToBin(      BinaryDataHandler &pMem,
                                  const BitVector         &whichField)
{
    Inherited::copyToBin(pMem, whichField);

    if(FieldBits::NoField != (ColorFieldMask & whichField))
    {
        _sfColor.copyToBin(pMem);
    }

    if(FieldBits::NoField != (TextureFieldMask & whichField))
    {
        _sfTexture.copyToBin(pMem);
    }

    if(FieldBits::NoField != (TexCoordsFieldMask & whichField))
    {
        _mfTexCoords.copyToBin(pMem);
    }


}

void TextureBackgroundBase::copyFromBin(      BinaryDataHandler &pMem,
                                    const BitVector    &whichField)
{
    Inherited::copyFromBin(pMem, whichField);

    if(FieldBits::NoField != (ColorFieldMask & whichField))
    {
        _sfColor.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (TextureFieldMask & whichField))
    {
        _sfTexture.copyFromBin(pMem);
    }

    if(FieldBits::NoField != (TexCoordsFieldMask & whichField))
    {
        _mfTexCoords.copyFromBin(pMem);
    }


}

void TextureBackgroundBase::executeSyncImpl(      TextureBackgroundBase *pOther,
                                        const BitVector         &whichField)
{

    Inherited::executeSyncImpl(pOther, whichField);

    if(FieldBits::NoField != (ColorFieldMask & whichField))
        _sfColor.syncWith(pOther->_sfColor);

    if(FieldBits::NoField != (TextureFieldMask & whichField))
        _sfTexture.syncWith(pOther->_sfTexture);

    if(FieldBits::NoField != (TexCoordsFieldMask & whichField))
        _mfTexCoords.syncWith(pOther->_mfTexCoords);


}



#include <OSGSFieldTypeDef.inl>

OSG_BEGIN_NAMESPACE

#if !defined(OSG_DO_DOC) || defined(OSG_DOC_DEV)
DataType FieldDataTraits<TextureBackgroundPtr>::_type("TextureBackgroundPtr", "BackgroundPtr");
#endif

OSG_DLLEXPORT_SFIELD_DEF1(TextureBackgroundPtr, OSG_SYSTEMLIB_DLLTMPLMAPPING);

OSG_END_NAMESPACE


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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGTextureBackgroundBase.cpp,v 1.1 2004/01/23 14:16:11 a-m-z Exp $";
    static Char8 cvsid_hpp       [] = OSGTEXTUREBACKGROUNDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGTEXTUREBACKGROUNDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGTEXTUREBACKGROUNDFIELDS_HEADER_CVSID;
}
