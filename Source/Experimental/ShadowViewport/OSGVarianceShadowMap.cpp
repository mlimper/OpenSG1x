#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>
#include <OSGTypedFunctors.h>
#include <OSGQuaternion.h>
#include <OSGDrawAction.h>

#include <OSGMatrix.h>
#include <OSGMatrixUtility.h>

#include <OSGBackground.h>
#include <OSGForeground.h>
#include <OSGGrabForeground.h>
#include <OSGPolygonForeground.h>
#include <OSGPolygonBackground.h>
#include <OSGTextureGrabForeground.h>
#include <OSGFileGrabForeground.h>
#include <OSGImageForeground.h>
#include <OSGImage.h>
#include <OSGGeometry.h>
#include <OSGSimpleGeometry.h>

#include <OSGLight.h>
#include <OSGMaterialGroup.h>

#include "OSGVarianceShadowMap.h"
#include "OSGShadowViewport.h"
#include "OSGTreeRenderer.h"

//--------------------------------------------------------------------
#ifndef GL_CLAMP_TO_EDGE
#   define GL_CLAMP_TO_EDGE    0x812F
#endif
#ifndef GL_CLAMP_TO_BORDER_ARB
#   define GL_CLAMP_TO_BORDER_ARB  0x812D
#endif
#ifndef GL_ARB_depth_texture
#   define GL_DEPTH_COMPONENT16_ARB    0x81A5
#   define GL_DEPTH_COMPONENT24_ARB    0x81A6
#   define GL_DEPTH_COMPONENT32_ARB    0x81A7
#   define GL_TEXTURE_DEPTH_SIZE_ARB   0x884A
#   define GL_DEPTH_TEXTURE_MODE_ARB   0x884B
#endif
#ifndef GL_ARB_shadow
#   define GL_TEXTURE_COMPARE_MODE_ARB 0x884C
#   define GL_TEXTURE_COMPARE_FUNC_ARB 0x884D
#   define GL_COMPARE_R_TO_TEXTURE_ARB 0x884E
#endif
#ifndef GL_SGIX_depth_texture
#   define GL_DEPTH_COMPONENT16_SGIX   0x81A5
#   define GL_DEPTH_COMPONENT24_SGIX   0x81A6
#   define GL_DEPTH_COMPONENT32_SGIX   0x81A7
#endif
#ifndef GL_SGIX_shadow
#   define GL_TEXTURE_COMPARE_SGIX             0x819A
#   define GL_TEXTURE_COMPARE_OPERATOR_SGIX    0x819B
#   define GL_TEXTURE_LEQUAL_R_SGIX            0x819C
#   define GL_TEXTURE_GEQUAL_R_SGIX            0x819D
#endif
#ifndef GL_DEPTH_COMPONENT_ARB
#   define GL_DEPTH_COMPONENT_ARB            0x1902
#endif
#if 1 //GL_ARB_texture_float
#	define	GL_RGBA32F_ARB                      0x8814
#	define	GL_RGB32F_ARB                       0x8815
#	define	GL_ALPHA32F_ARB                     0x8816
#	define	GL_INTENSITY32F_ARB                 0x8817
#	define	GL_LUMINANCE32F_ARB                 0x8818
#	define	GL_LUMINANCE_ALPHA32F_ARB           0x8819
#	define	GL_RGBA16F_ARB                      0x881A
#	define	GL_RGB16F_ARB                       0x881B
#	define	GL_ALPHA16F_ARB                     0x881C
#	define	GL_INTENSITY16F_ARB                 0x881D
#	define	GL_LUMINANCE16F_ARB                 0x881E
#	define	GL_LUMINANCE_ALPHA16F_ARB           0x881F
#endif

/* ----------------------- GL_EXT_framebuffer_object ----------------------- */

#ifndef GL_EXT_framebuffer_object
    #define GL_INVALID_FRAMEBUFFER_OPERATION_EXT 0x0506
    #define GL_MAX_RENDERBUFFER_SIZE_EXT 0x84E8
    #define GL_FRAMEBUFFER_BINDING_EXT 0x8CA6
    #define GL_RENDERBUFFER_BINDING_EXT 0x8CA7
    #define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT 0x8CD0
    #define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT 0x8CD1
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT 0x8CD2
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT 0x8CD3
    #define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT 0x8CD4
    #define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
    #define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT 0x8CD6
    #define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT 0x8CD7
    #define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT 0x8CD8
    #define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT 0x8CD9
    #define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT 0x8CDA
    #define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT 0x8CDB
    #define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT 0x8CDC
    #define GL_FRAMEBUFFER_UNSUPPORTED_EXT 0x8CDD
    #define GL_FRAMEBUFFER_STATUS_ERROR_EXT 0x8CDE
    #define GL_MAX_COLOR_ATTACHMENTS_EXT 0x8CDF
    #define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
    #define GL_COLOR_ATTACHMENT1_EXT 0x8CE1
    #define GL_COLOR_ATTACHMENT2_EXT 0x8CE2
    #define GL_COLOR_ATTACHMENT3_EXT 0x8CE3
    #define GL_COLOR_ATTACHMENT4_EXT 0x8CE4
    #define GL_COLOR_ATTACHMENT5_EXT 0x8CE5
    #define GL_COLOR_ATTACHMENT6_EXT 0x8CE6
    #define GL_COLOR_ATTACHMENT7_EXT 0x8CE7
    #define GL_COLOR_ATTACHMENT8_EXT 0x8CE8
    #define GL_COLOR_ATTACHMENT9_EXT 0x8CE9
    #define GL_COLOR_ATTACHMENT10_EXT 0x8CEA
    #define GL_COLOR_ATTACHMENT11_EXT 0x8CEB
    #define GL_COLOR_ATTACHMENT12_EXT 0x8CEC
    #define GL_COLOR_ATTACHMENT13_EXT 0x8CED
    #define GL_COLOR_ATTACHMENT14_EXT 0x8CEE
    #define GL_COLOR_ATTACHMENT15_EXT 0x8CEF
    #define GL_DEPTH_ATTACHMENT_EXT 0x8D00
    #define GL_STENCIL_ATTACHMENT_EXT 0x8D20
    #define GL_FRAMEBUFFER_EXT 0x8D40
    #define GL_RENDERBUFFER_EXT 0x8D41
    #define GL_RENDERBUFFER_WIDTH_EXT 0x8D42
    #define GL_RENDERBUFFER_HEIGHT_EXT 0x8D43
    #define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT 0x8D44
    #define GL_STENCIL_INDEX_EXT 0x8D45
    #define GL_STENCIL_INDEX1_EXT 0x8D46
    #define GL_STENCIL_INDEX4_EXT 0x8D47
    #define GL_STENCIL_INDEX8_EXT 0x8D48
    #define GL_STENCIL_INDEX16_EXT 0x8D49
#endif

#ifndef GL_TEXTURE_RECTANGLE_ARB
    #define GL_TEXTURE_RECTANGLE_ARB 0x84F5
#endif

#ifndef GL_DEPTH_COMPONENT_ARB
    #define GL_DEPTH_COMPONENT_ARB 0x1902
#endif

