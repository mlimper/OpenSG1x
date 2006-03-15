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

#ifndef _OSGSHLCHUNK_H_
#define _OSGSHLCHUNK_H_
#ifdef __sgi
#pragma once
#endif

#include <OSGConfig.h>

#include <string>
#include <vector>
#include <map>

#include "OSGDrawActionBase.h"
#include "OSGWindow.h"

#include <OSGSHLChunkBase.h>

OSG_BEGIN_NAMESPACE

/*! \brief SHLChunk class. See \ref
           PageKernelSHLChunk for a description.
*/

class OSG_SYSTEMLIB_DLLMAPPING SHLChunk : public SHLChunkBase
{
  private:

    typedef SHLChunkBase Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                 Chunk Class Access                           */
    /*! \{                                                                 */

    virtual const StateChunkClass * getClass         (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name              Static Chunk Class Access                       */
    /*! \{                                                                 */

    inline static        UInt32            getStaticClassId (void);
    inline static  const StateChunkClass * getStaticClass   (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                      Sync                                    */
    /*! \{                                                                 */

    virtual void changed(BitVector  whichField,
                         UInt32     origin    );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Output                                   */
    /*! \{                                                                 */

    virtual void dump(      UInt32     uiIndent = 0,
                      const BitVector  bvFlags  = 0) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Paramerters                            */
    /*! \{                                                                 */

    void updateParameters(Window *win, const MFShaderParameterPtr &parameters,
                          bool useProgram = true, bool force = false,
                          bool keepProgramActive = false);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       State                                  */
    /*! \{                                                                 */

    virtual void activate   ( DrawActionBase * action, UInt32 index = 0 );

    virtual void changeFrom ( DrawActionBase * action, StateChunk * old,
                             UInt32 index = 0 );

    virtual void deactivate ( DrawActionBase * action, UInt32 index = 0 );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Comparison                                */
    /*! \{                                                                 */

    virtual Real32 switchCost  ( StateChunk * chunk );

    virtual bool   operator <  (const StateChunk &other) const;

    virtual bool   operator == (const StateChunk &other) const;
    virtual bool   operator != (const StateChunk &other) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Parameter Callbacks                       */
    /*! \{                                                                 */

    typedef GLint (OSG_APIENTRY * PFNGLGETUNIFORMLOCATIONARBPROC)
            (GLuint programObj, const char *name);

    typedef void (*paramtercbfp) (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                  DrawActionBase *action, GLuint program);

    void addParameterCallback(const char *name, paramtercbfp fp);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Parameter funcs                           */
    /*! \{                                                                 */

    static inline UInt32 getFuncUniform1i(void);
    static inline UInt32 getFuncUniform2iv(void);
    static inline UInt32 getFuncUniform3iv(void);
    static inline UInt32 getFuncUniform4iv(void);

    static inline UInt32 getFuncUniform1f(void);
    static inline UInt32 getFuncUniform2fv(void);
    static inline UInt32 getFuncUniform3fv(void);
    static inline UInt32 getFuncUniform4fv(void);

    static inline UInt32 getFuncUniformMatrix4fv(void);

    static inline UInt32 getFuncGetUniformiv(void);
    static inline UInt32 getFuncGetUniformfv(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Cluster                                   */
    /*! \{                                                                 */

    static void setClusterId(Int32 id);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    // Variables should all be in SHLChunkBase.

    void onCreate(const SHLChunk *source = NULL);
    void onDestroy(void);

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructors                                */
    /*! \{                                                                 */

    SHLChunk(void);
    SHLChunk(const SHLChunk &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~SHLChunk(void);

    /*! \}                                                                 */

    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;
    friend class SHLChunkBase;

    // class. Used for indexing in State
    static StateChunkClass _class;

    /*---------------------------------------------------------------------*/
    /*! \name            OpenGL Extension Handling                         */
    /*! \{                                                                 */

    static UInt32 _shl_extension;
    static UInt32 _cg_extension;

    static UInt32 _funcCreateProgramObject;
    static UInt32 _funcCreateShaderObject;
    static UInt32 _funcDeleteObject;
    static UInt32 _funcDetachObject;
    static UInt32 _funcAttachObject;

    static UInt32 _funcShaderSource;
    static UInt32 _funcCompileShader;
    static UInt32 _funcLinkProgram;
    static UInt32 _funcGetInfoLog;
    static UInt32 _funcUseProgramObject;

    static UInt32 _funcGetObjectParameteriv;
    static UInt32 _funcGetObjectParameterfv;
    static UInt32 _funcGetUniformLocation;

    static UInt32 _funcBindAttribLocation;
    static UInt32 _funcGetAttribLocation;

    static UInt32 _funcUniform1i;
    static UInt32 _funcUniform2iv;
    static UInt32 _funcUniform3iv;
    static UInt32 _funcUniform4iv;
    
    static UInt32 _funcUniform1f;
    static UInt32 _funcUniform2fv;
    static UInt32 _funcUniform3fv;
    static UInt32 _funcUniform4fv;

    static UInt32 _funcUniformMatrix4fv;

    static UInt32 _funcGetUniformiv;
    static UInt32 _funcGetUniformfv;

    static Int32 _clusterId;

    /*! \}                                                                 */

    static void initMethod(void);

    void handleGL(Window *win, UInt32 id);

    // prohibit default functions (move to 'public' if you need one)
    void operator =(const SHLChunk &source);

    void updateProgram(Window *win);
    void checkOSGParameters(void);
    void updateOSGParameters(DrawActionBase *action, GLuint program);

    static void updateCameraOrientation (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateCameraPosition    (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateViewMatrix        (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateInvViewMatrix     (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateStereoLeftEye     (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateClusterId         (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateActiveLightsMask  (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight0Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight1Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight2Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight3Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight4Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight5Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight6Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);
    static void updateLight7Active      (PFNGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                         DrawActionBase *action, GLuint program);

    std::vector<paramtercbfp> _osgParametersCallbacks;

    UInt32 _oldParameterSize;

    typedef std::map<std::string, paramtercbfp> userParameterCallbacksMap;
    userParameterCallbacksMap _userParameterCallbacks;
};

typedef SHLChunk *SHLChunkP;

OSG_END_NAMESPACE

#include <OSGSHLChunkBase.inl>
#include <OSGSHLChunk.inl>

#define OSGSHLCHUNK_HEADER_CVSID "@(#)$Id: OSGSHLChunk.h,v 1.22 2006/03/15 11:51:46 a-m-z Exp $"

#endif /* _OSGCGCHUNK_H_ */
