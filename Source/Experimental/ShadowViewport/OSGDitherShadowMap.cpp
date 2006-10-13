#include <stdlib.h>
#include <stdio.h>
#include <OSGConfig.h>
#include <OSGGLEXT.h>
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
#include <OSGMatrixCameraDecorator.h>
#include <OSGLight.h>
#include <OSGMaterialGroup.h>
#include "OSGDitherShadowMap.h"
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
#    define    GL_RGBA32F_ARB                      0x8814
#    define    GL_RGB32F_ARB                       0x8815
#    define    GL_ALPHA32F_ARB                     0x8816
#    define    GL_INTENSITY32F_ARB                 0x8817
#    define    GL_LUMINANCE32F_ARB                 0x8818
#    define    GL_LUMINANCE_ALPHA32F_ARB           0x8819
#    define    GL_RGBA16F_ARB                      0x881A
#    define    GL_RGB16F_ARB                       0x881B
#    define    GL_ALPHA16F_ARB                     0x881C
#    define    GL_INTENSITY16F_ARB                 0x881D
#    define    GL_LUMINANCE16F_ARB                 0x881E
#    define    GL_LUMINANCE_ALPHA16F_ARB           0x881F
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

// Some installations have a typo in the name for this one
#ifndef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT 0x8CD6
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


OSG_USING_NAMESPACE


static std::string _dither_shadow_vp =
    "uniform mat4 lightPM;\n"
    "uniform float texFactor;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 texPos;\n"
    "\n"
    "const mat4 bias = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);\n""\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 realPos = gl_ModelViewMatrix * gl_Vertex;\n"
    "  projCoord = lightPM * realPos;\n"
    "  projCoord.x *= texFactor;\n"
    "  texPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "  projCoord = bias * projCoord;\n"
    "  texPos = bias * texPos;\n"
    "  gl_Position = ftransform();\n"
    "}\n";

static std::string _dither_shadow_fp =
    "uniform sampler2DShadow shadowMap;\n"
    "uniform sampler2D oldFactorMap;\n"
    "uniform float intensity;\n"
    "uniform int firstRun;\n"
    "uniform float xFactor;\n"
    "uniform float yFactor;\n"
    "uniform float mapSize;\n"
    "uniform float mapFactor;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 texPos;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	float pixSize = (1.0/mapSize)*mapFactor;\n"
    "	vec2 o = mod(floor(gl_FragCoord.xy),2.0)*pixSize;\n"
    "	vec4 projectiveBiased = vec4((projCoord.xyz / projCoord.q),1.0);\n"
    "   projectiveBiased = vec4(projectiveBiased.xy * vec2(mapFactor,mapFactor) + o, projectiveBiased.zw);\n"
    "	float shadowed;\n"
    "	shadowed = shadow2D(shadowMap, vec3(projectiveBiased.xy + vec2(-1.5*pixSize,1.5*pixSize),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap, vec3(projectiveBiased.xy + vec2(0.5*pixSize,1.5*pixSize),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap, vec3(projectiveBiased.xy + vec2(-1.5*pixSize,-0.5*pixSize),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap, vec3(projectiveBiased.xy + vec2(0.5*pixSize,-0.5*pixSize),projectiveBiased.z)).x;\n"
    "\n"
    "	shadowed = (1.0-(shadowed * 0.25))*intensity;\n"
    "	if(firstRun == 0) shadowed += texture2DProj(oldFactorMap,vec3(texPos.xy * vec2(xFactor,yFactor),texPos.w)).x;\n"
    "	gl_FragColor = vec4(shadowed,0.0,0.0,1.0);\n"
    "}\n";

static std::string _dither_shadow2_vp =
    "uniform mat4 lightPM1;\n"
    "uniform mat4 lightPM2;\n"
    "uniform float texFactor1;\n"
    "uniform float texFactor2;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 projCoord2;\n"
    "varying vec4 texPos;\n"
    "\n"
    "const mat4 bias = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);\n""\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 realPos = gl_ModelViewMatrix * gl_Vertex;\n"
    "  projCoord = lightPM1 * realPos;\n"
    "  projCoord2 = lightPM2 * realPos;\n"
    "  projCoord.x *= texFactor1;\n"
    "  projCoord2.x *= texFactor2;\n"
    "  texPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "  projCoord = bias * projCoord;\n"
    "  projCoord2 = bias * projCoord2;\n"
    "  texPos = bias * texPos;\n"
    "  gl_Position = ftransform();\n"
    "}\n";

static std::string _dither_shadow2_fp =
    "uniform sampler2DShadow shadowMap1;\n"
    "uniform sampler2DShadow shadowMap2;\n"
    "uniform sampler2D oldFactorMap;\n"
    "uniform float intensity1;\n"
    "uniform float intensity2;\n"
    "uniform int firstRun;\n"
    "uniform float xFactor;\n"
    "uniform float yFactor;\n"
    "uniform float mapSize;\n"
    "uniform float mapFactor1;\n"
    "uniform float mapFactor2;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 projCoord2;\n"
    "varying vec4 texPos;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	float pixSize1 = (1.0/mapSize)*mapFactor1;\n"
    "	float pixSize2 = (1.0/mapSize)*mapFactor2;\n"
    "	vec2 frag_xy = gl_FragCoord.xy;\n"
    "	vec2 o1 = mod(floor(frag_xy),2.0)*pixSize1;\n"
    "	vec2 o2 = mod(floor(frag_xy),2.0)*pixSize2;\n"
    "	vec4 projectiveBiased = vec4((projCoord.xyz / projCoord.q),1.0);\n"
    "	vec4 projectiveBiased2 = vec4((projCoord2.xyz / projCoord2.q),1.0);\n"
    "   projectiveBiased = vec4(projectiveBiased.xy * vec2(mapFactor1,mapFactor1) + o1, projectiveBiased.zw);\n"
    "   projectiveBiased2 = vec4(projectiveBiased2.xy * vec2(mapFactor2,mapFactor2) + o2, projectiveBiased2.zw);\n"
    "	float shadowed;\n"
    "	float shadowed2;\n"
    "\n"
    "	shadowed = shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(-1.5*pixSize1,1.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(0.5*pixSize1,1.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(-1.5*pixSize1,-0.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(0.5*pixSize1,-0.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed = (1.0 - (shadowed * 0.25))*intensity1;\n"
    "\n"
    "	shadowed2 = shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(-1.5*pixSize2,1.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(0.5*pixSize2,1.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(-1.5*pixSize2,-0.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(0.5*pixSize2,-0.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 = (1.0 - (shadowed2 * 0.25))*intensity2;\n"
    "\n"
    "	shadowed += shadowed2;\n"
    "\n"
    "	if(firstRun == 0) shadowed += texture2DProj(oldFactorMap,vec3(texPos.xy * vec2(xFactor,yFactor),texPos.w)).x;\n"
    "	gl_FragColor = vec4(shadowed,0.0,0.0,1.0);\n"
    "}\n";

static std::string _dither_shadow3_vp =
    "uniform mat4 lightPM1;\n"
    "uniform mat4 lightPM2;\n"
    "uniform mat4 lightPM3;\n"
    "uniform float texFactor1;\n"
    "uniform float texFactor2;\n"
    "uniform float texFactor3;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 projCoord2;\n"
    "varying vec4 projCoord3;\n"
    "varying vec4 texPos;\n"
    "\n"
    "const mat4 bias = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);\n""\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 realPos = gl_ModelViewMatrix * gl_Vertex;\n"
    "  projCoord = lightPM1 * realPos;\n"
    "  projCoord2 = lightPM2 * realPos;\n"
    "  projCoord3 = lightPM3 * realPos;\n"
    "  projCoord.x *= texFactor1;\n"
    "  projCoord2.x *= texFactor2;\n"
    "  projCoord3.x *= texFactor3;\n"
    "  texPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "  projCoord = bias * projCoord;\n"
    "  projCoord2 = bias * projCoord2;\n"
    "  projCoord3 = bias * projCoord3;\n"
    "  texPos = bias * texPos;\n"
    "  gl_Position = ftransform();\n"
    "}\n";

static std::string _dither_shadow3_fp =
    "uniform sampler2DShadow shadowMap1;\n"
    "uniform sampler2DShadow shadowMap2;\n"
    "uniform sampler2DShadow shadowMap3;\n"
    "uniform sampler2D oldFactorMap;\n"
    "uniform float intensity1;\n"
    "uniform float intensity2;\n"
    "uniform float intensity3;\n"
    "uniform int firstRun;\n"
    "uniform float xFactor;\n"
    "uniform float yFactor;\n"
    "uniform float mapSize;\n"
    "uniform float mapFactor1;\n"
    "uniform float mapFactor2;\n"
    "uniform float mapFactor3;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 projCoord2;\n"
    "varying vec4 projCoord3;\n"
    "varying vec4 texPos;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	float pixSize1 = (1.0/mapSize)*mapFactor1;\n"
    "	float pixSize2 = (1.0/mapSize)*mapFactor2;\n"
    "	float pixSize3 = (1.0/mapSize)*mapFactor3;\n"
    "	vec2 frag_xy = gl_FragCoord.xy;\n"
    "	vec2 o1 = mod(floor(frag_xy),2.0)*pixSize1;\n"
    "	vec2 o2 = mod(floor(frag_xy),2.0)*pixSize2;\n"
    "	vec2 o3 = mod(floor(frag_xy),2.0)*pixSize3;\n"
    "	vec4 projectiveBiased = vec4((projCoord.xyz / projCoord.q),1.0);\n"
    "	vec4 projectiveBiased2 = vec4((projCoord2.xyz / projCoord2.q),1.0);\n"
    "	vec4 projectiveBiased3 = vec4((projCoord3.xyz / projCoord3.q),1.0);\n"
    "   projectiveBiased = vec4(projectiveBiased.xy * vec2(mapFactor1,mapFactor1) + o1, projectiveBiased.zw);\n"
    "   projectiveBiased2 = vec4(projectiveBiased2.xy * vec2(mapFactor2,mapFactor2) + o2, projectiveBiased2.zw);\n"
    "   projectiveBiased3 = vec4(projectiveBiased3.xy * vec2(mapFactor3,mapFactor3) + o3, projectiveBiased3.zw);\n"
    "	float shadowed;\n"
    "	float shadowed2;\n"
    "	float shadowed3;\n"
    "\n"
    "	shadowed = shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(-1.5*pixSize1,1.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(0.5*pixSize1,1.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(-1.5*pixSize1,-0.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(0.5*pixSize1,-0.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed = (1.0-(shadowed * 0.25))*intensity1;\n"
    "\n"
    "	shadowed2 = shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(-1.5*pixSize2,1.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(0.5*pixSize2,1.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(-1.5*pixSize2,-0.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(0.5*pixSize2,-0.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 = (1.0-(shadowed2 * 0.25))*intensity2;\n"
    "\n"
    "	shadowed3 = shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(-1.5*pixSize3,1.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 += shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(0.5*pixSize3,1.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 += shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(-1.5*pixSize3,-0.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 += shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(0.5*pixSize3,-0.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 = (1.0-(shadowed3 * 0.25))*intensity3;\n"
    "\n"
    "	shadowed += shadowed2 + shadowed3;\n"
    "\n"
    "	if(firstRun == 0) shadowed += texture2DProj(oldFactorMap,vec3(texPos.xy * vec2(xFactor,yFactor),texPos.w)).x;\n"
    "	gl_FragColor = vec4(shadowed,0.0,0.0,1.0);\n"
    "}\n";

static std::string _dither_shadow4_vp =
    "uniform mat4 lightPM1;\n"
    "uniform mat4 lightPM2;\n"
    "uniform mat4 lightPM3;\n"
    "uniform mat4 lightPM4;\n"
    "uniform float texFactor1;\n"
    "uniform float texFactor2;\n"
    "uniform float texFactor3;\n"
    "uniform float texFactor4;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 projCoord2;\n"
    "varying vec4 projCoord3;\n"
    "varying vec4 projCoord4;\n"
    "varying vec4 texPos;\n"
    "\n"
    "const mat4 bias = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);\n""\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "  vec4 realPos = gl_ModelViewMatrix * gl_Vertex;\n"
    "  projCoord = lightPM1 * realPos;\n"
    "  projCoord2 = lightPM2 * realPos;\n"
    "  projCoord3 = lightPM3 * realPos;\n"
    "  projCoord4 = lightPM4 * realPos;\n"
    "  projCoord.x *= texFactor1;\n"
    "  projCoord2.x *= texFactor2;\n"
    "  projCoord3.x *= texFactor3;\n"
    "  projCoord4.x *= texFactor4;\n"
    "  texPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "  projCoord = bias * projCoord;\n"
    "  projCoord2 = bias * projCoord2;\n"
    "  projCoord3 = bias * projCoord3;\n"
    "  projCoord4 = bias * projCoord4;\n"
    "  texPos = bias * texPos;\n"
    "  gl_Position = ftransform();\n"
    "}\n";