#ifndef GL_ARB_depth_texture
    #define GL_DEPTH_COMPONENT16_ARB 0x81A5
    #define GL_DEPTH_COMPONENT24_ARB 0x81A6
    #define GL_DEPTH_COMPONENT32_ARB 0x81A7
    #define GL_TEXTURE_DEPTH_SIZE_ARB 0x884A
    #define GL_DEPTH_TEXTURE_MODE_ARB 0x884B
#endif

#ifndef GL_ARB_shadow
    #define GL_TEXTURE_COMPARE_MODE_ARB 0x884C
    #define GL_TEXTURE_COMPARE_FUNC_ARB 0x884D
    #define GL_COMPARE_R_TO_TEXTURE_ARB 0x884E
#endif

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF


OSG_USING_NAMESPACE


static std::string _depth_vp =
"varying vec3 lightVec;\n"
"varying vec4 screenPos;\n"
"\n"
"void main(void)\n"
"{\n"
"  lightVec = (gl_ModelViewMatrix * gl_Vertex).xyz;\n"
"  screenPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"  gl_Position = ftransform();\n"
"}\n";

static std::string _depth_fp =
"uniform float sceneDiagLength;\n"
"uniform bool isDirLight;\n"
"varying vec3 lightVec;\n"
"varying vec4 screenPos;\n"
"\n"
"const float dist_fact = 32.0;\n"
"\n"
"void main(void)\n"
"{\n"
"	float x;\n"
"	if(isDirLight) x = gl_FragCoord.z;\n"
"	else x = ((length(lightVec)/sceneDiagLength)*2.0)-1.0;\n"
"	vec2 moments = vec2(x, x*x);\n"
"	vec2 m1,m2;\n"
"	m2 = fract(moments * dist_fact);\n"
"	m1 = moments - (m2 / dist_fact);\n"
"	gl_FragColor = vec4(m1,m2);\n"
"}\n";

static std::string _variance_vp =
"uniform float shadowBias;\n"
"uniform mat4 lightPM;\n"
"uniform mat4 lightPM2;\n"
"uniform float texFactor;\n"
"varying vec4 projCoord;\n"
"varying vec4 projCoord2;\n"
"varying vec4 texPos;\n"
"varying vec4 realPos;\n"
"\n"
"const mat4 bias = {vec4(0.5,0.0,0.0,0.0), vec4(0.0,0.5,0.0,0.0), vec4(0.0,0.0,0.5,0.0), vec4(0.5,0.5,0.5,1.0)};\n"
"\n"
"void main(void)\n"
"{\n"
"  realPos = gl_ModelViewMatrix * gl_Vertex;\n"
"  projCoord = lightPM * realPos;\n"
"  projCoord.x *= texFactor;\n"
"  projCoord2 = lightPM2 * realPos;\n"
"  texPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"  projCoord = bias * projCoord;\n"
"  texPos = bias * texPos;\n"
"  gl_Position = ftransform();\n"
"}\n";

static std::string _variance_fp =
"uniform sampler2D shadowMap;\n"
"uniform sampler2D oldFactorMap;\n"
"uniform float intensity;\n"
"uniform int firstRun;\n"
"uniform float xFactor;\n"
"uniform float yFactor;\n"
"uniform float sceneDiagLength;\n"
"uniform float Lod;\n"
"uniform bool isDirLight;\n"
"\n"
"varying vec4 projCoord;\n"
"varying vec4 projCoord2;\n"
"varying vec4 texPos;\n"
"varying vec4 realPos;\n"
"\n"
"const float epsilon = 0.00025;\n"
"const float dist_factor = 32.0;\n"
"\n"
"void main(void)\n"
"{\n"
"	vec4 projectiveBiased = vec4((projCoord.xyz / projCoord.q),1.0);\n"
"	float shadowed = 0.0;\n"
"\n"
"	vec2 moments = texture2DLod(shadowMap,projectiveBiased.xy,Lod).xy;\n"
"	moments += texture2DLod(shadowMap,projectiveBiased.xy,Lod).zw / dist_factor;\n"
"	\n"
"	float distL;\n"
"	if(isDirLight) distL = projectiveBiased.z;\n"
"	else distL = ((length(projCoord2.xyz)/sceneDiagLength)*2.0)-1.0;\n"
"\n"
"	if(distL  < moments.x) shadowed = 0.0;\n"
"	else\n"
"	{\n"
"		float E_x2 = moments.y;\n"
"		float Ex_2 = moments.x * moments.x;\n"
"		float variance = min(max(E_x2 - Ex_2,epsilon), 1.0);\n"
"		float m_d = moments.x - distL;\n"
"		float p_max = variance / (variance + m_d * m_d);\n"
"		shadowed = 1.0 - p_max;\n"
"	}\n"
"	shadowed *= intensity;\n"
"	if(firstRun == 0) shadowed += texture2DProj(oldFactorMap,vec3(texPos.xy * vec2(xFactor,yFactor),texPos.w)).x;\n"
"	gl_FragColor = vec4(shadowed,0.0,0.0,1.0);\n"
"}\n";

static std::string _variance_combine_vp =
"varying vec4 projCoord;\n"
"\n"
"void main(void)\n"
"{\n"
" projCoord = gl_Vertex;\n"
"  gl_Position = ftransform();\n"
"}\n";

static std::string _variance_combine_fp =
"uniform sampler2D colorMap;\n"
"uniform sampler2D shadowFactorMap;\n"
"uniform float xFactor;\n"
"uniform float yFactor;\n"
"varying vec4 projCoord;\n"
"const mat4 bias = {vec4(0.5,0.0,0.0,0.0), vec4(0.0,0.5,0.0,0.0), vec4(0.0,0.0,0.5,0.0), vec4(0.5,0.5,0.5,1.0)};\n"
"\n"
"void main(void)\n"
"{\n"
"	vec4 colorProj = bias * projCoord;\n"
"	vec2 cp = colorProj.xy * vec2(xFactor,yFactor);\n"
"	vec3 color = texture2DProj(colorMap, vec3(cp,colorProj.w)).xyz;\n"
"	color *= 1.0-texture2DProj(shadowFactorMap, vec3(cp,colorProj.w)).x;\n"
"	gl_FragColor = vec4(color, 1.0);\n"
"}\n";


VarianceShadowMap::VarianceShadowMap(void)
{
}

