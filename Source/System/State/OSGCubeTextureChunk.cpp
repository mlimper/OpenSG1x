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
#include <OSGGLEXT.h>

#include "OSGDrawActionBase.h"

#include "OSGCubeTextureChunk.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::CubeTextureChunk
    \ingroup GrpSystemState

See \ref PageSystemCubeTextureChunk for a description.

The CubeTexture is defined by 6 separate texture images. As it is derived from 
the standard texture chunk it already has an image, which is used for the 
negative Z direction. The other 5 images are stored in variables that are named
according to the direction they represent (osg::CubeTextureChunk::_sfPosZImage,
osg::CubeTextureChunk::_sfNegYImage, osg::CubeTextureChunk::_sfPosYImage, 
osg::CubeTextureChunk::_sfNegXImage, osg::CubeTextureChunk::_sfPosXImage).

*/


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

CubeTextureChunk::CubeTextureChunk(void) :
    Inherited()
{
}

CubeTextureChunk::CubeTextureChunk(const CubeTextureChunk &source) :
    Inherited(source)
{
}

CubeTextureChunk::~CubeTextureChunk(void)
{
    if(_sfPosZImage.getValue() != NullFC)
        subRefCP(_sfPosZImage.getValue());

    if(_sfPosXImage.getValue() != NullFC)
        subRefCP(_sfPosXImage.getValue());
    if(_sfNegXImage.getValue() != NullFC)
        subRefCP(_sfNegXImage.getValue());

    if(_sfPosYImage.getValue() != NullFC)
        subRefCP(_sfPosYImage.getValue());
    if(_sfNegYImage.getValue() != NullFC)
        subRefCP(_sfNegYImage.getValue());
}

/*------------------------- Chunk Class Access ---------------------------*/

const StateChunkClass *CubeTextureChunk::getClass(void) const
{
    return TextureChunk::getClass();
}

/*------------------------------- Sync -----------------------------------*/

void CubeTextureChunk::changed(BitVector whichField, UInt32 origin)
{
    if(whichField & PosZImageFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfPosZImage.getValue());
            }
            else
            {
                ImagePtr pImage = _sfPosZImage.getValue();

                _sfPosZImage.setValue(NullFC);

                setPosZImage(pImage);
            }
        }
    }

    if(whichField & PosXImageFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfPosXImage.getValue());
            }
            else
            {
                ImagePtr pImage = _sfPosXImage.getValue();

                _sfPosXImage.setValue(NullFC);

                setPosXImage(pImage);
            }
        }
    }
    
    if(whichField & NegXImageFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfNegXImage.getValue());
            }
            else
            {
                ImagePtr pImage = _sfNegXImage.getValue();

                _sfNegXImage.setValue(NullFC);

                setNegXImage(pImage);
            }
        }
    }
    
    if(whichField & PosYImageFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfPosYImage.getValue());
            }
            else
            {
                ImagePtr pImage = _sfPosYImage.getValue();

                _sfPosYImage.setValue(NullFC);

                setPosYImage(pImage);
            }
        }
    }
    
    if(whichField & NegYImageFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfNegYImage.getValue());
            }
            else
            {
                ImagePtr pImage = _sfNegYImage.getValue();

                _sfNegYImage.setValue(NullFC);

                setNegYImage(pImage);
            }
        }
    }
    Inherited::changed(whichField, origin);
}

/*----------------------------- onCreate --------------------------------*/

void CubeTextureChunk::onCreate(const CubeTextureChunk *)
{
    if(GlobalSystemState == Startup)
        return;

    // !!! this temporary is needed to work around compiler problems (sgi)
    // CHECK CHECK
    //  TextureChunkPtr tmpPtr = FieldContainer::getPtr<TextureChunkPtr>(*this);
    CubeTextureChunkPtr tmpPtr(*this);

    beginEditCP(tmpPtr, CubeTextureChunk::GLIdFieldMask);

    setGLId(
        Window::registerGLObject(
            osgTypedMethodVoidFunctor2ObjCPtrPtr<CubeTextureChunkPtr, 
                                                 Window , 
                                                 UInt32>(
                                                tmpPtr, 
                                                &CubeTextureChunk::handleGL),
            1));

    endEditCP(tmpPtr, CubeTextureChunk::GLIdFieldMask);
}

/*------------------------------ Output ----------------------------------*/

