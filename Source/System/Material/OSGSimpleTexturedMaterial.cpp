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

#include <OSGGL.h>

#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGGeometry.h>

#include <OSGStateChunk.h>
#include <OSGState.h>
#include <OSGMaterialChunk.h>
#include <OSGImage.h>

#include "OSGSimpleTexturedMaterial.h"

OSG_USING_NAMESPACE

/*! \class osg::SimpleTexturedMaterial
    \ingroup GrpSystemMaterial

The simple textured material class. See \ref
PageSystemMaterialTexturedSimpleMaterial for a description.

A osg::SimpleMaterial with an added texture. It doesn't expose all features of the
texture, just the ones needed most often.

osg::SimpleTexturedMaterial::_sfImage defines the texture, 
osg::SimpleTexturedMaterial::_sfMinFilter and
osg::SimpleTexturedMaterial::_sfMagFilter the used filters and 
osg::SimpleTexturedMaterial::_sfEnvMode the environment mode. As a special case
osg::SimpleTexturedMaterial::_sfEnvMap can be used to use the txture as a
spherical environment map.

*/

/*----------------------- constructors & destructors ----------------------*/

SimpleTexturedMaterial::SimpleTexturedMaterial(void) :
    Inherited()
{
}

SimpleTexturedMaterial::SimpleTexturedMaterial(
    const SimpleTexturedMaterial &source) :

    Inherited(source)
{
}

SimpleTexturedMaterial::~SimpleTexturedMaterial(void)
{
    subRefCP(_textureChunk);
    subRefCP(_texGenChunk);
}

/*----------------------------- class specific ----------------------------*/

void SimpleTexturedMaterial::initMethod(void)
{
}

#if defined(OSG_WIN32_ICL) && !defined(OSG_CHECK_FIELDSETARG)
#pragma warning (disable : 383)
#endif

void SimpleTexturedMaterial::changed(BitVector whichField, UInt32 origin)
{
    prepareLocalChunks();

    // these two are very expensive, as they need to regenerate the
    // texture object, do only if really needed

    if(whichField & ImageFieldMask)
    {
        beginEditCP(_textureChunk, TextureChunk::ImageFieldMask);

        _textureChunk->setImage(getImage());

        endEditCP(_textureChunk, TextureChunk::ImageFieldMask);
    }
    if(whichField & MinFilterFieldMask || whichField & MagFilterFieldMask)
    {
        beginEditCP(_textureChunk, TextureChunk::MinFilterFieldMask |
                                   TextureChunk::MagFilterFieldMask);

        _textureChunk->setMinFilter(getMinFilter());
        _textureChunk->setMagFilter(getMagFilter());

        endEditCP(_textureChunk, TextureChunk::MinFilterFieldMask |
                                 TextureChunk::MagFilterFieldMask);
    }
    // this is not as expensive, but why do it more often then needed?
    if(whichField & EnvModeFieldMask)
    {
        beginEditCP(_textureChunk, TextureChunk::EnvModeFieldMask);

        _textureChunk->setEnvMode(getEnvMode());

        endEditCP(_textureChunk, TextureChunk::EnvModeFieldMask);
    }
    if(whichField & EnvMapFieldMask)
    {
        beginEditCP(_texGenChunk, TexGenChunk::GenFuncSFieldMask |
                                  TexGenChunk::GenFuncTFieldMask);
        
        if (getEnvMap())
        {
            _texGenChunk->setGenFuncS(GL_SPHERE_MAP);
            _texGenChunk->setGenFuncT(GL_SPHERE_MAP);
        }
        else
        {
            _texGenChunk->setGenFuncS(GL_NONE);
            _texGenChunk->setGenFuncT(GL_NONE);
        }

        endEditCP(_texGenChunk, TexGenChunk::GenFuncSFieldMask |
                                TexGenChunk::GenFuncTFieldMask);
    }

    Inherited::changed(whichField, origin);
}

#if defined(OSG_WIN32_ICL) && !defined(OSG_CHECK_FIELDSETARG)
#pragma warning (default : 383)
#endif

StatePtr SimpleTexturedMaterial::makeState(void)
{
    StatePtr state = Inherited::makeState();

    prepareLocalChunks();

    state->addChunk(_textureChunk);
    state->addChunk(_texGenChunk);

    return state;
}

void SimpleTexturedMaterial::rebuildState(void)
{
    Inherited::rebuildState();

    prepareLocalChunks();

    _pState->addChunk(_textureChunk);
    _pState->addChunk(_texGenChunk);
}

bool SimpleTexturedMaterial::isTransparent(void) const
{
    return Inherited::isTransparent() ||
           (getImage()!=NullFC &&
             (getImage()->hasAlphaChannel() && getEnvMode() != GL_DECAL)
          );
}

void SimpleTexturedMaterial::dump(     UInt32    OSG_CHECK_ARG(uiIndent),
                                  const BitVector OSG_CHECK_ARG(bvFlags)) const
{
    SLOG << "Dump SimpleTexturedMaterial NI" << std::endl;
}


/* Create the chunks needed by this Material, one for the material properties,
   one for the optional transparency blending.
*/

void SimpleTexturedMaterial::prepareLocalChunks(void)
{
    if(_textureChunk == NullFC)
    {
        _textureChunk = TextureChunk::create();

         addRefCP(_textureChunk);
    }

    if(_texGenChunk == NullFC)
    {
        _texGenChunk  = TexGenChunk::create();

        addRefCP(_texGenChunk);
    }
}


/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: $";
    static Char8 cvsid_hpp[] = OSGSIMPLETEXTUREDMATERIAL_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGSIMPLETEXTUREDMATERIAL_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGSIMPLETEXTUREDMATERIALFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