VarianceShadowMap::VarianceShadowMap(ShadowViewport *source)
: TreeRenderer(source)
{
	fb = 0;
    fb2 = 0;
	rb_depth = 0;
	rb_depth2 = 0;

	texChanged = false;

	width = 1;
    height = 1;
    //width = shadowVP->getParent()->getWidth();
    //height = shadowVP->getParent()->getHeight();
    if(shadowVP->getParent() != NullFC)
    {
        width = shadowVP->getPixelWidth();
        height = shadowVP->getPixelHeight();
    }

	if(width > height) widthHeightPOT = osgnextpower2(width);
	else widthHeightPOT = osgnextpower2(height);

	_tiledeco = TileCameraDecorator::create();
    addRefCP(_tiledeco);
    
    //Prepare Color Map grabbing
    _colorMap = TextureChunk::create();
    _colorMapImage = Image::create();

    beginEditCP(_colorMap);
        _colorMap->setImage(_colorMapImage);
        _colorMap->setInternalFormat(GL_RGB);
        _colorMap->setExternalFormat(GL_RGB);
        _colorMap->setMinFilter(GL_NEAREST);
        _colorMap->setMagFilter(GL_NEAREST);
        _colorMap->setWrapS(GL_REPEAT);
        _colorMap->setWrapT(GL_REPEAT);
        _colorMap->setTarget(GL_TEXTURE_2D);
    endEditCP(_colorMap);


	if(useNPOTTextures)
	{
		beginEditCP(_colorMapImage);
			_colorMapImage->set(GL_RGB, width, height);
		endEditCP(_colorMapImage);
	}
	else
	{
		beginEditCP(_colorMapImage);
			_colorMapImage->set(GL_RGB, widthHeightPOT, widthHeightPOT);
		endEditCP(_colorMapImage);
	}


    //Prepare Shadow Factor Map grabbing
    _shadowFactorMap = TextureChunk::create();
    _shadowFactorMapImage = Image::create();

    beginEditCP(_shadowFactorMap);
        _shadowFactorMap->setImage(_shadowFactorMapImage);
        _shadowFactorMap->setInternalFormat(GL_RGB);
        _shadowFactorMap->setExternalFormat(GL_RGB);
        _shadowFactorMap->setMinFilter(GL_LINEAR);
        _shadowFactorMap->setMagFilter(GL_LINEAR);
        _shadowFactorMap->setWrapS(GL_REPEAT);
        _shadowFactorMap->setWrapT(GL_REPEAT);
        _shadowFactorMap->setTarget(GL_TEXTURE_2D);
    endEditCP(_shadowFactorMap);

	if(useNPOTTextures)
	{
	    beginEditCP(_shadowFactorMapImage);
		    _shadowFactorMapImage->set(GL_RGB, width, height);
		endEditCP(_shadowFactorMapImage);
	}
	else
	{
		beginEditCP(_shadowFactorMapImage);
		    _shadowFactorMapImage->set(GL_RGB, widthHeightPOT, widthHeightPOT);
		endEditCP(_shadowFactorMapImage);
	}

    //SHL Chunk 1

    _shadowSHL = SHLChunk::create();
    beginEditCP(_shadowSHL);
        //_shadowSHL->readVertexProgram("Variance_Shadow.vert");
		//_shadowSHL->readFragmentProgram("Variance_Shadow.frag");
		_shadowSHL->setVertexProgram(_variance_vp);
		_shadowSHL->setFragmentProgram(_variance_fp);
    endEditCP(_shadowSHL);

    //SHL Chunk 2
    _combineSHL = SHLChunk::create();

    beginEditCP(_combineSHL);
        //_combineSHL->readVertexProgram("Variance_Shadow_combine.vert");
        //_combineSHL->readFragmentProgram("Variance_Shadow_combine.frag");
		_combineSHL->setVertexProgram(_variance_combine_vp);
		_combineSHL->setFragmentProgram(_variance_combine_fp);
    endEditCP(_combineSHL);

    _shadowCmat = ChunkMaterial::create();
    _shadowRoot = Node::create();

    _shadowShaderGroup = MaterialGroup::create();
    beginEditCP(_shadowShaderGroup, Geometry::MaterialFieldMask);
        _shadowShaderGroup->setMaterial(_shadowCmat);
    endEditCP(_shadowShaderGroup, Geometry::MaterialFieldMask);

    //Combine Shader
    _combineCmat = ChunkMaterial::create();
    _combineRoot = Node::create();

    _combineShaderGroup = MaterialGroup::create();
    beginEditCP(_combineShaderGroup, Geometry::MaterialFieldMask);
        _combineShaderGroup->setMaterial(_combineCmat);
    endEditCP(_combineShaderGroup, Geometry::MaterialFieldMask);

    beginEditCP(_combineRoot);
        _combineRoot->setCore(_combineShaderGroup);
    endEditCP(_combineRoot);


	//SHL Depth
    _depthSHL = SHLChunk::create();

    beginEditCP(_depthSHL);
        //_depthSHL->readVertexProgram("depth.vert");
        //_depthSHL->readFragmentProgram("depth.frag");
		_depthSHL->setVertexProgram(_depth_vp);
		_depthSHL->setFragmentProgram(_depth_fp);
    endEditCP(_depthSHL);

    _depthCmat = ChunkMaterial::create();
    _depthRoot = Node::create();

    _depthShaderGroup = MaterialGroup::create();
    beginEditCP(_depthShaderGroup, Geometry::MaterialFieldMask);
        _depthShaderGroup->setMaterial(_depthCmat);
    endEditCP(_depthShaderGroup, Geometry::MaterialFieldMask);

    //Kamera um Texturew�rfel zu rendern
    _matrixCam = MatrixCamera::create();
    MatrixOrthogonal(textureVM,-1,1,-1,1,-1,1);
    MatrixLookAt(texturePM,0,1,0,0,0,0,0,1,0);
    _matrixCam->setNear(-1.0);
    _matrixCam->setFar(1.0);
    _matrixCam->setProjectionMatrix(textureVM);
    _matrixCam->setModelviewMatrix(texturePM);

    //Box zum Texturzeichnen
    boxGeo = makeBoxGeo(2,2,2, 1, 1, 1);
    boxNode = Node::create();
    beginEditCP(boxNode, Node::CoreFieldMask);
        boxNode->setCore(boxGeo);
    endEditCP(boxNode, Node::CoreFieldMask);

    addRefCP(_shadowSHL);
    addRefCP(_combineSHL);
	addRefCP(_depthSHL);
	addRefCP(_shadowRoot);
    addRefCP(_combineRoot);
	addRefCP(_depthRoot);
	addRefCP(_shadowCmat);
    addRefCP(_combineCmat);
	addRefCP(_depthCmat);
	addRefCP(_shadowShaderGroup);
    addRefCP(_combineShaderGroup);
	addRefCP(_depthShaderGroup);
	addRefCP(_matrixCam);
    addRefCP(shadowVP->getRoot());
    addRefCP(boxGeo);
    addRefCP(boxNode);

}

