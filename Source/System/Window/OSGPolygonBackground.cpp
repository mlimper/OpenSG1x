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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>
#include <OSGMaterial.h>

#include "OSGPolygonBackground.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::PolygonBackground
A Background that renders a single polygon using the specified material.
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void PolygonBackground::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

PolygonBackground::PolygonBackground(void) :
    Inherited()
{
}

PolygonBackground::PolygonBackground(const PolygonBackground &source) :
    Inherited(source)
{
}

PolygonBackground::~PolygonBackground(void)
{
}

/*----------------------------- class specific ----------------------------*/

Real32 PolygonBackground::mapCoordinate(Real32 val, Real32 max, bool norm)
{
    if(val >= 0)
    {
        if (norm)
            val *= max;
    }
    else
    {
        val += 1;
        
        if (norm)
            val *= max;
        
        val = max + val;
    }
   
    return val;
}

void PolygonBackground::clear(DrawActionBase *act, Viewport *port)
{
    if (port->getPixelWidth()  == 0 ||
        port->getPixelHeight() == 0 ) 
    {   
        FWARNING(("Port has zero size: nothing to render to!\n"));
        return;
    }
        
    if (getPositions().getSize() == 0 ||
        getPositions().getSize() != getTexCoords().getSize())
    {
        FWARNING(("PolygonBackground::clear: positions and texcoords have "
                  "different/ invalid sizes (%d vs. %d)!\n", 
                  getPositions().getSize(), getTexCoords().getSize()));
        return;
    }
       
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);

    Real32 aspectX = 1.0f, aspectY = 1.0f;
    
    if (getAspectHeight() && getAspectWidth())
    {
        aspectX = ((Real32)port->getPixelHeight()/getAspectHeight()) /
                  ((Real32)port->getPixelWidth() / getAspectWidth());
    }
 
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    glScalef(aspectX, aspectY, 1);
    glOrtho(0, port->getPixelWidth(), 0, port->getPixelHeight(), 0, 1);

    getMaterial()->getState()->activate(act);
    
    Vec3f *tc  = &getTexCoords()[0];
    Pnt2f *pos = &getPositions()[0];
    
    glBegin(GL_POLYGON);
    
    for (UInt16 i=0; i<getPositions().size(); i++)
    {
        glTexCoord3fv( tc[i].getValues() );
        glVertex2f( mapCoordinate(pos[i][0], Real32(port->getPixelWidth()),
                                             getNormalizedX()),
                    mapCoordinate(pos[i][1], Real32(port->getPixelHeight()),
                                             getNormalizedY()) );
    }

    glEnd();
    
    getMaterial()->getState()->deactivate(act);

    glScalef(1, 1, 1);

    glClear(GL_DEPTH_BUFFER_BIT);
     
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
}

void PolygonBackground::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void PolygonBackground::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump PolygonBackground NI" << std::endl;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGPolygonBackground.cpp,v 1.3 2006/04/25 05:49:03 dirk Exp $";
    static Char8 cvsid_hpp       [] = OSGPOLYGONBACKGROUNDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGPOLYGONBACKGROUNDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGPOLYGONBACKGROUNDFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