static std::string _dither_shadow4_fp =
    "uniform sampler2DShadow shadowMap1;\n"
    "uniform sampler2DShadow shadowMap2;\n"
    "uniform sampler2DShadow shadowMap3;\n"
    "uniform sampler2DShadow shadowMap4;\n"
    "uniform sampler2D oldFactorMap;\n"
    "uniform float intensity1;\n"
    "uniform float intensity2;\n"
    "uniform float intensity3;\n"
    "uniform float intensity4;\n"
    "uniform int firstRun;\n"
    "uniform float xFactor;\n"
    "uniform float yFactor;\n"
    "uniform float mapSize;\n"
    "uniform float mapFactor1;\n"
    "uniform float mapFactor2;\n"
    "uniform float mapFactor3;\n"
    "uniform float mapFactor4;\n"
    "varying vec4 projCoord;\n"
    "varying vec4 projCoord2;\n"
    "varying vec4 projCoord3;\n"
    "varying vec4 projCoord4;\n"
    "varying vec4 texPos;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	float pixSize1 = (1.0/mapSize)*mapFactor1;\n"
    "	float pixSize2 = (1.0/mapSize)*mapFactor2;\n"
    "	float pixSize3 = (1.0/mapSize)*mapFactor3;\n"
    "	float pixSize4 = (1.0/mapSize)*mapFactor4;\n"
    "	vec2 frag_xy = gl_FragCoord.xy;\n"
    "	vec2 o1 = mod(floor(frag_xy),2.0)*pixSize1;\n"
    "	vec2 o2 = mod(floor(frag_xy),2.0)*pixSize2;\n"
    "	vec2 o3 = mod(floor(frag_xy),2.0)*pixSize3;\n"
    "	vec2 o4 = mod(floor(frag_xy),2.0)*pixSize4;\n"
    "	vec4 projectiveBiased = vec4((projCoord.xyz / projCoord.q),1.0);\n"
    "	vec4 projectiveBiased2 = vec4((projCoord2.xyz / projCoord2.q),1.0);\n"
    "	vec4 projectiveBiased3 = vec4((projCoord3.xyz / projCoord3.q),1.0);\n"
    "	vec4 projectiveBiased4 = vec4((projCoord4.xyz / projCoord4.q),1.0);\n"
    "   projectiveBiased = vec4(projectiveBiased.xy * vec2(mapFactor1,mapFactor1) + o1, projectiveBiased.zw);\n"
    "   projectiveBiased2 = vec4(projectiveBiased2.xy * vec2(mapFactor2,mapFactor2) + o2, projectiveBiased2.zw);\n"
    "   projectiveBiased3 = vec4(projectiveBiased3.xy * vec2(mapFactor3,mapFactor3) + o3, projectiveBiased3.zw);\n"
    "   projectiveBiased4 = vec4(projectiveBiased4.xy * vec2(mapFactor4,mapFactor4) + o4, projectiveBiased4.zw);\n"
    "	float shadowed;\n"
    "	float shadowed2;\n"
    "	float shadowed3;\n"
    "	float shadowed4;\n"
    "\n"
    "	shadowed = shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(-1.5*pixSize1,1.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(0.5*pixSize1,1.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(-1.5*pixSize1,-0.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap1, vec3(projectiveBiased.xy + vec2(0.5*pixSize1,-0.5*pixSize1),projectiveBiased.z)).x;\n"
    "	shadowed = (1.0-(shadowed * 0.25))*intensity1;\n"
    "\n"
    "	shadowed2 = shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(-1.5*pixSize2,1.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(0.5*pixSize2,1.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(-1.5*pixSize2,-0.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 += shadow2D(shadowMap2, vec3(projectiveBiased2.xy + vec2(0.5*pixSize2,-0.5*pixSize2),projectiveBiased2.z)).x;\n"
    "	shadowed2 = (1.0-(shadowed2 * 0.25))*intensity2;\n"
    "\n"
    "	shadowed3 = shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(-1.5*pixSize3,1.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 += shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(0.5*pixSize3,1.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 += shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(-1.5*pixSize3,-0.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 += shadow2D(shadowMap3, vec3(projectiveBiased3.xy + vec2(0.5*pixSize3,-0.5*pixSize3),projectiveBiased3.z)).x;\n"
    "	shadowed3 = (1.0-(shadowed3 * 0.25))*intensity3;\n"
    "\n"
    "	shadowed4 = shadow2D(shadowMap4, vec3(projectiveBiased4.xy + vec2(-1.5*pixSize4,1.5*pixSize4),projectiveBiased4.z)).x;\n"
    "	shadowed4 += shadow2D(shadowMap4, vec3(projectiveBiased4.xy + vec2(0.5*pixSize4,1.5*pixSize4),projectiveBiased4.z)).x;\n"
    "	shadowed4 += shadow2D(shadowMap4, vec3(projectiveBiased4.xy + vec2(-1.5*pixSize4,-0.5*pixSize4),projectiveBiased4.z)).x;\n"
    "	shadowed4 += shadow2D(shadowMap4, vec3(projectiveBiased4.xy + vec2(0.5*pixSize4,-0.5*pixSize4),projectiveBiased4.z)).x;\n"
    "	shadowed4 = (1.0-(shadowed4 * 0.25))*intensity4;\n"
    "\n"
    "	shadowed += shadowed2 + shadowed3 + shadowed4;\n"
    "\n"
    "	if(firstRun == 0) shadowed += texture2DProj(oldFactorMap,vec3(texPos.xy * vec2(xFactor,yFactor),texPos.w)).x;\n"
    "	gl_FragColor = vec4(shadowed,0.0,0.0,1.0);\n"
    "}\n";

static std::string _dither_cubeshadow_vp =
    "uniform float shadowBias;\n"
    "uniform mat4 lightPMOP;\n"
    "uniform mat4 KKtoWK;\n"
    "uniform float texFactor;\n"
    "varying vec4 texPos;\n"
    "varying vec4 realPos;\n"
    "varying vec4 realPos2;\n"
    "\n"
    "const mat4 bias = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);\n""\n"
    "void main(void)\n"
    "{\n"
    "  realPos = gl_ModelViewMatrix * gl_Vertex;\n"
    "  realPos2 = lightPMOP * (KKtoWK * realPos);\n"
    "  texPos = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "  texPos = bias * texPos;\n"
    "  gl_Position = ftransform();\n"
    "}\n";

static std::string _dither_cubeshadow_fp =
    "uniform sampler2DShadow shadowMap;\n"
    "uniform sampler2D oldFactorMap;\n"
    "uniform float intensity;\n"
    "uniform int firstRun;\n"
    "uniform float mapSize;\n"
    "uniform float xFactor;\n"
    "uniform float yFactor;\n"
    "\n"
    "uniform mat4 lightPMA;\n"
    "uniform mat4 lightPMB;\n"
    "uniform mat4 lightPMC;\n"
    "uniform mat4 lightPMD;\n"
    "uniform mat4 lightPME;\n"
    "uniform mat4 lightPMF;\n"
    "\n"
    "uniform float texFactor;\n"
    "uniform float maxTexSize;\n"
    "\n"
    "varying vec4 texPos;\n"
    "varying vec4 realPos;\n"
    "varying vec4 realPos2;\n"
    "\n"
    "const mat4 bias = mat4(0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.0,0.0,0.0,0.5,0.0,0.5,0.5,0.5,1.0);\n""\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "	float xOffset = 0.0;\n"
    "	float yOffset = 0.0;\n"
    "	float maxStep = 0.25;\n"
    "\n"
    "	vec4 projCoord2;\n"
    "\n"
    "	if(abs(realPos2.x) > abs(realPos2.y) && abs(realPos2.x) > abs(realPos2.z))\n"
    "	{\n"
    "		if(realPos2.x >= 0.0) {projCoord2 = lightPME * realPos; xOffset = 0.0, yOffset = maxStep;}\n"
    "		else {projCoord2 = lightPMF * realPos; xOffset = maxStep, yOffset = maxStep;}\n"
    "	}\n"
    "	else if(abs(realPos2.y) > abs(realPos2.x) && abs(realPos2.y) > abs(realPos2.z))\n"
    "	{\n"
    "		if(realPos2.y >= 0.0) {projCoord2 = lightPMC * realPos; xOffset = 2.0 * maxStep, yOffset = 0.0;}\n"
    "		else {projCoord2 = lightPMD * realPos; xOffset = 3.0 * maxStep, yOffset = 0.0;}\n"
    "	}\n"
    "	else\n"
    "	{\n"
    "		if(realPos2.z >= 0.0) {projCoord2 = lightPMA * realPos;  xOffset = 0.0, yOffset = 0.0;}\n"
    "		else {projCoord2 = lightPMB * realPos; xOffset = maxStep, yOffset = 0.0;}\n"
    "	}\n"
    "\n"
    "	projCoord2.x *= texFactor;\n"
    "	projCoord2 = bias * projCoord2;\n"
    "\n"
    "	vec4 projectiveBiased = vec4((projCoord2.xyz / projCoord2.q),1.0);\n"
    "\n"
    "	float pixSize = 1.0/(mapSize*4.0);\n"
    "	vec2 o = mod(floor(gl_FragCoord.xy),2.0)*pixSize;\n"
    "	projectiveBiased.x = projectiveBiased.x/4.0 + xOffset + o.x;\n"
    "	projectiveBiased.y = projectiveBiased.y/4.0 + yOffset + o.y;\n"
    "	\n"
    "\n"
    "	float shadowed;\n"
    "	shadowed = shadow2D(shadowMap,vec3(vec2(vec2(projectiveBiased.xy + vec2(-1.5*pixSize,1.5*pixSize))),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap,vec3(vec2(vec2(projectiveBiased.xy + vec2(0.5*pixSize,1.5*pixSize))),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap,vec3(vec2(vec2(projectiveBiased.xy + vec2(-1.5*pixSize,-0.5*pixSize))),projectiveBiased.z)).x;\n"
    "	shadowed += shadow2D(shadowMap,vec3(vec2(vec2(projectiveBiased.xy + vec2(0.5*pixSize,-0.5*pixSize))),projectiveBiased.z)).x;\n"
    "	shadowed = (1.0-(shadowed * 0.25))*intensity;\n"
    "\n"
    "	if(firstRun == 0) shadowed += texture2DProj(oldFactorMap,vec3(texPos.xy * vec2(xFactor,yFactor),texPos.w)).x;\n"
    "	gl_FragColor = vec4(shadowed,0.0,0.0,1.0);\n"
    "}\n";

static std::string _dither_shadow_combine_vp =
    "varying vec2 texCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    texCoord = gl_MultiTexCoord0.xy;\n"
    "    gl_Position = ftransform();\n"
    "}\n";

static std::string _dither_shadow_combine_fp =
    "uniform sampler2D colorMap;\n"
    "uniform sampler2D shadowFactorMap;\n"
    "uniform float xFactor;\n"
    "uniform float yFactor;\n"
    "varying vec2 texCoord;\n"
    "\n"
    "void main(void)\n"
    "{\n"
    "    vec2 tc = texCoord * vec2(xFactor, yFactor);\n"
    "    vec3 color = texture2D(colorMap, tc).rgb;\n"
    "    color *= 1.0 - texture2D(shadowFactorMap, tc).r;\n"
    "    gl_FragColor = vec4(color, 1.0);\n"
    "}\n";

