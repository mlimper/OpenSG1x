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
#include <OSGGLU.h>
#include <OSGGLEXT.h>
#include <OSGImage.h>

#include "OSGDrawActionBase.h"
#include "OSGRenderAction.h"

#include "OSGTextureChunk.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::TextureChunk
    \ingroup GrpSystemState

See \ref PageSystemTextureChunk for a description.

This chunk wraps glTexImage[123]D (osg::TextureChunk::_sfImage,
osg::TextureChunk::_sfInternalFormat, osg::TextureChunk::_sfExternalFormat),
glTexParameter (osg::TextureChunk::_sfMinFilter,
osg::TextureChunk::_sfMagFilter, osg::TextureChunk::_sfWrapS,
osg::TextureChunk::_sfWrapT, osg::TextureChunk::_sfWrapR), glTexEnv
(osg::TextureChunk::_sfEnvMode, osg::TextureChunk::_sfEnvColor,
osg::TextureChunk::_sfPriority). The ARB combine extension is also supported,
where available (osg::TextureChunk::_sfEnvCombineRGB,
osg::TextureChunk::_sfEnvScaleRGB, osg::TextureChunk::_sfEnvSource0RGB,
osg::TextureChunk::_sfEnvSource1RGB, osg::TextureChunk::_sfEnvSource2RGB,
osg::TextureChunk::_sfEnvOperand0RGB, osg::TextureChunk::_sfEnvOperand1RGB,
osg::TextureChunk::_sfEnvOperand2RGB,
osg::TextureChunk::_sfEnvCombineAlpha,   osg::TextureChunk::_sfEnvScaleAlpha,
osg::TextureChunk::_sfEnvSource0Alpha, osg::TextureChunk::_sfEnvSource1Alpha,
osg::TextureChunk::_sfEnvSource2Alpha, osg::TextureChunk::_sfEnvOperand0Alpha,
osg::TextureChunk::_sfEnvOperand1Alpha,
osg::TextureChunk::_sfEnvOperand2Alpha). It is possible to enable the point
sprite coordinate replacement  (osg::TextureChunk::_sfPointSprite), see \ref
PageSystemPointChunk for details. The two parameters
osg::TextureChunk::_sfScale and osg::TextureChunk::_sfFrame specify details
about the texture.

On hardware that supports it (i.e. NVidia boards) the texture shader
extension(s) are also available.

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char TextureChunk::cvsid[] = "@(#)$Id: OSGTextureChunk.cpp,v 1.41 2002/06/17 12:27:14 jbehr Exp $";

StateChunkClass TextureChunk::_class("Texture", osgMaxTexImages);

UInt32 TextureChunk::_extTex3D;
UInt32 TextureChunk::_arbMultiTex;
UInt32 TextureChunk::_arbCubeTex;
UInt32 TextureChunk::_nvPointSprite;
UInt32 TextureChunk::_nvTextureShader;
UInt32 TextureChunk::_nvTextureShader2;
UInt32 TextureChunk::_nvTextureShader3;
UInt32 TextureChunk::_sgisGenerateMipmap;
UInt32 TextureChunk::_extTextureLodBias;
UInt32 TextureChunk::_arbTextureCompression;
UInt32 TextureChunk::_arbTextureRectangle;
UInt32 TextureChunk::_arbTextureNonPowerOfTwo;
UInt32 TextureChunk::_extTextureFilterAnisotropic;
UInt32 TextureChunk::_funcTexImage3D              = Window::invalidFunctionID;
UInt32 TextureChunk::_funcTexSubImage3D           = Window::invalidFunctionID;
UInt32 TextureChunk::_funcActiveTexture           = Window::invalidFunctionID;
UInt32 TextureChunk::_funcCompressedTexImage1D    = Window::invalidFunctionID;
UInt32 TextureChunk::_funcCompressedTexSubImage1D = Window::invalidFunctionID;
UInt32 TextureChunk::_funcCompressedTexImage2D    = Window::invalidFunctionID;
UInt32 TextureChunk::_funcCompressedTexSubImage2D = Window::invalidFunctionID;
UInt32 TextureChunk::_funcCompressedTexImage3D    = Window::invalidFunctionID;
UInt32 TextureChunk::_funcCompressedTexSubImage3D = Window::invalidFunctionID;

// define GL_TEXTURE_3D, if not defined yet
#ifndef GL_VERSION_1_2
#  define GL_FUNC_TEXIMAGE3D    OSG_DLSYM_UNDERSCORE"glTexImage3DEXT"
#  define GL_FUNC_TEXSUBIMAGE3D OSG_DLSYM_UNDERSCORE"glTexSubImage3DEXT"
#else
#  define GL_FUNC_TEXIMAGE3D    OSG_DLSYM_UNDERSCORE"glTexImage3D"
#  define GL_FUNC_TEXSUBIMAGE3D OSG_DLSYM_UNDERSCORE"glTexSubImage3D"
#endif

//#warning GL_VERSION_1_2
//#warning GL_FUNC_TEXIMAGE3D


UInt32 TextureChunk::_numTexCreate        = 0;
Time   TextureChunk::_summedTexCreateTime = 0;


/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void TextureChunk::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/


/*------------- constructors & destructors --------------------------------*/

TextureChunk::TextureChunk(void) :
    Inherited()
{
    _extTex3D               =
        Window::registerExtension("GL_EXT_texture3D"                );
    _arbMultiTex            =
        Window::registerExtension("GL_ARB_multitexture"             );
    _arbCubeTex             =
        Window::registerExtension("GL_ARB_texture_cube_map"         );
    _nvPointSprite          =
        Window::registerExtension("GL_NV_point_sprite"              );
    _nvTextureShader        =
        Window::registerExtension("GL_NV_texture_shader"            );
    _nvTextureShader2       =
        Window::registerExtension("GL_NV_texture_shader2"           );
    _nvTextureShader3       =
        Window::registerExtension("GL_NV_texture_shader3"           );
    _sgisGenerateMipmap     =
        Window::registerExtension("GL_SGIS_generate_mipmap"         );
    _extTextureLodBias      = 
        Window::registerExtension("GL_EXT_texture_lod_bias"         );
    _arbTextureCompression  = 
        Window::registerExtension("GL_ARB_texture_compression"      );
    _arbTextureRectangle  = 
        Window::registerExtension("GL_ARB_texture_rectangle"        );
    _arbTextureNonPowerOfTwo  = 
        Window::registerExtension("GL_ARB_texture_non_power_of_two" );
    _extTextureFilterAnisotropic = 
        Window::registerExtension("GL_EXT_texture_filter_anisotropic" );

    _funcTexImage3D    =
        Window::registerFunction (GL_FUNC_TEXIMAGE3D                        , 
            _extTex3D, 0x0102);
    _funcTexSubImage3D =
        Window::registerFunction (GL_FUNC_TEXSUBIMAGE3D                     , 
            _extTex3D, 0x0102);
    _funcActiveTexture =
        Window::registerFunction (OSG_DLSYM_UNDERSCORE"glActiveTextureARB"  , 
            _arbMultiTex);
    _funcCompressedTexImage1D    =
        Window::registerFunction(OSG_DLSYM_UNDERSCORE
                                    "glCompressedTexImage1DARB"             , 
            _arbTextureCompression);
    _funcCompressedTexSubImage1D    =
        Window::registerFunction(OSG_DLSYM_UNDERSCORE
                                    "glCompressedTexSubImage1DARB"          , 
            _arbTextureCompression);
    _funcCompressedTexImage2D    =
        Window::registerFunction(OSG_DLSYM_UNDERSCORE
                                    "glCompressedTexImage2DARB"             , 
            _arbTextureCompression);
    _funcCompressedTexSubImage2D    =
        Window::registerFunction(OSG_DLSYM_UNDERSCORE
                                    "glCompressedTexSubImage2DARB"          , 
            _arbTextureCompression);
    _funcCompressedTexImage3D    =
        Window::registerFunction(OSG_DLSYM_UNDERSCORE
                                    "glCompressedTexImage3DARB"             , 
            _arbTextureCompression);
    _funcCompressedTexSubImage3D    =
        Window::registerFunction(OSG_DLSYM_UNDERSCORE
                                    "glCompressedTexSubImage3DARB"          , 
            _arbTextureCompression);

    Window::registerConstant(GL_MAX_TEXTURE_UNITS_ARB);
    Window::registerConstant(GL_MAX_TEXTURE_IMAGE_UNITS_ARB);
    Window::registerConstant(GL_MAX_TEXTURE_COORDS_ARB);    
}

