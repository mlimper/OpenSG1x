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

// Forget everything if we're not doing a Mac OS X compile
#ifdef __APPLE__

#include "OSGCocoaWindow.h"

OSG_BEGIN_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::CocoaWindow
The class for MacOS X Cocoa windows. 	
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void CocoaWindow::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

CocoaWindow::CocoaWindow(void) :
    Inherited()
{
}

CocoaWindow::CocoaWindow(const CocoaWindow &source) :
    Inherited(source)
{
}

CocoaWindow::~CocoaWindow(void)
{
}

/*----------------------------- class specific ----------------------------*/

void CocoaWindow::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void CocoaWindow::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump CocoaWindow NI" << std::endl;
}


/*! Init the window: create the context and setup the OpenGL.
*/
void CocoaWindow::init(void)
{
    [getContext() makeCurrentContext];
    setupGL();
}

// activate the window: bind the OGL context
void CocoaWindow::activate( void )
{
    [getContext() makeCurrentContext];
}

// activate the window: bind the OGL context
void CocoaWindow::deactivate( void )
{
    [NSOpenGLContext clearCurrentContext];
}

// swap front and back buffers
void CocoaWindow::swap( void )
{
    [getContext() flushBuffer];
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGCocoaWindow.mm,v 1.1 2006/10/16 17:24:24 pdaehne Exp $";
    static Char8 cvsid_hpp       [] = OSGCOCOAWINDOWBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGCOCOAWINDOWBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGCOCOAWINDOWFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

OSG_END_NAMESPACE

#endif // __APPLE__