void CubeTextureChunk::dump(      UInt32    , 
                             const BitVector ) const
{
    SLOG << "Dump CubeTextureChunk NI" << std::endl;
}


/*-------------------------------- GL ------------------------------------*/

void CubeTextureChunk::handleGL(Window *win, UInt32 idstatus)
{
    Window::GLObjectStatusE mode;
    UInt32 osgid;
    GLuint id;
    
    // does the window support cubemaps?
    if(win->hasExtension(_arbCubeTex) == false)
        return;
        
    Window::unpackIdStatus(idstatus, osgid, mode);

    id = win->getGLObjectId(osgid);
    
    if(mode == Window::destroy)
    {
        glDeleteTextures(1, &id);
    }
    else if(mode == Window::finaldestroy)
    {
        //SWARNING << "Last texture user destroyed" << std::endl;
    }
    else if(mode == Window::initialize || mode == Window::reinitialize)
    {
        if(mode == Window::initialize)
        {
            glGenTextures(1, &id);
            win->setGLObjectId(osgid, id);
        }

        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 
                      mode, getImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 
                      Window::initialize, getPosZImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 
                      Window::initialize, getNegYImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 
                      Window::initialize, getPosYImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 
                      Window::initialize, getNegXImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 
                      Window::initialize, getPosXImage());        
    }
    else if(mode == Window::needrefresh)
    {
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 
                      mode, getImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 
                      mode, getPosZImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 
                      mode, getNegYImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 
                      mode, getPosYImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 
                      mode, getNegXImage());        
        handleTexture(win, id, 
                      GL_TEXTURE_CUBE_MAP_ARB, 
                      GL_NONE, 
                      GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 
                      mode, getPosXImage());        
    }
    else
    {
        SWARNING << "Win:" << win << "TextureChunk(" << this 
                 << "::handleGL: Illegal mode: "
                 << mode << " for id " << id << std::endl;
    }
}


/*------------------------------ State ------------------------------------*/

void CubeTextureChunk::activate( DrawActionBase *action, UInt32 idx )
{   
    // does the window support cubemaps?
    if(action->getWindow()->hasExtension(_arbCubeTex) == false)
        return;
        
    Window *win = action->getWindow();
    
    Real32 nteximages, ntexcoords;
    if((nteximages = win->getConstantValue(GL_MAX_TEXTURE_IMAGE_UNITS_ARB)) ==
       Window::unknownConstant
      )
    {
        nteximages = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(nteximages == Window::unknownConstant)
            nteximages = 1.0f;
    }
    if((ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_COORDS_ARB)) ==
       Window::unknownConstant
      )
    {
        ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(ntexcoords == Window::unknownConstant)
            ntexcoords = 1.0f;
    }

    if(idx >= static_cast<UInt32>(nteximages))
    {
#ifdef OSG_DEBUG
        FWARNING(("CubeTextureChunk::activate: Trying to bind image unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, nteximages));
#endif
        return;        
    }
    
    TextureChunk::activateTexture(win, idx);
    
    win->validateGLObject(getGLId());

    glErr("CubeTextureChunk::activate precheck");
  
    FDEBUG(("CubeTextureChunk::activate - %d\n", getGLId()));

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, win->getGLObjectId(getGLId()));

#ifdef GL_NV_point_sprite
    if(idx < static_cast<UInt32>(ntexcoords))
    {
        if(getPointSprite() &&
           win->hasExtension(_nvPointSprite))
        {
            glTexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV, GL_TRUE);
        }
    }
#endif

    if(idx < static_cast<UInt32>(nteximages))
    {
        if(getLodBias() != 0.0f &&
           win->hasExtension(_extTextureLodBias))
        {
            glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT,
	              getLodBias());
        }
    }

    Real32 ntexunits = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);

    // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
    if(ntexunits == Window::unknownConstant)
        ntexunits = 1.0f;

    if(idx < static_cast<UInt32>(ntexunits))
    {
        // texture env
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, getEnvMode());
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,
                    (GLfloat*)getEnvColor().getValuesRGBA());

        if(getEnvMode() == GL_COMBINE_EXT)
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  getEnvCombineRGB ());
            glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT,    getEnvScaleRGB   ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  getEnvSource0RGB ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  getEnvSource1RGB ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,  getEnvSource2RGB ());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, getEnvOperand0RGB());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, getEnvOperand1RGB());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, getEnvOperand2RGB());

            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, getEnvCombineAlpha ());
            glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE,       getEnvScaleAlpha   ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, getEnvSource0Alpha ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, getEnvSource1Alpha ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, getEnvSource2Alpha ());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT,getEnvOperand0Alpha());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT,getEnvOperand1Alpha());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT,getEnvOperand2Alpha());
        }

        TextureChunk::handleTextureShader(win, GL_TEXTURE_CUBE_MAP_ARB);

        if(getShaderOperation() != GL_NONE &&
           win->hasExtension(_nvTextureShader) &&
           idx == 0)
        {
            glEnable(GL_TEXTURE_SHADER_NV);
        }
        glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    }
    
    glErr("CubeTextureChunk::activate");
}