TextureChunk::TextureChunk(const TextureChunk &source) :
    Inherited(source)
{
}

TextureChunk::~TextureChunk(void)
{
}

/*------------------------- Chunk Class Access ---------------------------*/

const StateChunkClass *TextureChunk::getClass(void) const
{
    return &_class;
}

/*------------------------------- Sync -----------------------------------*/

/*! React to field changes.
    Note: this function also handles CubeTexture changes, make sure to keep
    it consistent with the cubeTexture specifics
*/

void TextureChunk::changed(BitVector whichField, UInt32 origin)
{
    if(Thread::getAspect() != _sfIgnoreGLForAspect.getValue())
    {
        if(getGLId() == 0)
        {
            TextureChunkPtr tmpPtr(*this);

            beginEditCP(tmpPtr, TextureChunk::GLIdFieldMask);
            
            setGLId(
                Window::registerGLObject(
                    osgTypedMethodVoidFunctor2ObjCPtrPtr<TextureChunkPtr,
                                                 Window ,
                                                 UInt32>(
                                                     tmpPtr,
                                                     &TextureChunk::handleGL),
                    1));
            
            endEditCP(tmpPtr, TextureChunk::GLIdFieldMask);
        }
    }

    // Only filter changed? Mipmaps need reinit.
    if((whichField & ~(MinFilterFieldMask | MagFilterFieldMask)) == 0)
    {
        if((getMinFilter() != GL_NEAREST) &&
           (getMinFilter() != GL_LINEAR))
        {
            triggerReInit();
        }
        else // switching to GL_NEAREST or GL_LINEAR only requires refresh
        {
            imageContentChanged();
        }
    } // Only priority changed? Refresh is fine.
    else if((whichField & ~(PriorityFieldMask | FrameFieldMask)) == 0)
    {
        imageContentChanged();
    } // Only dirty rectangle changed? Refresh is fine.
    else if ((whichField & ~(DirtyMinXFieldMask | DirtyMaxXFieldMask |
                             DirtyMinYFieldMask | DirtyMaxYFieldMask |
                             DirtyMinZFieldMask | DirtyMaxZFieldMask)) == 0)
    {
        triggerRefresh();
    }
    else // Play it safe, do a reinit
    {
        triggerReInit();
    }

    if(whichField & ImageFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfImage.getValue());
            }
            else
            {
                ImagePtr pImage = _sfImage.getValue();

                _sfImage.setValue(NullFC);

                setImage(pImage);
            }
        }
    }

    Inherited::changed(whichField, origin);
}

bool TextureChunk::isTransparent(void) const
{
    // Even if the texture has alpha, the Blending makes the sorting
    // important, thus textures per se are not transparent
    return false;
}


/*----------------------------- onCreate --------------------------------*/

void TextureChunk::onCreate(const TextureChunk *)
{
    if(GlobalSystemState == Startup)
        return;

    // !!! this temporary is needed to work around compiler problems (sgi)
    // CHECK CHECK
    //  TextureChunkPtr tmpPtr = FieldContainer::getPtr<TextureChunkPtr>(*this);
    TextureChunkPtr tmpPtr(*this);

    if(Thread::getAspect() != _sfIgnoreGLForAspect.getValue())
    {
        beginEditCP(tmpPtr, TextureChunk::GLIdFieldMask);

        setGLId(
            Window::registerGLObject(
                osgTypedMethodVoidFunctor2ObjCPtrPtr<TextureChunkPtr,
                                                 Window ,
                                                 UInt32>(
                                                     tmpPtr,
                                                     &TextureChunk::handleGL),
                1));

        endEditCP(tmpPtr, TextureChunk::GLIdFieldMask);
    }
}

void TextureChunk::onDestroy(void)
{
    Inherited::onDestroy();

    if(_sfImage.getValue() != NullFC)
        subRefCP(_sfImage.getValue());
    if(getGLId() > 0)
        Window::destroyGLObject(getGLId(), 1);
}

/*------------------------------ Output ----------------------------------*/

void TextureChunk::dump(      UInt32    OSG_CHECK_ARG(uiIndent),
                        const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    SLOG << "Dump TextureChunk NI" << std::endl;
}


/*------------------------------ State ------------------------------------*/

/*! Texture handler. Create/update a single texture.
    Also used by derived CubeMap chunk.
*/

void TextureChunk::handleTextureShader(Window *win, GLenum bindtarget)
{
    if(!win->hasExtension(_nvTextureShader))
    {
        if(getShaderOperation() != GL_NONE)
            FINFO(("NV Texture Shaders not supported on Window %p!\n", win));
        return;
    }

    glErr("textureShader precheck");

    glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV,
                getShaderOperation());

    glErr("textureShader setup: operation");

    if(getShaderOperation() == GL_NONE)
        return;

    if(bindtarget == GL_TEXTURE_3D && !win->hasExtension(_nvTextureShader2))
    {
        FINFO(("NV Texture Shaders 2 not supported on Window %p!\n", win));
        return;
    }

    if(getShaderInput() != GL_NONE)
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_PREVIOUS_TEXTURE_INPUT_NV,
                    getShaderInput());

    glErr("textureShader setup: input");

    if(getShaderRGBADotProduct() != GL_NONE)
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV,
                    getShaderRGBADotProduct());

    glErr("textureShader setup: rgba dotprod");

    if(getShaderOffsetMatrix().size() == 4)
    {
        glTexEnvfv(GL_TEXTURE_SHADER_NV, GL_OFFSET_TEXTURE_MATRIX_NV,
                    (GLfloat*)&(getShaderOffsetMatrix()[0]));

        glErr("textureShader setup: offset matrix");
    }
    else if(getShaderOffsetMatrix().size() != 0)
    {
        FWARNING(("TextureChunk::handleTextureShader: shaderOffsetMatrix has"
                    " to have 4 entries, not %d!\n",
                    getShaderOffsetMatrix().size() ));
    }

    glTexEnvf(GL_TEXTURE_SHADER_NV, GL_OFFSET_TEXTURE_SCALE_NV,
                getShaderOffsetScale());

    glErr("textureShader setup: offset scale");

    glTexEnvf(GL_TEXTURE_SHADER_NV, GL_OFFSET_TEXTURE_BIAS_NV,
                getShaderOffsetBias());

    glErr("textureShader setup: offset bias");

    GLint cullmodes[4];
    if(getShaderCullModes() & 0x1)
    {
        cullmodes[0] = GL_GEQUAL;
    }
    else
    {
        cullmodes[0] = GL_LESS;
    }

    if(getShaderCullModes() & 0x2)
    {
        cullmodes[1] = GL_GEQUAL;
    }
    else
    {
        cullmodes[1] = GL_LESS;
    }

    if(getShaderCullModes() & 0x4)
    {
        cullmodes[2] = GL_GEQUAL;
    }
    else
    {
        cullmodes[2] = GL_LESS;
    }

    if(getShaderCullModes() & 0x8)
    {
        cullmodes[3] = GL_GEQUAL;
    }
    else
    {
        cullmodes[3] = GL_LESS;
    }

    glTexEnviv(GL_TEXTURE_SHADER_NV, GL_CULL_MODES_NV,
                    cullmodes);

    glErr("textureShader setup: cull modes");

    glTexEnvfv(GL_TEXTURE_SHADER_NV, GL_CONST_EYE_NV,
                    getShaderConstEye().getValues());

    glErr("textureShader setup: const eye");



#ifdef OSG_DEBUG
    GLint consistent;
    glGetTexEnviv(GL_TEXTURE_SHADER_NV, GL_SHADER_CONSISTENT_NV,
          &consistent);
    if(!consistent)
    {
        FWARNING(("Texture shaders not consistent!\n"));
    }
#endif
}