DitherShadowMap::DitherShadowMap(ShadowViewport *source) :
    TreeRenderer(source),
    _fb(0),
    _fb2(0),
    _rb_depth(0),
    _transforms(),
    _tiledeco(NullFC),
    _blender(NullFC),
    _colorMap(NullFC),
    _shadowFactorMap(NullFC),
    _colorMapImage(NullFC),
    _shadowFactorMapImage(NullFC),
    _shadowCmat(NullFC),
    _combineCmat(NullFC),
    _shadowSHL(NullFC),
    _shadowSHL2(NullFC),
    _shadowSHL3(NullFC),
    _shadowSHL4(NullFC),
    _shadowCubeSHL(NullFC),
    _combineSHL(NullFC),
    _unlitMat(NullFC),
    _pf(NullFC),
    _firstRun(1),
    _width(1),
    _height(1),
    _widthHeightPOT(0),
    _activeFactorMap(1),
    _shadowFactorMap2(NullFC),
    _shadowFactorMapImage2(NullFC),
    _initTexturesDone(false)
{
    if(_shadowVP->getParent() != NullFC)
    {
        _width = _shadowVP->getPixelWidth();
        _height = _shadowVP->getPixelHeight();
    }

    if(_width == 0)
        _width = 1;
    if(_height == 0)
        _height = 1;

    if(_width > _height)
        _widthHeightPOT = osgnextpower2(_width - 1);
    else
        _widthHeightPOT = osgnextpower2(_height - 1);

    _tiledeco = NullFC;

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

    if(_useNPOTTextures)
    {
        beginEditCP(_colorMapImage);
        _colorMapImage->set(GL_RGB, _width, _height);
        endEditCP(_colorMapImage);
    }
    else
    {
        beginEditCP(_colorMapImage);
        _colorMapImage->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
        endEditCP(_colorMapImage);
    }

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

    if(_useNPOTTextures)
    {
        beginEditCP(_shadowFactorMapImage);
        _shadowFactorMapImage->set(GL_RGB, _width, _height);
        endEditCP(_shadowFactorMapImage);
    }
    else
    {
        beginEditCP(_shadowFactorMapImage);
        _shadowFactorMapImage->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
        endEditCP(_shadowFactorMapImage);
    }

    _shadowFactorMap2 = TextureChunk::create();
    _shadowFactorMapImage2 = Image::create();

    beginEditCP(_shadowFactorMap2);
    _shadowFactorMap2->setImage(_shadowFactorMapImage2);
    _shadowFactorMap2->setInternalFormat(GL_RGB);
    _shadowFactorMap2->setExternalFormat(GL_RGB);
    _shadowFactorMap2->setMinFilter(GL_LINEAR);
    _shadowFactorMap2->setMagFilter(GL_LINEAR);
    _shadowFactorMap2->setWrapS(GL_REPEAT);
    _shadowFactorMap2->setWrapT(GL_REPEAT);
    _shadowFactorMap2->setTarget(GL_TEXTURE_2D);
    endEditCP(_shadowFactorMap2);

    if(_useNPOTTextures)
    {
        beginEditCP(_shadowFactorMapImage2);
        _shadowFactorMapImage2->set(GL_RGB, _width, _height);
        endEditCP(_shadowFactorMapImage2);
    }
    else
    {
        beginEditCP(_shadowFactorMapImage2);
        _shadowFactorMapImage2->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
        endEditCP(_shadowFactorMapImage2);
    }

    //SHL Chunk 2
    _combineSHL = SHLChunk::create();

    beginEditCP(_combineSHL);
    //_combineSHL->readVertexProgram("Dither_Shadow_combine.vert");
    //_combineSHL->readFragmentProgram("Dither_Shadow_combine.frag");
    _combineSHL->setVertexProgram(_dither_shadow_combine_vp);
    _combineSHL->setFragmentProgram(_dither_shadow_combine_fp);
    endEditCP(_combineSHL);

    //SHL Chunk 3
    _shadowCubeSHL = SHLChunk::create();

    _shadowCmat = ChunkMaterial::create();

    _unlitMat = SimpleMaterial::create();
    beginEditCP(_unlitMat);
    _unlitMat->setLit(false);
    endEditCP(_unlitMat);

    //Combine Shader
    _combineCmat = ChunkMaterial::create();
    beginEditCP(_combineCmat);
    _combineCmat->addChunk(_combineSHL);
    _combineCmat->addChunk(_colorMap);
    _combineCmat->addChunk(_shadowFactorMap);
    endEditCP(_combineCmat);

    _pf = PolygonForeground::create();
    beginEditCP(_pf);
    _pf->setMaterial(_combineCmat);
    _pf->getTexCoords().push_back(Vec3f(0.0f, 0.0f, 0.0f));
    _pf->getPositions().push_back(Pnt2f(0.0f, 0.0f));

    _pf->getTexCoords().push_back(Vec3f(1.0f, 0.0f, 0.0f));
    _pf->getPositions().push_back(Pnt2f(1.0f, 0.0f));

    _pf->getTexCoords().push_back(Vec3f(1.0f, 1.0f, 0.0f));
    _pf->getPositions().push_back(Pnt2f(1.0f, 1.0f));

    _pf->getTexCoords().push_back(Vec3f(0.0f, 1.0f, 0.0f));
    _pf->getPositions().push_back(Pnt2f(0.0f, 1.0f));

    _pf->setNormalizedX(true);
    _pf->setNormalizedY(true);
    endEditCP(_pf);

    //SHL Chunk 1
    _shadowSHL = SHLChunk::create();
    beginEditCP(_shadowSHL);
    //_shadowSHL->readVertexProgram("Dither_Shadow.vert");
    //_shadowSHL->readFragmentProgram("Dither_Shadow.frag");
    _shadowSHL->setVertexProgram(_dither_shadow_vp);
    _shadowSHL->setFragmentProgram(_dither_shadow_fp);
    endEditCP(_shadowSHL);

    _shadowSHL2 = SHLChunk::create();
    beginEditCP(_shadowSHL2);
    //_shadowSHL2->readVertexProgram("Dither_Shadow2.vert");
    //_shadowSHL2->readFragmentProgram("Dither_Shadow2.frag");
    _shadowSHL2->setVertexProgram(_dither_shadow2_vp);
    _shadowSHL2->setFragmentProgram(_dither_shadow2_fp);
    endEditCP(_shadowSHL2);

    _shadowSHL3 = SHLChunk::create();
    beginEditCP(_shadowSHL3);
    //_shadowSHL3->readVertexProgram("Dither_Shadow3.vert");
    //_shadowSHL3->readFragmentProgram("Dither_Shadow3.frag");
    _shadowSHL3->setVertexProgram(_dither_shadow3_vp);
    _shadowSHL3->setFragmentProgram(_dither_shadow3_fp);
    endEditCP(_shadowSHL3);

    _shadowSHL4 = SHLChunk::create();
    beginEditCP(_shadowSHL4);
    //_shadowSHL4->readVertexProgram("Dither_Shadow4.vert");
    //_shadowSHL4->readFragmentProgram("Dither_Shadow4.frag");
    _shadowSHL4->setVertexProgram(_dither_shadow4_vp);
    _shadowSHL4->setFragmentProgram(_dither_shadow4_fp);
    endEditCP(_shadowSHL4);

    beginEditCP(_shadowCubeSHL);
    //_shadowCubeSHL->readVertexProgram("Dither_CubeShadow.vert");
    //_shadowCubeSHL->readFragmentProgram("Dither_CubeShadow.frag");
    _shadowCubeSHL->setVertexProgram(_dither_cubeshadow_vp);
    _shadowCubeSHL->setFragmentProgram(_dither_cubeshadow_fp);
    endEditCP(_shadowCubeSHL);

    _transforms[0] = Matrix(1, 0, 0, 0,
                            0, -1, 0, 0,
                            0, 0, -1, 0,
                            0, 0, 0, 1);

    _transforms[1] = Matrix(1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1);

    _transforms[2] = Matrix(1, 0, 0, 0,
                            0, 0, 1, 0,
                            0, -1, 0, 0,
                            0, 0, 0, 1);

    _transforms[3] = Matrix(1, 0, 0, 0,
                            0, 0, -1, 0,
                            0, 1, 0, 0,
                            0, 0, 0, 1);

    _transforms[4] = Matrix(0, 0, 1, 0,
                            0, 1, 0, 0,
                            -1, 0, 0, 0,
                            0, 0, 0, 1);

    _transforms[5] = Matrix(0, 0, -1, 0,
                            0, 1, 0, 0,
                            1, 0, 0, 0,
                            0, 0, 0, 1);

    addRefCP(_colorMap);
    addRefCP(_shadowFactorMap);
    addRefCP(_shadowFactorMap2);
    addRefCP(_shadowSHL);
    addRefCP(_shadowSHL2);
    addRefCP(_shadowSHL3);
    addRefCP(_shadowSHL4);
    addRefCP(_combineSHL);
    addRefCP(_shadowCubeSHL);
    addRefCP(_combineCmat);
    addRefCP(_shadowCmat);
    addRefCP(_unlitMat);
    addRefCP(_pf);

}

DitherShadowMap::~DitherShadowMap(void)
{
    _shadowCmat->clearChunks();

    if(_tiledeco != NullFC)
        subRefCP(_tiledeco);
    subRefCP(_blender);
    subRefCP(_colorMap);
    subRefCP(_shadowFactorMap);
    subRefCP(_shadowFactorMap2);
    subRefCP(_shadowSHL);
    subRefCP(_shadowSHL2);
    subRefCP(_shadowSHL3);
    subRefCP(_shadowSHL4);
    subRefCP(_combineSHL);
    subRefCP(_shadowCubeSHL);
    subRefCP(_combineCmat);
    subRefCP(_shadowCmat);
    subRefCP(_unlitMat);
    subRefCP(_pf);

    if(_fb != 0)
        glDeleteFramebuffersEXT(1, &_fb);
    if(_rb_depth != 0)
        glDeleteRenderbuffersEXT(1, &_rb_depth);
    if(_fb2 != 0)
        glDeleteFramebuffersEXT(1, &_fb2);
}

/// Checks if FBO status is ok
bool DitherShadowMap::checkFrameBufferStatus(Window *win)
{
    GLenum  errCode, status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch(status)
    {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            FINFO(("%x: framebuffer complete!\n", status));
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            FWARNING(("%x: framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n",
                      status));
            // choose different formats
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
            FWARNING(("%x: framebuffer INCOMPLETE_ATTACHMENT\n", status));
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            FWARNING(("%x: framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n",
                      status));
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            FWARNING(("%x: framebuffer FRAMEBUFFER_DIMENSIONS\n", status));
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

    if((errCode = glGetError()) != GL_NO_ERROR)
    {
        const GLubyte   *errString = gluErrorString(errCode);
        FWARNING(("OpenGL Error: %s!\n", errString));
        return false;
    }
    return true;
}


bool DitherShadowMap::initFBO(Window *win)
{
    if(_useFBO)
    {
        Int32   width = _shadowVP->getPixelWidth();
        Int32   height = _shadowVP->getPixelHeight();

        if(width <= 0 || height <= 0)
            return false;

        if(_fb != 0)
            return true;

        glGenFramebuffersEXT(1, &_fb);
        glGenRenderbuffersEXT(1, &_rb_depth);

        win->validateGLObject(_colorMap->getGLId());
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D,
                                  win->getGLObjectId(_colorMap->getGLId()), 0);
        win->validateGLObject(_shadowFactorMap->getGLId());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
                                  GL_TEXTURE_2D,
                                  win->getGLObjectId(_shadowFactorMap->getGLId
                                                     ()), 0);
        win->validateGLObject(_shadowFactorMap2->getGLId());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
                                  GL_TEXTURE_2D,
                                  win->getGLObjectId(_shadowFactorMap2->getGLId
                                                     ()), 0);

        //Initialize Depth Renderbuffer
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _rb_depth);
        if(_useNPOTTextures)
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB
                                     , width, height);
        else
            glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB
                                     , _widthHeightPOT, _widthHeightPOT);
        //Attach Renderbuffer to Framebuffer depth Buffer
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT
                                     , GL_RENDERBUFFER_EXT, _rb_depth);

        win->validateGLObject(_colorMap->getGLId());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D,
                                  win->getGLObjectId(_colorMap->getGLId()), 0);

        win->validateGLObject(_shadowFactorMap->getGLId());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
                                  GL_TEXTURE_2D,
                                  win->getGLObjectId(_shadowFactorMap->getGLId
                                                     ()), 0);

        win->validateGLObject(_shadowFactorMap2->getGLId());
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
                                  GL_TEXTURE_2D,
                                  win->getGLObjectId(_shadowFactorMap2->getGLId
                                                     ()), 0);

        bool    result = checkFrameBufferStatus(win);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

        glGenFramebuffersEXT(1, &_fb2);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb2);

        glDrawBuffer(GL_NONE);    // no color buffer dest
        glReadBuffer(GL_NONE);    // no color buffer src

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

    }
    return true;
}

void DitherShadowMap::reInit(Window *win)
{
    Int32 width = _shadowVP->getPixelWidth();
    Int32 height = _shadowVP->getPixelHeight();

    win->validateGLObject(_colorMap->getGLId());
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                              GL_TEXTURE_2D,
                              win->getGLObjectId(_colorMap->getGLId()), 0);
    win->validateGLObject(_shadowFactorMap->getGLId());
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
                              GL_TEXTURE_2D,
                              win->getGLObjectId(_shadowFactorMap->getGLId()),
                              0);
    win->validateGLObject(_shadowFactorMap2->getGLId());
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
                              GL_TEXTURE_2D,
                              win->getGLObjectId(_shadowFactorMap2->getGLId()),
                              0);

    //Initialize Depth Renderbuffer
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, _rb_depth);
    if(_useNPOTTextures)
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB,
                                 width, height);
    else
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24_ARB,
                                 _widthHeightPOT, _widthHeightPOT);
    //Attach Renderbuffer to Framebuffer depth Buffer
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT
                                 , _rb_depth);
}

void DitherShadowMap::initTextures(Window *win)
{
    _initTexturesDone = true;

    Int32   width = _shadowVP->getPixelWidth();
    Int32   height = _shadowVP->getPixelHeight();

    //if no NPOTTextures supported, resize images
    if(!_useNPOTTextures)
    {
        if(width > height)
            _widthHeightPOT = osgnextpower2(width - 1);
        else
            _widthHeightPOT = osgnextpower2(height - 1);

        beginEditCP(_colorMap);
        beginEditCP(_colorMapImage);
        _colorMapImage->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
        endEditCP(_colorMapImage);
        endEditCP(_colorMap);

        beginEditCP(_shadowFactorMap);
        beginEditCP(_shadowFactorMapImage);
        _shadowFactorMapImage->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
        endEditCP(_shadowFactorMapImage);
        endEditCP(_shadowFactorMap);

        beginEditCP(_shadowFactorMap2);
        beginEditCP(_shadowFactorMapImage2);
        _shadowFactorMapImage2->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
        endEditCP(_shadowFactorMapImage2);
        endEditCP(_shadowFactorMap2);
    }
}