void CubeTextureChunk::changeFrom(  DrawActionBase *action, 
                                    StateChunk     *old   , 
                                    UInt32          idx )
{
    // does the window support cubemaps?
    if(action->getWindow()->hasExtension(_arbCubeTex) == false)
        return;
        
    // change from me to me?
    // this assumes I haven't changed in the meantime. 
    // is that a valid assumption?
    if(old == this)
        return;
    
    // If the old one is not a cube texture chunk, deactivate it and
    // activate ourselves
    if(getTypeId() != old->getTypeId())
    {
        old->deactivate(action, idx);
        activate(action, idx);
        return;
    }

    CubeTextureChunk *oldp      = dynamic_cast<CubeTextureChunk *>(old);

    glErr("CubeTextureChunk::changeFrom precheck");

    Window *win = action->getWindow();   

    if(TextureChunk::activateTexture(win, idx))
        return; // trying to use too many textures

    UInt32 nteximages, ntexcoords, ntexunits;
    
    Real32 dummy = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);

    if(dummy == Window::unknownConstant)
    {
        ntexunits = 1;
    }
    else
    {
        ntexunits = static_cast<UInt32>(dummy);
    }

    if((dummy = win->getConstantValue(GL_MAX_TEXTURE_IMAGE_UNITS_ARB)) ==
       Window::unknownConstant
      )
    {
        nteximages = ntexunits;
    }
    else
    {
        nteximages = static_cast<UInt32>(dummy);
    }
    
    if((dummy = win->getConstantValue(GL_MAX_TEXTURE_COORDS_ARB)) ==
       Window::unknownConstant
      )
    {
        ntexcoords = ntexunits;
    }
    else
    {
        ntexcoords = static_cast<UInt32>(dummy);
    }

    if(idx >= nteximages)
    {
#ifdef OSG_DEBUG
        FWARNING(("TextureChunk::activate: Trying to bind image unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, nteximages));
#endif
        return;        
    }

    win->validateGLObject(getGLId());
    
    FDEBUG(("CubeTextureChunk::activate - %d\n", getGLId()));

    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, win->getGLObjectId(getGLId()));

#ifdef GL_NV_point_sprite
    if(idx < ntexcoords)
    {
        if(oldp->getPointSprite() != getPointSprite() &&
           win->hasExtension(_nvPointSprite)
          )
        {
            glTexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV, getPointSprite());
        }
    }
#endif

    if(idx < nteximages)
    {
        if(oldp->getLodBias() != getLodBias() &&
           win->hasExtension(_extTextureLodBias)
          )
        {
            glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT,
	              getLodBias());
        }
    }

    if(idx < ntexunits)
    {
        if(oldp->getEnvMode() != getEnvMode())
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, getEnvMode());

        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,
                        (GLfloat*)getEnvColor().getValuesRGBA());

        if(getEnvMode() == GL_COMBINE_EXT)
        {
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT,  getEnvCombineRGB ());
            glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT,    getEnvScaleRGB   ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT,  getEnvSource0RGB ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT,  getEnvSource1RGB ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_EXT,  getEnvSource2RGB ());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_EXT, getEnvOperand0RGB());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_EXT, getEnvOperand1RGB());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_EXT, getEnvOperand2RGB());

            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, getEnvCombineAlpha ());
            glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE,       getEnvScaleAlpha   ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, getEnvSource0Alpha ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, getEnvSource1Alpha ());
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, getEnvSource2Alpha ());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_EXT,getEnvOperand0Alpha());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_EXT,getEnvOperand1Alpha());
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_EXT,getEnvOperand2Alpha());
        }

        if(win->hasExtension(_nvTextureShader))
        {
            if(      getShaderOperation() != GL_NONE &&
               oldp->getShaderOperation() == GL_NONE
              )
            {
                handleTextureShader(win, GL_TEXTURE_CUBE_MAP_ARB);
                if(idx == 0)
                    glEnable(GL_TEXTURE_SHADER_NV);
            }
            else if(      getShaderOperation() == GL_NONE &&
                    oldp->getShaderOperation() != GL_NONE
                   )
            {
                glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
                if(idx == 0)
                    glDisable(GL_TEXTURE_SHADER_NV);
            }
        }
    }

    glErr("CubeTextureChunk::changeFrom");
}