void TextureChunk::handleTexture(Window *win, UInt32 id,
    GLenum bindtarget,
    GLenum paramtarget,
    GLenum imgtarget,
    Window::GLObjectStatusE mode, ImagePtr img, int side)
{
    if( img==NullFC || ! img->getDimension()) // no image ?
        return;

    if(mode == Window::initialize || mode == Window::reinitialize)
    {
        if(bindtarget == GL_TEXTURE_3D && 
            !win->hasExtension(_extTex3D) &&
             win->getGLVersion() < 0x0102
          )
        {
            FNOTICE(("3D textures not supported on Window %p!\n", win));
            return;
        }

        if(imgtarget == GL_TEXTURE_RECTANGLE_ARB &&
           !win->hasExtension(_arbTextureRectangle))
        {
            FNOTICE(("Rectangular textures not supported on Window %p!\n", win));
            return;
        }

        if(paramtarget == GL_TEXTURE_CUBE_MAP_ARB && 
           !win->hasExtension(_arbCubeTex))
        {
            FNOTICE(("Cube textures not supported on Window %p!\n", win));
            return;
        }
        
        if(img->hasCompressedData() && 
           !win->hasExtension(_arbTextureCompression))
        {
            FNOTICE(("Compressed textures not supported on Window %p!\n", win));
            return;
        }

        if(mode == Window::reinitialize)
        {
            GLuint tex = id;
            glDeleteTextures(1, &tex);
        }

        // 3D texture functions
        void (OSG_APIENTRY*TexImage3D)(GLenum target, GLint level, GLenum internalformat,
                           GLsizei width, GLsizei height, GLsizei depth,
                           GLint border, GLenum format, GLenum type,
                           const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, GLenum internalformat,
                      GLsizei width, GLsizei height, GLsizei depth,
                      GLint border, GLenum format, GLenum type,
                      const GLvoid *pixels))
            win->getFunction(_funcTexImage3D);

        void (OSG_APIENTRY*TexSubImage3D)
                          (GLenum target, GLint level, GLint xoffset,
                           GLint yoffset, GLint zoffset, GLsizei width,
                           GLsizei height, GLsizei depth, GLenum format,
                           GLenum type, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, GLint xoffset,
                      GLint yoffset, GLint zoffset, GLsizei width,
                      GLsizei height, GLsizei depth, GLenum format,
                      GLenum type, const GLvoid *pixels))
            win->getFunction(_funcTexSubImage3D);

        // Compressed texture functions
        void (OSG_APIENTRY*CompressedTexImage1D)(GLenum target, GLint level, 
                           GLenum internalformat, GLsizei width, GLint border, 
                           GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLenum internalformat, GLsizei width, GLint border,  
                           GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexImage1D);

        void (OSG_APIENTRY*CompressedTexSubImage1D)
                          (GLenum target, GLint level, GLint xoffset, GLsizei width,
                           GLenum format, GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, GLint xoffset, GLsizei width,
                           GLenum format, GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexSubImage1D);

        void (OSG_APIENTRY*CompressedTexImage2D)(GLenum target, GLint level, 
                           GLenum internalformat, GLsizei width, GLsizei height, 
                           GLint border, 
                           GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLenum internalformat, GLsizei width, GLsizei height, 
                           GLint border, 
                           GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexImage2D);

        void (OSG_APIENTRY*CompressedTexSubImage2D)
                          (GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, 
                           GLsizei width, GLsizei height, GLenum format,
                           GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, 
                           GLsizei width, GLsizei height, GLenum format,
                           GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexSubImage2D);

        void (OSG_APIENTRY*CompressedTexImage3D)(GLenum target, GLint level, 
                           GLenum internalformat,
                           GLsizei width, GLsizei height, GLsizei depth,
                           GLint border,
                           GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLenum internalformat,
                           GLsizei width, GLsizei height, GLsizei depth,
                           GLint border,
                           GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexImage3D);

        void (OSG_APIENTRY*CompressedTexSubImage3D)
                          (GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, GLint zoffset, 
                           GLsizei width, GLsizei height, GLsizei depth, 
                           GLenum format, GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, GLint zoffset, 
                           GLsizei width, GLsizei height, GLsizei depth, 
                           GLenum format, GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexSubImage3D);

        // as we're not allocating anything here, the same code can be used
        // for reinitialization
        if(! img || ! img->getDimension()) // no image ?
            return;

        glErr("TextureChunk::initialize precheck");

        FDEBUG(("texture (re-)initialize\n"));
        
        glBindTexture(bindtarget, id);

        if(paramtarget != GL_NONE)
        {
            // set the parameters
            glTexParameterf(paramtarget, GL_TEXTURE_PRIORITY,   getPriority());
            glTexParameteri(paramtarget, GL_TEXTURE_MIN_FILTER, getMinFilter());
            glTexParameteri(paramtarget, GL_TEXTURE_MAG_FILTER, getMagFilter());
            glTexParameteri(paramtarget, GL_TEXTURE_WRAP_S, getWrapS());
            if(paramtarget == GL_TEXTURE_2D ||
               paramtarget == GL_TEXTURE_3D ||
               paramtarget == GL_TEXTURE_CUBE_MAP_ARB ||
               paramtarget == GL_TEXTURE_RECTANGLE_ARB
              )
                glTexParameteri(paramtarget, GL_TEXTURE_WRAP_T, getWrapT());
            if(paramtarget == GL_TEXTURE_3D ||
               paramtarget == GL_TEXTURE_CUBE_MAP_ARB)
                glTexParameteri(paramtarget, GL_TEXTURE_WRAP_R, getWrapR());

            if(getAnisotropy() > 1.0f && win->hasExtension(_extTextureFilterAnisotropic))
                glTexParameterf(paramtarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, getAnisotropy());

            glTexParameterfv(paramtarget, GL_TEXTURE_BORDER_COLOR,
                             (GLfloat*)getBorderColor().getValuesRGBA());

            glErr("TextureChunk::initialize params");
        }

        // set the image
        GLenum internalFormat = getInternalFormat();
        GLenum externalFormat = img->getPixelFormat();
        GLenum type           = img->getDataType();
        UInt32 width          = img->getWidth();
        UInt32 height         = img->getHeight();
        UInt32 depth          = img->getDepth();
        bool   compressedData = img->hasCompressedData();

        bool doScale = getScale(); // scale the texture to 2^?
        UInt32 frame = getFrame();

        bool defined = false;   // Texture defined ?
        bool needMipmaps =  getMinFilter() == GL_NEAREST_MIPMAP_NEAREST ||
                            getMinFilter() == GL_LINEAR_MIPMAP_NEAREST  ||
                            getMinFilter() == GL_NEAREST_MIPMAP_LINEAR  ||
                            getMinFilter() == GL_LINEAR_MIPMAP_LINEAR   ;

        if(internalFormat == GL_NONE)
        {
            switch(externalFormat)
            {
#if defined(GL_BGR) && defined(GL_BGR_EXT)
            case GL_BGR:
#else
#  if defined(GL_BGR)
            case GL_BGR:
#  endif
#  if defined(GL_BGR_EXT)
            case GL_BGR_EXT:
#  endif
#endif
#if defined(GL_BGR) || defined(GL_BGR_EXT)
                            internalFormat = GL_RGB;
                            break;
#endif
#if defined(GL_BGRA) && defined(GL_BGRA_EXT)
            case GL_BGRA:
#else
#  if defined(GL_BGRA)
            case GL_BGRA:
#  endif
#  if defined(GL_BGRA_EXT)
            case GL_BGRA_EXT:
#  endif
#endif
#if defined(GL_BGRA) || defined(GL_BGRA_EXT)
                            internalFormat = GL_RGBA;
                            break;
#endif
            case GL_INTENSITY:
                            internalFormat = GL_INTENSITY;
                            externalFormat = GL_LUMINANCE;
                            break;
                            
            default:    internalFormat = externalFormat;
                        break;
            }
        }
         
        if(getExternalFormat() != GL_NONE)
            externalFormat = getExternalFormat();
       
        if(imgtarget == GL_TEXTURE_RECTANGLE_ARB && needMipmaps)
        {
            SWARNING << "TextureChunk::initialize: Can't do mipmaps"
                     << "with GL_TEXTURE_RECTANGLE_ARB target! Ignored"
                     << std::endl;
            needMipmaps= false;
        }
        
        // do we need mipmaps?
        if(needMipmaps)
        {
            // do we have usable mipmaps ?
            if(img->getMipMapCount() == img->calcMipmapLevelCount() &&
                 osgispower2(width) && osgispower2(height) &&
                 osgispower2(depth)
              )
            {
                for(UInt16 i = 0; i < img->getMipMapCount(); i++)
                {
                    UInt32 w, h, d;
                    img->calcMipmapGeometry(i, w, h, d);

                    if(compressedData)
                    {
                        switch (imgtarget)
                        {
                        case GL_TEXTURE_1D:
                            CompressedTexImage1D(GL_TEXTURE_1D, i, internalFormat,
                                            w, getBorderWidth(),
                                            img->calcMipmapLevelSize(i),
                                            img->getData(i, frame, side));
                            break;
                        case GL_TEXTURE_2D:
                            CompressedTexImage2D(imgtarget, i, internalFormat,
                                            w, h, getBorderWidth(),
                                            img->calcMipmapLevelSize(i),
                                            img->getData(i, frame, side));
                            break;
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                            CompressedTexImage2D(imgtarget, i, internalFormat,
                                            w, h, getBorderWidth(),
                                            img->calcMipmapLevelSize(i), 
                                            img->getData(i, frame, side));
                            break;
                        case GL_TEXTURE_3D:
                            CompressedTexImage3D(GL_TEXTURE_3D, i, internalFormat,
                                            w, h, d, getBorderWidth(),
                                            img->calcMipmapLevelSize(i),
                                            img->getData(i, frame, side));
                            break;
                       default:
                                SFATAL << "TextureChunk::initialize1: unknown target "
                                       << imgtarget << "!!!" << std::endl;
                                break;
                        }
                    }
                    else
                    {
                        switch (imgtarget)
                        {
                        case GL_TEXTURE_1D:
                            glTexImage1D(GL_TEXTURE_1D, i, internalFormat,
                                            w, getBorderWidth(),
                                            externalFormat, type,
                                            img->getData(i, frame, side));
                            break;
                        case GL_TEXTURE_2D:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                            glTexImage2D(imgtarget, i, internalFormat,
                                            w, h, getBorderWidth(),
                                            externalFormat, type,
                                            img->getData(i, frame, side));
                            break;
                        case GL_TEXTURE_3D:
                              TexImage3D(GL_TEXTURE_3D, i, internalFormat,
                                            w, h, d, getBorderWidth(),
                                            externalFormat, type,
                                            img->getData(i, frame, side));
                            break;
                       default:
                                SFATAL << "TextureChunk::initialize1: unknown target "
                                       << imgtarget << "!!!" << std::endl;
                                break;
                        }
                    }
                }
                defined = true;
            }

            if(! defined)
            {
                // Nope, do we have SGIS_generate_mipmaps?
                if(win->hasExtension(_sgisGenerateMipmap))
                {
                    if(paramtarget != GL_NONE)
                        glTexParameteri(paramtarget, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
                        // same as GL_GENERATE_MIPMAP which is part of the standard since 1.4
                    glErr("TextureChunk::activate generate_mipmaps");
                    needMipmaps = false; // automagic does it
                }
                else
                {
                    // Nope, try to use gluBuild?DMipmaps
                    void * data = NULL;
                    
                    // can we use it directly?
                    if(! osgispower2(width) ||
                         ! osgispower2(height) ||
                         ! osgispower2(depth)
                      )
                    {
                        // scale is only implemented for 2D
                        if(imgtarget != GL_TEXTURE_2D)
                        {
                            SWARNING << "TextureChunk::initialize: can't mipmap "
                                     << "non-2D textures that are not 2^x !!!"
                                     << std::endl;
                        }
                        else
                        {
                            UInt32 outw = osgnextpower2(width);
                            UInt32 outh = osgnextpower2(height);

                            data = malloc(outw * outh * img->getBpp());
                            
                            // should we scale to next power of 2?
                            if(doScale)
                            {
                                GLint res = gluScaleImage(externalFormat,
                                                width, height, type, img->getData(0, frame, side),
                                                outw, outh, type, data);

                                if(res)
                                {
                                    SWARNING << "TextureChunk::initialize: "
                                             << "gluScaleImage failed: "
                                             << gluErrorString(res) << "("
                                             << res << ")!"
                                             << std::endl;
                                    free(data);
                                    data = NULL;
                                }
                                else
                                {
                                    width = outw;
                                    height = outh;
                                }
                            }
                            else // nope, just copy the image to the lower left part
                            {
                                memset(data, 0, outw * outh * img->getBpp());

                                UInt16 bpl = width * img->getBpp();
                                UInt8 * src = (UInt8 *) img->getData(0, frame, side);
                                UInt8 * dest= (UInt8 *) data;

                                for(UInt32 y = 0; y < height; y++)
                                {
                                    memcpy(dest, src, bpl);

                                    src  += bpl;
                                    dest += outw * img->getBpp();
                                }
                                width = outw;
                                height = outh;
                            }
                        }
                    }
                    else
                        data = img->getData(0, frame, side);
                    
                    if(data)
                    {
                        switch (imgtarget)
                        {
                        case GL_TEXTURE_1D:
                                gluBuild1DMipmaps(imgtarget, internalFormat, width,
                                                    externalFormat, type, data);
                                break;
                        case GL_TEXTURE_2D:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                                gluBuild2DMipmaps(imgtarget, internalFormat,
                                                    width, height,
                                                    externalFormat, type, data);
                                break;
                        case GL_TEXTURE_3D:
#  ifdef GLU_VERSION_1_3
                                gluBuild3DMipmaps(imgtarget, internalFormat,
                                                    width, height, depth,
                                                    externalFormat, type, data);
#  else
                                FWARNING(("TextureChunk::initialize: 3d textures "
                                          "supported, but GLU version < 1.3, thus "
                                          "gluBuild3DMipmaps not supported!\n"));
#  endif
                                break;
                        default:
                                SFATAL << "TextureChunk::initialize2: unknown target "
                                       << imgtarget << "!!!" << std::endl;
                        }

                        
                        if(data != img->getData(0, frame, side))
                            free(data);
                        defined = true;
                    } // data
                } // need to use gluBuildMipmaps?
            } // got them from the image already?
        } // need mipmaps?

        // no mipmaps, or mipmapping failed?
        if(! defined)
        {
            // got here needing mipmaps?
            if(needMipmaps && paramtarget != GL_NONE)  // turn them off
                glTexParameteri(paramtarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            void * data = NULL;
            UInt32 datasize = 0;

            // Do we need to massage the texture or can we just use it?
            if(imgtarget != GL_TEXTURE_RECTANGLE_ARB &&
               !win->hasExtension(_arbTextureNonPowerOfTwo) &&
               (!osgispower2(width) || !osgispower2(height) || !osgispower2(depth))
              )
            {
                // No, need to scale or cut
                
                // should we scale to next power of 2?
                if(doScale)
                {
                    // scale is only implemented for 2D
                    if(imgtarget != GL_TEXTURE_2D &&
                       imgtarget != GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB &&
                       imgtarget != GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB &&
                       imgtarget != GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB &&
                       imgtarget != GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB &&
                       imgtarget != GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB &&
                       imgtarget != GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
                      )
                    {
                        SWARNING << "TextureChunk::initialize: can't scale "
                                 << "non-2D textures that are not 2^x !!!"
                                 << std::endl;
                    }
                    else
                    {
                        UInt32 outw = osgnextpower2(width);
                        UInt32 outh = osgnextpower2(height);

                        data = malloc(outw * outh * img->getBpp());
                        datasize = outw * outh * img->getBpp();
                        
                        GLint res = gluScaleImage(externalFormat,
                                        width, height, type, 
                                        img->getData(0, frame, side),
                                        outw, outh, type, data);

                        if(res)
                        {
                            SWARNING << "TextureChunk::initialize: "
                                     << "gluScaleImage failed: "
                                     << gluErrorString(res) << "("
                                     << res << ")!" << std::endl;
                            free(data);
                            data = NULL;
                        }
                        else
                        {
                            width = outw;
                            height = outh;
                        }
                    }
                
                }
                else // don't scale, just use ll corner
                {
                   if(compressedData)
                   {
                       switch (imgtarget)
                       {
                       case GL_TEXTURE_1D:
                           CompressedTexImage1D(GL_TEXTURE_1D, 0, internalFormat,
                                           osgnextpower2(width), getBorderWidth(), 0, NULL);
                           CompressedTexSubImage1D(GL_TEXTURE_1D, 0, 0, width,
                                           externalFormat, 
                                           img->getFrameSize(), 
                                           img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_2D:
                           CompressedTexImage2D(imgtarget, 0, internalFormat,
                                           osgnextpower2(width),
                                           osgnextpower2(height), getBorderWidth(),
                                           0, NULL);
                           CompressedTexSubImage2D(imgtarget, 0, 0, 0, width, height,
                                           externalFormat,
                                           img->getFrameSize(), 
                                           img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                       case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                       case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                       case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                       case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                       case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                           CompressedTexImage2D(imgtarget, 0, internalFormat,
                                           osgnextpower2(width),
                                           osgnextpower2(height), getBorderWidth(),
                                           0, NULL);
                           CompressedTexSubImage2D(imgtarget, 0, 0, 0, width, height,
                                           externalFormat,
                                           (img->getSideCount() > 1) ? img->getSideSize() :
                                                img->getFrameSize(), 
                                           img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_RECTANGLE_ARB:
                           CompressedTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat,
                                           width, height, getBorderWidth(),
                                           img->getFrameSize(), 
                                           img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_3D:
                           CompressedTexImage3D(GL_TEXTURE_3D, 0, internalFormat,
                                           osgnextpower2(width),
                                           osgnextpower2(height),
                                           osgnextpower2(depth),
                                           getBorderWidth(), 0, NULL);
                           CompressedTexSubImage3D(GL_TEXTURE_3D, 0,  0, 0, 0,
                                           width, height, depth,
                                           externalFormat,
                                           img->getFrameSize(), 
                                           img->getData(0, frame, side));
                           break;
                       default:
                               SFATAL << "TextureChunk::initialize4: unknown target "
                                      << imgtarget << "!!!" << std::endl;
                       }
                   }
                   else
                   {
                       switch (imgtarget)
                       {
                       case GL_TEXTURE_1D:
                           glTexImage1D(GL_TEXTURE_1D, 0, internalFormat,
                                           osgnextpower2(width), getBorderWidth(),
                                           externalFormat, type,
                                           NULL);
                           glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width,
                                           externalFormat, type,
                                            img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_2D:
                       case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                       case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                       case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                       case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                       case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                       case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                           glTexImage2D(imgtarget, 0, internalFormat,
                                           osgnextpower2(width),
                                           osgnextpower2(height), getBorderWidth(),
                                           externalFormat, type,
                                           NULL);
                           glTexSubImage2D(imgtarget, 0, 0, 0, width, height,
                                           externalFormat, type,
                                           img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_RECTANGLE_ARB:
                           glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat,
                                           width, height, getBorderWidth(),
                                           externalFormat, type,
                                           img->getData(0, frame, side));
                           break;
                       case GL_TEXTURE_3D:
                             TexImage3D(GL_TEXTURE_3D, 0, internalFormat,
                                           osgnextpower2(width),
                                           osgnextpower2(height),
                                           osgnextpower2(depth),
                                           getBorderWidth(), externalFormat, type, NULL);
                             TexSubImage3D(GL_TEXTURE_3D, 0,  0, 0, 0,
                                           width, height, depth,
                                           externalFormat, type,
                                           img->getData(0, frame, side));
                           break;
                       default:
                               SFATAL << "TextureChunk::initialize4: unknown target "
                                      << imgtarget << "!!!" << std::endl;
                       } // switch imgtarget
                   } // compressed data?
                       
                   defined = true;
                } // do scale  
            } 
            else // can we use it directly?
            {
               data = img->getData(0, frame, side);
               datasize = (img->getSideCount() > 1) ? img->getSideSize() :
                                                      img->getFrameSize();
            } // can we use it directly?
            
            if(!defined) // either we can use the texture directly, or it was scaled
            {
                // A image without data is quite handy if you need the
                // texture only on the graphics card. So don't check for data here
               
                 if(compressedData)
                 {
                     switch (imgtarget)
                     {
                     case GL_TEXTURE_1D:
                         CompressedTexImage1D(GL_TEXTURE_1D, 0, internalFormat,
                                         width, getBorderWidth(),
                                         datasize, data);
                         break;
                     case GL_TEXTURE_2D:
                     case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                     case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                     case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                     case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                     case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                     case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                         CompressedTexImage2D(imgtarget, 0, internalFormat,
                                         width, height, getBorderWidth(),
                                          datasize, data);
                         break;
                     case GL_TEXTURE_RECTANGLE_ARB:
                         CompressedTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat,
                                         width, height, getBorderWidth(),
                                         datasize, data);
                         break;
                     case GL_TEXTURE_3D:
                         CompressedTexImage3D(GL_TEXTURE_3D, 0, internalFormat,
                                         width, height, depth, getBorderWidth(),
                                         datasize, data);
                         break;
                     default:
                         SFATAL << "TextureChunk::initialize3: unknown target "
                                << imgtarget << "!!!" << std::endl;
                     }
                 }
                 else
                 {
                        switch (imgtarget)
                        {
                        case GL_TEXTURE_1D:
                            glTexImage1D(GL_TEXTURE_1D, 0, internalFormat,
                                            width, getBorderWidth(),
                                            externalFormat, type,
                                            data);
                            break;
                        case GL_TEXTURE_2D:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                            glTexImage2D(imgtarget, 0, internalFormat,
                                            width, height, getBorderWidth(),
                                            externalFormat, type,
                                            data);
                            break;
                        case GL_TEXTURE_RECTANGLE_ARB:
                            glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, internalFormat,
                                            width, height, getBorderWidth(),
                                            externalFormat, type,
                                            data);
                            break;
                        case GL_TEXTURE_3D:
                              TexImage3D(GL_TEXTURE_3D, 0, internalFormat,
                                            width, height, depth, getBorderWidth(),
                                            externalFormat, type,
                                            data);
                            break;
                        default:
                            SFATAL << "TextureChunk::initialize3: unknown target "
                                   << imgtarget << "!!!" << std::endl;
                        }
                    }
                
            }

            if(data != img->getData(0, frame, side))
                free(data);
        }

        glErr("TextureChunk::initialize image");
    }
    else if(mode == Window::needrefresh)
    {
        void (OSG_APIENTRY*TexSubImage3D)
                          (GLenum target, GLint level, GLint xoffset,
                           GLint yoffset, GLint zoffset, GLsizei width,
                           GLsizei height, GLsizei depth, GLenum format,
                           GLenum type, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, GLint xoffset,
                      GLint yoffset, GLint zoffset, GLsizei width,
                      GLsizei height, GLsizei depth, GLenum format,
                      GLenum type, const GLvoid *pixels))
            win->getFunction(_funcTexSubImage3D);

        void (OSG_APIENTRY*CompressedTexSubImage1D)
                          (GLenum target, GLint level, GLint xoffset, GLsizei width,
                           GLenum format, GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, GLint xoffset, GLsizei width,
                           GLenum format, GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexSubImage1D);

        void (OSG_APIENTRY*CompressedTexSubImage2D)
                          (GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, 
                           GLsizei width, GLsizei height, GLenum format,
                           GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, 
                           GLsizei width, GLsizei height, GLenum format,
                           GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexSubImage2D);

        void (OSG_APIENTRY*CompressedTexSubImage3D)
                          (GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, GLint zoffset, 
                           GLsizei width, GLsizei height, GLsizei depth, 
                           GLenum format, GLsizei imageSize, const GLvoid *pixels) =
            (void (OSG_APIENTRY*)(GLenum target, GLint level, 
                           GLint xoffset, GLint yoffset, GLint zoffset, 
                           GLsizei width, GLsizei height, GLsizei depth, 
                           GLenum format, GLsizei imageSize, const GLvoid *pixels))
            win->getFunction(_funcCompressedTexSubImage3D);

        GLenum externalFormat = img->getPixelFormat();
        GLenum type           = img->getDataType();
        bool   compressedData = img->hasCompressedData();
        bool   has3DTex       = win->hasExtension(_extTex3D) ||
                                win->getGLVersion() >= 0x0102;
        
        if(bindtarget == GL_TEXTURE_3D && !has3DTex)
        {
            FINFO(("3D textures not supported on Window %p!\n", win));
            return;
        }

        if(! img) // no image ?
            return;

        if(getExternalFormat() != GL_NONE)
            externalFormat = getExternalFormat();

        if(!getScale() || imgtarget == GL_TEXTURE_RECTANGLE_ARB
                       || win->hasExtension(_arbTextureNonPowerOfTwo) 
                       ||  (osgispower2(img->getWidth() ) &&
                            osgispower2(img->getHeight()) &&
                            osgispower2(img->getDepth() )
          )                )
        {
            // activate the texture
            glBindTexture(bindtarget, id);

            // Find out what to update
            UInt32 ix, ax, iy, ay, iz, az;
            ix = (getDirtyMinX()!=-1) ? getDirtyMinX() : 0;
            ax = (getDirtyMaxX()!=-1) ? getDirtyMaxX() : img->getWidth() - 1;
            iy = (getDirtyMinY()!=-1) ? getDirtyMinY() : 0;
            ay = (getDirtyMaxY()!=-1) ? getDirtyMaxY() : img->getHeight() - 1;
            iz = (getDirtyMinZ()!=-1) ? getDirtyMinZ() : 0;
            az = (getDirtyMaxZ()!=-1) ? getDirtyMaxZ() : img->getDepth() - 1;
            
            UInt32 w, h, d;
            w = ax - ix + 1;
            h = ay - iy + 1;
            d = az - iz + 1;

            if(w != img->getWidth())
                glPixelStorei(GL_UNPACK_ROW_LENGTH,  img->getWidth());
            if(ix != 0)
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, ix);
            if(iy != 0)
                glPixelStorei(GL_UNPACK_SKIP_ROWS,   iy);
            if(has3DTex && iz != 0)
                glPixelStorei(GL_UNPACK_SKIP_IMAGES, iz);
            
            if(compressedData)
            {
                switch (imgtarget)
                {
                case GL_TEXTURE_1D:
                    CompressedTexSubImage1D(GL_TEXTURE_1D, 0, ix, w,
                                    externalFormat, img->getFrameSize(),
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_2D:
                    CompressedTexSubImage2D(imgtarget, 0, ix, iy, w, h,
                                    externalFormat, img->getFrameSize(),
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                    CompressedTexSubImage2D(imgtarget, 0, ix, iy, w, h,
                                    externalFormat,     
                                    (img->getSideCount() > 1) ? img->getSideSize() :
                                                                img->getFrameSize(), 
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_RECTANGLE_ARB:
                    CompressedTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 
                                    ix, iy, w, h,
                                    externalFormat, img->getFrameSize(),
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_3D:
                      CompressedTexSubImage3D(GL_TEXTURE_3D, 0,  ix, iy, iz,
                                    w, h, d,
                                    externalFormat, img->getFrameSize(),
                                    img->getData( 0, getFrame(), side ) );
                    break;
                default:
                        SFATAL << "TextureChunk::refresh: unknown target "
                               << imgtarget << "!!!" << std::endl;
                }
            }
            else
            {
                switch (imgtarget)
                {
                case GL_TEXTURE_1D:
                    glTexSubImage1D(GL_TEXTURE_1D, 0, ix, w,
                                    externalFormat, type,
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
                    glTexSubImage2D(imgtarget, 0, ix, iy, w, h,
                                    externalFormat, type,
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_RECTANGLE_ARB:
                    glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0,
                                    ix, iy, w, h,
                                    externalFormat, type,
                                    img->getData( 0, getFrame(), side ) );
                    break;
                case GL_TEXTURE_3D:
                      TexSubImage3D(GL_TEXTURE_3D, 0,  ix, iy, iz,
                                    w, h, d,
                                    externalFormat, type,
                                    img->getData( 0, getFrame(), side ) );
                    break;
                default:
                        SFATAL << "TextureChunk::refresh: unknown target "
                               << imgtarget << "!!!" << std::endl;
                }
            }


            if(w != img->getWidth())
                glPixelStorei(GL_UNPACK_ROW_LENGTH,  0);
            if(ix != 0)
                glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
            if(iy != 0)
                glPixelStorei(GL_UNPACK_SKIP_ROWS,   0);
            if(has3DTex && iz != 0)
                glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0);
           
            if(paramtarget != GL_NONE)
            {
                // need to call these here, since switching between GL_NEAREST
                // and GL_LINEAR only triggers a refresh (which is safe).
                // (all other parameters will trigger a reinit)
                glTexParameterf(paramtarget, GL_TEXTURE_PRIORITY,
                                getPriority()                      );
                glTexParameteri(paramtarget, GL_TEXTURE_MIN_FILTER,
                                getMinFilter()                     );
                glTexParameteri(paramtarget, GL_TEXTURE_MAG_FILTER,
                                getMagFilter()                     );
            }
        }
        else
        {
            SWARNING << "TextureChunk::refresh: not implemented yet for "
                     << "scaling!!!" << std::endl;
        }

        glErr("TextureChunk::refresh image");
    }
}


/*! GL object handler
    create the texture and destroy it
*/
void TextureChunk::handleGL(Window *win, UInt32 idstatus)
{
    Window::GLObjectStatusE mode;
    UInt32 osgid;
    GLuint id;

    Window::unpackIdStatus(idstatus, osgid, mode);

    id = win->getGLObjectId(osgid);
    
    if(mode == Window::destroy)
    {
        glDeleteTextures(1, &id);
        win->setGLObjectId(osgid, 0);
    }
    else if(mode == Window::finaldestroy)
    {
        //SWARNING << "Last texture user destroyed" << std::endl;
    }
    else if(mode == Window::initialize || mode == Window::reinitialize ||
            mode == Window::needrefresh )
    {
        Time t0 = OSG::getSystemTime();
        _numTexCreate++;
        
        if(mode == Window::initialize)
        {
            glGenTextures(1, &id);
            win->setGLObjectId(osgid, id);
        }
        
        GLenum target;

        ImagePtr img = getImage();

        if (img != NullFC)
        {
            if(img->getSideCount() == 1)
            {
                target = getTarget();
                if ( target == GL_NONE )
                {
                    if(img->getDepth() > 1)
                    {
                        if(win->hasExtension(_extTex3D) ||
                           win->getGLVersion() >= 0x0102
                          )
                            target = GL_TEXTURE_3D;
                        else
                        {
                            FWARNING(("TextureChunk::initialize: 3D textures not "
                                        "supported for this window!\n"));
                            return;
                        }
                    }
                    else if(img->getHeight() > 1)        target = GL_TEXTURE_2D;
                    else                                 target = GL_TEXTURE_1D;
                }

                handleTexture(win, id, target, target, target, mode, img);
            }
            else // Got a cubemap...
            {
                handleTexture(win, id, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, 
                              mode, getImage(), 5); 
                // Have to use initialize mode here, otherwise the
                // texture is destroyed for every side      
                handleTexture(win, id, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_NONE, 
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, 
                              Window::initialize, getImage(), 4);        
                handleTexture(win, id, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_NONE, 
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, 
                              Window::initialize, getImage(), 3);        
                handleTexture(win, id, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_NONE, 
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, 
                              Window::initialize, getImage(), 2);        
                handleTexture(win, id, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_NONE, 
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, 
                              Window::initialize, getImage(), 1);        
                handleTexture(win, id, 
                              GL_TEXTURE_CUBE_MAP_ARB, 
                              GL_NONE, 
                              GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, 
                              Window::initialize, getImage(), 0);        

            }
        }
        
        Time t1 = OSG::getSystemTime();
        _summedTexCreateTime += (t1 - t0);
    }
    else
    {
        SWARNING << "TextureChunk(" << this << "::handleGL: Illegal mode: "
             << mode << " for id " << id << std::endl;
    }

}

void TextureChunk::activate( DrawActionBase *action, UInt32 idx )
{    
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
        FWARNING(("TextureChunk::activate: Trying to bind image unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, nteximages));
#endif
        return;        
    }

    if(activateTexture(win, idx))
        return; // trying to access too many textures

    win->validateGLObject(getGLId());

    ImagePtr img = getImage();
    GLenum target = getTarget();

    if( img == NullFC || ! img->getDimension()) // no image ?
        return;

    glErr("TextureChunk::activate precheck");

    if(img->getSideCount() == 1)
    {
        if ( target == GL_NONE )
        {
            if ( img->getDepth() > 1 )
            {
                    if(win->hasExtension(_extTex3D) ||
                       win->getGLVersion() >= 0x0102
                      )
                        target = GL_TEXTURE_3D;
                    else
                    {
                        FWARNING(("TextureChunk::activate: 3D textures not "
                                    "supported for this window!\n"));
                        return;
                    }
            }
            else if(img->getHeight() > 1)   target = GL_TEXTURE_2D;
            else                            target = GL_TEXTURE_1D;
        }
    }
    else
    {
        target = GL_TEXTURE_CUBE_MAP_ARB;
    }

    FDEBUG(("TextureChunk::activate - %d\n", getGLId()));

    action->getStatistics()->getElem(RenderAction::statNTextures)->inc(getGLId());
    action->getStatistics()->getElem(RenderAction::statNTexBytes)->add(
                                     getGLId(), getImage()->getSize());

    glBindTexture(target, win->getGLObjectId(getGLId()));

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

        handleTextureShader(win, target);

        if(getShaderOperation() != GL_NONE &&
           win->hasExtension(_nvTextureShader) &&
           idx == 0)
        {
            glEnable(GL_TEXTURE_SHADER_NV);
        }
        glEnable(target);
    }
    
    // Use texture matrix for scaling
    UInt32 NpotMatScale = getNPOTMatrixScale();
    bool setMatrix = false;
    
    if ( idx < static_cast<UInt32>(ntexcoords) &&
        !getScale() && NpotMatScale )
    {
        ImagePtr i = getImage();
        
        if (i != NullFC)
        {
            Real32 sw=1.f, sh=1.f, sd=1.f,
                   tw=0.f, th=0.f, td=0.f;
            
            if ( (NpotMatScale & NPotTexScale_TT) &&
                  getTarget() != GL_TEXTURE_RECTANGLE_ARB &&
                 !win->hasExtension(_arbTextureNonPowerOfTwo) )
            {
                UInt32 w,h,d,nw,nh,nd;
                
                w = i->getWidth();
                h = i->getHeight();
                d = i->getDepth();
                
                nw = osgnextpower2(w);
                nh = osgnextpower2(h);
                nd = osgnextpower2(d);
                
                sw = w / static_cast<Real32>(nw);
                sh = h / static_cast<Real32>(nh);
                sd = d / static_cast<Real32>(nd);
                
                setMatrix = true;
            }
            if ( (NpotMatScale & XFlip_TT) )
            {
                tw = sw - 1.f;
                sw *= -1.f;
                
                setMatrix = true;
            }
            if ( (NpotMatScale & YFlip_TT) )
            {
                th = sh - 1.f;
                sh *= -1.f;
                
                setMatrix = true;
            }
            if ( (NpotMatScale & ZFlip_TT) )
            {
                td = sd - 1.f;
                sd *= -1.f;
                
                setMatrix = true;
            }
            
            if (setMatrix)
            {
                Matrix m;
                
                m.setIdentity();
                m.setScale( Vec3f(sw, sh, sd) );
                m.setTranslate( Vec3f(tw, th, td) );

                glPushAttrib(GL_TRANSFORM_BIT);
                glMatrixMode(GL_TEXTURE);
                glLoadMatrixf(m.getValues());
                glPopAttrib();
            }
        }
    }
    
    if (!setMatrix && idx < static_cast<UInt32>(ntexcoords))
    {
        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glPopAttrib();
    }
    
    glErr("TextureChunk::activate");
}


void TextureChunk::changeFrom(DrawActionBase *action,
                              StateChunk     *old   ,
                              UInt32          idx )
{
    // change from me to me?
    // this assumes I haven't changed in the meantime.
    // is that a valid assumption?
    if(old == this)
        return;

    // If the old one is not a texture chunk, deactivate it and activate
    // ourselves
    // Need to check for exact type, as derived chunks might change
    // different state (e.g. CubeTexture)
    if(getTypeId() != old->getTypeId())
    {
        old->deactivate(action, idx);
        activate(action, idx);
        return;
    }

    TextureChunk *oldp      = dynamic_cast<TextureChunk *>(old);

    ImagePtr      img       = getImage();
    GLenum        target    = getTarget();
    GLenum        oldtarget = oldp->getTarget();
    bool          oldused   = (oldp->getImage() != NullFC &&
                               oldp->getImage()->getDimension());
    
    if(img == NullFC || img->getDimension() == 0)
    {
        oldp->deactivate(action, idx);
        return;
    }

    glErr("TextureChunk::changeFrom precheck");

    Window *win = action->getWindow();   

    if(activateTexture(win, idx))
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

    if(img->getSideCount() == 1)
    {
        if ( target == GL_NONE )
        {
            if ( img->getDepth() > 1 )
            {
                    if(win->hasExtension(_extTex3D) ||
                       win->getGLVersion() >= 0x0102
                      )
                        target = GL_TEXTURE_3D;
                    else
                    {
                        FWARNING(("TextureChunk::activate: 3D textures not "
                                    "supported for this window!\n"));
                        return;
                    }
            }
            else if(img->getHeight() > 1)   target = GL_TEXTURE_2D;
            else                            target = GL_TEXTURE_1D;
        }
    }
    else
    {
        target = GL_TEXTURE_CUBE_MAP_ARB;
    }

    if(oldused)
    {
        if(oldp->getImage()->getSideCount() == 1)
        {
            if(oldtarget == GL_NONE)
            {
                if(oldp->getImage()->getDepth() > 1)
                {
                    if(win->hasExtension(_extTex3D) ||
                       win->getGLVersion() >= 0x0102
                      )
                        oldtarget = GL_TEXTURE_3D;
                    else
                    {
                        FWARNING(("TextureChunk::changeFrom: 3D textures not "
                                    "supported for this window!\n"));
                        oldp->deactivate(action, idx);
                        return;
                    }
                }
                else if(oldp->getImage()->getHeight() > 1)
                {
                    oldtarget = GL_TEXTURE_2D;
                }
                else
                {
                    oldtarget = GL_TEXTURE_1D;
                }
            }
        }
        else
        {
            oldtarget = GL_TEXTURE_CUBE_MAP_ARB;
        }

        if(target != oldtarget && idx < ntexunits)
        {
            glDisable(oldtarget);
        }
    }

    win->validateGLObject(getGLId());

    action->getStatistics()->getElem(RenderAction::statNTextures)->inc(getGLId());
    action->getStatistics()->getElem(RenderAction::statNTexBytes)->add(
        getGLId(), getImage()->getSize());

    glBindTexture(target, win->getGLObjectId(getGLId()));
 
#ifdef GL_NV_point_sprite
    if(idx < ntexcoords)
    {
        if((!oldused || oldp->getPointSprite() != getPointSprite()) &&
           win->hasExtension(_nvPointSprite)
          )
        {
            glTexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV, getPointSprite());
        }
    }
#endif

    if(idx < nteximages)
    {
        if((!oldused || oldp->getLodBias() != getLodBias()) &&
           win->hasExtension(_extTextureLodBias)
          )
        {
            glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT,
                  getLodBias());
        }
    }

    if(idx < ntexunits)
    {
        if(!oldused || oldp->getEnvMode() != getEnvMode())
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

        if(target != oldtarget)
        {
            glEnable(target);
        }

        if(win->hasExtension(_nvTextureShader))
        {
            if(      getShaderOperation() != GL_NONE &&
               (!oldused || oldp->getShaderOperation() == GL_NONE)
              )
            {
                handleTextureShader(win, target);
                if(idx == 0)
                    glEnable(GL_TEXTURE_SHADER_NV);
            }
            else if(      getShaderOperation() == GL_NONE &&
                    (!oldused || oldp->getShaderOperation() != GL_NONE)
                   )
            {
                glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);
                if(idx == 0)
                    glDisable(GL_TEXTURE_SHADER_NV);
            }
        }
    }
    
    // Use texture matrix for scaling
    UInt32 NpotMatScale = getNPOTMatrixScale();
    bool setMatrix = false;
    
    if ( idx < static_cast<UInt32>(ntexcoords) &&
        !getScale() && NpotMatScale )
    {
        ImagePtr i = getImage();
        
        if (i != NullFC)
        {
            Real32 sw=1.f, sh=1.f, sd=1.f,
                   tw=0.f, th=0.f, td=0.f;
            
            if ( (NpotMatScale & NPotTexScale_TT) &&
                  getTarget() != GL_TEXTURE_RECTANGLE_ARB &&
                 !win->hasExtension(_arbTextureNonPowerOfTwo) )
            {
                UInt32 w,h,d,nw,nh,nd;
                
                w = i->getWidth();
                h = i->getHeight();
                d = i->getDepth();
                
                nw = osgnextpower2(w);
                nh = osgnextpower2(h);
                nd = osgnextpower2(d);
                
                sw = w / static_cast<Real32>(nw);
                sh = h / static_cast<Real32>(nh);
                sd = d / static_cast<Real32>(nd);
                
                setMatrix = true;
            }
            if ( (NpotMatScale & XFlip_TT) )
            {
                tw = sw - 1.f;
                sw *= -1.f;
                
                setMatrix = true;
            }
            if ( (NpotMatScale & YFlip_TT) )
            {
                th = sh - 1.f;
                sh *= -1.f;
                
                setMatrix = true;
            }
            if ( (NpotMatScale & ZFlip_TT) )
            {
                td = sd - 1.f;
                sd *= -1.f;
                
                setMatrix = true;
            }
            
            if (setMatrix)
            {
                Matrix m;
                
                m.setIdentity();
                m.setScale( Vec3f(sw, sh, sd) );
                m.setTranslate( Vec3f(tw, th, td) );
                
                glPushAttrib(GL_TRANSFORM_BIT);
                glMatrixMode(GL_TEXTURE);
                glLoadMatrixf(m.getValues());
                glPopAttrib();
            }
        }
    }

    if (!setMatrix && idx < static_cast<UInt32>(ntexcoords))
    {
        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glPopAttrib();
    }
    
    glErr("TextureChunk::changeFrom");
}

