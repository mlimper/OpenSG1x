/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *           Copyright (C) 2000-2002,2002 by the OpenSG Forum                *
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

#ifndef _OSGCNODEPTRIMPL_INL_
#define _OSGCNODEPTRIMPL_INL_

#ifdef OSG_DOC_FILES_IN_MODULE
/*! \file OSGCNodePtrImpl.inl
    \ingroup GrpSystemFieldContainer
 */
#endif

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

inline
CNodePtr::CNodePtr(void) :
    Inherited()
{
}

inline
CNodePtr::CNodePtr(const CNodePtr &source) :
    Inherited(source)
{
}

inline
CNodePtr::CNodePtr(const NullFieldContainerPtr &source) :
    Inherited(source)
{
}

inline
CNodePtr::CNodePtr(const NodePtr &source) :
    Inherited(source)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

inline
CNodePtr::~CNodePtr(void)
{
}

/*-------------------------------------------------------------------------*/
/*                                Node                                     */

inline
Node *CNodePtr::getNode(void)
{
    return reinterpret_cast<Node *>(getElemP(Thread::getAspect()));
}

inline
Node *CNodePtr::getNode(void) const
{
    return reinterpret_cast<Node *>(getElemP(Thread::getAspect()));
}

/*-------------------------------------------------------------------------*/
/*                            Container Access                             */

inline
NodeCore *CNodePtr::operator->(void)
{
    return (reinterpret_cast<Node *>(
                getElemP(Thread::getAspect())))->getCore().getCPtr();
}

inline
NodeCore *CNodePtr::operator->(void) const
{
    return (reinterpret_cast<Node *>(
                getElemP(Thread::getAspect())))->getCore().getCPtr();
}

inline
NodeCore &CNodePtr::operator *(void)
{
    return *((reinterpret_cast<Node *>(
                  getElemP(Thread::getAspect())))->getCore().getCPtr());
}

inline
NodeCore &CNodePtr::operator *(void) const
{
    return *((reinterpret_cast<Node *>(
                  getElemP(Thread::getAspect())))->getCore().getCPtr());
}

inline
NodeCore *CNodePtr::getCPtr(void)
{
    return (reinterpret_cast<Node *>(
                getElemP(Thread::getAspect())))->getCore().getCPtr();
}

inline
NodeCore *CNodePtr::getCPtr(void) const
{
    return (reinterpret_cast<Node *>(
                getElemP(Thread::getAspect())))->getCore().getCPtr();
}

/*-------------------------------------------------------------------------*/
/*                             Assignment                                  */

inline
void CNodePtr::operator = (const NodePtr &source)
{
    // copy parts inherited from parent
    *(static_cast<Inherited *>(this)) = source;
}

inline
void CNodePtr::operator = (const CNodePtr &source)
{
    // copy parts inherited from parent
    *(static_cast<Inherited *>(this)) = source;
}

inline
void CNodePtr::operator = (const NullFieldContainerPtr &source)
{
    // copy parts inherited from parent
    *(static_cast<Inherited *>(this)) = source;
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

inline
CNodePtr::CNodePtr(const Node &source) :
    Inherited(source)
{
}

inline
CNodePtr::CNodePtr(const Node *source) :
    Inherited(source)
{
}

inline
CNodePtr::CNodePtr(const Node   *source,
                   const UInt16  uiSize,
                   const UInt16  uiParentPos) :
    Inherited(source, uiSize, uiParentPos)
{
}

OSG_END_NAMESPACE

#define OSGNODEPTR_INLINE_CVSID "@(#)$Id: $"

#endif /* _OSGCNODEPTRIMPL_INL_ */
