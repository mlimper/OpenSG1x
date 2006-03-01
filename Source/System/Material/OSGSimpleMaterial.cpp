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
 * MERCHANTABILITY or FITNESS forA PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License formore details.                          *
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

#include <OSGGL.h>

#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGGeometry.h>

#include <OSGStateChunk.h>
#include <OSGState.h>
#include <OSGMaterialChunk.h>

#include "OSGSimpleMaterial.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::SimpleMaterial
    \ingroup GrpSystemMaterial

The simple material class. See \ref PageSystemMaterialSimpleMaterial for a
description.

This material wraps the standard calls to glMaterial() in
osg::SimpleMaterial::_sfAmbient, osg::SimpleMaterial::_sfDiffuse, 
osg::SimpleMaterial::_sfEmission, osg::SimpleMaterial::_sfSpecular, 
osg::SimpleMaterial::_sfShininess. In addition it supports transparency 
(osg::SimpleMaterial::_sfTransparency), can switch lighting 
(osg::SimpleMaterial::_sfLit) and the color material 
(osg::SimpleMaterial::_sfColorMaterial).

*/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/* Create the chunks needed by this Material, one for the material properties,
   one for the optional transparency blending.
*/

void SimpleMaterial::prepareLocalChunks(void)
{
    if(_materialChunk == NullFC)
    {
        _materialChunk = MaterialChunk::create();

        addRefCP(_materialChunk);
    }

    if(_blendChunk == NullFC)
    {
        _blendChunk    = BlendChunk   ::create();

        beginEditCP(_blendChunk);
        {
            _blendChunk->setSrcFactor (GL_SRC_ALPHA);
            _blendChunk->setDestFactor(GL_ONE_MINUS_SRC_ALPHA);
        }
        endEditCP  (_blendChunk);

        addRefCP   (_blendChunk);
    }
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void SimpleMaterial::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

SimpleMaterial::SimpleMaterial(void) :
    Inherited(),
    _materialChunk(NullFC),
    _blendChunk   (NullFC)
{
}

SimpleMaterial::SimpleMaterial(const SimpleMaterial &source) :
     Inherited    (source               ),
    _materialChunk(source._materialChunk),
    _blendChunk   (source._blendChunk   )
{
}

SimpleMaterial::~SimpleMaterial(void)
{
    subRefCP(_materialChunk);
    subRefCP(_blendChunk   );    
}

void SimpleMaterial::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------- your_category---------------------------------*/

StatePtr SimpleMaterial::makeState(void)
{
    StatePtr state = State::create();

    Color3f v3;
    Color4f v4;
    float alpha = 1.f - getTransparency();

    prepareLocalChunks();

    beginEditCP(_materialChunk);
    {
         v3 = getAmbient(); 
         v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        _materialChunk->setAmbient(v4);

         v3 = getDiffuse(); 
         v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        _materialChunk->setDiffuse(v4);

         v3 = getSpecular(); 
         v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        _materialChunk->setSpecular(v4);

        _materialChunk->setShininess(getShininess());

         v3 = getEmission(); 
         v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        _materialChunk->setEmission(v4);
        
        _materialChunk->setLit(getLit());
        _materialChunk->setColorMaterial(getColorMaterial());
    }
    endEditCP  (_materialChunk);

    state->addChunk(_materialChunk);

    if(isTransparent())
    {
        state->addChunk(_blendChunk);
    }

    addChunks(state); // XXX DR This is a hack. Should call Inherited

    return state;
}

void SimpleMaterial::rebuildState(void)
{
    Color3f v3;
    Color4f v4;
    Real32  alpha = 1.f - getTransparency();

    if(_pState != NullFC)
    {
        _pState->clearChunks();
    }
    else
    {
        _pState = State::create();

        addRefCP(_pState);
    }

    prepareLocalChunks();

    beginEditCP(_materialChunk);
    {
        v3 = getAmbient();
        v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);

        _materialChunk->setAmbient(v4);

        v3 = getDiffuse();
        v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        
        _materialChunk->setDiffuse(v4);
        
        v3 = getSpecular();
        v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        
        _materialChunk->setSpecular(v4);
        
        _materialChunk->setShininess(getShininess());
        
        v3 = getEmission();
        v4.setValuesRGBA(v3[0], v3[1], v3[2], alpha);
        
        _materialChunk->setEmission(v4);
        
        _materialChunk->setLit(getLit());
        _materialChunk->setColorMaterial(getColorMaterial());
    }
    endEditCP  (_materialChunk);

    _pState->addChunk(_materialChunk);

    if(isTransparent())
    {
        _pState->addChunk(_blendChunk);
    }

    addChunks(_pState); // XXX DR This is a hack. Should call Inherited
}

bool SimpleMaterial::isTransparent(void) const
{
    return ((getTransparency() > Eps) || (Inherited::isTransparent()));
}

/*------------------------------- dump ----------------------------------*/

void SimpleMaterial::dump(      UInt32    uiIndent,
                          const BitVector OSG_CHECK_ARG(bvFlags )) const
{

    SimpleMaterialPtr thisP(*this);

    thisP.dump(uiIndent, FCDumpFlags::RefCount);

    indentLog(uiIndent, PLOG);
    PLOG << "SimpleMaterial at " << this << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tambient: " << getAmbient() << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tdiffuse: " << getDiffuse()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tspecular: " << getSpecular()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tshininess: " << getShininess()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\temission: " << getEmission()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\ttransparency: " << getTransparency()  << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tlit: " << getLit() << std::endl;

    indentLog(uiIndent, PLOG);
    PLOG << "\tChunks: " << std::endl;

    for(MFStateChunkPtr::const_iterator i = _mfChunks.begin();
            i != _mfChunks.end(); i++)
    {
        indentLog(uiIndent, PLOG);
        PLOG << "\t" << *i << std::endl;
    }

    indentLog(uiIndent, PLOG);
    PLOG << "SimpleMaterial end " << this << std::endl;
}

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id:$";
    static Char8 cvsid_hpp       [] = OSGSIMPLEMATERIAL_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGSIMPLEMATERIAL_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGSIMPLEMATERIALFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