void TextureChunk::deactivate(DrawActionBase *action, UInt32 idx)
{
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
        FWARNING(("TextureChunk::deactivate: Trying to bind image unit %d,"
                  " but Window %p only supports %d!\n",
                  idx, win, nteximages));
#endif
        return;        
    }

    ImagePtr img = getImage();
    GLenum target = getTarget();

    if(img == NullFC || ! img->getDimension())
      return;

    glErr("TextureChunk::deactivate precheck");

    bool isActive = false;

#ifdef GL_NV_point_sprite
    if(getPointSprite() &&
       win->hasExtension(_nvPointSprite) &&
       idx < static_cast<UInt32>(ntexcoords)
      )
    {
        if(!isActive)
        {
            activateTexture(win, idx);
            isActive = true;
        }
        glTexEnvi(GL_POINT_SPRITE_NV, GL_COORD_REPLACE_NV, GL_FALSE);
    }
#endif

    if(getLodBias() != 0.0f &&
       win->hasExtension(_extTextureLodBias))
    {
        if(!isActive)
            activateTexture(win, idx);
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT,
                0.0f);
    }
    
    Real32 ntexunits = win->getConstantValue(GL_MAX_TEXTURE_UNITS_ARB);
    if(ntexunits == Window::unknownConstant)
        ntexunits = 1.0f;

    if(idx >= static_cast<UInt32>(ntexunits))
        return; // textures >= MTU are not enabled and don't have an env
        
    if(!isActive)
        activateTexture(win, idx);

    if(img->getSideCount() == 1)
    {
        if ( target == GL_NONE )
        {
            if ( img->getDepth() > 1 )
            {
                    if(win->hasExtension(_extTex3D) ||
                       win->getGLVersion() >= 0x0102
                      )
                        target = GL_TEXTURE_3D;
                    else
                    {
                        FWARNING(("TextureChunk::activate: 3D textures not "
                                    "supported for this window!\n"));
                        return;
                    }
            }
            else if(img->getHeight() > 1)   target = GL_TEXTURE_2D;
            else                            target = GL_TEXTURE_1D;
        }
    }
    else
    {
        target = GL_TEXTURE_CUBE_MAP_ARB;
    }

    if(getShaderOperation() != GL_NONE &&
       win->hasExtension(_nvTextureShader))
    {
        glTexEnvi(GL_TEXTURE_SHADER_NV, GL_SHADER_OPERATION_NV, GL_NONE);

        if(idx == 0)
            glDisable(GL_TEXTURE_SHADER_NV);
    }
    
    // be consistent with TextureTransform which has to multiply
    //UInt32 NpotMatScale = getNPOTMatrixScale();
    
    if ( idx < static_cast<UInt32>(ntexcoords) )
    {
        glPushAttrib(GL_TRANSFORM_BIT);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glPopAttrib();
    }
    
    glDisable(target);

    glErr("TextureChunk::deactivate");
}

