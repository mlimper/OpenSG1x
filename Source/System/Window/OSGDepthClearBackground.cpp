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

#include <OSGFieldContainer.h>
#include <OSGNode.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGWindow.h"
#include "OSGBackground.h"
#include "OSGDepthClearBackground.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::DepthClearBackground
    \ingroup GrpSystemWindowBackgrounds

A depth-clear background, see \ref PageSystemWindowBackgroundDepthClear for a
description.

*/
/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void DepthClearBackground::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

DepthClearBackground::DepthClearBackground(void) :
    Inherited()
{
}

DepthClearBackground::DepthClearBackground(const DepthClearBackground &source) :
    Inherited(source)
{
}

DepthClearBackground::~DepthClearBackground(void)
{
}

void DepthClearBackground::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------- your_category---------------------------------*/

void DepthClearBackground::clear(DrawActionBase *, Viewport *)
{
    Int32 bit = getClearStencilBit();
    
    if (bit >= 0)
    {
        glClearStencil(bit);

        if (getClearDepth())
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        else
            glClear(GL_STENCIL_BUFFER_BIT);
    }
    else
    {
        if (getClearDepth())
            glClear(GL_DEPTH_BUFFER_BIT);
    }
}

/*------------------------------- dump ----------------------------------*/

void DepthClearBackground::dump(      UInt32    OSG_CHECK_ARG(uiIndent), 
                             const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    SLOG << "Dump DepthClearBackground NI" << std::endl;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGDepthClearBackground.cpp,v 1.2 2006/12/06 17:58:08 yjung Exp $";
    static Char8 cvsid_hpp       [] = OSGDEPTHCLEARBACKGROUND_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGDEPTHCLEARBACKGROUND_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGDEPTHCLEARBACKGROUNDFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


