/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
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

// Forget everything if we're not doing a windows compile
#ifdef WIN32

#include "OSGWIN32Window.h"

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace
{
    static char cvsid_cpp[] = "@(#)$Id: $";
    static char cvsid_hpp[] = OSGWIN32WINDOW_HEADER_CVSID;
    static char cvsid_inl[] = OSGWIN32WINDOW_INLINE_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

/*! \class osg::WIN32Window
The class for WIN32 windows. 	
*/

/*----------------------- constructors & destructors ----------------------*/

//! Constructor

WIN32Window::WIN32Window(void) :
    Inherited()
{
}

//! Copy Constructor

WIN32Window::WIN32Window(const WIN32Window &source) :
    Inherited(source)
{
}

//! Destructor

WIN32Window::~WIN32Window(void)
{
    if(getHglrc() != NULL)
        wglDeleteContext(getHglrc());
}

/*----------------------------- class specific ----------------------------*/

//! initialize the static features of the class, e.g. action callbacks

void WIN32Window::initMethod (void)
{
}

//! react to field changes

void WIN32Window::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

//! output the instance for debug purposes

void WIN32Window::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump WIN32Window NI" << endl;
}

/*-------------------------- your_category---------------------------------*/

void (*WIN32Window::getFunctionByName( const Char8 *s ))(void)
{
    return (void(*)(void))wglGetProcAddress(s);
}

// init the window: create the HDC and context
void WIN32Window::init( void )
{
    setHdc(GetDC(getHwnd()));

    if(getHglrc() == NULL )
    {
        setHglrc(wglCreateContext(getHdc()));
        
        if(getHglrc() == NULL)
            cout << "WIN32Window::init: failed: " << GetLastError() << endl;        
    }

    ReleaseDC(getHwnd(),getHdc());
    activate();
    setupGL();
    deactivate();
}

// activate the window: bind the OGL context
void WIN32Window::activate( void )
{    
    setHdc(GetDC(getHwnd()));

    if(!wglMakeCurrent(getHdc(), getHglrc() ) )
    {
        cout << "WIN32Window::activate: failed: " << GetLastError() << endl;        
    }
}

void WIN32Window::deactivate ( void )
{
    // unbind the context
    wglMakeCurrent(NULL, NULL);

    // release the hardware device context
    ReleaseDC(getHwnd(),getHdc());
}

// swap front and back buffers
void WIN32Window::swap( void )
{
    SwapBuffers(getHdc());
}

#endif // WIN32
