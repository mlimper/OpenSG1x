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


#ifndef _OSGTHREADMANAGER_H_
#define _OSGTHREADMANAGER_H_
#ifdef __sgi
#pragma once
#endif


#include <OSGBase.h>
#include <OSGBaseTypes.h>
#include <OSGBaseFunctions.h>
#include <OSGIDStringLink.h>

#if defined(OSG_USE_SPROC)
#include <ulocks.h>
#endif

#include <map>

#include <OSGBaseThread.h>
#include <OSGBarrier.h>
#include <OSGLock.h>
#include <OSGLog.h>

OSG_BEGIN_NAMESPACE


/*! \ingroup
 *  \brief Brief
 *
 *  detailed
 */

template <class MPFieldT>
class OSG_BASE_DLLMAPPING MPFieldStore
{
    /*==========================  PUBLIC  =================================*/
  public:

    typedef typename MPFieldT::Type MPFieldType;

    /*==========================  PRIVATE  ================================*/
  private:

    /*---------------------------------------------------------------------*/
    /*! \name                      Types                                   */
    /*! \{                                                                 */

    typedef map<IDStringLink, MPFieldType  *>         MPFieldTypeMap;
    typedef typename MPFieldTypeMap::iterator       MPFieldTypeMapIt;

    typedef typename MPFieldTypeMap::const_iterator MPFieldTypeMapCIt;

    typedef map<IDStringLink, MPFieldT *>             MPFieldMap;
    typedef typename MPFieldMap::iterator           MPFieldMapIt;

    /*! \}                                                                 */

    friend class ThreadManager;

    static char cvsid[];

    // prohibit default functions (move to 'public' if you need one)

    MPFieldStore   (const MPFieldStore &source);
    void operator =(const MPFieldStore &source);

    /*=========================  PROTECTED  ===============================*/
  protected:


    MPFieldTypeMap _mFieldTypeMap;
    MPFieldMap     _mFieldMap;

    UInt32 registerMPType(MPFieldType *pType);

    /*==========================  PUBLIC  =================================*/
  public :

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructor                                */
    /*! \{                                                                 */

    MPFieldStore(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~MPFieldStore(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  Your Category                               */
    /*! \{                                                                 */

    MPFieldT *getMPField   (const Char8    *szName,
                            const Char8    *szTypeName);

    MPFieldT *findMPField  (const Char8    *szName);

    void      removeMPField(      MPFieldT *pField);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Operators                                 */
    /*! \{                                                                 */

    void clear(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Assignment                               */
    /*! \{                                                                 */

    MPFieldType *findMPFieldType(const Char8 *szName) const;

    /*! \}                                                                 */

};


/*! \ingroup BaseThreading
 *  \brief ThreadManager
 */

class OSG_BASE_DLLMAPPING ThreadManager
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                      Types                                   */
    /*! \{                                                                 */

    typedef MPFieldStore<BaseThread> ThreadStore;
    typedef MPFieldStore<Barrier   > BarrierStore;
    typedef MPFieldStore<Lock      > LockStore;
    typedef MPFieldStore<LockPool  > LockPoolStore;

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
  private:

    /*---------------------------------------------------------------------*/
    /*! \name                  Friend Classes                              */
    /*! \{                                                                 */

    friend class MPThreadType;
    friend class MPBarrierType;
    friend class MPLockType;
    friend class MPLockPoolType;

    friend class BaseThread;
    friend class Barrier;
    friend class Lock;
    friend class LockPool;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Class Variable                             */
    /*! \{                                                                 */

    static char cvsid[];

    static ThreadManager   *_pThreadManager;
    static BaseThread      *_pAppThread;

    static Bool             _bShutdownInProgress;

    static UInt32          _uiNumAspects;

#ifdef OSG_RUNTIME_NUM_ASPECTS
    static Bool            _bNumAspectSet;
#endif

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                Instance Variables                            */
    /*! \{                                                                 */

    ThreadStore    _sThreadStore;
    BarrierStore   _sBarrierStore;
    LockStore      _sLockStore;
    LockPoolStore  _sLockPoolStore;

    Lock          *_storePLock;

    static Char8  *_szAppThreadType;

#if defined(OSG_USE_SPROC)
    usptr_t *_pArena;
#endif

    /*! \}                                                                 */


    // prohibit default functions (move to 'public' if you need one)

    ThreadManager  (const ThreadManager &source);
    void operator =(const ThreadManager &source);

    /*=========================  PROTECTED  ===============================*/
  protected:

    static Bool initialize(Int32 &argc, Char8 **argv);
    static Bool terminate (void);

    void removeThread  (BaseThread *pThread);
    void removeBarrier (Barrier    *pBarrier);
    void removeLock    (Lock       *pLock);
    void removeLockPool(LockPool   *pLockPool);

    UInt32 registerThreadType  (MPThreadType   *pType);
    UInt32 registerBarrierType (MPBarrierType  *pType);
    UInt32 registerLockType    (MPLockType     *pType);
    UInt32 registerLockPoolType(MPLockPoolType *pType);

    Bool init    (void);
    Bool shutdown(void);

    ThreadManager(void);
    virtual ~ThreadManager(void);

    /*==========================  PUBLIC  =================================*/
  public :

    /*---------------------------------------------------------------------*/
    /*! \name                  Class Functions                             */
    /*! \{                                                                 */

    static void           setAppThreadType(const Char8 *szAppThreadType);

    static ThreadManager *the             (void);

    static BaseThread    *getAppThread    (void);

    static void           setNumAspects   (UInt32 uiNumApects);
    static UInt32         getNumAspects   (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name               Create Threading Element                       */
    /*! \{                                                                 */

    BaseThread   *getThread   (const Char8 *szName,
                               const Char8 *szTypeName = "OSGThread");
    Barrier      *getBarrier  (const Char8 *szName,
                               const Char8 *szTypeName = "OSGBarrier");
    Lock         *getLock     (const Char8 *szName,
                               const Char8 *szTypeName = "OSGLock");
    LockPool     *getLockPool (const Char8 *szName,
                               const Char8 *szTypeName = "OSGLockPool");

    BaseThread   *findThread  (const Char8 *szName);
    Barrier      *findBarrier (const Char8 *szName);
    Lock         *findLock    (const Char8 *szName);
    LockPool     *findLockPool(const Char8 *szName);

#if defined(OSG_USE_SPROC)
    usptr_t *getArena(void);
#endif

    /*! \}                                                                 */

};

typedef ThreadManager *ThreadManagerP;

OSG_END_NAMESPACE

#include <OSGThreadManager.inl>

#endif /* _OSGTHREADMANAGER_H_ */