void CubeTextureChunk::deactivate(DrawActionBase *action, UInt32 idx)
{
    // does the window support cubemaps?
    if(action->getWindow()->hasExtension(_arbCubeTex) == false)
        return;
        
    Window *win = action->getWindow();   

    Real32 nteximages, ntexcoords;
    if((nteximages = win->getConstantValue(GL_MAX_TEXTURE_IMAGE_UNITS_ARB)) ==
       Window::unknownConstant
      )
    {
        nteximages = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(nteximages == Window::unknownConstant)
            nteximages = 1.0f;
    }
    if((ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_COORDS_ARB)) ==
       Window::unknownConstant
      )
    {
        ntexcoords = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
        // sgi doesn't support GL_MAX_TEXTURE_UNITS_ARB!
        if(ntexcoords == Window::unknownConstant)
            ntexcoords = 1.0f;
    }

    if(idx >= static_cast<UInt32>(nteximages))
    {
#ifdef OSG_DEBUG
        FWARNING(("CubeTextureChunk::deactivate: Trying to bind image unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, nteximages));
#endif
        return;        
    }


    bool isActive = false;

#ifdef GL_NV_point_sprite
    if(getPointSprite() &&
       win->hasExtension(_nvPointSprite) &&
       idx < static_cast<UInt32>(ntexcoords)
      )
    {
        if(!isActive)
        {
            TextureChunk::activateTexture(win, idx);
            isActive = true;
        }
        glTexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV, GL_FALSE);
    }
#endif

    if(getLodBias() != 0.0f &&
       win->hasExtension(_extTextureLodBias))
    {
        if(!isActive)
            TextureChunk::activateTexture(win, idx);
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT,
	          0.0f);
    }

    Real32 ntexunits = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
    if(ntexunits == Window::unknownConstant)
        ntexunits = 1.0f;

    if(idx >= static_cast<UInt32>(ntexunits))
        return; // tetxures >= MTU are not enabled and don't have an env
        
    if(!isActive)
        TextureChunk::activateTexture(win, idx);

    if(getShaderOperation() != GL_NONE &&
       win->hasExtension(_nvTextureShader))
    {
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);

        if(idx == 0)
            glDisable(GL_TEXTURE_SHADER_NV);
    }

    glDisable(GL_TEXTURE_CUBE_MAP_ARB);

    glErr("CubeTextureChunk::deactivate");
}

/*-------------------------- Comparison -----------------------------------*/

Real32 CubeTextureChunk::switchCost(StateChunk *OSG_CHECK_ARG(chunk))
{
    return 0;
}

bool CubeTextureChunk::operator < (const StateChunk &other) const
{
    return this < &other;
}

bool CubeTextureChunk::operator == (const StateChunk &other) const
{
    CubeTextureChunk const *tother = 
            dynamic_cast<CubeTextureChunk const*>(&other);

    if(!tother)
        return false;

    if(tother == this)
        return true;

    return  TextureChunk::operator==(other) &&
            getPosZImage()  == tother->getPosZImage() &&
            getPosYImage()  == tother->getPosYImage() &&
            getNegYImage()  == tother->getNegYImage() &&
            getPosXImage()  == tother->getPosXImage() &&
            getNegXImage()  == tother->getNegXImage();
}

bool CubeTextureChunk::operator != (const StateChunk &other) const
{
    return ! (*this == other);
}

/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */

#ifdef __sgi
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp[] = "@(#)$Id: $";
    static Char8 cvsid_hpp[] = OSGCUBETEXTURECHUNK_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGCUBETEXTURECHUNK_INLINE_CVSID;
}