VarianceShadowMap::~VarianceShadowMap(void)
{
    subRefCP(_tiledeco);

    subRefCP(_colorMap);
    subRefCP(_shadowFactorMap);
    subRefCP(_shadowFactorMap);
    subRefCP(_matrixCam);
    subRefCP(_shadowSHL);
    subRefCP(_combineSHL);
	subRefCP(_depthSHL);
	subRefCP(_shadowShaderGroup);
    subRefCP(_combineShaderGroup);
	subRefCP(_depthShaderGroup);
	
    subRefCP(_shadowRoot);
    subRefCP(_combineRoot);
    subRefCP(_shadowSHL);
    subRefCP(_combineSHL);
	subRefCP(_depthSHL);
	
    subRefCP(boxGeo);
    subRefCP(boxNode);

    if(fb != 0)
        glDeleteFramebuffersEXT(1, &fb);
    if(rb_depth != 0)
        glDeleteRenderbuffersEXT( 1, &rb_depth);
    if(fb2 != 0)
        glDeleteFramebuffersEXT(1, &fb2);
	if(rb_depth2 != 0)
        glDeleteRenderbuffersEXT( 1, &rb_depth2);

	for(UInt32 i = 0; i<shadowVP->_lights.size();i++)
	{
		beginEditCP(shadowVP->_texChunks[i]);
			shadowVP->_texChunks[i]->setImage(shadowVP->_shadowImages[i]);
			shadowVP->_texChunks[i]->setInternalFormat(GL_DEPTH_COMPONENT_ARB);
			shadowVP->_texChunks[i]->setExternalFormat(GL_DEPTH_COMPONENT_ARB);
			shadowVP->_texChunks[i]->setMinFilter(GL_LINEAR);
			shadowVP->_texChunks[i]->setMagFilter(GL_LINEAR);
			shadowVP->_texChunks[i]->setWrapS(GL_REPEAT);
			shadowVP->_texChunks[i]->setWrapT(GL_REPEAT);
			shadowVP->_texChunks[i]->setTarget(GL_TEXTURE_2D);
		endEditCP(shadowVP->_texChunks[i]);
		
		beginEditCP(shadowVP->_shadowImages[i]);
			shadowVP->_shadowImages[i]->set(Image::OSG_L_PF,shadowVP->getMapSize(), shadowVP->getMapSize(),
                                  1, 1, 1, 0, NULL,
                                  Image::OSG_FLOAT16_IMAGEDATA, false);
		endEditCP(shadowVP->_shadowImages[i]);
	}
}

/// Checks if FBO status is ok
bool VarianceShadowMap::checkFrameBufferStatus(Window *win)
{
    GLenum errCode, status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    
    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT: 
        FINFO(("%x: framebuffer complete!\n", status));
        break; 
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT: 
        FWARNING(("%x: framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n", status));
        // choose different formats
        return false;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: 
        FWARNING(("%x: framebuffer INCOMPLETE_ATTACHMENT\n", status));
        break; 
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: 
        FWARNING(("%x: framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n", status));
        break; 
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: 
        FWARNING(("%x: framebuffer FRAMEBUFFER_DIMENSIONS\n", status));
        break; 
        case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT: 
        FWARNING(("%x: framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n", status));
        break; 
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: 
        FWARNING(("%x: framebuffer INCOMPLETE_FORMATS\n", status));
        break; 
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: 
        FWARNING(("%x: framebuffer INCOMPLETE_DRAW_BUFFER\n", status));
        break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: 
        FWARNING(("%x: framebuffer INCOMPLETE_READ_BUFFER\n", status));
        break; 
        case GL_FRAMEBUFFER_BINDING_EXT: 
        FWARNING(("%x: framebuffer BINDING_EXT\n", status));
        break; 
        default: 
        return false;
    }
    
    if ((errCode = glGetError()) != GL_NO_ERROR)
    {
        const GLubyte *errString = gluErrorString(errCode);
        FWARNING(("OpenGL Error: %s!\n", errString));
        return false;
    }
    return true;
}


bool VarianceShadowMap::initFBO(Window *win)
{
	initialize(win);

	if(!texChanged)
	{
		//Set Shadow Map Texture to the needed Format
		Real32 maximumAnistropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistropy);
		maximumAnistropy = osgMin(maximumAnistropy,Real32(8.0));
		
		for(UInt32 i = 0; i<shadowVP->_lights.size();i++)
		{
			beginEditCP(shadowVP->_texChunks[i]);
				shadowVP->_texChunks[i]->setImage(shadowVP->_shadowImages[i]);
				shadowVP->_texChunks[i]->setInternalFormat(GL_RGBA16F_ARB);
				shadowVP->_texChunks[i]->setExternalFormat(GL_RGBA);
				shadowVP->_texChunks[i]->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
				//shadowVP->_texChunks[i]->setMinFilter(GL_LINEAR);
				shadowVP->_texChunks[i]->setMagFilter(GL_LINEAR);
				shadowVP->_texChunks[i]->setAnisotropy(maximumAnistropy);
				shadowVP->_texChunks[i]->setWrapS(GL_REPEAT);
				shadowVP->_texChunks[i]->setWrapT(GL_REPEAT);
				shadowVP->_texChunks[i]->setTarget(GL_TEXTURE_2D);
			endEditCP(shadowVP->_texChunks[i]);

			beginEditCP(shadowVP->_shadowImages[i]);
				shadowVP->_shadowImages[i]->set(Image::OSG_RGBA_PF, shadowVP->getMapSize(), shadowVP->getMapSize(), 1,1,1,0,0,Image::OSG_FLOAT16_IMAGEDATA,false);
			endEditCP(shadowVP->_shadowImages[i]);
		}
		texChanged = true;
	}

	if(useFBO)
	{

	Int32 width  = shadowVP->getPixelWidth();
    Int32 height = shadowVP->getPixelHeight();

    if (width <= 0 || height <= 0)
        return false;
        
    if (fb != NULL)
        return true;

	glGenFramebuffersEXT(1, &fb);
	glGenRenderbuffersEXT( 1, &rb_depth);

	win->validateGLObject(_colorMap->getGLId());
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, win->getGLObjectId(_colorMap->getGLId()), 0);
	win->validateGLObject(_shadowFactorMap->getGLId());
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, win->getGLObjectId(_shadowFactorMap->getGLId()), 0);

	//Initialize Depth Renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rb_depth);
	if(useNPOTTextures) glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, shadowVP->getPixelWidth(), shadowVP->getPixelHeight() );
	else glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, widthHeightPOT, widthHeightPOT );

	//Attach Renderbuffer to Framebuffer depth Buffer
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rb_depth);

	win->validateGLObject(_colorMap->getGLId());
	//setTarget(win, win->getGLObjectId(_colorMap->getGLId()), 0, GL_TEXTURE_2D);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, win->getGLObjectId(_colorMap->getGLId()), 0);

	win->validateGLObject(_shadowFactorMap->getGLId());
	//setTarget(win, win->getGLObjectId(_shadowFactorMap->getGLId()), 1, GL_TEXTURE_2D);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, win->getGLObjectId(_shadowFactorMap->getGLId()), 0);

	bool result = checkFrameBufferStatus(win);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	//Variance Shadow Map Grabbing
	glGenFramebuffersEXT(1, &fb2);
	glGenRenderbuffersEXT( 1, &rb_depth2);

	//Initialize Depth Renderbuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb2);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rb_depth2);
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, shadowVP->getMapSize(), shadowVP->getMapSize() );

	//Attach Renderbuffer to Framebuffer depth Buffer
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rb_depth2);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	
	}

	//if no NPOTTextures supported, resize images
	if(!useNPOTTextures)
	{
		if(width > height) widthHeightPOT = osgnextpower2(width);
		else widthHeightPOT = osgnextpower2(height);

		beginEditCP(_colorMap);
		beginEditCP(_colorMapImage);
			_colorMapImage->set(GL_RGB, widthHeightPOT, widthHeightPOT);
		endEditCP(_colorMapImage);
		endEditCP(_colorMap);

		beginEditCP(_shadowFactorMap);
		beginEditCP(_shadowFactorMapImage);
			_shadowFactorMapImage->set(GL_RGB, widthHeightPOT, widthHeightPOT);
		endEditCP(_shadowFactorMapImage);
		endEditCP(_shadowFactorMap);
	}
	
	return true;
}

