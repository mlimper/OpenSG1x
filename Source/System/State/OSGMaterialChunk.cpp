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

#include <OSGGL.h>

#include "OSGMaterialChunk.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::MaterialChunk
    \ingroup GrpSystemState

See \ref PageSystemMaterialChunk for details.

This chunk wraps glMaterial() (osg::MaterialChunk::_sfDiffuse,
osg::MaterialChunk::_sfSpecular, osg::MaterialChunk::_sfEmission,
osg::MaterialChunk::_sfShininess, osg::MaterialChunk::_sfAmbient),
glEnable(GL_LIGHTING)(osg::MaterialChunk::_sfLit) and glColorMaterial()
(osg::MaterialChunk::_sfColorMaterial).

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

StateChunkClass MaterialChunk::_class("Material");

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void MaterialChunk::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

MaterialChunk::MaterialChunk(void) :
    Inherited()
{
}

MaterialChunk::MaterialChunk(const MaterialChunk &source) :
    Inherited(source)
{
}

MaterialChunk::~MaterialChunk(void)
{
}

/*------------------------- Chunk Class Access ---------------------------*/

const StateChunkClass *MaterialChunk::getClass(void) const
{
    return &_class;
}

/*------------------------------- Sync -----------------------------------*/

void MaterialChunk::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*------------------------------ Output ----------------------------------*/

void MaterialChunk::dump(      UInt32    OSG_CHECK_ARG(uiIndent),
                         const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    SLOG << "Dump MaterialChunk NI" << std::endl;
}

/*------------------------------ State ------------------------------------*/

void MaterialChunk::activate(DrawActionBase *, UInt32)
{
    GLenum target;
    
    if(getLit())
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }

    if(getBackMaterial())
    {
        target = GL_FRONT_AND_BACK;
    }
    else
    {
        target = GL_FRONT;
    }
        
    if(getColorMaterial() == GL_NONE)
    {
        glDisable(GL_COLOR_MATERIAL);
    }
    else
    {
        glColorMaterial(target, getColorMaterial());
        if(getBackMaterial())
        {
            glColorMaterial(GL_BACK, getBackColorMaterial());
        }
        glEnable(GL_COLOR_MATERIAL);
    }

    glColor4fv(_sfDiffuse.getValue().getValuesRGBA());

    if(getLit())
    {
        glMaterialfv(target, GL_DIFFUSE,
                      _sfDiffuse.getValue().getValuesRGBA());
        glMaterialfv(target, GL_AMBIENT,
                      _sfAmbient.getValue().getValuesRGBA());
        glMaterialfv(target, GL_SPECULAR,
                      _sfSpecular.getValue().getValuesRGBA());
        glMaterialfv(target, GL_EMISSION,
                      _sfEmission.getValue().getValuesRGBA());
        glMaterialf (target, GL_SHININESS, _sfShininess.getValue());

        if(getBackMaterial())
        {
            glMaterialfv(GL_BACK, GL_DIFFUSE,
                          _sfBackDiffuse.getValue().getValuesRGBA());
            glMaterialfv(GL_BACK, GL_AMBIENT,
                          _sfBackAmbient.getValue().getValuesRGBA());
            glMaterialfv(GL_BACK, GL_SPECULAR,
                          _sfBackSpecular.getValue().getValuesRGBA());
            glMaterialfv(GL_BACK, GL_EMISSION,
                          _sfBackEmission.getValue().getValuesRGBA());
            glMaterialf (GL_BACK, GL_SHININESS, _sfBackShininess.getValue());
        }
    }
}