void DitherShadowMap::createShadowMaps(RenderActionBase *action)
{
    if(_tiledeco == NullFC)
    {
        _tiledeco = TileCameraDecorator::create();
        addRefCP(_tiledeco);
    }

    //Checking for the smallest Window-Dimension
    UInt32  minSize = _shadowVP->getPixelWidth();

    if(_shadowVP->getPixelHeight() < minSize)
        minSize = _shadowVP->getPixelHeight();

    //Checking for biggest PowerOf2 that fits in smallest Window-Dimension
    UInt32  _mapRenderSize = osgnextpower2(minSize + 1) / 2;
    if(_mapRenderSize == 0)
        _mapRenderSize = 128;

    Real32  vpTop, vpBottom, vpLeft, vpRight;

    //------Setting up Window to fit size of ShadowMap----------------

    // Saving original Viewport-Dimensions
    vpTop = _shadowVP->getTop();
    vpBottom = _shadowVP->getBottom();
    vpLeft = _shadowVP->getLeft();
    vpRight = _shadowVP->getRight();

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_TRUE);

    // disable all lights more speed
    for(UInt32 i = 0;i < _shadowVP->_lights.size();++i)
    {
        if(_shadowVP->_lightStates[i] != 0)
            _shadowVP->_lights[i].second->setOn(false);
    }

    // deactivate exclude nodes:
    for(UInt32 i = 0;i < _shadowVP->getExcludeNodes().getSize();++i)
    {
        NodePtr exnode = _shadowVP->getExcludeNodes()[i];
        if(exnode != NullFC)
            exnode->setActive(false);
    }

    for(UInt32 i = 0;i < _shadowVP->_lights.size();++i)
    {
        if(_shadowVP->_lightStates[i] != 0)
        {
            if(_shadowVP->getGlobalShadowIntensity() != 0.0 ||
               _shadowVP->_lights[i].second->getShadowIntensity() != 0.0)
            {
                if(_shadowVP->_lights[i].second->getType() != PointLight::getClassType
                   () || !_shadowVP->_realPointLight[i])
                {
                    if(_mapRenderSize > _shadowVP->getMapSize())
                        _mapRenderSize = _shadowVP->getMapSize();
                    _shadowVP->setVPSize(0, 0, _mapRenderSize - 1,
                                         _mapRenderSize - 1);
                    // we use a tiledecorator to create shadow maps with
                    // a higher resolutions than the viewport or the screen.
                    beginEditCP(_tiledeco);
                    _tiledeco->setDecoratee(_shadowVP->_lightCameras[i]);
                    _tiledeco->setFullSize(_shadowVP->getMapSize(),
                                           _shadowVP->getMapSize());
                    endEditCP(_tiledeco);

                    action->setCamera(_tiledeco.getCPtr());

                    Real32  step = (1.0 / Real32(_shadowVP->getMapSize())) *
                        Real32(_mapRenderSize);

                    UInt32  ypos = 0;
                    for(Real32 y = 0;y < 1.0;y += step)
                    {
                        UInt32  xpos = 0;
                        for(Real32 x = 0;x < 1.0;x += step)
                        {
                            beginEditCP(_tiledeco);
                            _tiledeco->setSize(x, y, x + step, y + step);
                            endEditCP(_tiledeco);

                            glClear(GL_DEPTH_BUFFER_BIT);
                            glPolygonOffset(_shadowVP->getOffFactor(),
                                            _shadowVP->getOffBias());
                            glEnable(GL_POLYGON_OFFSET_FILL);

                            _shadowVP->renderLight(action, _unlitMat.getCPtr(), i);

                            action->getWindow()->validateGLObject(
                                _shadowVP->_texChunks[i]->getGLId());

                            glDisable(GL_POLYGON_OFFSET_FILL);

                            //----------Shadow-Texture-Parameters and Indices-------------

                            glBindTexture(GL_TEXTURE_2D,
                                          action->getWindow()->getGLObjectId(
                                          _shadowVP->_texChunks[i]->getGLId
                                          ()));
                            if(glGetError() != GL_NO_ERROR)
                                SWARNING << "Error on binding Texture!" <<
                                    endLog;

                            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xpos, ypos,
                                                0, 0,
                                                _mapRenderSize,
                                                _mapRenderSize);

                            if(glGetError() != GL_NO_ERROR)
                                SWARNING << "Error on copying Texture!" <<
                                    endLog;

                            glBindTexture(GL_TEXTURE_2D, 0);
                            if(glGetError() != GL_NO_ERROR)
                                SWARNING << "Error on releasing Texture!" <<
                                    endLog;

                            if(glGetError() != GL_NO_ERROR)
                                SWARNING << "Error while Texture-Creation!" <<
                                    endLog;

                            xpos += _mapRenderSize;
                        }
                        ypos += _mapRenderSize;
                    }
                }
                else
                {
                    if(_mapRenderSize > _PLMapSize)
                        _mapRenderSize = _PLMapSize;
                    _shadowVP->setVPSize(0, 0, _mapRenderSize - 1,
                                         _mapRenderSize - 1);

                    MatrixCameraDecoratorPtr    deco =
                        MatrixCameraDecorator::create();

                    for(UInt32 j = 0;j < 6;j++)
                    {
                        //Offset berechnen
                        UInt32  xOffset, yOffset;
                        if(j == 0)
                        {
                            xOffset = 0;
                            yOffset = 0;
                        }
                        else if(j == 1)
                        {
                            xOffset = _PLMapSize;
                            yOffset = 0;
                        }
                        else if(j == 2)
                        {
                            xOffset = 2 * _PLMapSize;
                            yOffset = 0;
                        }
                        else if(j == 3)
                        {
                            xOffset = 3 * _PLMapSize;
                            yOffset = 0;
                        }
                        else if(j == 4)
                        {
                            xOffset = 0;
                            yOffset = _PLMapSize;
                        }
                        else
                        {
                            xOffset = _PLMapSize;
                            yOffset = _PLMapSize;
                        }

                        beginEditCP(deco);
                        deco->setDecoratee(_shadowVP->_lightCameras[i]);
                        deco->setPreProjection(_transforms[j]);
                        endEditCP(deco);

                        // we use a tiledecorator to create shadow maps with
                        // a higher resolutions than the viewport or the screen.
                        beginEditCP(_tiledeco);
                        _tiledeco->setDecoratee(deco);
                        _tiledeco->setFullSize(_PLMapSize, _PLMapSize);
                        endEditCP(_tiledeco);

                        action->setCamera(_tiledeco.getCPtr());

                        Real32  step = (1.0 / Real32(_PLMapSize)) *
                            Real32(_mapRenderSize);

                        UInt32  ypos = 0;
                        for(Real32 y = 0;y < 1.0;y += step)
                        {
                            UInt32  xpos = 0;
                            for(Real32 x = 0;x < 1.0;x += step)
                            {
                                beginEditCP(_tiledeco);
                                _tiledeco->setSize(x, y, x + step, y + step);
                                endEditCP(_tiledeco);

                                glClear(GL_DEPTH_BUFFER_BIT);
                                glPolygonOffset(_shadowVP->getOffFactor(),
                                                _shadowVP->getOffBias());
                                glEnable(GL_POLYGON_OFFSET_FILL);

                                _shadowVP->renderLight(action, _unlitMat.getCPtr(), i);

                                action->getWindow()->validateGLObject(
                                    _shadowVP->_texChunks[i]->getGLId());

                                glDisable(GL_POLYGON_OFFSET_FILL);

                                //----------Shadow-Texture-Parameters and Indices-------------

                                glBindTexture(GL_TEXTURE_2D,
                                              action->getWindow
                                              ()->getGLObjectId(
                                              _shadowVP->_texChunks[i]->getGLId
                                              ()));
                                if(glGetError() != GL_NO_ERROR)
                                    SWARNING << "Error on binding Texture!" <<
                                        endLog;

                                glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                                                    xpos + xOffset,
                                                    ypos + yOffset, 0, 0,
                                                    _mapRenderSize,
                                                    _mapRenderSize);

                                if(glGetError() != GL_NO_ERROR)
                                    SWARNING << "Error on copying Texture!" <<
                                        endLog;

                                glBindTexture(GL_TEXTURE_2D, 0);
                                if(glGetError() != GL_NO_ERROR)
                                    SWARNING <<
                                        "Error on releasing Texture!" <<
                                        endLog;

                                if(glGetError() != GL_NO_ERROR)
                                    SWARNING << "Error while Texture-Creation!"
                                        << endLog;

                                xpos += _mapRenderSize;
                            }
                            ypos += _mapRenderSize;
                        }
                    }
                    subRefCP(deco);
                }
            }
        }
    }

    // activate exclude nodes:
    for(UInt32 i = 0;i < _shadowVP->getExcludeNodes().getSize();++i)
    {
        NodePtr exnode = _shadowVP->getExcludeNodes()[i];
        if(exnode != NullFC)
            if(_shadowVP->_excludeNodeActive[i])
                exnode->setActive(true);
    }

    // enable all lights.
    for(UInt32 i = 0;i < _shadowVP->_lights.size();++i)
    {
        if(_shadowVP->_lightStates[i] != 0)
            _shadowVP->_lights[i].second->setOn(true);
    }

    //-------Restoring old states of Window and Viewport----------

    _shadowVP->setVPSize(vpLeft, vpBottom, vpRight, vpTop);

    action->setCamera(_shadowVP->getCamera().getCPtr());

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
}

void DitherShadowMap::createShadowMapsFBO(RenderActionBase *action)
{
    // Saving original Viewport-Dimensions
    Real32  vpTop, vpBottom, vpLeft, vpRight;
    vpTop = _shadowVP->getTop();
    vpBottom = _shadowVP->getBottom();
    vpLeft = _shadowVP->getLeft();
    vpRight = _shadowVP->getRight();

    //------Setting up Window to fit size of ShadowMap----------------

    _shadowVP->setVPSize(0, 0, _shadowVP->getMapSize() - 1,
                         _shadowVP->getMapSize() - 1);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glShadeModel(GL_FLAT);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_TRUE);

    // disable all lights more speed
    for(UInt32 i = 0;i < _shadowVP->_lights.size();++i)
    {
        if(_shadowVP->_lightStates[i] != 0)
            _shadowVP->_lights[i].second->setOn(false);
    }

    // deactivate exclude nodes:
    for(UInt32 i = 0;i < _shadowVP->getExcludeNodes().getSize();++i)
    {
        NodePtr exnode = _shadowVP->getExcludeNodes()[i];
        if(exnode != NullFC)
            exnode->setActive(false);
    }

    for(UInt32 i = 0;i < _shadowVP->_lights.size();++i)
    {
        if(_shadowVP->_lightStates[i])
        {
            if(_shadowVP->getGlobalShadowIntensity() != 0.0 ||
               _shadowVP->_lights[i].second->getShadowIntensity() != 0.0)
            {
                //------Setting up Window to fit size of ShadowMap----------------

                _shadowVP->setVPSize(0, 0,
                                     _shadowVP->_texChunks[i]->getImage
                                     ()->getWidth() - 1,
                                     _shadowVP->_texChunks[i]->getImage
                                     ()->getHeight() - 1);

                if(_shadowVP->_lights[i].second->getType() != PointLight::getClassType
                   () || !_shadowVP->_realPointLight[i])
                {

                    action->getWindow()->validateGLObject(
                        _shadowVP->_texChunks[i]->getGLId());

                    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb2);

                    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT
                                              , GL_TEXTURE_2D,
                                              action->getWindow
                                              ()->getGLObjectId(
                                              _shadowVP->_texChunks[i]->getGLId
                                              ()), 0);

                    glDrawBuffer(GL_NONE);
                    glReadBuffer(GL_NONE);

                    glPolygonOffset(_shadowVP->getOffFactor(),
                                    _shadowVP->getOffBias());
                    glEnable(GL_POLYGON_OFFSET_FILL);

                    glClearColor(1.0, 1.0, 1.0, 1.0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    _shadowVP->setVPSize(0, 0, _shadowVP->getMapSize(),
                                         _shadowVP->getMapSize());

                    action->setCamera(_shadowVP->_lightCameras[i].getCPtr());

                    _shadowVP->renderLight(action, _unlitMat.getCPtr(), i);

                    glDisable(GL_POLYGON_OFFSET_FILL);

                    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

                    glClearColor(0.0, 0.0, 0.0, 1.0);

                    action->setCamera(_shadowVP->getCamera().getCPtr());
                }
                else
                {
                    MatrixCameraDecoratorPtr    deco =
                        MatrixCameraDecorator::create();
                    action->getWindow()->validateGLObject(
                        _shadowVP->_texChunks[i]->getGLId());

                    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb2);

                    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT
                                              , GL_TEXTURE_2D,
                                              action->getWindow
                                              ()->getGLObjectId(
                                              _shadowVP->_texChunks[i]->getGLId
                                              ()), 0);

                    glDrawBuffer(GL_NONE);
                    glReadBuffer(GL_NONE);

                    glClearColor(1.0, 1.0, 1.0, 1.0);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    for(UInt32 j = 0;j < 6;j++)
                    {
                        //Offset berechnen
                        UInt32  xOffset, yOffset;
                        if(j == 0)
                        {
                            xOffset = 0;
                            yOffset = 0;
                        }
                        else if(j == 1)
                        {
                            xOffset = _PLMapSize;
                            yOffset = 0;
                        }
                        else if(j == 2)
                        {
                            xOffset = 2 * _PLMapSize;
                            yOffset = 0;
                        }
                        else if(j == 3)
                        {
                            xOffset = 3 * _PLMapSize;
                            yOffset = 0;
                        }
                        else if(j == 4)
                        {
                            xOffset = 0;
                            yOffset = _PLMapSize;
                        }
                        else
                        {
                            xOffset = _PLMapSize;
                            yOffset = _PLMapSize;
                        }

                        _shadowVP->setVPSize(xOffset, yOffset,
                                             xOffset + _PLMapSize,
                                             yOffset + _PLMapSize);

                        beginEditCP(deco);
                        deco->setDecoratee(_shadowVP->_lightCameras[i]);
                        deco->setPreProjection(_transforms[j]);
                        endEditCP(deco);

                        glPolygonOffset(_shadowVP->getOffFactor(),
                                        _shadowVP->getOffBias());
                        glEnable(GL_POLYGON_OFFSET_FILL);

                        action->setCamera(deco.getCPtr());

                        _shadowVP->renderLight(action, _unlitMat.getCPtr(), i);

                        glDisable(GL_POLYGON_OFFSET_FILL);
                    }
                    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
                    glClearColor(0.0, 0.0, 0.0, 1.0);
                    subRefCP(deco);
                    action->setCamera(_shadowVP->getCamera().getCPtr());
                }
            }
        }
    }

    //-------Restoring old states of Window and Viewport----------

    // activate exclude nodes:
    for(UInt32 i = 0;i < _shadowVP->getExcludeNodes().getSize();++i)
    {
        NodePtr exnode = _shadowVP->getExcludeNodes()[i];
        if(exnode != NullFC)
            if(_shadowVP->_excludeNodeActive[i])
                exnode->setActive(true);
    }

    // enable all lights.
    for(UInt32 i = 0;i < _shadowVP->_lights.size();++i)
    {
        if(_shadowVP->_lightStates[i] != 0)
            _shadowVP->_lights[i].second->setOn(true);
    }

    _shadowVP->setVPSize(vpLeft, vpBottom, vpRight, vpTop);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
}