void VarianceShadowMap::reInit(Window *win)
{
	Int32 width  = shadowVP->getPixelWidth();
    Int32 height = shadowVP->getPixelHeight();

	win->validateGLObject(_colorMap->getGLId());
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, win->getGLObjectId(_colorMap->getGLId()), 0);
	win->validateGLObject(_shadowFactorMap->getGLId());
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, win->getGLObjectId(_shadowFactorMap->getGLId()), 0);

	//Initialize Depth Renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rb_depth);
	if(useNPOTTextures) glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, shadowVP->getPixelWidth(), shadowVP->getPixelHeight() );
	else glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB, widthHeightPOT, widthHeightPOT );

	//Attach Renderbuffer to Framebuffer depth Buffer
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,  GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rb_depth);
}

void VarianceShadowMap::drawTextureBoxShader(RenderActionBase* action, ChunkMaterialPtr cmat)
{
	glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable( GL_DEPTH_TEST );

    addRefCP(cmat);

    beginEditCP(boxGeo, Geometry::MaterialFieldMask);
        boxGeo->setMaterial(cmat);
    endEditCP(boxGeo, Geometry::MaterialFieldMask);

    action->setCamera(_matrixCam.getCPtr());

    //Render Texturebox
    action->apply(boxNode);

    //Restore old Parameters
    action->setCamera    (shadowVP->getCamera().getCPtr());

    subRefCP(cmat);
}


void VarianceShadowMap::createShadowMapsFBO(RenderActionBase* action)
{
	UInt32 oldWidth, oldHeight;
	oldWidth = shadowVP->getPixelWidth();
	oldHeight = shadowVP->getPixelHeight();

    //------Setting up Window to fit size of ShadowMap----------------

	shadowVP->setVPSize(0,0,shadowVP->getMapSize()-1,shadowVP->getMapSize()-1);

	// disable all lights more speed
    for(UInt32 i = 0; i < shadowVP->_lights.size(); ++i)
    {
        if(shadowVP->_lightStates[i] != 0)
            shadowVP->_lights[i]->setOn(false);
    }

	// activate exclude nodes:
    for(UInt32 i = 0; i < shadowVP->getExcludeNodes().getSize(); ++i)
    {
        NodePtr exnode = shadowVP->getExcludeNodes()[i];
        if(exnode != NullFC)
            if(shadowVP->_excludeNodeActive[i]) exnode->setActive(true);
    }
    
	for(UInt32 i = 0; i< shadowVP->_lights.size(); ++i)
    {
        if(shadowVP->_lightStates[i] != 0)
		{
		if(shadowVP->getGlobalShadowIntensity() != 0.0 || shadowVP->_lights[i]->getShadowIntensity() != 0.0)
        {
			//action->getWindow()->validateGLObject(shadowVP->_texChunks[i]->getGLId());

			GLenum *buffers = NULL;
			buffers = new GLenum[1];
			buffers[0] = GL_COLOR_ATTACHMENT0_EXT;

			action->getWindow()->validateGLObject(shadowVP->_texChunks[i]->getGLId());
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb2);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, action->getWindow()->getGLObjectId(shadowVP->_texChunks[i]->getGLId()), 0);
	
			//shadowVP->_texChunks[i]->activate(action, action->getWindow()->getGLObjectId(shadowVP->_texChunks[i]->getGLId()));
			glBindTexture(GL_TEXTURE_2D,action->getWindow()->getGLObjectId(shadowVP->_texChunks[i]->getGLId()));
				glGenerateMipmapEXT(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,0);
			//shadowVP->_texChunks[i]->deactivate(action, action->getWindow()->getGLObjectId(shadowVP->_texChunks[i]->getGLId()));
			
			glDrawBuffersARB(1, buffers);

			glClearColor(1.0,1.0,1.0,1.0);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			action->setCamera(shadowVP->_lightCameras[i].getCPtr());

			Pnt3f lPos;
			bool isDirLight;
			Real32 sceneDiagLength;

			if(shadowVP->_lights[i]->getType() == PointLight::getClassType())
			{
				PointLightPtr tmpPoint;
				tmpPoint = PointLightPtr::dcast(shadowVP->_lights[i]);

				lPos = tmpPoint->getPosition();

				if(tmpPoint->getBeacon() != NullFC)
				{
					Matrix m = tmpPoint->getBeacon()->getToWorld();
					m.mult(lPos);
				}
				isDirLight = false;

				Pnt3f center;
				shadowVP->getSceneRoot()->getVolume().getCenter(center);
    
				Vec3f dir = lPos - center;
				Real32 dirLength = dir.length();

				Vec3f diff = (shadowVP->getSceneRoot()->getVolume().getMax() - center);
				Real32 diffLength = diff.length();

				sceneDiagLength = dirLength + diffLength;
			}

			else if(shadowVP->_lights[i]->getType() == SpotLight::getClassType())
			{
				SpotLightPtr tmpSpot;
				tmpSpot = SpotLightPtr::dcast(shadowVP->_lights[i]);
				lPos = tmpSpot->getPosition();
				if(tmpSpot->getBeacon() != NullFC)
				{
					Matrix m = tmpSpot->getBeacon()->getToWorld();
					m.mult(lPos);
				}
				isDirLight = false;
				Pnt3f center;
				shadowVP->getSceneRoot()->getVolume().getCenter(center);
    
				Vec3f dir = lPos - center;
				Real32 dirLength = dir.length();

				Vec3f diff = (shadowVP->getSceneRoot()->getVolume().getMax() - center);
				Real32 diffLength = diff.length();

				sceneDiagLength = dirLength + diffLength;
			}

			else
			{
				isDirLight = true;
				sceneDiagLength = 1.0;
			}

			beginEditCP(_depthSHL, ShaderChunk::ParametersFieldMask);
		        _depthSHL->setUniformParameter("sceneDiagLength", Real32(sceneDiagLength));
				_depthSHL->setUniformParameter("isDirLight", bool(isDirLight));
			endEditCP(_depthSHL, ShaderChunk::ParametersFieldMask);

			beginEditCP(_depthCmat);
			    _depthCmat->getChunks().clear();
			   _depthCmat->addChunk(_depthSHL);
			endEditCP(_depthCmat);

			subRefCP(_depthSHL);

			addRefCP(shadowVP->getRoot());

			beginEditCP(_depthRoot, Node::CoreFieldMask | Node::ChildrenFieldMask);
				_depthRoot->setCore(_depthShaderGroup);
				_depthRoot->addChild(shadowVP->getRoot());
			endEditCP(_depthRoot, Node::ChildrenFieldMask | Node::ChildrenFieldMask);
    
			//draw the Scene
    
			action->apply(_depthRoot);

			beginEditCP(_depthRoot, Node::CoreFieldMask | Node::ChildrenFieldMask);
				_depthRoot->subChild(shadowVP->getRoot());
			endEditCP(_depthRoot, Node::ChildrenFieldMask | Node::ChildrenFieldMask);

			subRefCP(shadowVP->getRoot());

            //action->apply(shadowVP->getRoot());
             
			//shadowVP->_poly->deactivate(action,0);
			//glDisable( GL_POLYGON_OFFSET_FILL );

			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

			delete[] buffers;

			glClearColor(0.0,0.0,0.0,1.0);

			action->setCamera(shadowVP->getCamera().getCPtr());
		}
		}
	}
	
    //-------Restoring old states of Window and Viewport----------

	// enable all lights.
    for(UInt32 i = 0; i< shadowVP->_lights.size(); ++i)
    {
        if(shadowVP->_lightStates[i] != 0)
            shadowVP->_lights[i]->setOn(true);
    }

	// activate exclude nodes:
    for(UInt32 i = 0; i < shadowVP->getExcludeNodes().getSize(); ++i)
    {
        NodePtr exnode = shadowVP->getExcludeNodes()[i];
        if(exnode != NullFC)
            if(shadowVP->_excludeNodeActive[i]) exnode->setActive(true);
    }

	shadowVP->setVPSize(0,0,oldWidth-1,oldHeight-1);
	shadowVP->setVPSize(0,0,1,1);
}

