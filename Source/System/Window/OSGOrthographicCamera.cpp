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
#include <OSGMatrixUtility.h>

#include "OSGOrthographicCamera.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::OrthographicCamera
An orthographic camera.
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void OrthographicCamera::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

OrthographicCamera::OrthographicCamera(void) :
    Inherited()
{
}

OrthographicCamera::OrthographicCamera(const OrthographicCamera &source) :
    Inherited(source)
{
}

OrthographicCamera::~OrthographicCamera(void)
{
}

/*----------------------------- class specific ----------------------------*/

void OrthographicCamera::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------- your_category---------------------------------*/

void OrthographicCamera::draw(      DrawAction *OSG_CHECK_ARG(action), 
                             const Viewport&   OSG_CHECK_ARG(port ))
{
}

void OrthographicCamera::getProjection(Matrix& result, 
    UInt32 width, UInt32 height)
{
    Real32 vs = getVerticalSize();

    // catch some illegal cases
    if(vs < 0 || width == 0 || height == 0)
    {
        result.setIdentity();
        return;
    }

    Real32 a = width /(Real32) height * getAspect();
    
    MatrixOrthogonal(result, -vs / 2 * a,  vs / 2 * a, 
                             -vs / 2, vs / 2,
                             getNear(), getFar());
}
    

void OrthographicCamera::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump OrthographicCamera NI" << std::endl;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGOrthographicCamera.cpp,v 1.1 2005/09/26 00:08:18 dirk Exp $";
    static Char8 cvsid_hpp       [] = OSGORTHOGRAPHICCAMERABASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGORTHOGRAPHICCAMERABASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGORTHOGRAPHICCAMERAFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

