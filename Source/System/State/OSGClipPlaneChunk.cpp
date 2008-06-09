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

#include <OSGDrawActionBase.h>
#include <OSGRenderAction.h>
#include <OSGCamera.h>

#include "OSGClipPlaneChunk.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::ClipPlaneChunk
    \ingroup GrpSystemState
    
See \ref PageSystemClipPlaneChunk for a description.

This chunk wraps glCLipPlane() (osg::ClipPlaneChunk::_sfEquation) and
glEnable(GL_CLIP_PLANEi) (osg::ClipPlaneChunk::_sfEnable). The coordinate
system the plane is in is defined by osg::ClipPlaneChunk::_sfBeacon.

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/
   
StateChunkClass ClipPlaneChunk::_class("ClipPlane", 6);

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/
   
/*----------------------- constructors & destructors ----------------------*/

ClipPlaneChunk::ClipPlaneChunk(void) :
    Inherited()
{
}

ClipPlaneChunk::ClipPlaneChunk(const ClipPlaneChunk &source) :
    Inherited(source)
{
}

ClipPlaneChunk::~ClipPlaneChunk(void)
{
}

/*----------------------- Chunk Class Access -----------------------------*/

const StateChunkClass *ClipPlaneChunk::getClass(void) const
{
     return &_class;
}

/*------------------------------- Sync -----------------------------------*/

void ClipPlaneChunk::changed(BitVector, UInt32)
{
}

/*------------------------------ Output ----------------------------------*/

void ClipPlaneChunk::dump(      UInt32    , 
                          const BitVector) const
{
    SLOG << "Dump ClipPlaneChunk NI" << std::endl;
}


/*------------------------------ State ------------------------------------*/

void ClipPlaneChunk::activate(DrawActionBase *action, UInt32 idx)
{
    RenderAction *ra = dynamic_cast<RenderAction *>(action);

    Matrix beaconMat;

    if(ra != NULL)
    {
        beaconMat = ra->top_matrix();
    }
    else
    {
        beaconMat = action->getActNode()->getToWorld();
    }


    Matrix cameraMat = action->getCameraToWorld();

    cameraMat.invert();

    if(getBeacon() != NullFC) 
    {
        getBeacon()->getToWorld(beaconMat);
    }
    else
    {
        SWARNING << "NO beacon" << std::endl;
    }

    cameraMat.mult(beaconMat);

    if(getEnable())
    {
        GLdouble glEq[4];
        const Vec4f   & eq = getEquation();
        glEq[0] = eq[0];
        glEq[1] = eq[1];
        glEq[2] = eq[2];
        glEq[3] = eq[3];
        
        glPushMatrix();
        glLoadMatrixf(cameraMat.getValues());
        
        glClipPlane( GL_CLIP_PLANE0 + idx, glEq);
        glEnable( GL_CLIP_PLANE0 + idx);
        
        glPopMatrix();
    }
}




void ClipPlaneChunk::changeFrom(DrawActionBase *action, StateChunk * old_chunk, 
                                UInt32 idx)
{
    ClipPlaneChunk const *old = dynamic_cast<ClipPlaneChunk const*>(old_chunk);

    // change from me to me?
    // this assumes I haven't changed in the meantime. is that a valid assumption?
    if(old == this)
        return;


    RenderAction *ra = dynamic_cast<RenderAction *>(action);

    Matrix beaconMat;

    if(ra != NULL)
    {
        beaconMat = ra->top_matrix();
    }
    else
    {
        beaconMat = action->getActNode()->getToWorld();
    }


    Matrix cameraMat = action->getCameraToWorld();

    cameraMat.invert();

    if (getBeacon() != NullFC) 
    {
        getBeacon()->getToWorld(beaconMat);
    }
    else
    {
        SWARNING << "ClipPlaneChunk::changeFrom: NO beacon" << std::endl;
    }

    cameraMat.mult(beaconMat);

    if(getEnable() != old->getEnable() ||
       getBeacon() != old->getBeacon())
    {
        if(getEnable())
        {
            GLdouble glEq[4];
            const Vec4f   & eq = getEquation();
            glEq[0] = eq[0];
            glEq[1] = eq[1];
            glEq[2] = eq[2];
            glEq[3] = eq[3];
            
            glPushMatrix();
            glLoadMatrixf(cameraMat.getValues());
        
            glClipPlane( GL_CLIP_PLANE0 + idx, glEq);
            glEnable( GL_CLIP_PLANE0 + idx);
    
            glPopMatrix();
        }
        else
        {
            glDisable( GL_CLIP_PLANE0 + idx);
        }
    }
    else
    {
        ;//SWARNING << " - are the SAME" <<  std::endl;
    }
}


void ClipPlaneChunk::deactivate(DrawActionBase *, UInt32 idx)
{
    if(getEnable())
    {
        glDisable( GL_CLIP_PLANE0 + idx);
    }
}


/*-------------------------- Comparison -----------------------------------*/

Real32 ClipPlaneChunk::switchCost(StateChunk *OSG_CHECK_ARG(chunk))
{
    return 0;
}

bool ClipPlaneChunk::operator < (const StateChunk &other) const
{
    return this < &other;
}

bool ClipPlaneChunk::operator == (const StateChunk &other) const
{
    ClipPlaneChunk const *tother = dynamic_cast<ClipPlaneChunk const*>(&other);

    if(!tother)
        return false;

    if(getEnable() != tother->getEnable())
        return false;

    if(getEquation() != tother->getEquation())
        return false;

    if(getBeacon() != tother->getBeacon())
        return false;
    
    return true;
}

bool ClipPlaneChunk::operator != (const StateChunk &other) const
{
    return ! (*this == other);
}


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177)
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: FCTemplate_cpp.h,v 1.13 2002/06/01 10:37:25 vossg Exp $";
    static Char8 cvsid_hpp       [] = OSGCLIPPLANECHUNK_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGCLIPPLANECHUNK_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGCLIPPLANECHUNKFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