void MaterialChunk::changeFrom(DrawActionBase *, StateChunk * old_chunk, UInt32)
{
    MaterialChunk const *old = dynamic_cast<MaterialChunk const*>(old_chunk);

    // change from me to me?
    // this assumes I haven't changed in the meantime. is that a valid assumption?
    if(old == this)
    {
        // Reset it, as Geometry colors might have changed it
        if(getColorMaterial() != GL_NONE || 
           (getBackMaterial() && getBackColorMaterial() != GL_NONE)
          )
            glColor4fv(_sfDiffuse.getValue().getValuesRGBA());

        return;
    }

    if(getColorMaterial() == GL_NONE && getBackColorMaterial() == GL_NONE &&
       (old->getColorMaterial()     != GL_NONE || 
        old->getBackColorMaterial() != GL_NONE
       )
      )
    {
        glDisable(GL_COLOR_MATERIAL);
    }
    else if(getColorMaterial()     != old->getColorMaterial() || 
            getBackColorMaterial() != old->getBackColorMaterial()
           )
    {
        if(getBackMaterial())
        {
            glColorMaterial(GL_FRONT, getColorMaterial());
            glColorMaterial(GL_BACK,  getBackColorMaterial());
        }
        else
        {
             glColorMaterial(GL_FRONT_AND_BACK, getColorMaterial());
      
        }
        glEnable(GL_COLOR_MATERIAL);
    }
    
    if(getColorMaterial() != GL_NONE || getBackColorMaterial() != GL_NONE)
        glColor4fv(_sfDiffuse.getValue().getValuesRGBA());

    if(getLit() && ! old->getLit())
        glEnable(GL_LIGHTING);
    else if(! getLit() && old->getLit())
        glDisable(GL_LIGHTING);

    if(getLit())
    {
        GLenum target;

        if(getBackMaterial())
        {
            target = GL_FRONT_AND_BACK;
        }
        else
        {
            target = GL_FRONT;
        }
        
        glMaterialfv(target, GL_DIFFUSE,
                      _sfDiffuse.getValue().getValuesRGBA());
        glMaterialfv(target, GL_AMBIENT,
                      _sfAmbient.getValue().getValuesRGBA());
        glMaterialfv(target, GL_SPECULAR,
                      _sfSpecular.getValue().getValuesRGBA());
        glMaterialfv(target, GL_EMISSION,
                      _sfEmission.getValue().getValuesRGBA());
        // adjust shininess only if it differs enough
        if(osgabs(_sfShininess.getValue() - old->getShininess()) > 1e-4)
            glMaterialf( target, GL_SHININESS, _sfShininess.getValue());
        
        if(getBackMaterial())
        {       
            glMaterialfv(GL_BACK, GL_DIFFUSE,
                          _sfBackDiffuse.getValue().getValuesRGBA());
            glMaterialfv(GL_BACK, GL_AMBIENT,
                          _sfBackAmbient.getValue().getValuesRGBA());
            glMaterialfv(GL_BACK, GL_SPECULAR,
                          _sfBackSpecular.getValue().getValuesRGBA());
            glMaterialfv(GL_BACK, GL_EMISSION,
                          _sfBackEmission.getValue().getValuesRGBA());
            // adjust shininess only if it differs enough
            if(osgabs(_sfBackShininess.getValue() - 
                      old->getBackShininess()
                     ) > 1e-4)
                glMaterialf( GL_BACK, GL_SHININESS, _sfBackShininess.getValue());
        }
    }
}

void MaterialChunk::deactivate(DrawActionBase *, UInt32)
{

    if(getLit())
        glDisable(GL_LIGHTING);

    if(getColorMaterial() != GL_NONE)
        glDisable(GL_COLOR_MATERIAL);
}


/*-------------------------- Comparison -----------------------------------*/

Real32 MaterialChunk::switchCost(StateChunk *OSG_CHECK_ARG(chunk))
{
    return 0;
}

bool MaterialChunk::isTransparent(void) const
{
    return(getDiffuse()[3] <(1. - Eps));
}


bool MaterialChunk::operator <(const StateChunk &other) const
{
    return this < &other;
}

bool MaterialChunk::operator ==(const StateChunk &other) const
{
    MaterialChunk const *tother = dynamic_cast<MaterialChunk const*>(&other);

    if(!tother)
        return false;

    if(tother == this)
        return true;

    if(!getAmbient  ().equals(tother->getAmbient  (), Eps) ||
       !getDiffuse  ().equals(tother->getDiffuse  (), Eps) ||
       !getSpecular ().equals(tother->getSpecular (), Eps) ||
       !getEmission ().equals(tother->getEmission (), Eps) ||
        getShininess()     != tother->getShininess()       ||
        getLit()           != tother->getLit()             ||
        getColorMaterial() != tother->getColorMaterial()
      )
        return false;

    return true;
}

bool MaterialChunk::operator !=(const StateChunk &other) const
{
    return !(*this == other);
}


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
    static Char8 cvsid_cpp       [] = "@(#)$Id: FCTemplate_cpp.h,v 1.13 2002/06/01 10:37:25 vossg Exp $";
    static Char8 cvsid_hpp       [] = OSGMATERIALCHUNK_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGMATERIALCHUNK_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGMATERIALCHUNKFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


