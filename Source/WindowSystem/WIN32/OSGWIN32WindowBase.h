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

/*****************************************************************************\
 *****************************************************************************
 **                                                                         **
 **                  This file is automatically generated.                  **
 **                                                                         **
 **          Any changes made to this file WILL be lost when it is          **
 **           regenerated, which can become necessary at any time.          **
 **                                                                         **
 **     Do not change this file, changes should be done in the derived      **
 **     class WIN32Window
 **                                                                         **
 *****************************************************************************
\*****************************************************************************/


#ifndef _OSGWIN32WINDOWBASE_H_
#define _OSGWIN32WINDOWBASE_H_
#ifdef __sgi
#pragma once
#endif


#include "OSGConfig.h"
#include "OSGWindowWIN32Def.h"

#include "OSGBaseTypes.h"
#include "OSGRefPtr.h"
#include "OSGCoredNodePtr.h"

#include "OSGWindow.h" // Parent

#include "OSGWIN32WindowDataFields.h" // Hwnd type
#include "OSGWIN32WindowDataFields.h" // Hdc type
#include "OSGWIN32WindowDataFields.h" // Hglrc type

#include "OSGWIN32WindowFields.h"

OSG_BEGIN_NAMESPACE

class WIN32Window;
class BinaryDataHandler;

//! \brief WIN32Window Base Class.

class OSG_WINDOWWIN32LIB_DLLMAPPING WIN32WindowBase : public Window
{
  private:

    typedef Window    Inherited;

    /*==========================  PUBLIC  =================================*/
  public:

    typedef WIN32WindowPtr  Ptr;

    enum
    {
        HwndFieldId  = Inherited::NextFieldId,
        HdcFieldId   = HwndFieldId  + 1,
        HglrcFieldId = HdcFieldId   + 1,
        NextFieldId  = HglrcFieldId + 1
    };

    static const OSG::BitVector HwndFieldMask;
    static const OSG::BitVector HdcFieldMask;
    static const OSG::BitVector HglrcFieldMask;


    static const OSG::BitVector MTInfluenceMask;

    /*---------------------------------------------------------------------*/
    /*! \name                    Class Get                                 */
    /*! \{                                                                 */

    static        FieldContainerType &getClassType    (void); 
    static        UInt32              getClassTypeId  (void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                FieldContainer Get                            */
    /*! \{                                                                 */

    virtual       FieldContainerType &getType  (void); 
    virtual const FieldContainerType &getType  (void) const; 

    virtual       UInt32              getContainerSize(void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Get                                 */
    /*! \{                                                                 */


           SFHWND              *editSFHwnd           (void);
     const SFHWND              *getSFHwnd           (void) const;
#ifndef OSG_2_PREP
           SFHWND              *getSFHwnd           (void);
#endif

           SFHDC               *editSFHdc            (void);
     const SFHDC               *getSFHdc            (void) const;
#ifndef OSG_2_PREP
           SFHDC               *getSFHdc            (void);
#endif

           SFHGLRC             *editSFHglrc          (void);
     const SFHGLRC             *getSFHglrc          (void) const;
#ifndef OSG_2_PREP
           SFHGLRC             *getSFHglrc          (void);
#endif


           HWND                &editHwnd           (void);
     const HWND                &getHwnd           (void) const;
#ifndef OSG_2_PREP
           HWND                &getHwnd           (void);
#endif

           HDC                 &editHdc            (void);
     const HDC                 &getHdc            (void) const;
#ifndef OSG_2_PREP
           HDC                 &getHdc            (void);
#endif

           HGLRC               &editHglrc          (void);
     const HGLRC               &getHglrc          (void) const;
#ifndef OSG_2_PREP
           HGLRC               &getHglrc          (void);
#endif

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Field Set                                 */
    /*! \{                                                                 */

     void setHwnd           ( const HWND &value );
     void setHdc            ( const HDC &value );
     void setHglrc          ( const HGLRC &value );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Binary Access                              */
    /*! \{                                                                 */

    virtual UInt32 getBinSize (const BitVector         &whichField);
    virtual void   copyToBin  (      BinaryDataHandler &pMem,
                               const BitVector         &whichField);
    virtual void   copyFromBin(      BinaryDataHandler &pMem,
                               const BitVector         &whichField);


    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Construction                               */
    /*! \{                                                                 */

    static  WIN32WindowPtr      create          (void); 
    static  WIN32WindowPtr      createEmpty     (void); 

    /*! \}                                                                 */

    /*---------------------------------------------------------------------*/
    /*! \name                       Copy                                   */
    /*! \{                                                                 */

    virtual FieldContainerPtr     shallowCopy     (void) const; 

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
  protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Fields                                  */
    /*! \{                                                                 */

    SFHWND              _sfHwnd;
    SFHDC               _sfHdc;
    SFHGLRC             _sfHglrc;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    WIN32WindowBase(void);
    WIN32WindowBase(const WIN32WindowBase &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~WIN32WindowBase(void); 

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Sync                                   */
    /*! \{                                                                 */

#if !defined(OSG_FIXED_MFIELDSYNC)
    void executeSyncImpl(      WIN32WindowBase *pOther,
                         const BitVector         &whichField);

    virtual void   executeSync(      FieldContainer    &other,
                               const BitVector         &whichField);
#else
    void executeSyncImpl(      WIN32WindowBase *pOther,
                         const BitVector         &whichField,
                         const SyncInfo          &sInfo     );

    virtual void   executeSync(      FieldContainer    &other,
                               const BitVector         &whichField,
                               const SyncInfo          &sInfo);

    virtual void execBeginEdit     (const BitVector &whichField,
                                          UInt32     uiAspect,
                                          UInt32     uiContainerSize);

            void execBeginEditImpl (const BitVector &whichField,
                                          UInt32     uiAspect,
                                          UInt32     uiContainerSize);

    virtual void onDestroyAspect(UInt32 uiId, UInt32 uiAspect);
#endif

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    friend class FieldContainer;

    static FieldDescription   *_desc[];
    static FieldContainerType  _type;


    // prohibit default functions (move to 'public' if you need one)
    void operator =(const WIN32WindowBase &source);
};

//---------------------------------------------------------------------------
//   Exported Types
//---------------------------------------------------------------------------


typedef WIN32WindowBase *WIN32WindowBaseP;

typedef osgIF<WIN32WindowBase::isNodeCore,
              CoredNodePtr<WIN32Window>,
              FieldContainer::attempt_to_create_CoredNodePtr_on_non_NodeCore_FC
              >::_IRet WIN32WindowNodePtr;

typedef RefPtr<WIN32WindowPtr> WIN32WindowRefPtr;

OSG_END_NAMESPACE

#define OSGWIN32WINDOWBASE_HEADER_CVSID "@(#)$Id: FCBaseTemplate_h.h,v 1.41 2008/06/09 07:30:44 vossg Exp $"

#endif /* _OSGWIN32WINDOWBASE_H_ */
