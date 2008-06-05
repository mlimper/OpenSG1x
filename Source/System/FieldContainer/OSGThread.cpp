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

#ifdef OSG_DOC_FILES_IN_MODULE
/*! \file OSGThread.cpp
    \ingroup GrpSystemMultithreading
 */
#endif

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include <iostream>

#include "OSGThread.h"
#include "OSGBaseFunctions.h"
#include "OSGChangeList.h"
#include "OSGThreadManager.h"
#include "OSGLog.h"

#if ! defined (OSG_USE_PTHREADS) && ! defined (OSG_USE_WINTHREADS)
#include <sys/types.h>
#include <sys/prctl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#endif

OSG_USING_NAMESPACE

const UInt32 ThreadCommonBase::InvalidAspect = 
    TypeTraits<UInt32>::BitsSet;

Int32 ThreadCommonBase::_force_aspect = -1;

/*-------------------------------------------------------------------------*/
/*                          Force Aspect                                   */

void ThreadCommonBase::setForceAspect(Int32 aspect)
{
#ifdef OSG_FORCE_ASPECT
    _force_aspect = aspect;
#else
    SFATAL << "ThreadCommonBase::setForceAspect: not supported, please enable it in scons build system via 'force_aspect=1'" << std::endl;
#endif
}

Int32 ThreadCommonBase::getForceAspect(void)
{
    return _force_aspect;
}

/*-------------------------------------------------------------------------*/
/*                                Get                                      */

