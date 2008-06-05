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

#include <OSGGL.h>
#include <OSGGLU.h>

#include <OSGNodePtr.h>
#include <OSGViewport.h>
#include <OSGImage.h>
#include <OSGTextureChunk.h>


#include "OSGTextureGrabBackground.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::TextureGrabBackground
    \ingroup GrpSystemWindowBackgrounds

A background that grabs what's underneath it into a texture before it clears 
the viewport to a constant color.    

The color of the background is given by the _sfColor field, the texture to grab
into .

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void TextureGrabBackground::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

TextureGrabBackground::TextureGrabBackground(void) :
    Inherited()
{
}

TextureGrabBackground::TextureGrabBackground(const TextureGrabBackground &source) :
    Inherited(source)
{
}

TextureGrabBackground::~TextureGrabBackground(void)
{
}

/*----------------------------- class specific ----------------------------*/

void TextureGrabBackground::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------- your_category---------------------------------*/

void TextureGrabBackground::clear(DrawActionBase *action, Viewport *port)
{   
    // grab the texture
    TextureChunkPtr t = getTexture();
    
    if(t == NullFC)       // No texture, no grab.
    {
        Inherited::clear(action, port);
        return;
    }
    
    Int32  pw = port->getPixelWidth(),
           ph = port->getPixelHeight();
    
    // Ignore empty viewports
    if(pw < 1 || ph < 1)
        return;
 
    ImagePtr i = t->getImage();
             
    // If image is smaller than 2x2, resize it to vp size
    // the 2x2 is because you can't create 0x0 images
    if((i->getWidth() <= 1 && i->getHeight() <= 1) ||
       (getAutoResize() && (osgabs(i->getWidth()  - pw) > 1 ||
                            osgabs(i->getHeight() - ph) > 1 )
       )
      )
    {
        i->set(i->getPixelFormat(), pw, ph);
        // Tell the texture...
        beginEditCP(t, TextureChunk::ImageFieldMask);
        endEditCP  (t, TextureChunk::ImageFieldMask);
    }
    
    UInt32 w = osgMin(Int32(i->getWidth()),  pw);
    UInt32 h = osgMin(Int32(i->getHeight()), ph);

    glErr("TextureGrabBackground::activate precheck");
    
    action->getWindow()->validateGLObject(t->getGLId());

    glErr("TextureGrabBackground::bind precheck");
    
    GLenum bindTarget = getBindTarget(), copyTarget = getCopyTarget();
    
    if(bindTarget == GL_NONE)
    {
       if(i->getDepth() > 1)
       {
            FWARNING(("TextureGrabBackground:: 3D textures not "
                        "supported for this window!\n"));
            Inherited::clear(action, port);
            return;
       }
       else if(h > 1)        bindTarget = GL_TEXTURE_2D;
       else                  bindTarget = GL_TEXTURE_1D;        
    }
    
    if(copyTarget == GL_NONE)
        copyTarget = bindTarget;
    
    glBindTexture(bindTarget,
                  action->getWindow()->getGLObjectId(t->getGLId()));

    glErr("TextureGrabBackground::copy precheck");

    if(copyTarget == GL_TEXTURE_3D)
    {
        FWARNING(("TextureGrabBackground:: grabbing to 3D textures not "
                  "supported yet!\n"));      
    }
    else if(copyTarget == GL_TEXTURE_1D)
    {
        glCopyTexSubImage1D(copyTarget, 0, 0, 
                            port->getPixelLeft(), port->getPixelBottom(), 
                            w);
    }
    else
    {
        glCopyTexSubImage2D(copyTarget, 0, 0, 0, 
                            port->getPixelLeft(), port->getPixelBottom(), 
                            w, h);
    }
    
    glErr("TextureGrabBackground::copy postcheck");
   
    glBindTexture(bindTarget, 0);
   
    // now do the clearing
    Inherited::clear(action, port);
}

/*------------------------------- dump ----------------------------------*/

void TextureGrabBackground::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump TextureGrabBackground NI" << std::endl;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: $";
    static Char8 cvsid_hpp       [] = OSGTEXTUREGRABBACKGROUNDBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGTEXTUREGRABBACKGROUNDBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGTEXTUREGRABBACKGROUNDFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