/*-------------------------- Comparison -----------------------------------*/

Real32 TextureChunk::switchCost(StateChunk *OSG_CHECK_ARG(chunk))
{
    return 0;
}

bool TextureChunk::operator < (const StateChunk &other) const
{
    return this < &other;
}

bool TextureChunk::operator == (const StateChunk &other) const
{
    TextureChunk const *tother = dynamic_cast<TextureChunk const*>(&other);

    if(!tother)
        return false;

    if(tother == this)
        return true;

    bool returnValue =
        getImage    () == tother->getImage    () &&
        getMinFilter() == tother->getMinFilter() &&
        getMagFilter() == tother->getMagFilter() &&
        getWrapS    () == tother->getWrapS    () &&
        getWrapT    () == tother->getWrapT    () &&
        getWrapR    () == tother->getWrapR    () &&
        getPriority () == tother->getPriority () &&
        getEnvMode  () == tother->getEnvMode  () &&
        getNPOTMatrixScale() == tother->getNPOTMatrixScale();

    if(returnValue == true && getEnvMode() == GL_COMBINE_EXT)
    {
        returnValue =
            getEnvCombineRGB ()   == tother->getEnvCombineRGB   () &&

            getEnvSource0RGB ()   == tother->getEnvSource0RGB   () &&
            getEnvSource1RGB ()   == tother->getEnvSource1RGB   () &&
            getEnvSource2RGB ()   == tother->getEnvSource2RGB   () &&

            getEnvOperand0RGB()   == tother->getEnvOperand0RGB  () &&
            getEnvOperand1RGB()   == tother->getEnvOperand1RGB  () &&
            getEnvOperand2RGB()   == tother->getEnvOperand2RGB  () &&

            getEnvCombineAlpha () == tother->getEnvCombineAlpha () &&

            getEnvSource0Alpha () == tother->getEnvSource0Alpha () &&
            getEnvSource1Alpha () == tother->getEnvSource1Alpha () &&
            getEnvSource2Alpha () == tother->getEnvSource2Alpha () &&

            getEnvOperand0Alpha() == tother->getEnvOperand0Alpha() &&
            getEnvOperand1Alpha() == tother->getEnvOperand1Alpha() &&
            getEnvOperand2Alpha() == tother->getEnvOperand2Alpha();

        returnValue &=
           ((        getEnvScaleRGB  () - tother->getEnvScaleRGB  ()) < Eps) &&
           ((tother->getEnvScaleRGB  () -         getEnvScaleRGB  ()) < Eps) &&
           ((        getEnvScaleAlpha() - tother->getEnvScaleAlpha()) < Eps) &&
           ((tother->getEnvScaleAlpha() -         getEnvScaleAlpha()) < Eps);
    }

    return returnValue;
}

bool TextureChunk::operator != (const StateChunk &other) const
{
    return ! (*this == other);
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: FCTemplate_cpp.h,v 1.13 2002/06/01 10:37:25 vossg Exp $";
    static Char8 cvsid_hpp       [] = OSGTEXTURECHUNK_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGTEXTURECHUNK_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGTEXTURECHUNKFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

