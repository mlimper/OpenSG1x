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
#include <OSGGLU.h>

#include "OSGDrawActionBase.h"

#include "OSGTextureChunk.h"

#include "OSGTexGenChunk.h"
#include "OSGCamera.h"
#include "OSGViewport.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::TexGenChunk
    \ingroup GrpSystemState

See \ref PageSystemTexGenChunk for a description.

This chunk wraps glTexGen(). All 4 coordinates are wrapped in a single chunk
by using separate gen funcs for each variable (osg::TexGenChunk::_sfGenFuncS, 
osg::TexGenChunk::_sfGenFuncT, osg::TexGenChunk::_sfGenFuncR, 
osg::TexGenChunk::_sfGenFuncQ). The default for each of these is GL_NONE, or
off. The optional plane parameters are stored in
osg::TexGenChunk::_sfGenFuncSPlane, 
osg::TexGenChunk::_sfGenFuncTPlane, osg::TexGenChunk::_sfGenFuncRPlane and
osg::TexGenChunk::_sfGenFuncQPlane.

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

StateChunkClass TexGenChunk::_class("TexGen", osgMaxTexCoords);

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void TexGenChunk::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

TexGenChunk::TexGenChunk(void) :
    Inherited()
{
}

TexGenChunk::TexGenChunk(const TexGenChunk &source) :
    Inherited(source)
{
}

TexGenChunk::~TexGenChunk(void)
{
}

/*------------------------- Chunk Class Access ---------------------------*/

const StateChunkClass *TexGenChunk::getClass(void) const
{
    return &_class;
}

/*------------------------------- Sync -----------------------------------*/

void TexGenChunk::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*------------------------------ Output ----------------------------------*/

void TexGenChunk::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump TexGenChunk NI" << std::endl;
}


/*------------------------------ State ------------------------------------*/