ChangeList *ThreadCommonBase::getChangeList(void)
{
    return _pChangeList;
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

ThreadCommonBase::ThreadCommonBase(const Char8  *szName,
                                         UInt32  uiId) :
    
     Inherited  (szName, 
                 uiId  ),

    _uiAspectId (0     ),
    _pChangeList(NULL  )
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

ThreadCommonBase::~ThreadCommonBase(void)
{
    subRefP(_pChangeList);
}

/*-------------------------------------------------------------------------*/
/*                                 Set                                     */

void ThreadCommonBase::setAspect(UInt32 uiAspectId)
{
    _uiAspectId = uiAspectId;
}

void ThreadCommonBase::setChangeList(ChangeList *pChangeList)
{
    setRefP(_pChangeList, pChangeList);
}




#if defined (OSG_USE_PTHREADS)

#if defined(OSG_PTHREAD_ELF_TLS)
__thread UInt32      PThreadBase::_uiTLSAspectId  = 0;
__thread ChangeList *PThreadBase::_pTLSChangeList = NULL;
#else
pthread_key_t PThreadBase::_aspectKey;
pthread_key_t PThreadBase::_changeListKey;
#endif

/*-------------------------------------------------------------------------*/
/*                                Get                                      */

UInt32 PThreadBase::getAspect(void)
{
#ifdef OSG_FORCE_ASPECT
    if(_force_aspect != -1)
        return _force_aspect;
#endif

#if defined(OSG_PTHREAD_ELF_TLS)
    return _uiTLSAspectId;
#else
    UInt32 *pUint;

    pUint = (UInt32 *) pthread_getspecific(_aspectKey);

    return *pUint;
#endif

}

ChangeList *PThreadBase::getCurrentChangeList(void)
{
#if defined(OSG_PTHREAD_ELF_TLS)
    return _pTLSChangeList;
#else
    ChangeList **pCList;

    pCList = (ChangeList **) pthread_getspecific(_changeListKey);

    return *pCList;
#endif
}

/*-------------------------------------------------------------------------*/
/*                                Run                                      */

// This workaround was brought to you by gcc 2.95.3

bool PThreadBase::runFunction(ThreadFuncF  fThreadFunc, 
                              UInt32       uiAspectId,
                              void        *pThreadArg)
{
    if(uiAspectId >= ThreadManager::getNumAspects())
    {
        SFATAL << "OSGPTB : invalid aspect id" << std::endl;
        return false;
    }

    Inherited::setAspect(uiAspectId);

    return Inherited::runFunction(fThreadFunc, pThreadArg);
}

/*-------------------------------------------------------------------------*/
/*                               Free                                      */

#if !defined(OSG_PTHREAD_ELF_TLS)
void PThreadBase::freeAspect(void *pAspect)
{
    UInt32 *pUint = (UInt32 *) pAspect;

    if(pUint != NULL)
        delete pUint;
}

void PThreadBase::freeChangeList(void *pChangeList)
{
    ChangeList **pCl = (ChangeList **) pChangeList;

    if(pCl != NULL)
        delete pCl;
}
#endif

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

PThreadBase::PThreadBase(const Char8   *szName, 
                               UInt32   uiId) :
    Inherited(szName, uiId)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

PThreadBase::~PThreadBase(void)
{
}

/*-------------------------------------------------------------------------*/
/*                               Setup                                     */

void PThreadBase::init(void)
{
    if(_bInitialized == true)
        return;

    Inherited::init();

    if(_bInitialized == true)
    {
        setupAspect    ();        
        setupChangeList();        
    }
}


void PThreadBase::setupAspect(void)
{

#if defined(OSG_PTHREAD_ELF_TLS)
    _uiTLSAspectId  = Inherited::_uiAspectId;
#else
    UInt32 *pUint = new UInt32;

    *pUint = Inherited::_uiAspectId;

    pthread_setspecific(_aspectKey, (void *) pUint);  
#endif
}

void PThreadBase::setupChangeList(void)
{
#if defined(OSG_PTHREAD_ELF_TLS)
    if(Inherited::_pChangeList == NULL)
    {
        _pTLSChangeList = new ChangeList;

        Inherited::setChangeList(_pTLSChangeList);
    }
    else
    {
        _pTLSChangeList = Inherited::_pChangeList;
        
        _pTLSChangeList->clearAll();
    }
    
    _pTLSChangeList->setAspect(Inherited::_uiAspectId);
#else
    ChangeList **pChangeList = new ChangeList *;

    if(Inherited::_pChangeList == NULL)
    {
        *pChangeList = new ChangeList;

        Inherited::setChangeList(*pChangeList);
    }
    else
    {
        *pChangeList = Inherited::_pChangeList;
        
        (*pChangeList)->clearAll();
    }

    (*pChangeList)->setAspect(Inherited::_uiAspectId);
    pthread_setspecific(_changeListKey, (void *) pChangeList);  
#endif
}

#endif /* OSG_USE_PTHREADS */




#if defined (OSG_USE_SPROC)

/*-------------------------------------------------------------------------*/
/*                                Get                                      */

UInt32 SprocBase::getAspect(void)
{
#ifdef OSG_FORCE_ASPECT
    if(_force_aspect != -1)
        return _force_aspect;
#endif

    return ((OSGProcessData *) PRDA->usr_prda.fill)->_uiAspectId;
}

ChangeList *SprocBase::getCurrentChangeList(void)
{
    return ((OSGProcessData *) PRDA->usr_prda.fill)->_pChangeList;
}

/*-------------------------------------------------------------------------*/
/*                                Run                                      */

// This workaround was brought to you by gcc 2.95.3

bool SprocBase::runFunction(ThreadFuncF  fThreadFunc, 
                            UInt32       uiAspectId,
                            void        *pThreadArg)
{
    if(uiAspectId >= ThreadManager::getNumAspects())
    {
        SFATAL << "OSGPTB : invalid aspect id" << std::endl;
        return false;
    }

    Inherited::setAspect(uiAspectId);

    return Inherited::runFunction(fThreadFunc, pThreadArg);
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

SprocBase::SprocBase(const Char8  *szName,
                           UInt32  uiId) :    
    Inherited(szName, uiId)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

SprocBase::~SprocBase(void)
{
}

/*-------------------------------------------------------------------------*/
/*                                Set                                      */

void SprocBase::setAspectInternal(UInt32 uiAspect)
{
    ((OSGProcessData *) PRDA->usr_prda.fill)->_uiAspectId = uiAspect;
}

/*-------------------------------------------------------------------------*/
/*                               Setup                                     */

void SprocBase::init(void)
{
    if(_bInitialized == true)
        return; 

   Inherited::init();

    if(_bInitialized == true)
    {
        setAspectInternal      (this->_uiAspectId);
        setupChangeListInternal();
    }
}

void SprocBase::setupChangeListInternal(void)
{
    if(Inherited::_pChangeList == NULL)
    {
        ((OSGProcessData *) PRDA->usr_prda.fill)->_pChangeList = 
            new ChangeList();

        Inherited::setChangeList(
            ((OSGProcessData *) PRDA->usr_prda.fill)->_pChangeList);
    }
    else
    {
        ((OSGProcessData *) PRDA->usr_prda.fill)->_pChangeList = 
            Inherited::_pChangeList;

        Inherited::_pChangeList->clearAll();
    }

    Inherited::_pChangeList->setAspect(Inherited::_uiAspectId);
}

#endif /* OSG_USE_SPROC */




#if defined (OSG_USE_WINTHREADS)

#if defined(OSG_ASPECT_USE_LOCALSTORAGE)
UInt32 WinThreadBase::_aspectKey     = 0;
UInt32 WinThreadBase::_changeListKey = 0;
#endif

#if defined(OSG_ASPECT_USE_DECLSPEC)
static __declspec (thread) UInt32      _uiAspectLocal    = 0;
static __declspec (thread) ChangeList *_pChangeListLocal = NULL;
#endif

/*-------------------------------------------------------------------------*/
/*                                Get                                      */

UInt32 WinThreadBase::getAspect(void)
{
#ifdef OSG_FORCE_ASPECT
    if(_force_aspect != -1)
        return _force_aspect;
#endif

#ifdef OSG_ASPECT_USE_LOCALSTORAGE
    UInt32 *pUint;

    pUint = (UInt32 *) TlsGetValue(_aspectKey);

    return *pUint;
#endif
#ifdef OSG_ASPECT_USE_DECLSPEC
    return _uiAspectLocal;
#endif
}

ChangeList *WinThreadBase::getCurrentChangeList(void)
{
#ifdef OSG_ASPECT_USE_LOCALSTORAGE
    ChangeList **pCList;

    pCList = (ChangeList **) TlsGetValue(_changeListKey);

    return *pCList;
#endif
#ifdef OSG_ASPECT_USE_DECLSPEC
    return _pChangeListLocal;
#endif
}

/*-------------------------------------------------------------------------*/
/*                                Run                                      */

// This workaround was brought to you by gcc 2.95.3

bool WinThreadBase::runFunction(ThreadFuncF  fThreadFunc, 
                                UInt32       uiAspectId,
                                void        *pThreadArg)
{
    if(uiAspectId >= ThreadManager::getNumAspects())
    {
        SFATAL << "OSGPTB : invalid aspect id" << std::endl;
        return false;
    }

    Inherited::setAspect(uiAspectId);

    return Inherited::runFunction(fThreadFunc, pThreadArg);
}

/*-------------------------------------------------------------------------*/
/*                               Free                                      */

#if defined (OSG_ASPECT_USE_LOCALSTORAGE)
void WinThreadBase::freeAspect(void)
{
    UInt32 *pUint;

    pUint = (UInt32 *) TlsGetValue(_aspectKey);

    delete pUint;
}

void WinThreadBase::freeChangeList(void)
{
    ChangeList **pCList;

    pCList = (ChangeList **) TlsGetValue(_changeListKey);

    delete pCList;
}
#endif

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

WinThreadBase::WinThreadBase(const Char8  *szName,
                                   UInt32  uiId) :
    Inherited(szName, uiId)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

WinThreadBase::~WinThreadBase(void)
{
}

/*-------------------------------------------------------------------------*/
/*                               Setup                                     */

void WinThreadBase::init(void)
{
    if(_bInitialized == true)
        return;

    Inherited::init();

    if(_bInitialized == true)
    {
        setupAspect    ();
        setupChangeList();        
    }
}

void WinThreadBase::setupAspect(void)
{
#ifdef OSG_ASPECT_USE_LOCALSTORAGE
    UInt32 *pUint = new UInt32;

    *pUint = Inherited::_uiAspectId;

    TlsSetValue(_aspectKey, pUint);
#endif

#ifdef OSG_ASPECT_USE_DECLSPEC
    _uiAspectLocal = Inherited::_uiAspectId;
#endif
}

void WinThreadBase::setupChangeList(void)
{
#if defined (OSG_ASPECT_USE_LOCALSTORAGE)
    ChangeList **pChangeList = new ChangeList *;

   if(Inherited::_pChangeList == NULL)
    {
        *pChangeList = new ChangeList;

        Inherited::setChangeList(*pChangeList);
    }
    else
    {
        *pChangeList = Inherited::_pChangeList;
        
        (*pChangeList)->clearAll();
    }

    (*pChangeList)->setAspect(Inherited::_uiAspectId);
    TlsSetValue(_changeListKey, pChangeList);
#endif

#if defined (OSG_ASPECT_USE_DECLSPEC)
    if(Inherited::_pChangeList == NULL)
    {
        _pChangeListLocal = new ChangeList;
        Inherited::setChangeList(_pChangeListLocal);
    }
    else
    {
        _pChangeListLocal = Inherited::_pChangeList;

        _pChangeListLocal->clearAll();
    }

    _pChangeListLocal->setAspect(Inherited::_uiAspectId);
#endif
}

#endif /* OSG_USE_WINTHREADS */




MPThreadType Thread::_type("OSGThread", 
                           "OSGBaseThread", 
                           reinterpret_cast<CreateThreadF>(Thread::create),
                           static_cast<InitThreadingF>(Thread::initThreading));

/*-------------------------------------------------------------------------*/
/*                                Get                                      */

ThreadBase *Thread::getCurrent(void)
{
    return static_cast<ThreadBase *>(Inherited::getCurrent());
}

Thread *Thread::get(const Char8 *szName)
{
    BaseThread *pThread = ThreadManager::the()->getThread(szName, "OSGThread");

    return dynamic_cast<Thread *>(pThread);
}

Thread *Thread::find(const Char8 *szName)
{
    BaseThread *pThread = ThreadManager::the()->findThread(szName);

    return dynamic_cast<Thread *>(pThread);
}

void Thread::run(UInt32 uiAspectId)
{
    Inherited::runFunction(runWorkProc, uiAspectId, this);
}

/*-------------------------------------------------------------------------*/
/*                               Setup                                     */

Thread *Thread::create(const Char8 *szName, UInt32 uiId)
{
    return new Thread(szName, uiId);
}

void Thread::initThreading(void)
{
    FINFO(("Thread::initThreading\n"))

#if defined(OSG_USE_PTHREADS) && !defined(OSG_PTHREAD_ELF_TLS)
    int rc; 

    rc = pthread_key_create(&(Thread::_aspectKey), 
                              Thread::freeAspect);

    FFASSERT((rc == 0), 1, ("Failed to create pthread aspect key\n");)

    rc = pthread_key_create(&(Thread::_changeListKey), 
                              Thread::freeChangeList);

    FFASSERT((rc == 0), 1, ("Failed to create pthread changelist key\n");)
#endif

#if defined(OSG_USE_WINTHREADS) && defined(OSG_ASPECT_USE_LOCALSTORAGE)       
    Thread::_aspectKey     = TlsAlloc();

    FFASSERT((Thread::_aspectKey != 0xFFFFFFFF), 1, 
             ("Failed to alloc aspect key local storage\n");)

    Thread::_changeListKey = TlsAlloc();

    FFASSERT((Thread::_changeListKey != 0xFFFFFFFF), 1, 
             ("Failed to alloc changelist key local storage\n");)
#endif

    ThreadManager::setAppThreadType("OSGThread");
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

Thread::Thread(const Char8 *szName, UInt32 uiId) :
    Inherited(szName, uiId)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

Thread::~Thread(void)
{
}




MPThreadType ExternalThread::_type(
    "OSGExternalThread", 
    "OSGMPBase", 
    reinterpret_cast<CreateThreadF>(ExternalThread::create),
    NULL);

/*-------------------------------------------------------------------------*/
/*                                Get                                      */

ThreadBase *ExternalThread::getCurrent(void)
{
    return static_cast<ThreadBase *>(Inherited::getCurrent());
}

ExternalThread *ExternalThread::get(const Char8 *szName)
{
    BaseThread *pThread = ThreadManager::the()->getThread(szName, 
                                                          "OSGExternalThread");

    return dynamic_cast<ExternalThread *>(pThread);
}

ExternalThread *ExternalThread::find(const Char8 *szName)
{
    BaseThread *pThread = ThreadManager::the()->findThread(szName);

    return dynamic_cast<ExternalThread *>(pThread);
}

void ExternalThread::initialize(UInt32 uiAspectId)
{
    if(_bInitialized == true)
        return;

    Inherited::setAspect(uiAspectId);
    
    this->init();    
}

/*-------------------------------------------------------------------------*/
/*                               Setup                                     */

ExternalThread *ExternalThread::create(const Char8 *szName, 
                                             UInt32 uiId)
{
    return new ExternalThread(szName, uiId);
}

#if 0
void ExternalThread::initThreading(void)
{
    FINFO(("Thread::initThreading\n"))

#ifdef OSG_ASPECT_USE_PTHREADKEY
    int rc; 

    rc = pthread_key_create(&(Thread::_aspectKey), 
                              Thread::freeAspect);

    FFASSERT((rc == 0), 1, ("Failed to create pthread aspect key\n");)

    rc = pthread_key_create(&(Thread::_changeListKey), 
                              Thread::freeChangeList);

    FFASSERT((rc == 0), 1, ("Failed to create pthread changelist key\n");)
#endif

#ifdef OSG_ASPECT_USE_PTHREADSELF

    Thread::_vAspects    .resize(16);
    Thread::_vChangelists.resize(16);

    for(UInt32 i = 0; i < 16; i++)
    {
        Thread::_vAspects[i]     = 0;
        Thread::_vChangelists[i] = NULL;
    }
#endif

#if defined (OSG_ASPECT_USE_LOCALSTORAGE)       
    Thread::_aspectKey     = TlsAlloc();

    FFASSERT((Thread::_aspectKey != 0xFFFFFFFF), 1, 
             ("Failed to alloc aspect key local storage\n");)

    Thread::_changeListKey = TlsAlloc();

    FFASSERT((Thread::_changeListKey != 0xFFFFFFFF), 1, 
             ("Failed to alloc changelist key local storage\n");)
#endif

    ThreadManager::setAppThreadType("OSGThread");
}
#endif

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

ExternalThread::ExternalThread(const Char8 *szName, UInt32 uiId) :
    Inherited(szName, uiId)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

ExternalThread::~ExternalThread(void)
{
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
    static Char8 cvsid_hpp[] = OSGTHREAD_HEADER_CVSID;
}