void VarianceShadowMap::createColorMap(RenderActionBase* action)
{
	shadowVP->getBackground()->clear(action, shadowVP);

    action->apply(shadowVP->getRoot());

    action->getWindow()->validateGLObject(_colorMap->getGLId());

    glBindTexture(GL_TEXTURE_2D, action->getWindow()->getGLObjectId(_colorMap->getGLId()));
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    glBindTexture(GL_TEXTURE_2D,0);
}


void VarianceShadowMap::createColorMapFBO(RenderActionBase* action)
{
	glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Window *win = action->getWindow();

	GLenum *buffers = NULL;
	buffers = new GLenum[1];
	buffers[0] = GL_COLOR_ATTACHMENT0_EXT;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);
	
	glDrawBuffersARB(1, buffers);

	shadowVP->getBackground()->clear(action, shadowVP);
    action->apply(shadowVP->getRoot());

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	delete[] buffers;
}

void VarianceShadowMap::createShadowFactorMap(RenderActionBase* action, UInt32 num)
{
	glClearColor(0.0,0.0,0.0,1.0);
	if(firstRun) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Finde alle aktiven Lichtquellen
    Real32 activeLights = 0;
	if(shadowVP->getGlobalShadowIntensity() != 0.0) 
	{
		for (UInt32 i = 0; i<shadowVP->_lights.size();i++)
		{
			if (shadowVP->_lightStates[i] != 0) activeLights++;
		}
	}
	else
	{
		for (UInt32 i = 0; i<shadowVP->_lights.size();i++)
		{
			if (shadowVP->_lightStates[i] != 0 && shadowVP->_lights[i]->getShadowIntensity() != 0.0) activeLights++;
		}
	}
    
    Real32 shadowIntensity;
	if(shadowVP->getGlobalShadowIntensity() != 0.0) shadowIntensity = (shadowVP->getGlobalShadowIntensity()/activeLights);
	else  shadowIntensity = (shadowVP->_lights[num]->getShadowIntensity()/activeLights);
	if(shadowVP->_lights[num]->getShadowIntensity() != 0.0 || shadowVP->getGlobalShadowIntensity() != 0.0)
	{

    Matrix LVM,LPM,CVM;
    shadowVP->_lightCameras[num]->getViewing(LVM, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    shadowVP->_lightCameras[num]->getProjection(LPM, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    PerspectiveCameraPtr::dcast(shadowVP->getCamera())->getViewing(CVM, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    Matrix iCVM = CVM;
    iCVM.invert();

	Real32 texFactor;
	if(shadowVP->_lights[num]->getType() == SpotLight::getClassType() || shadowVP->_lights[num]->getType() == PointLight::getClassType())
		texFactor = Real32(width)/Real32(height);
	else texFactor = 1.0;

    Matrix shadowMatrix = LPM;
    shadowMatrix.mult(LVM);
    shadowMatrix.mult(iCVM);

	Matrix shadowMatrix2 = LVM;
    shadowMatrix2.mult(iCVM);


	Real32 xFactor = 1.0;
	Real32 yFactor = 1.0;
	if(!useNPOTTextures)
	{
		xFactor = Real32(width)/Real32(widthHeightPOT);
		yFactor = Real32(height)/Real32(widthHeightPOT);
	}

	Pnt3f lPos;
	bool isDirLight;
	Real32 sceneDiagLength;

			if(shadowVP->_lights[num]->getType() == PointLight::getClassType())
			{
				PointLightPtr tmpPoint;
				tmpPoint = PointLightPtr::dcast(shadowVP->_lights[num]);

				lPos = tmpPoint->getPosition();

				if(tmpPoint->getBeacon() != NullFC)
				{
					Matrix m = tmpPoint->getBeacon()->getToWorld();
					m.mult(lPos);
				}
				isDirLight = false;
				Pnt3f center;
				shadowVP->getSceneRoot()->getVolume().getCenter(center);
    
				Vec3f dir = lPos - center;
				Real32 dirLength = dir.length();
	
				Vec3f diff = (shadowVP->getSceneRoot()->getVolume().getMax() - center);
				Real32 diffLength = diff.length();

				sceneDiagLength = dirLength + diffLength;
			}

			else if(shadowVP->_lights[num]->getType() == SpotLight::getClassType())
			{
				SpotLightPtr tmpSpot;
				tmpSpot = SpotLightPtr::dcast(shadowVP->_lights[num]);
				lPos = tmpSpot->getPosition();
				if(tmpSpot->getBeacon() != NullFC)
				{
					Matrix m = tmpSpot->getBeacon()->getToWorld();
					m.mult(lPos);
				}
				isDirLight = false;
				Pnt3f center;
				shadowVP->getSceneRoot()->getVolume().getCenter(center);
    
				Vec3f dir = lPos - center;
				Real32 dirLength = dir.length();
	
				Vec3f diff = (shadowVP->getSceneRoot()->getVolume().getMax() - center);
				Real32 diffLength = diff.length();

				sceneDiagLength = dirLength + diffLength;
			}

			else
			{
				isDirLight = true;
				sceneDiagLength = 1.0;
			}

                
			Real32 lod;

			if(shadowVP->getShadowSmoothness() <= 0.1999) lod = 0.5;
			else if(shadowVP->getShadowSmoothness() <= 0.3999) lod = 1.5;
			else if(shadowVP->getShadowSmoothness() <= 0.5999) lod = 2.5;
			else if(shadowVP->getShadowSmoothness() <= 0.7999) lod = 3.5;
			else lod = 4.5;

    beginEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
        _shadowSHL->setUniformParameter("shadowMap", 0);
        _shadowSHL->setUniformParameter("oldFactorMap", 1);
        _shadowSHL->setUniformParameter("firstRun", firstRun);
        _shadowSHL->setUniformParameter("intensity", shadowIntensity);
		_shadowSHL->setUniformParameter("texFactor", texFactor);
        //_shadowSHL->setUniformParameter("shadowBias", 0.0075f);
        _shadowSHL->setUniformParameter("lightPM", shadowMatrix);
		_shadowSHL->setUniformParameter("lightPM2", shadowMatrix2);
		//_shadowSHL->setUniformParameter("shadowRange", Real32(shadowRange));
		_shadowSHL->setUniformParameter("xFactor",Real32(xFactor));
		_shadowSHL->setUniformParameter("yFactor",Real32(yFactor));
		_shadowSHL->setUniformParameter("sceneDiagLength", Real32(sceneDiagLength));
		_shadowSHL->setUniformParameter("Lod",Real32(lod));
		_shadowSHL->setUniformParameter("isDirLight",bool(isDirLight));
    endEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);

    beginEditCP(_shadowCmat);
        _shadowCmat->getChunks().clear();
        _shadowCmat->addChunk(_shadowSHL);
        _shadowCmat->addChunk(shadowVP->_texChunks[num]);
        _shadowCmat->addChunk(_shadowFactorMap);
    endEditCP(_shadowCmat);

    /////////////
    subRefCP(_shadowSHL);
    subRefCP(_shadowFactorMap);

    addRefCP(shadowVP->getRoot());

    beginEditCP(_shadowRoot, Node::CoreFieldMask | Node::ChildrenFieldMask);
        _shadowRoot->setCore(_shadowShaderGroup);
        _shadowRoot->addChild(shadowVP->getRoot());
    endEditCP(_shadowRoot, Node::ChildrenFieldMask | Node::ChildrenFieldMask);
    
//draw the Scene

	action->apply(_shadowRoot);

	action->getWindow()->validateGLObject(_shadowFactorMap->getGLId());

    glBindTexture(GL_TEXTURE_2D, action->getWindow()->getGLObjectId(_shadowFactorMap->getGLId()));
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    glBindTexture(GL_TEXTURE_2D,0);

    beginEditCP(_shadowRoot, Node::CoreFieldMask | Node::ChildrenFieldMask);
        _shadowRoot->subChild(shadowVP->getRoot());
    endEditCP(_shadowRoot, Node::ChildrenFieldMask | Node::ChildrenFieldMask);

    subRefCP(shadowVP->getRoot());
	firstRun = 0;
	}
}

void VarianceShadowMap::createShadowFactorMapFBO(RenderActionBase* action, UInt32 num)
{
	glClearColor(0.0,0.0,0.0,1.0);

    //Finde alle aktiven Lichtquellen
    Real32 activeLights = 0;
	if(shadowVP->getGlobalShadowIntensity() != 0.0) 
	{
		for (UInt32 i = 0; i<shadowVP->_lights.size();i++)
		{
			if (shadowVP->_lightStates[i] != 0) activeLights++;
		}
	}
	else
	{
		for (UInt32 i = 0; i<shadowVP->_lights.size();i++)
		{
			if (shadowVP->_lightStates[i] != 0 && shadowVP->_lights[i]->getShadowIntensity() != 0.0) activeLights++;
		}
	}
    
    Real32 shadowIntensity;
	if(shadowVP->getGlobalShadowIntensity() != 0.0) shadowIntensity = (shadowVP->getGlobalShadowIntensity()/activeLights);
	else  shadowIntensity = (shadowVP->_lights[num]->getShadowIntensity()/activeLights);
	if(shadowVP->_lights[num]->getShadowIntensity() != 0.0 || shadowVP->getGlobalShadowIntensity() != 0.0)
	{

    Matrix LVM,LPM,CVM;
    shadowVP->_lightCameras[num]->getViewing(LVM, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    shadowVP->_lightCameras[num]->getProjection(LPM, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    PerspectiveCameraPtr::dcast(shadowVP->getCamera())->getViewing(CVM, shadowVP->getPixelWidth(), shadowVP->getPixelHeight());
    Matrix iCVM = CVM;
    iCVM.invert();

	Real32 texFactor;
	if(shadowVP->_lights[num]->getType() == SpotLight::getClassType() || shadowVP->_lights[num]->getType() == PointLight::getClassType())
		texFactor = Real32(width)/Real32(height);
	else texFactor = 1.0;

    Matrix shadowMatrix = LPM;
    shadowMatrix.mult(LVM);
    shadowMatrix.mult(iCVM);

	Matrix shadowMatrix2 = LVM;
    shadowMatrix2.mult(iCVM);


	Real32 xFactor = 1.0;
	Real32 yFactor = 1.0;
	if(!useNPOTTextures)
	{
		xFactor = Real32(width)/Real32(widthHeightPOT);
		yFactor = Real32(height)/Real32(widthHeightPOT);
	}

	Pnt3f lPos;
	bool isDirLight;
	Real32 sceneDiagLength;

			if(shadowVP->_lights[num]->getType() == PointLight::getClassType())
			{
				PointLightPtr tmpPoint;
				tmpPoint = PointLightPtr::dcast(shadowVP->_lights[num]);

				lPos = tmpPoint->getPosition();

				if(tmpPoint->getBeacon() != NullFC)
				{
					Matrix m = tmpPoint->getBeacon()->getToWorld();
					m.mult(lPos);
				}
				isDirLight = false;
				Pnt3f center;
				shadowVP->getSceneRoot()->getVolume().getCenter(center);
    
				Vec3f dir = lPos - center;
				Real32 dirLength = dir.length();
	
				Vec3f diff = (shadowVP->getSceneRoot()->getVolume().getMax() - center);
				Real32 diffLength = diff.length();

				sceneDiagLength = dirLength + diffLength;
			}

			else if(shadowVP->_lights[num]->getType() == SpotLight::getClassType())
			{
				SpotLightPtr tmpSpot;
				tmpSpot = SpotLightPtr::dcast(shadowVP->_lights[num]);
				lPos = tmpSpot->getPosition();
				if(tmpSpot->getBeacon() != NullFC)
				{
					Matrix m = tmpSpot->getBeacon()->getToWorld();
					m.mult(lPos);
				}
				isDirLight = false;
				Pnt3f center;
				shadowVP->getSceneRoot()->getVolume().getCenter(center);
    
				Vec3f dir = lPos - center;
				Real32 dirLength = dir.length();
	
				Vec3f diff = (shadowVP->getSceneRoot()->getVolume().getMax() - center);
				Real32 diffLength = diff.length();

				sceneDiagLength = dirLength + diffLength;
			}

			else
			{
				isDirLight = true;
				sceneDiagLength = 1.0;
			}

                
			Real32 lod;

			if(shadowVP->getShadowSmoothness() <= 0.1999) lod = 0.5;
			else if(shadowVP->getShadowSmoothness() <= 0.3999) lod = 1.5;
			else if(shadowVP->getShadowSmoothness() <= 0.5999) lod = 2.5;
			else if(shadowVP->getShadowSmoothness() <= 0.7999) lod = 3.5;
			else lod = 4.5;

    beginEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
        _shadowSHL->setUniformParameter("shadowMap", 0);
        _shadowSHL->setUniformParameter("oldFactorMap", 1);
        _shadowSHL->setUniformParameter("firstRun", firstRun);
        _shadowSHL->setUniformParameter("intensity", shadowIntensity);
		_shadowSHL->setUniformParameter("texFactor", texFactor);
        //_shadowSHL->setUniformParameter("shadowBias", 0.0075f);
        _shadowSHL->setUniformParameter("lightPM", shadowMatrix);
		_shadowSHL->setUniformParameter("lightPM2", shadowMatrix2);
		//_shadowSHL->setUniformParameter("shadowRange", Real32(shadowRange));
		_shadowSHL->setUniformParameter("xFactor",Real32(xFactor));
		_shadowSHL->setUniformParameter("yFactor",Real32(yFactor));
		_shadowSHL->setUniformParameter("sceneDiagLength", Real32(sceneDiagLength));
		_shadowSHL->setUniformParameter("Lod",Real32(lod));
		_shadowSHL->setUniformParameter("isDirLight",bool(isDirLight));
    endEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);

    beginEditCP(_shadowCmat);
        _shadowCmat->getChunks().clear();
        _shadowCmat->addChunk(_shadowSHL);
        _shadowCmat->addChunk(shadowVP->_texChunks[num]);
        _shadowCmat->addChunk(_shadowFactorMap);
    endEditCP(_shadowCmat);

    subRefCP(_shadowSHL);
    subRefCP(_shadowFactorMap);

    addRefCP(shadowVP->getRoot());

    beginEditCP(_shadowRoot, Node::CoreFieldMask | Node::ChildrenFieldMask);
        _shadowRoot->setCore(_shadowShaderGroup);
        _shadowRoot->addChild(shadowVP->getRoot());
    endEditCP(_shadowRoot, Node::ChildrenFieldMask | Node::ChildrenFieldMask);
    
    	GLenum *buffers = NULL;
	buffers = new GLenum[1];
	buffers[0] = GL_COLOR_ATTACHMENT1_EXT;
    
	//Setup FBO
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fb);

	glDrawBuffersARB(1, buffers);

    //draw the Scene
	if(firstRun) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    action->apply(_shadowRoot);

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0);

    beginEditCP(_shadowRoot, Node::CoreFieldMask | Node::ChildrenFieldMask);
        _shadowRoot->subChild(shadowVP->getRoot());
    endEditCP(_shadowRoot, Node::ChildrenFieldMask | Node::ChildrenFieldMask);

    subRefCP(shadowVP->getRoot());

	delete[] buffers;
	firstRun = 0;
	}
}

void VarianceShadowMap::drawCombineMap(RenderActionBase* action)
{
	Real32 xFactor = 1.0;
	Real32 yFactor = 1.0;
	if(!useNPOTTextures)
	{
		xFactor = Real32(width)/Real32(widthHeightPOT);
		yFactor = Real32(height)/Real32(widthHeightPOT);
	}

	beginEditCP(_combineSHL, ShaderChunk::ParametersFieldMask);
        _combineSHL->setUniformParameter("colorMap", 0);
        _combineSHL->setUniformParameter("shadowFactorMap", 1);
		_combineSHL->setUniformParameter("xFactor",Real32(xFactor));
		_combineSHL->setUniformParameter("yFactor",Real32(yFactor));
    endEditCP(_combineSHL, ShaderChunk::ParametersFieldMask);

    beginEditCP(_combineCmat);
        _combineCmat->getChunks().clear();
        _combineCmat->addChunk(_combineSHL);
        _combineCmat->addChunk(_colorMap);
        _combineCmat->addChunk(_shadowFactorMap);
    endEditCP(_combineCmat);

    //draw the Scene
    drawTextureBoxShader(action, _combineCmat);

    subRefCP(_colorMap);
    subRefCP(_shadowFactorMap);
    subRefCP(_combineSHL);
}

void VarianceShadowMap::render(RenderActionBase* action)
{
	if(!useGLSL || !useShadowExt || !useFBO) shadowVP->Viewport::render(action);
	else
	{

	Window *win = action->getWindow();

	if(useFBO)
	{
		if(!initFBO(win)) printf("ERROR with FBOBJECT\n");
	}

	GLfloat globalAmbient[] = {0.0,0.0,0.0,1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globalAmbient);
	//Used for ShadowFactorMap
    firstRun = 1;

	if(shadowVP->getPixelWidth() != width ||
       shadowVP->getPixelHeight() != height)
    {
        width = shadowVP->getPixelWidth();
        height = shadowVP->getPixelHeight();
    
		if(useNPOTTextures)
		{
			beginEditCP(_colorMap);
			beginEditCP(_colorMapImage);
				_colorMapImage->set(GL_RGB, width, height);
			endEditCP(_colorMapImage);
			endEditCP(_colorMap);

			beginEditCP(_shadowFactorMap);
			beginEditCP(_shadowFactorMapImage);
				_shadowFactorMapImage->set(GL_RGB, width, height);
			endEditCP(_shadowFactorMapImage);
			endEditCP(_shadowFactorMap);

			reInit(win);
		}
		else
		{
			if(width > height) widthHeightPOT = osgnextpower2(width);
			else widthHeightPOT = osgnextpower2(height);

			beginEditCP(_colorMap);
			beginEditCP(_colorMapImage);
				_colorMapImage->set(GL_RGB, widthHeightPOT, widthHeightPOT);
			endEditCP(_colorMapImage);
			endEditCP(_colorMap);

			beginEditCP(_shadowFactorMap);
			beginEditCP(_shadowFactorMapImage);
				_shadowFactorMapImage->set(GL_RGB, widthHeightPOT, widthHeightPOT);
			endEditCP(_shadowFactorMapImage);
			endEditCP(_shadowFactorMap);

		}
    }

    if(shadowVP->getMapAutoUpdate())
    {
		if(useNPOTTextures) createColorMapFBO(action);
		else createColorMap(action);

		//deactivate transparent Nodes
		for(UInt32 t=0;t<shadowVP->_transparent.size();++t)
			shadowVP->_transparent[t]->setActive(false);
        
		createShadowMapsFBO(action);

		//filterShadowMaps(action);
       
        for(UInt32 i = 0; i<shadowVP->_lights.size();i++)
        {
            if(shadowVP->_lightStates[i] != 0)
			{
			if(shadowVP->getGlobalShadowIntensity() != 0.0 || shadowVP->_lights[i]->getShadowIntensity() != 0.0)
			{
				if(useNPOTTextures) createShadowFactorMapFBO(action, i);
				else createShadowFactorMap(action, i);
			}
			}
        }
    }
    else
    {
        if(shadowVP->_trigger_update)
        {
			if(useNPOTTextures) createColorMapFBO(action);
			else createColorMap(action);

			//deactivate transparent Nodes
			for(UInt32 t=0;t<shadowVP->_transparent.size();++t)
				shadowVP->_transparent[t]->setActive(false);

			createShadowMapsFBO(action);

			//filterShadowMaps(action);
			
            for(UInt32 i = 0; i<shadowVP->_lights.size();i++)
            {
                if(shadowVP->_lightStates[i] != 0)
				{
				if(shadowVP->getGlobalShadowIntensity() != 0.0 || shadowVP->_lights[i]->getShadowIntensity() != 0.0)
				{
					if(useNPOTTextures) createShadowFactorMapFBO(action, i);
					else createShadowFactorMap(action, i);
                }
				}
            }
            shadowVP->_trigger_update = false;
        }
    }

	drawCombineMap(action);

	// switch on all transparent geos
    for(UInt32 t=0;t<shadowVP->_transparent.size();++t)
        shadowVP->_transparent[t]->setActive(true);

    // render the foregrounds.
    for(UInt16 i=0; i < shadowVP->getForegrounds().size(); ++i)
	{
        shadowVP->getForegrounds(i)->draw(action, shadowVP);
	}

	}
}