static inline void setGenFunc(GLenum coord, GLenum gen, GLenum func, 
                              Vec4f &plane, NodePtr beacon, Matrix &cameraMat)
{
    if(beacon != NullFC)
    {
        Matrix beaconMat;
        beacon->getToWorld(beaconMat);
        beaconMat.multLeft(cameraMat);
        glPushMatrix();
        glLoadMatrixf(beaconMat.getValues());
        glTexGenfv(coord, 
                   GL_EYE_PLANE, 
                   static_cast<GLfloat*>(plane.getValues()));
        glTexGeni(coord, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glPopMatrix();
        glEnable(gen);
    }        
    else if(func == GL_EYE_LINEAR)
    {
        glPushMatrix();
        glLoadIdentity();
        glTexGenfv(coord, 
                   GL_EYE_PLANE, 
                   static_cast<GLfloat*>(plane.getValues()));
        glTexGeni(coord, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glPopMatrix();
        glEnable(gen);
    }        
    else if(func != GL_NONE)                                         
    {                                                                   
        glTexGeni(coord, GL_TEXTURE_GEN_MODE, func);
        
        if(func == GL_OBJECT_LINEAR)
        {
            glTexGenfv(coord, 
                       GL_OBJECT_PLANE, 
                       static_cast<GLfloat*>(plane.getValues()));
        }
        else if(func == GL_EYE_LINEAR)
        {
            glTexGenfv(coord, 
                       GL_EYE_PLANE, 
                       static_cast<GLfloat*>(plane.getValues()));
        }
            
        glEnable(gen);
    }
}

void TexGenChunk::activate(DrawActionBase *action, UInt32 idx )
{
    glErr("TexGenChunk::activate precheck");
 
    Window *win = action->getWindow();   

    Real32 ntexcoords;
    if((ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_COORDS_ARB)) ==
       Window::unknownConstant
      )
    {
        ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(ntexcoords == Window::unknownConstant)
            ntexcoords = 1.0f;
    }

    if(idx >= static_cast<UInt32>(ntexcoords))
    {
#ifdef OSG_DEBUG
        FWARNING(("TexGenChunk::activate: Trying to bind texcoord unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, ntexcoords));
#endif
        return;        
    }
        
    TextureChunk::activateTexture(win, idx);

    FDEBUG(("TexGenChunk::activate\n"));

    Matrix cameraMat;   
    Viewport *vp = action->getViewport();
    if(vp != NULL && action->getCamera() != NULL)
    {
        action->getCamera()->getViewing(cameraMat, vp->getPixelWidth(), 
                                        vp->getPixelHeight());
    }

    // genfuncs
    setGenFunc(GL_S, GL_TEXTURE_GEN_S, getGenFuncS(), getGenFuncSPlane(),
                getSBeacon(), cameraMat);
    glErr("TexGenChunk::activateS");
    setGenFunc(GL_T, GL_TEXTURE_GEN_T, getGenFuncT(), getGenFuncTPlane(),
                getTBeacon(), cameraMat);
    glErr("TexGenChunk::activateT");
    setGenFunc(GL_R, GL_TEXTURE_GEN_R, getGenFuncR(), getGenFuncRPlane(),
                getRBeacon(), cameraMat);
    glErr("TexGenChunk::activateR");
    setGenFunc(GL_Q, GL_TEXTURE_GEN_Q, getGenFuncQ(), getGenFuncQPlane(),
                getQBeacon(), cameraMat);
    glErr("TexGenChunk::activateQ");
}


static inline void changeGenFunc(GLenum oldfunc, NodePtr oldbeacon, 
                GLenum coord, GLenum gen, 
                GLenum func, Vec4f &plane, NodePtr beacon, Matrix &cameraMat)
{
    if(beacon != NullFC)
    {
        Matrix beaconMat;
        beacon->getToWorld(beaconMat);
        beaconMat.multLeft(cameraMat);
        glPushMatrix();
        glLoadMatrixf(beaconMat.getValues());
        glTexGenfv(coord, 
                   GL_EYE_PLANE, 
                   static_cast<GLfloat*>(plane.getValues()));
        glTexGeni(coord, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glPopMatrix();
        if(oldfunc == GL_NONE && oldbeacon == NullFC) 
            glEnable(gen);
    }
    else if(func != GL_NONE)                                         
    {                                                                   
        glTexGeni(coord, GL_TEXTURE_GEN_MODE, func);        
        
        if(func == GL_OBJECT_LINEAR)
        {
            glTexGenfv(coord, 
                       GL_OBJECT_PLANE, 
                       static_cast<GLfloat*>(plane.getValues()));
        }
        else if(func == GL_EYE_LINEAR)
        {
            glTexGenfv(coord, 
                       GL_EYE_PLANE,
                       static_cast<GLfloat*>(plane.getValues()));
        }
            
        if(oldfunc == GL_NONE && oldbeacon == NullFC) 
            glEnable(gen);
    }
    else if(oldfunc != GL_NONE || oldbeacon != NullFC) 
        glDisable(gen);  
}

void TexGenChunk::changeFrom(   DrawActionBase *action, 
                                StateChunk     *old   , 
                                UInt32          idx)
{
    // change from me to me?
    // this assumes I haven't changed in the meantime. 
    // is that a valid assumption?
    // No, for TexGen it's not, as TexGen depends on the current 
    // toWorld matrix!!!
    // if(old == this)
    //     return;

    TexGenChunk *oldp      = dynamic_cast<TexGenChunk *>(old);
    
    // If the old one is not a texgen chunk, deactivate it and activate
    // ourselves
    if(!oldp)
    {
        old->deactivate(action, idx);
        activate(action, idx);
        return;
    }

    glErr("TexGenChunk::changeFrom precheck");

    Window *win = action->getWindow();   

    Real32 ntexcoords;
    if((ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_COORDS_ARB)) ==
       Window::unknownConstant
      )
    {
        ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(ntexcoords == Window::unknownConstant)
            ntexcoords = 1.0f;
    }

    if(idx >= static_cast<UInt32>(ntexcoords))
    {
#ifdef OSG_DEBUG
        FWARNING(("TexGenChunk::changeFrom: Trying to bind texcoord unit "
                  "%d, but Window %p only supports %d!\n",
                  idx, win, ntexcoords));
#endif
        return;        
    }
 
    Matrix cameraMat;   
    Viewport *vp = action->getViewport();
    if(vp != NULL && action->getCamera() != NULL)
    {
        action->getCamera()->getViewing(cameraMat, vp->getPixelWidth(), 
                                        vp->getPixelHeight());
    }

    TextureChunk::activateTexture(win, idx);

    changeGenFunc(oldp->getGenFuncS(), oldp->getSBeacon(), GL_S, 
                GL_TEXTURE_GEN_S, 
                getGenFuncS(), getGenFuncSPlane(), getSBeacon(), cameraMat);
    changeGenFunc(oldp->getGenFuncT(), oldp->getTBeacon(), GL_T, 
                GL_TEXTURE_GEN_T, 
                getGenFuncT(), getGenFuncTPlane(), getTBeacon(), cameraMat);
    changeGenFunc(oldp->getGenFuncR(), oldp->getRBeacon(), GL_R, 
                GL_TEXTURE_GEN_R, 
                getGenFuncR(), getGenFuncRPlane(), getRBeacon(), cameraMat);
    changeGenFunc(oldp->getGenFuncQ(), oldp->getQBeacon(), GL_Q, 
                GL_TEXTURE_GEN_Q, 
                getGenFuncQ(), getGenFuncQPlane(), getQBeacon(), cameraMat);

    glErr("TexGenChunk::changeFrom");
}

void TexGenChunk::deactivate(DrawActionBase *action, UInt32 idx)
{
    glErr("TexGenChunk::deactivate precheck");

    Window *win = action->getWindow();   

    Real32 ntexcoords;
    if((ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_COORDS_ARB)) ==
       Window::unknownConstant
      )
    {
        ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(ntexcoords == Window::unknownConstant)
            ntexcoords = 1.0f;
    }

    if(idx >= static_cast<UInt32>(ntexcoords))
    {
#ifdef OSG_DEBUG
        FWARNING(("TexGenChunk::deactivate: Trying to bind texcoord unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, ntexcoords));
#endif
        return;        
    }

    TextureChunk::activateTexture(win, idx);

    if(getGenFuncS() != GL_NONE || getSBeacon() != NullFC)
        glDisable(GL_TEXTURE_GEN_S);

    if(getGenFuncT() != GL_NONE || getTBeacon() != NullFC)
        glDisable(GL_TEXTURE_GEN_T);

    if(getGenFuncR() != GL_NONE || getRBeacon() != NullFC)
        glDisable(GL_TEXTURE_GEN_R);

    if(getGenFuncQ() != GL_NONE || getQBeacon() != NullFC)
        glDisable(GL_TEXTURE_GEN_Q);

    glErr("TexGenChunk::deactivate");
}

/*-------------------------- Comparison -----------------------------------*/

Real32 TexGenChunk::switchCost(StateChunk *OSG_CHECK_ARG(chunk))
{
    return 0;
}

bool TexGenChunk::operator < (const StateChunk &other) const
{
    return this < &other;
}

bool TexGenChunk::operator == (const StateChunk &other) const
{
    TexGenChunk const *tother = dynamic_cast<TexGenChunk const*>(&other);

    if(!tother)
        return false;

    if(tother == this)
        return true;

    return  getGenFuncS()   == tother->getGenFuncS() &&
            getGenFuncT()   == tother->getGenFuncT() &&
            getGenFuncR()   == tother->getGenFuncR() &&
            getGenFuncQ()   == tother->getGenFuncQ() &&
            // not quite right. needs only to be tested for genfuncs using them
            getGenFuncSPlane()  == tother->getGenFuncSPlane() &&
            getGenFuncTPlane()  == tother->getGenFuncTPlane() &&
            getGenFuncRPlane()  == tother->getGenFuncRPlane() &&
            getGenFuncQPlane()  == tother->getGenFuncQPlane() ;
}

bool TexGenChunk::operator != (const StateChunk &other) const
{
    return ! (*this == other);
}


#ifdef __sgi
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: $";
    static Char8 cvsid_hpp[] = OSGTEXGENCHUNK_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGTEXGENCHUNK_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGTEXGENCHUNKFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

