/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     * 
 *                                                                           * 
 *                                                                           * 
 *           Copyright (C) 2000,2001,2002 by the OpenSG Forum                * 
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

#ifndef _OSGITERATORBASE_H_
#define _OSGITERATORBASE_H_
#ifdef __sgi
#pragma once
#endif

#include "OSGNewActionBase.h"
#include "OSGActiveNodeList.h"
#include <OSGNodePtr.h>

OSG_BEGIN_NAMESPACE

class NewAction;
class ActorStateChunk;

/*! \brief IteratorBase class.
 */

class OSG_SYSTEMLIB_DLLMAPPING IteratorBase
{
    /*==========================  PUBLIC  =================================*/
  public:
    typedef NewActionBase::RootList        RootList;
    typedef NewActionBase::RootListIt      RootListIt;
    typedef NewActionBase::RootListConstIt RootListConstIt;

    typedef NewActionBase::ResultE         ResultE;
    typedef NewActionBase::PriorityType    PriorityType;
    
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~IteratorBase(void) = 0;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Traverse                                 */
    /*! \{                                                                 */

    virtual ResultE  start          (      void               ) = 0;
    virtual void     stop           (      void               ) = 0;
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Receive Signals                            */
    /*! \{                                                                 */

    virtual void receiveActorAdd   (ActorBase *pActor,   UInt32 uiActorId) = 0;
    virtual void receiveActorSub   (ActorBase *pActor,   UInt32 uiActorId) = 0;
    virtual void receiveActorsClear(void                                 ) = 0;

    virtual void receiveStateRead  (UInt32     uiActorId                 ) = 0;
    virtual void receiveStateWrite (UInt32     uiActorId                 ) = 0;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                     Roots                                    */
    /*! \{                                                                 */

    void            addRoot         (      NodePtr      pRoot   );
    void            addRoot         (const RootList    &roots   );

    void            clearRoots      (      void                 );

    const RootList &getRoots        (      void                 ) const;
          RootList &getRoots        (      void                 );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Active Nodes                               */
    /*! \{                                                                 */

    void                   resetNodeList (void);
    void                   setUseNodeList(void);
    bool                   getUseNodeList(void) const;

    ActiveNodeListIt       beginNodes    (void);
    ActiveNodeListIt       endNodes      (void);

    ActiveNodeListConstIt  beginNodes    (void) const;
    ActiveNodeListConstIt  endNodes      (void) const;

          ActiveNodeList  &getNodeList   (void);
    const ActiveNodeList  &getNodeList   (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Access                                 */
    /*! \{                                                                 */


          NewAction *getAction      (      void                 );
          
          NodePtr    getCurrentNode (      void                 );
    
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
protected:
    typedef NewActionBase::ActorStore        ActorStore;
    typedef NewActionBase::ActorStoreIt      ActorStoreIt;
    typedef NewActionBase::ActorStoreConstIt ActorStoreConstIt;
    
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructor                                */
    /*! \{                                                                 */
    
    IteratorBase(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                  Action Interface                            */
    /*! \{                                                                 */

    ResultE           callEnter  (NodePtr pNode);
    ResultE           callLeave  (NodePtr pNode);
    
    ResultE           startActors(void         );
    ResultE           stopActors (void         );

    void              cloneState (ActorStateChunk *pChunk);
    void              getState   (ActorStateChunk *pChunk);
    void              setState   (ActorStateChunk *pChunk);

    ActorStoreIt      beginActors(void);
    ActorStoreIt      endActors  (void);
    
    ActorStoreConstIt beginActors(void) const;
    ActorStoreConstIt endActors  (void) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Derived Classes Access                       */
    /*! \{                                                                 */

    void             setCurrentNode          (NodePtr pCurrNode);

    void             buildActorLists         (void             );
    
    ActorStoreIt     beginEnterList          (void             );
    ActorStoreIt     endEnterList            (void             );

    ActorStoreIt     beginLeaveList          (void             );
    ActorStoreIt     endLeaveList            (void             );

    UInt32           getNumActors            (void             );
    UInt32           getNumLeaveActors       (void             );
    UInt32           getNumOrderedStateActors(void             );

    bool             hasLeaveActors          (void             );
    bool             hasOrderedStateActors   (void             );

    /*! \}                                                                 */
    /*==========================  PRIVATE  ================================*/
private:
    friend class OSG::NewAction;

    void setAction(NewAction *pAction);

    NewAction             *_pAction;

    ActiveNodeList         _activeNodes;
    bool                   _bUseNodeList;

    RootList               _roots;

    NodePtr                _pCurrNode;

    ActorStore             _enterList;
    ActorStore             _leaveList;

    UInt32                 _uiNumActors;
    UInt32                 _uiNumLeaveActors;
    UInt32                 _uiNumOrderedStateActors;
};

OSG_END_NAMESPACE

#include "OSGIteratorBase.inl"

#define OSGITERATORBASE_HEADER_CVSID "@(#)$Id: OSGIteratorBase.h,v 1.2 2003/10/10 14:27:44 neumannc Exp $"

#endif /* _OSGITERATORBASE_H_ */