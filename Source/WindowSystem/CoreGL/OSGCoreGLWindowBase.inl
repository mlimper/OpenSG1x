/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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
 **     class CoreGLWindow!
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/

#include <OSGConfig.h>

OSG_BEGIN_NAMESPACE


//! access the type of the class
inline
OSG::FieldContainerType &CoreGLWindowBase::getClassType(void)
{
    return _type; 
} 

//! access the numerical type of the class
inline
OSG::UInt32 CoreGLWindowBase::getClassTypeId(void) 
{
    return _type.getId(); 
} 

//! create a new instance of the class
inline
CoreGLWindowPtr CoreGLWindowBase::create(void) 
{
    CoreGLWindowPtr fc; 

    if(getClassType().getPrototype() != OSG::NullFC) 
    {
        fc = CoreGLWindowPtr::dcast(
            getClassType().getPrototype()-> shallowCopy()); 
    }
    
    return fc; 
}

//! create an empty new instance of the class, do not copy the prototype
inline
CoreGLWindowPtr CoreGLWindowBase::createEmpty(void) 
{ 
    CoreGLWindowPtr returnValue; 
    
    newPtr(returnValue); 

    return returnValue; 
}


/*------------------------------ get -----------------------------------*/

//! Get the CoreGLWindow::_sfContext field.
inline
SFCGLContextObj *CoreGLWindowBase::getSFContext(void)
{
    return &_sfContext;
}


//! Get the value of the CoreGLWindow::_sfContext field.
inline
CGLContextObj &CoreGLWindowBase::getContext(void)
{
    return _sfContext.getValue();
}

//! Get the value of the CoreGLWindow::_sfContext field.
inline
const CGLContextObj &CoreGLWindowBase::getContext(void) const
{
    return _sfContext.getValue();
}

//! Set the value of the CoreGLWindow::_sfContext field.
inline
void CoreGLWindowBase::setContext(const CGLContextObj &value)
{
    _sfContext.setValue(value);
}


OSG_END_NAMESPACE

#define OSGCOREGLWINDOWBASE_INLINE_CVSID "@(#)$Id: OSGCoreGLWindowBase.inl,v 1.1 2006/09/08 10:09:06 pdaehne Exp $"