void DitherShadowMap::createColorMap(RenderActionBase *action)
{
    // HACK but we need this for a correct clear.
    GLint   pl = _shadowVP->getPixelLeft(), pr = _shadowVP->getPixelRight(),
            pb = _shadowVP->getPixelBottom(),
            pt = _shadowVP->getPixelTop();
    GLint   pw = pr - pl + 1, ph = pt - pb + 1;
    glViewport(pl, pb, pw, ph);
    glScissor(pl, pb, pw, ph);
    glEnable(GL_SCISSOR_TEST);
    _shadowVP->getBackground()->clear(action, _shadowVP);
    glDisable(GL_SCISSOR_TEST);

    action->apply(_shadowVP->getRoot());
    action->getWindow()->validateGLObject(_colorMap->getGLId());

    glBindTexture(GL_TEXTURE_2D,
                  action->getWindow()->getGLObjectId(_colorMap->getGLId()));
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _shadowVP->getPixelLeft(),
                        _shadowVP->getPixelBottom(),
                        _shadowVP->getPixelWidth(),
                        _shadowVP->getPixelHeight());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DitherShadowMap::createColorMapFBO(RenderActionBase *action)
{
    Real32  vpTop, vpBottom, vpLeft, vpRight;
    vpTop = _shadowVP->getTop();
    vpBottom = _shadowVP->getBottom();
    vpLeft = _shadowVP->getLeft();
    vpRight = _shadowVP->getRight();
    _shadowVP->setVPSize(0, 0, _shadowVP->getPixelWidth() - 1,
                         _shadowVP->getPixelHeight() - 1);

    Window  *win = action->getWindow();

    GLenum  *buffers = NULL;
    buffers = new GLenum[1];
    buffers[0] = GL_COLOR_ATTACHMENT0_EXT;

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
    glDrawBuffersARB(1, buffers);

    GLint   pw = _shadowVP->getPixelWidth();
    GLint   ph = _shadowVP->getPixelHeight();
    glViewport(0, 0, pw, ph);
    glScissor(0, 0, pw, ph);
    glEnable(GL_SCISSOR_TEST);
    _shadowVP->getBackground()->clear(action, _shadowVP);
    glDisable(GL_SCISSOR_TEST);

    action->apply(_shadowVP->getRoot());

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

    delete[] buffers;

    _shadowVP->setVPSize(vpLeft, vpBottom, vpRight, vpTop);
}

void DitherShadowMap::createShadowFactorMap(RenderActionBase *action)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    if(_firstRun)
    {
        // HACK but we need this for a correct clear.
        GLint   pl = _shadowVP->getPixelLeft(), pr = _shadowVP->getPixelRight
            (), pb = _shadowVP->getPixelBottom(),
                pt = _shadowVP->getPixelTop();
        GLint   pw = pr - pl + 1, ph = pt - pb + 1;
        glViewport(pl, pb, pw, ph);
        glScissor(pl, pb, pw, ph);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);
    }

    //Finde alle aktiven Lichtquellen
    Real32              activeLights = 0;
    if(_shadowVP->getGlobalShadowIntensity() != 0.0)
    {
        for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
        {
            if(_shadowVP->_lightStates[i] != 0)
                activeLights++;
        }
    }
    else
    {
        for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
        {
            if(_shadowVP->_lightStates[i] != 0 &&
               _shadowVP->_lights[i].second->getShadowIntensity() != 0.0)
                activeLights++;
        }
    }

    //Zuerst alle echte Pointlights
    for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
    {
        if(_shadowVP->_lightStates[i] != 0)
        {
            if((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
                _shadowVP->_lights[i].second->getShadowIntensity() != 0.0) &&
               _shadowVP->_realPointLight[i])
            {
                Real32  shadowIntensity;
                if(_shadowVP->getGlobalShadowIntensity() != 0.0)
                    shadowIntensity = (_shadowVP->getGlobalShadowIntensity() /
                                       activeLights);
                else
                    shadowIntensity =
                        (_shadowVP->_lights[i].second->getShadowIntensity() /
                         activeLights);

                Matrix  LVM, LPM, CVM;
                _shadowVP->_lightCameras[i]->getViewing(LVM,
                                                        _shadowVP->getPixelWidth(), _shadowVP->getPixelHeight());
                _shadowVP->_lightCameras[i]->getProjection(LPM,
                                                           _shadowVP->getPixelWidth(), _shadowVP->getPixelHeight());
                _shadowVP->getCamera()->getViewing(CVM,
                                                   _shadowVP->getPixelWidth(),
                                                   _shadowVP->getPixelHeight
                                                   ());
                Matrix  iCVM = CVM;
                iCVM.invert();

                Real32  texFactor;
                if(_shadowVP->_lights[i].second->getType() == PointLight::getClassType
                   () || _shadowVP->_lights[i].second->getType() ==
                   SpotLight::getClassType())
                    texFactor = Real32(_width) / Real32(_height);
                else
                    texFactor = 1.0;

                Matrix  shadowMatrix = LPM;
                shadowMatrix.mult(LVM);
                shadowMatrix.mult(iCVM);

                Real32  xFactor = 1.0;
                Real32  yFactor = 1.0;

                if(!_useNPOTTextures)
                {
                    xFactor = Real32(_width) / Real32(_widthHeightPOT);
                    yFactor = Real32(_height) / Real32(_widthHeightPOT);
                }

                Matrix  m = action->getCamera()->getBeacon()->getToWorld();

                Matrix  shadowMatrixOP = LVM;
                shadowMatrix.mult(iCVM);

                Matrix  shadowMatrixA = LPM;
                shadowMatrixA.mult(_transforms[0]);
                shadowMatrixA.mult(LVM);
                shadowMatrixA.mult(iCVM);

                Matrix  shadowMatrixB = LPM;
                shadowMatrixB.mult(_transforms[1]);
                shadowMatrixB.mult(LVM);
                shadowMatrixB.mult(iCVM);

                Matrix  shadowMatrixC = LPM;
                shadowMatrixC.mult(_transforms[2]);
                shadowMatrixC.mult(LVM);
                shadowMatrixC.mult(iCVM);

                Matrix  shadowMatrixD = LPM;
                shadowMatrixD.mult(_transforms[3]);
                shadowMatrixD.mult(LVM);
                shadowMatrixD.mult(iCVM);

                Matrix  shadowMatrixE = LPM;
                shadowMatrixE.mult(_transforms[4]);
                shadowMatrixE.mult(LVM);
                shadowMatrixE.mult(iCVM);

                Matrix  shadowMatrixF = LPM;
                shadowMatrixF.mult(_transforms[5]);
                shadowMatrixF.mult(LVM);
                shadowMatrixF.mult(iCVM);

                beginEditCP(_shadowCubeSHL, ShaderChunk::ParametersFieldMask);
                _shadowCubeSHL->setUniformParameter("shadowMap", 0);
                _shadowCubeSHL->setUniformParameter("oldFactorMap", 1);
                _shadowCubeSHL->setUniformParameter("firstRun", _firstRun);
                _shadowCubeSHL->setUniformParameter("intensity",
                                                    shadowIntensity);
                _shadowCubeSHL->setUniformParameter("texFactor", texFactor);
                _shadowCubeSHL->setUniformParameter("lightPMA", shadowMatrixA);
                _shadowCubeSHL->setUniformParameter("lightPMB", shadowMatrixB);
                _shadowCubeSHL->setUniformParameter("lightPMC", shadowMatrixC);
                _shadowCubeSHL->setUniformParameter("lightPMD", shadowMatrixD);
                _shadowCubeSHL->setUniformParameter("lightPME", shadowMatrixE);
                _shadowCubeSHL->setUniformParameter("lightPMF", shadowMatrixF);
                _shadowCubeSHL->setUniformParameter("lightPMOP",
                                                    shadowMatrixOP);
                _shadowCubeSHL->setUniformParameter("KKtoWK", m);
                _shadowCubeSHL->setUniformParameter("mapSize",
                                                    Real32(_PLMapSize));
                _shadowCubeSHL->setUniformParameter("xFactor",
                                                    Real32(xFactor));
                _shadowCubeSHL->setUniformParameter("yFactor",
                                                    Real32(yFactor));
                endEditCP(_shadowCubeSHL, ShaderChunk::ParametersFieldMask);

                beginEditCP(_shadowCmat);
                _shadowCmat->clearChunks();
                _shadowCmat->addChunk(_shadowCubeSHL);
                _shadowCmat->addChunk(_shadowVP->_texChunks[i]);
                _shadowCmat->addChunk(_shadowFactorMap);
                endEditCP(_shadowCmat);

                _shadowVP->renderLight(action, _shadowCmat.getCPtr(), i);

                action->getWindow()->validateGLObject(_shadowFactorMap->getGLId
                                                      ());

                glBindTexture(GL_TEXTURE_2D,
                              action->getWindow()->getGLObjectId(
                              _shadowFactorMap->getGLId()));
                glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                    _shadowVP->getPixelLeft(),
                                    _shadowVP->getPixelBottom(),
                                    _shadowVP->getPixelWidth(),
                                    _shadowVP->getPixelHeight());
                glBindTexture(GL_TEXTURE_2D, 0);
                _firstRun = 0;
            }
        }
    }

    std::vector<Real32> shadowIntensityF;
    std::vector<Real32> texFactorF;
    std::vector<Real32> mapFactorF;
    std::vector<Matrix> shadowMatrixF;

    UInt32              lightCounter = 0;

    Real32              xFactor = 1.0;
    Real32              yFactor = 1.0;

    if(!_useNPOTTextures)
    {
        xFactor = Real32(_width) / Real32(_widthHeightPOT);
        yFactor = Real32(_height) / Real32(_widthHeightPOT);
    }

    //Jetzt alle normalen Lichtquellen
    for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
    {
        if(_shadowVP->_lightStates[i] != 0 &&
           ((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
             _shadowVP->_lights[i].second->getShadowIntensity() != 0.0) &&
            !_shadowVP->_realPointLight[i]))
        {

            Real32  shadowIntensity;
            if(_shadowVP->getGlobalShadowIntensity() != 0.0)
                shadowIntensity = (_shadowVP->getGlobalShadowIntensity() /
                                   activeLights);
            else
                shadowIntensity = (_shadowVP->_lights[i].second->getShadowIntensity
                                   () / activeLights);
            shadowIntensityF.push_back(shadowIntensity);

            Matrix  LVM, LPM, CVM;
            _shadowVP->_lightCameras[i]->getViewing(LVM,
                                                    _shadowVP->getPixelWidth(),
                                                    _shadowVP->getPixelHeight
                                                    ());
            _shadowVP->_lightCameras[i]->getProjection(LPM,
                                                       _shadowVP->getPixelWidth
                                                       (),
                                                       _shadowVP->getPixelHeight());
            _shadowVP->getCamera()->getViewing(CVM, _shadowVP->getPixelWidth(),
                                               _shadowVP->getPixelHeight());
            Matrix  iCVM = CVM;
            iCVM.invert();

            Real32  texFactor;
            if(_shadowVP->_lights[i].second->getType() == PointLight::getClassType
               () || _shadowVP->_lights[i].second->getType() ==
               SpotLight::getClassType())
                texFactor = Real32(_width) / Real32(_height);
            else
                texFactor = 1.0;

            texFactorF.push_back(texFactor);

            Matrix  shadowMatrix = LPM;
            shadowMatrix.mult(LVM);
            shadowMatrix.mult(iCVM);

            shadowMatrixF.push_back(shadowMatrix);

            Real32  mapFactor;
            mapFactor = Real32(_shadowVP->getMapSize()) /
                Real32(_shadowVP->_shadowImages[i]->getWidth());
            mapFactorF.push_back(mapFactor);
            lightCounter++;
        }
    }

    if(_useShaderModel3)
    {

        if(lightCounter != 0)
        {
            UInt32  renderTimes = 1;
            if(lightCounter > 4)
                renderTimes = ceil(Real32(lightCounter) / 4.0f);

            for(UInt32 i = 0;i < renderTimes;i++)
            {

                UInt32  lightOffset = lightCounter - (i * 4);

                //clear chunk and add Textures
                beginEditCP(_shadowCmat);
                _shadowCmat->clearChunks();

                UInt32  lightNum = 0;
                for(UInt32 j = 0;j < _shadowVP->_lights.size();j++)
                {
                    if(_shadowVP->_lightStates[j] != 0)
                    {
                        if((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
                            _shadowVP->_lights[j].second->getShadowIntensity() !=
                            0.0) && !_shadowVP->_realPointLight[j])
                        {
                            if(lightNum >= (i * 4) && lightNum < ((i + 1) * 4))
                            {
                                _shadowCmat->addChunk(
                                    _shadowVP->_texChunks[j]);
                            }
                            lightNum++;
                        }
                    }
                }

                if(lightOffset == 1)
                {
                    _shadowCmat->addChunk(_shadowSHL);
                    _shadowCmat->addChunk(_shadowFactorMap);

                    beginEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
                    _shadowSHL->setUniformParameter("oldFactorMap", 1);
                    _shadowSHL->setUniformParameter("shadowMap", 0);
                    _shadowSHL->setUniformParameter("firstRun", _firstRun);
                    _shadowSHL->setUniformParameter("intensity",
                                                    shadowIntensityF[0 +
                                                    (i * 4)]);
                    _shadowSHL->setUniformParameter("texFactor", texFactorF[0 +
                                                    (i * 4)]);
                    _shadowSHL->setUniformParameter("lightPM",
                                                    shadowMatrixF[0 +
                                                    (i * 4)]);
                    _shadowSHL->setUniformParameter("xFactor",
                                                    Real32(xFactor));
                    _shadowSHL->setUniformParameter("yFactor",
                                                    Real32(yFactor));
                    _shadowSHL->setUniformParameter("mapSize",
                                                    Real32(
                                                    _shadowVP->getMapSize()));
                    _shadowSHL->setUniformParameter("mapFactor",
                                                    Real32(mapFactorF[0 + (i *
                                                                           4)])
                                                    );
                    endEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
                }

                else if(lightOffset == 2)
                {
                    _shadowCmat->addChunk(_shadowSHL2);
                    _shadowCmat->addChunk(_shadowFactorMap);

                    beginEditCP(_shadowSHL2, ShaderChunk::ParametersFieldMask);
                    _shadowSHL2->setUniformParameter("oldFactorMap", 2);
                    _shadowSHL2->setUniformParameter("shadowMap1", 0);
                    _shadowSHL2->setUniformParameter("shadowMap2", 1);
                    _shadowSHL2->setUniformParameter("firstRun", _firstRun);
                    _shadowSHL2->setUniformParameter("intensity1",
                                                     shadowIntensityF[0 +
                                                     (i * 4)]);
                    _shadowSHL2->setUniformParameter("intensity2",
                                                     shadowIntensityF[1 +
                                                     (i * 4)]);
                    _shadowSHL2->setUniformParameter("texFactor1",
                                                     texFactorF[0 + (i * 4)]);
                    _shadowSHL2->setUniformParameter("texFactor2",
                                                     texFactorF[1 + (i * 4)]);
                    _shadowSHL2->setUniformParameter("lightPM1",
                                                     shadowMatrixF[0 +
                                                     (i * 4)]);
                    _shadowSHL2->setUniformParameter("lightPM2",
                                                     shadowMatrixF[1 +
                                                     (i * 4)]);
                    _shadowSHL2->setUniformParameter("xFactor",
                                                     Real32(xFactor));
                    _shadowSHL2->setUniformParameter("yFactor",
                                                     Real32(yFactor));
                    _shadowSHL2->setUniformParameter("mapSize",
                                                     Real32(
                                                     _shadowVP->getMapSize()));
                    _shadowSHL2->setUniformParameter("mapFactor1",
                                                     Real32(mapFactorF[0 + (i *
                                                                            4
                                                                            )])
                                                     );
                    _shadowSHL2->setUniformParameter("mapFactor2",
                                                     Real32(mapFactorF[1 + (i *
                                                                            4
                                                                            )])
                                                     );
                    endEditCP(_shadowSHL2, ShaderChunk::ParametersFieldMask);
                }

                else if(lightOffset == 3)
                {
                    _shadowCmat->addChunk(_shadowSHL3);
                    _shadowCmat->addChunk(_shadowFactorMap);

                    beginEditCP(_shadowSHL3, ShaderChunk::ParametersFieldMask);
                    _shadowSHL3->setUniformParameter("oldFactorMap", 3);
                    _shadowSHL3->setUniformParameter("shadowMap1", 0);
                    _shadowSHL3->setUniformParameter("shadowMap2", 1);
                    _shadowSHL3->setUniformParameter("shadowMap3", 2);
                    _shadowSHL3->setUniformParameter("firstRun", _firstRun);
                    _shadowSHL3->setUniformParameter("intensity1",
                                                     shadowIntensityF[0 +
                                                     (i * 4)]);
                    _shadowSHL3->setUniformParameter("intensity2",
                                                     shadowIntensityF[1 +
                                                     (i * 4)]);
                    _shadowSHL3->setUniformParameter("intensity3",
                                                     shadowIntensityF[2 +
                                                     (i * 4)]);
                    _shadowSHL3->setUniformParameter("texFactor1",
                                                     texFactorF[0 + (i * 4)]);
                    _shadowSHL3->setUniformParameter("texFactor2",
                                                     texFactorF[1 + (i * 4)]);
                    _shadowSHL3->setUniformParameter("texFactor3",
                                                     texFactorF[2 + (i * 4)]);
                    _shadowSHL3->setUniformParameter("lightPM1",
                                                     shadowMatrixF[0 +
                                                     (i * 4)]);
                    _shadowSHL3->setUniformParameter("lightPM2",
                                                     shadowMatrixF[1 +
                                                     (i * 4)]);
                    _shadowSHL3->setUniformParameter("lightPM3",
                                                     shadowMatrixF[2 +
                                                     (i * 4)]);
                    _shadowSHL3->setUniformParameter("xFactor",
                                                     Real32(xFactor));
                    _shadowSHL3->setUniformParameter("yFactor",
                                                     Real32(yFactor));
                    _shadowSHL3->setUniformParameter("mapSize",
                                                     Real32(
                                                     _shadowVP->getMapSize()));
                    _shadowSHL3->setUniformParameter("mapFactor1",
                                                     Real32(mapFactorF[0 + (i *
                                                                            4
                                                                            )])
                                                     );
                    _shadowSHL3->setUniformParameter("mapFactor2",
                                                     Real32(mapFactorF[1 + (i *
                                                                            4
                                                                            )])
                                                     );
                    _shadowSHL3->setUniformParameter("mapFactor3",
                                                     Real32(mapFactorF[2 + (i *
                                                                            4
                                                                            )])
                                                     );
                    endEditCP(_shadowSHL3, ShaderChunk::ParametersFieldMask);
                }

                else
                {
                    _shadowCmat->addChunk(_shadowSHL4);
                    _shadowCmat->addChunk(_shadowFactorMap);

                    beginEditCP(_shadowSHL4, ShaderChunk::ParametersFieldMask);
                    _shadowSHL4->setUniformParameter("oldFactorMap", 4);
                    _shadowSHL4->setUniformParameter("shadowMap1", 0);
                    _shadowSHL4->setUniformParameter("shadowMap2", 1);
                    _shadowSHL4->setUniformParameter("shadowMap3", 2);
                    _shadowSHL4->setUniformParameter("shadowMap4", 3);
                    _shadowSHL4->setUniformParameter("firstRun", _firstRun);
                    _shadowSHL4->setUniformParameter("intensity1",
                                                     shadowIntensityF[0 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("intensity2",
                                                     shadowIntensityF[1 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("intensity3",
                                                     shadowIntensityF[2 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("intensity4",
                                                     shadowIntensityF[3 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("texFactor1",
                                                     texFactorF[0 + (i * 4)]);
                    _shadowSHL4->setUniformParameter("texFactor2",
                                                     texFactorF[1 + (i * 4)]);
                    _shadowSHL4->setUniformParameter("texFactor3",
                                                     texFactorF[2 + (i * 4)]);
                    _shadowSHL4->setUniformParameter("texFactor4",
                                                     texFactorF[3 + (i * 4)]);
                    _shadowSHL4->setUniformParameter("lightPM1",
                                                     shadowMatrixF[0 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("lightPM2",
                                                     shadowMatrixF[1 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("lightPM3",
                                                     shadowMatrixF[2 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("lightPM4",
                                                     shadowMatrixF[3 +
                                                     (i * 4)]);
                    _shadowSHL4->setUniformParameter("xFactor",
                                                     Real32(xFactor));
                    _shadowSHL4->setUniformParameter("yFactor",
                                                     Real32(yFactor));
                    _shadowSHL4->setUniformParameter("mapSize",
                                                     Real32(
                                                     _shadowVP->getMapSize()));
                    _shadowSHL4->setUniformParameter("mapFactor1",
                                                     Real32(mapFactorF[0 + (i *
                                                                            4
                                                                            )])
                                                     );
                    _shadowSHL4->setUniformParameter("mapFactor2",
                                                     Real32(mapFactorF[1 + (i *
                                                                            4
                                                                            )])
                                                     );
                    _shadowSHL4->setUniformParameter("mapFactor3",
                                                     Real32(mapFactorF[2 + (i *
                                                                            4
                                                                            )])
                                                     );
                    _shadowSHL4->setUniformParameter("mapFactor4",
                                                     Real32(mapFactorF[3 + (i *
                                                                            4
                                                                            )])
                                                     );
                    endEditCP(_shadowSHL4, ShaderChunk::ParametersFieldMask);
                }

                endEditCP(_shadowCmat);

                _shadowVP->renderLight(action, _shadowCmat.getCPtr(), i);

                action->getWindow()->validateGLObject(_shadowFactorMap->getGLId
                                                      ());

                glBindTexture(GL_TEXTURE_2D,
                              action->getWindow()->getGLObjectId(
                              _shadowFactorMap->getGLId()));
                glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                    _shadowVP->getPixelLeft(),
                                    _shadowVP->getPixelBottom(),
                                    _shadowVP->getPixelWidth(),
                                    _shadowVP->getPixelHeight());
                glBindTexture(GL_TEXTURE_2D, 0);

                _firstRun = 0;
            }
        }
    }
    else
        //No Shader Model 3.0 supported, Nuber of Instructions is limited...
    {
        for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
        {
            if(lightCounter != 0 && _shadowVP->_lightStates[i] != 0)
            {
                if((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
                    _shadowVP->_lights[i].second->getShadowIntensity() != 0.0) &&
                   !_shadowVP->_realPointLight[i])
                {
                    //clear chunk and add Textures
                    beginEditCP(_shadowCmat);
                    _shadowCmat->clearChunks();
                    _shadowCmat->addChunk(_shadowSHL);
                    _shadowCmat->addChunk(_shadowVP->_texChunks[i]);
                    _shadowCmat->addChunk(_shadowFactorMap);
                    endEditCP(_shadowCmat);

                    beginEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
                    _shadowSHL->setUniformParameter("oldFactorMap", 1);
                    _shadowSHL->setUniformParameter("shadowMap", 0);
                    _shadowSHL->setUniformParameter("firstRun", _firstRun);
                    _shadowSHL->setUniformParameter("intensity",
                                                    shadowIntensityF[i]);
                    _shadowSHL->setUniformParameter("texFactor",
                                                    texFactorF[i]);
                    _shadowSHL->setUniformParameter("lightPM",
                                                    shadowMatrixF[i]);
                    _shadowSHL->setUniformParameter("xFactor",
                                                    Real32(xFactor));
                    _shadowSHL->setUniformParameter("yFactor",
                                                    Real32(yFactor));
                    _shadowSHL->setUniformParameter("mapSize",
                                                    Real32(
                                                    _shadowVP->getMapSize()));
                    _shadowSHL->setUniformParameter("mapFactor",
                                                    Real32(mapFactorF[i]));
                    endEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);

                    UInt32  lightNum = 0;

                    _shadowVP->renderLight(action, _shadowCmat.getCPtr(), i);

                    action->getWindow()->validateGLObject(
                        _shadowFactorMap->getGLId());

                    glBindTexture(GL_TEXTURE_2D,
                                  action->getWindow()->getGLObjectId(
                                  _shadowFactorMap->getGLId()));
                    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                                        _shadowVP->getPixelLeft(),
                                        _shadowVP->getPixelBottom(),
                                        _shadowVP->getPixelWidth(),
                                        _shadowVP->getPixelHeight());
                    glBindTexture(GL_TEXTURE_2D, 0);

                    _firstRun = 0;
                }

            }
        }
    }

    _firstRun = 0;
    shadowIntensityF.clear();
    texFactorF.clear();
    mapFactorF.clear();
    shadowMatrixF.clear();
}

void DitherShadowMap::createShadowFactorMapFBO(RenderActionBase *action)
{
    Real32              vpTop, vpBottom, vpLeft, vpRight;
    vpTop = _shadowVP->getTop();
    vpBottom = _shadowVP->getBottom();
    vpLeft = _shadowVP->getLeft();
    vpRight = _shadowVP->getRight();
    _shadowVP->setVPSize(0, 0, _shadowVP->getPixelWidth() - 1,
                         _shadowVP->getPixelHeight() - 1);

    _activeFactorMap = 0;
    glClearColor(0.0, 0.0, 0.0, 1.0);

    //Finde alle aktiven Lichtquellen
    Real32              activeLights = 0;
    if(_shadowVP->getGlobalShadowIntensity() != 0.0)
    {
        for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
        {
            if(_shadowVP->_lightStates[i] != 0)
                activeLights++;
        }
    }
    else
    {
        for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
        {
            if(_shadowVP->_lightStates[i] != 0 &&
               _shadowVP->_lights[i].second->getShadowIntensity() != 0.0)
                activeLights++;
        }
    }

    {
        //clear all ShadowFactorMaps
        GLenum  *buffers = NULL;
        buffers = new GLenum[1];
        buffers[0] = GL_COLOR_ATTACHMENT1_EXT;

        //Setup FBO
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
        glDrawBuffersARB(1, buffers);

        //clear all ShadowFactorMaps
        // ACHTUNG der fbo kann nur 0,w,0,h rendern
        // damit es auch mit mehreren viewports klappt ...
        GLint   pw = _shadowVP->getPixelWidth();
        GLint   ph = _shadowVP->getPixelHeight();
        glViewport(0, 0, pw, ph);
        glScissor(0, 0, pw, ph);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        delete[] buffers;

        buffers = new GLenum[1];
        buffers[0] = GL_COLOR_ATTACHMENT2_EXT;

        //Setup FBO
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);
        glDrawBuffersARB(1, buffers);

        // ACHTUNG der fbo kann nur 0,w,0,h rendern
        // damit es auch mit mehreren viewports klappt ...
        glViewport(0, 0, pw, ph);
        glScissor(0, 0, pw, ph);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_SCISSOR_TEST);

        delete[] buffers;
    }

    //Zuerst alle echte Pointlights
    for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
    {
        if(_shadowVP->_lightStates[i] != 0)
        {
            if((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
                _shadowVP->_lights[i].second->getShadowIntensity() != 0.0) &&
               _shadowVP->_realPointLight[i])
            {
                Real32  shadowIntensity;
                if(_shadowVP->getGlobalShadowIntensity() != 0.0)
                    shadowIntensity = (_shadowVP->getGlobalShadowIntensity() /
                                       activeLights);
                else
                    shadowIntensity =
                        (_shadowVP->_lights[i].second->getShadowIntensity() /
                         activeLights);

                Matrix  LVM, LPM, CVM;
                _shadowVP->_lightCameras[i]->getViewing(LVM,
                                                        _shadowVP->getPixelWidth(), _shadowVP->getPixelHeight());
                _shadowVP->_lightCameras[i]->getProjection(LPM,
                                                           _shadowVP->getPixelWidth(), _shadowVP->getPixelHeight());
                _shadowVP->getCamera()->getViewing(CVM,
                                                   _shadowVP->getPixelWidth(),
                                                   _shadowVP->getPixelHeight
                                                   ());
                Matrix  iCVM = CVM;
                iCVM.invert();

                Real32  texFactor;
                if(_shadowVP->_lights[i].second->getType() == PointLight::getClassType
                   () || _shadowVP->_lights[i].second->getType() ==
                   SpotLight::getClassType())
                    texFactor = Real32(_width) / Real32(_height);
                else
                    texFactor = 1.0;

                Matrix  shadowMatrix = LPM;
                shadowMatrix.mult(LVM);
                shadowMatrix.mult(iCVM);

                Real32  xFactor = 1.0;
                Real32  yFactor = 1.0;

                if(!_useNPOTTextures)
                {
                    xFactor = Real32(_width) / Real32(_widthHeightPOT);
                    yFactor = Real32(_height) / Real32(_widthHeightPOT);
                }

                Matrix  m = action->getCamera()->getBeacon()->getToWorld();

                Matrix  shadowMatrixOP = LVM;
                shadowMatrix.mult(iCVM);

                Matrix  shadowMatrixA = LPM;
                shadowMatrixA.mult(_transforms[0]);
                shadowMatrixA.mult(LVM);
                shadowMatrixA.mult(iCVM);

                Matrix  shadowMatrixB = LPM;
                shadowMatrixB.mult(_transforms[1]);
                shadowMatrixB.mult(LVM);
                shadowMatrixB.mult(iCVM);

                Matrix  shadowMatrixC = LPM;
                shadowMatrixC.mult(_transforms[2]);
                shadowMatrixC.mult(LVM);
                shadowMatrixC.mult(iCVM);

                Matrix  shadowMatrixD = LPM;
                shadowMatrixD.mult(_transforms[3]);
                shadowMatrixD.mult(LVM);
                shadowMatrixD.mult(iCVM);

                Matrix  shadowMatrixE = LPM;
                shadowMatrixE.mult(_transforms[4]);
                shadowMatrixE.mult(LVM);
                shadowMatrixE.mult(iCVM);

                Matrix  shadowMatrixF = LPM;
                shadowMatrixF.mult(_transforms[5]);
                shadowMatrixF.mult(LVM);
                shadowMatrixF.mult(iCVM);

                beginEditCP(_shadowCubeSHL, ShaderChunk::ParametersFieldMask);
                _shadowCubeSHL->setUniformParameter("shadowMap", 0);
                _shadowCubeSHL->setUniformParameter("oldFactorMap", 1);
                _shadowCubeSHL->setUniformParameter("firstRun", _firstRun);
                _shadowCubeSHL->setUniformParameter("intensity",
                                                    shadowIntensity);
                _shadowCubeSHL->setUniformParameter("texFactor", texFactor);
                _shadowCubeSHL->setUniformParameter("lightPMA", shadowMatrixA);
                _shadowCubeSHL->setUniformParameter("lightPMB", shadowMatrixB);
                _shadowCubeSHL->setUniformParameter("lightPMC", shadowMatrixC);
                _shadowCubeSHL->setUniformParameter("lightPMD", shadowMatrixD);
                _shadowCubeSHL->setUniformParameter("lightPME", shadowMatrixE);
                _shadowCubeSHL->setUniformParameter("lightPMF", shadowMatrixF);
                _shadowCubeSHL->setUniformParameter("lightPMOP",
                                                    shadowMatrixOP);
                _shadowCubeSHL->setUniformParameter("KKtoWK", m);
                _shadowCubeSHL->setUniformParameter("mapSize",
                                                    Real32(_PLMapSize));
                _shadowCubeSHL->setUniformParameter("xFactor",
                                                    Real32(xFactor));
                _shadowCubeSHL->setUniformParameter("yFactor",
                                                    Real32(yFactor));
                endEditCP(_shadowCubeSHL, ShaderChunk::ParametersFieldMask);

                beginEditCP(_shadowCmat);
                _shadowCmat->clearChunks();
                _shadowCmat->addChunk(_shadowCubeSHL);
                _shadowCmat->addChunk(_shadowVP->_texChunks[i]);
                if(_activeFactorMap == 0)
                    _shadowCmat->addChunk(_shadowFactorMap2);
                else
                    _shadowCmat->addChunk(_shadowFactorMap);
                endEditCP(_shadowCmat);

                GLenum  *buffers = NULL;
                buffers = new GLenum[1];
                if(_activeFactorMap == 0)
                    buffers[0] = GL_COLOR_ATTACHMENT1_EXT;
                else
                    buffers[0] = GL_COLOR_ATTACHMENT2_EXT;

                //Setup FBO
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);

                glDrawBuffersARB(1, buffers);

                _shadowVP->renderLight(action, _shadowCmat.getCPtr(), i);

                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

                delete[] buffers;
                _firstRun = 0;
                if(_activeFactorMap == 0)
                    _activeFactorMap = 1;
                else
                    _activeFactorMap = 0;
            }
        }
    }

    std::vector<Real32> shadowIntensityF;
    std::vector<Real32> texFactorF;
    std::vector<Real32> mapFactorF;
    std::vector<Matrix> shadowMatrixF;

    UInt32              lightCounter = 0;

    Real32              xFactor = 1.0;
    Real32              yFactor = 1.0;

    if(!_useNPOTTextures)
    {
        xFactor = Real32(_width) / Real32(_widthHeightPOT);
        yFactor = Real32(_height) / Real32(_widthHeightPOT);
    }

    //Jetzt alle normalen Lichtquellen
    for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
    {
        if(_shadowVP->_lightStates[i] != 0 &&
           ((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
             _shadowVP->_lights[i].second->getShadowIntensity() != 0.0) &&
            !_shadowVP->_realPointLight[i]))
        {

            Real32  shadowIntensity;
            if(_shadowVP->getGlobalShadowIntensity() != 0.0)
                shadowIntensity = (_shadowVP->getGlobalShadowIntensity() /
                                   activeLights);
            else
                shadowIntensity = (_shadowVP->_lights[i].second->getShadowIntensity
                                   () / activeLights);
            shadowIntensityF.push_back(shadowIntensity);

            Matrix  LVM, LPM, CVM;
            _shadowVP->_lightCameras[i]->getViewing(LVM,
                                                    _shadowVP->getPixelWidth(),
                                                    _shadowVP->getPixelHeight
                                                    ());
            _shadowVP->_lightCameras[i]->getProjection(LPM,
                                                       _shadowVP->getPixelWidth
                                                       (),
                                                       _shadowVP->getPixelHeight());
            _shadowVP->getCamera()->getViewing(CVM, _shadowVP->getPixelWidth(),
                                               _shadowVP->getPixelHeight());
            Matrix  iCVM = CVM;
            iCVM.invert();

            Real32  texFactor;
            if(_shadowVP->_lights[i].second->getType() == PointLight::getClassType
               () || _shadowVP->_lights[i].second->getType() ==
               SpotLight::getClassType())
                texFactor = Real32(_width) / Real32(_height);
            else
                texFactor = 1.0;

            texFactorF.push_back(texFactor);

            Matrix  shadowMatrix = LPM;
            shadowMatrix.mult(LVM);
            shadowMatrix.mult(iCVM);

            shadowMatrixF.push_back(shadowMatrix);

            Real32  mapFactor;
            mapFactor = Real32(_shadowVP->getMapSize()) /
                Real32(_shadowVP->_shadowImages[i]->getWidth());
            mapFactorF.push_back(mapFactor);
            lightCounter++;
        }
    }

    if(lightCounter != 0)
    {
        UInt32  renderTimes = 1;
        if(lightCounter > 4)
            renderTimes = ceil(Real32(lightCounter) / 4.0f);

        for(UInt32 i = 0;i < renderTimes;i++)
        {

            GLenum  *buffers = NULL;
            buffers = new GLenum[1];
            if(_activeFactorMap == 0)
                buffers[0] = GL_COLOR_ATTACHMENT1_EXT;
            else
                buffers[0] = GL_COLOR_ATTACHMENT2_EXT;

            UInt32  lightOffset = lightCounter - (i * 4);

            //clear chunk and add Textures
            beginEditCP(_shadowCmat);
            _shadowCmat->clearChunks();

            UInt32  lightNum = 0;
            for(UInt32 j = 0;j < _shadowVP->_lights.size();j++)
            {
                if(_shadowVP->_lightStates[j] != 0)
                {
                    if((_shadowVP->getGlobalShadowIntensity() != 0.0 ||
                        _shadowVP->_lights[j].second->getShadowIntensity() != 0.0) &&
                       !_shadowVP->_realPointLight[j])
                    {
                        if(lightNum >= (i * 4) && lightNum < ((i + 1) * 4))
                        {
                            _shadowCmat->addChunk(_shadowVP->_texChunks[j]);
                        }
                        lightNum++;
                    }
                }
            }

            if(lightOffset == 1)
            {
                _shadowCmat->addChunk(_shadowSHL);
                if(_activeFactorMap == 0)
                    _shadowCmat->addChunk(_shadowFactorMap2);
                else
                    _shadowCmat->addChunk(_shadowFactorMap);

                beginEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
                _shadowSHL->setUniformParameter("oldFactorMap", 1);
                _shadowSHL->setUniformParameter("shadowMap", 0);
                _shadowSHL->setUniformParameter("firstRun", _firstRun);
                _shadowSHL->setUniformParameter("intensity",
                                                shadowIntensityF[0 + (i * 4)]);
                _shadowSHL->setUniformParameter("texFactor", texFactorF[0 +
                                                (i * 4)]);
                _shadowSHL->setUniformParameter("lightPM", shadowMatrixF[0 +
                                                (i * 4)]);
                _shadowSHL->setUniformParameter("xFactor", Real32(xFactor));
                _shadowSHL->setUniformParameter("yFactor", Real32(yFactor));
                _shadowSHL->setUniformParameter("mapSize",
                                                Real32(_shadowVP->getMapSize
                                                       ()));
                _shadowSHL->setUniformParameter("mapFactor",
                                                Real32(mapFactorF[0 +
                                                       (i * 4)]));
                endEditCP(_shadowSHL, ShaderChunk::ParametersFieldMask);
            }

            else if(lightOffset == 2)
            {
                _shadowCmat->addChunk(_shadowSHL2);
                if(_activeFactorMap == 0)
                    _shadowCmat->addChunk(_shadowFactorMap2);
                else
                    _shadowCmat->addChunk(_shadowFactorMap);

                beginEditCP(_shadowSHL2, ShaderChunk::ParametersFieldMask);
                _shadowSHL2->setUniformParameter("oldFactorMap", 2);
                _shadowSHL2->setUniformParameter("shadowMap1", 0);
                _shadowSHL2->setUniformParameter("shadowMap2", 1);
                _shadowSHL2->setUniformParameter("firstRun", _firstRun);
                _shadowSHL2->setUniformParameter("intensity1",
                                                 shadowIntensityF[0 +
                                                 (i * 4)]);
                _shadowSHL2->setUniformParameter("intensity2",
                                                 shadowIntensityF[1 +
                                                 (i * 4)]);
                _shadowSHL2->setUniformParameter("texFactor1", texFactorF[0 +
                                                 (i * 4)]);
                _shadowSHL2->setUniformParameter("texFactor2", texFactorF[1 +
                                                 (i * 4)]);
                _shadowSHL2->setUniformParameter("lightPM1", shadowMatrixF[0 +
                                                 (i * 4)]);
                _shadowSHL2->setUniformParameter("lightPM2", shadowMatrixF[1 +
                                                 (i * 4)]);
                _shadowSHL2->setUniformParameter("xFactor", Real32(xFactor));
                _shadowSHL2->setUniformParameter("yFactor", Real32(yFactor));
                _shadowSHL2->setUniformParameter("mapSize",
                                                 Real32(
                                                 _shadowVP->getMapSize()));
                _shadowSHL2->setUniformParameter("mapFactor1",
                                                 Real32(mapFactorF[0 + (i *
                                                                        4)]));
                _shadowSHL2->setUniformParameter("mapFactor2",
                                                 Real32(mapFactorF[1 + (i *
                                                                        4)]));
                endEditCP(_shadowSHL2, ShaderChunk::ParametersFieldMask);
            }

            else if(lightOffset == 3)
            {
                _shadowCmat->addChunk(_shadowSHL3);
                if(_activeFactorMap == 0)
                    _shadowCmat->addChunk(_shadowFactorMap2);
                else
                    _shadowCmat->addChunk(_shadowFactorMap);

                beginEditCP(_shadowSHL3, ShaderChunk::ParametersFieldMask);
                _shadowSHL3->setUniformParameter("oldFactorMap", 3);
                _shadowSHL3->setUniformParameter("shadowMap1", 0);
                _shadowSHL3->setUniformParameter("shadowMap2", 1);
                _shadowSHL3->setUniformParameter("shadowMap3", 2);
                _shadowSHL3->setUniformParameter("firstRun", _firstRun);
                _shadowSHL3->setUniformParameter("intensity1",
                                                 shadowIntensityF[0 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("intensity2",
                                                 shadowIntensityF[1 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("intensity3",
                                                 shadowIntensityF[2 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("texFactor1", texFactorF[0 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("texFactor2", texFactorF[1 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("texFactor3", texFactorF[2 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("lightPM1", shadowMatrixF[0 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("lightPM2", shadowMatrixF[1 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("lightPM3", shadowMatrixF[2 +
                                                 (i * 4)]);
                _shadowSHL3->setUniformParameter("xFactor", Real32(xFactor));
                _shadowSHL3->setUniformParameter("yFactor", Real32(yFactor));
                _shadowSHL3->setUniformParameter("mapSize",
                                                 Real32(
                                                 _shadowVP->getMapSize()));
                _shadowSHL3->setUniformParameter("mapFactor1",
                                                 Real32(mapFactorF[0 + (i *
                                                                        4)]));
                _shadowSHL3->setUniformParameter("mapFactor2",
                                                 Real32(mapFactorF[1 + (i *
                                                                        4)]));
                _shadowSHL3->setUniformParameter("mapFactor3",
                                                 Real32(mapFactorF[2 + (i *
                                                                        4)]));
                endEditCP(_shadowSHL3, ShaderChunk::ParametersFieldMask);
            }

            else
            {
                _shadowCmat->addChunk(_shadowSHL4);
                if(_activeFactorMap == 0)
                    _shadowCmat->addChunk(_shadowFactorMap2);
                else
                    _shadowCmat->addChunk(_shadowFactorMap);

                beginEditCP(_shadowSHL4, ShaderChunk::ParametersFieldMask);
                _shadowSHL4->setUniformParameter("oldFactorMap", 4);
                _shadowSHL4->setUniformParameter("shadowMap1", 0);
                _shadowSHL4->setUniformParameter("shadowMap2", 1);
                _shadowSHL4->setUniformParameter("shadowMap3", 2);
                _shadowSHL4->setUniformParameter("shadowMap4", 3);
                _shadowSHL4->setUniformParameter("firstRun", _firstRun);
                _shadowSHL4->setUniformParameter("intensity1",
                                                 shadowIntensityF[0 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("intensity2",
                                                 shadowIntensityF[1 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("intensity3",
                                                 shadowIntensityF[2 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("intensity4",
                                                 shadowIntensityF[3 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("texFactor1", texFactorF[0 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("texFactor2", texFactorF[1 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("texFactor3", texFactorF[2 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("texFactor4", texFactorF[3 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("lightPM1", shadowMatrixF[0 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("lightPM2", shadowMatrixF[1 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("lightPM3", shadowMatrixF[2 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("lightPM4", shadowMatrixF[3 +
                                                 (i * 4)]);
                _shadowSHL4->setUniformParameter("xFactor", Real32(xFactor));
                _shadowSHL4->setUniformParameter("yFactor", Real32(yFactor));
                _shadowSHL4->setUniformParameter("mapSize",
                                                 Real32(
                                                 _shadowVP->getMapSize()));
                _shadowSHL4->setUniformParameter("mapFactor1",
                                                 Real32(mapFactorF[0 + (i *
                                                                        4)]));
                _shadowSHL4->setUniformParameter("mapFactor2",
                                                 Real32(mapFactorF[1 + (i *
                                                                        4)]));
                _shadowSHL4->setUniformParameter("mapFactor3",
                                                 Real32(mapFactorF[2 + (i *
                                                                        4)]));
                _shadowSHL4->setUniformParameter("mapFactor4",
                                                 Real32(mapFactorF[3 + (i *
                                                                        4)]));
                endEditCP(_shadowSHL4, ShaderChunk::ParametersFieldMask);
            }

            endEditCP(_shadowCmat);

            //Setup FBO
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fb);

            glDrawBuffersARB(1, buffers);

            _shadowVP->renderLight(action, _shadowCmat.getCPtr(), i);

            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

            _firstRun = 0;
            if(_activeFactorMap == 0)
                _activeFactorMap = 1;
            else
                _activeFactorMap = 0;
            delete[] buffers;
        }
    }
    _firstRun = 0;
    shadowIntensityF.clear();
    texFactorF.clear();
    mapFactorF.clear();
    shadowMatrixF.clear();

    _shadowVP->setVPSize(vpLeft, vpBottom, vpRight, vpTop);
}

void DitherShadowMap::drawCombineMap(RenderActionBase *action)
{
    Real32  xFactor = 1.0f;
    Real32  yFactor = 1.0f;
    if(!_useNPOTTextures)
    {
        xFactor = Real32(_width) / Real32(_widthHeightPOT);
        yFactor = Real32(_height) / Real32(_widthHeightPOT);
    }

    beginEditCP(_combineCmat);
    _combineCmat->clearChunks();
    _combineCmat->addChunk(_combineSHL);
    _combineCmat->addChunk(_colorMap);
    if(_activeFactorMap == 0 && _useFBO)
        _combineCmat->addChunk(_shadowFactorMap2);
    else
        _combineCmat->addChunk(_shadowFactorMap);
    endEditCP(_combineCmat);

    beginEditCP(_combineSHL, ShaderChunk::ParametersFieldMask);
    _combineSHL->setUniformParameter("colorMap", 0);
    _combineSHL->setUniformParameter("shadowFactorMap", 1);
    _combineSHL->setUniformParameter("xFactor", Real32(xFactor));
    _combineSHL->setUniformParameter("yFactor", Real32(yFactor));
    endEditCP(_combineSHL, ShaderChunk::ParametersFieldMask);

    // glViewport is called in the render action but we don't use the renderaction here!
    GLint   pl = _shadowVP->getPixelLeft(), pr = _shadowVP->getPixelRight(),
            pb = _shadowVP->getPixelBottom(),
            pt = _shadowVP->getPixelTop();
    GLint   pw = pr - pl + 1, ph = pt - pb + 1;
    glViewport(pl, pb, pw, ph);
    glScissor(pl, pb, pw, ph);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    _pf->draw(action, _shadowVP);
    glDisable(GL_SCISSOR_TEST);
}

void DitherShadowMap::render(RenderActionBase *action)
{
    Window  *win = action->getWindow();
    initialize(win);

    if(!_useGLSL || !_useShadowExt)
        _shadowVP->Viewport::render(action);
    else
    {

        glPushAttrib(GL_ENABLE_BIT);

        if(!_initTexturesDone)
            initTextures(win);

        if(_shadowVP->getMapSize() / 4 > _maxPLMapSize)
            _PLMapSize = _maxPLMapSize;
        else
            _PLMapSize = _shadowVP->getMapSize() / 4;

        for(UInt32 i = 0;i < _shadowVP->_lights.size();i++)
        {
            glBindTexture(GL_TEXTURE_2D,
                          action->getWindow()->getGLObjectId(
                          _shadowVP->_texChunks[i]->getGLId()));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB,
                            GL_COMPARE_R_TO_TEXTURE_ARB);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB,
                            GL_LEQUAL);
            glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB,
                            GL_LUMINANCE);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        if(_useFBO)
        {
            if(!initFBO(win))
                printf("ERROR with FBOBJECT\n");
        }

        GLfloat globalAmbient[] =
        {
            0.0, 0.0, 0.0, 1.0
        };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
        //Used for ShadowFactorMap
        _firstRun = 1;

        if(_shadowVP->getPixelWidth() != _width ||
           _shadowVP->getPixelHeight() != _height)
        {
            _width = _shadowVP->getPixelWidth();
            _height = _shadowVP->getPixelHeight();

            if(_useNPOTTextures)
            {
                beginEditCP(_colorMap);
                beginEditCP(_colorMapImage);
                _colorMapImage->set(GL_RGB, _width, _height);
                endEditCP(_colorMapImage);
                endEditCP(_colorMap);

                beginEditCP(_shadowFactorMap);
                beginEditCP(_shadowFactorMapImage);
                _shadowFactorMapImage->set(GL_RGB, _width, _height);
                endEditCP(_shadowFactorMapImage);
                endEditCP(_shadowFactorMap);

                beginEditCP(_shadowFactorMap2);
                beginEditCP(_shadowFactorMapImage2);
                _shadowFactorMapImage2->set(GL_RGB, _width, _height);
                endEditCP(_shadowFactorMapImage2);
                endEditCP(_shadowFactorMap2);

                reInit(win);
            }
            else
            {
                if(_width > _height)
                    _widthHeightPOT = osgnextpower2(_width - 1);
                else
                    _widthHeightPOT = osgnextpower2(_height - 1);

                beginEditCP(_colorMap);
                beginEditCP(_colorMapImage);
                _colorMapImage->set(GL_RGB, _widthHeightPOT, _widthHeightPOT);
                endEditCP(_colorMapImage);
                endEditCP(_colorMap);

                beginEditCP(_shadowFactorMap);
                beginEditCP(_shadowFactorMapImage);
                _shadowFactorMapImage->set(GL_RGB, _widthHeightPOT,
                                           _widthHeightPOT);
                endEditCP(_shadowFactorMapImage);
                endEditCP(_shadowFactorMap);

                beginEditCP(_shadowFactorMap2);
                beginEditCP(_shadowFactorMapImage2);
                _shadowFactorMapImage2->set(GL_RGB, _widthHeightPOT,
                                            _widthHeightPOT);
                endEditCP(_shadowFactorMapImage2);
                endEditCP(_shadowFactorMap2);
            }
        }

        if(_shadowVP->getMapAutoUpdate())
        {
            if(_useFBO && _useNPOTTextures)
                createColorMapFBO(action);
            else
                createColorMap(action);

            //deactivate transparent Nodes
            for(UInt32 t = 0;t < _shadowVP->_transparent.size();++t)
                _shadowVP->_transparent[t]->setActive(false);

            if(_useFBO)
                createShadowMapsFBO(action);
            else
                createShadowMaps(action);


            if(_useFBO && _useNPOTTextures)
                createShadowFactorMapFBO(action);
            else
                createShadowFactorMap(action);
        }
        else
        {
            if(_shadowVP->_trigger_update)
            {
                if(_useFBO && _useNPOTTextures)
                    createColorMapFBO(action);
                else
                    createColorMap(action);

                //deactivate transparent Nodes
                for(UInt32 t = 0;t < _shadowVP->_transparent.size();++t)
                    _shadowVP->_transparent[t]->setActive(false);

                if(_useFBO)
                    createShadowMapsFBO(action);
                else
                    createShadowMaps(action);

                if(_useFBO && _useNPOTTextures)
                    createShadowFactorMapFBO(action);
                else
                    createShadowFactorMap(action);
                _shadowVP->_trigger_update = false;
            }
        }

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

        drawCombineMap(action);

        // switch on all transparent geos
        for(UInt32 t = 0;t < _shadowVP->_transparent.size();++t)
            _shadowVP->_transparent[t]->setActive(true);

        glPopAttrib();

        // render the foregrounds.
        for(UInt16 i = 0;i < _shadowVP->getForegrounds().size();++i)
        {
            _shadowVP->getForegrounds(i)->draw(action, _shadowVP);
        }

    }
}
