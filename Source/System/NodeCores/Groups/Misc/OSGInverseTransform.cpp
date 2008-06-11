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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>
#include <OSGGL.h>

#include "OSGInverseTransform.h"
#include "OSGNodePtr.h"
#include "OSGDrawAction.h"
#include "OSGIntersectAction.h"
#include "OSGRenderAction.h"
#include "OSGNode.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::InverseTransform

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void InverseTransform::initMethod (void)
{
    DrawAction::registerEnterDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            InverseTransformPtr    ,
            CNodePtr        ,
            Action         *>(&InverseTransform::drawEnter));

    DrawAction::registerLeaveDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            InverseTransformPtr    ,
            CNodePtr        ,
            Action         *>(&InverseTransform::drawLeave));


    IntersectAction::registerEnterDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            InverseTransformPtr    ,
            CNodePtr        ,
            Action         *>(&InverseTransform::intersectEnter));

    IntersectAction::registerLeaveDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            InverseTransformPtr    ,
            CNodePtr        ,
            Action         *>(&InverseTransform::intersectLeave));


    RenderAction::registerEnterDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            InverseTransformPtr    ,
            CNodePtr        ,
            Action         *>(&InverseTransform::renderEnter));

    RenderAction::registerLeaveDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            InverseTransformPtr    ,
            CNodePtr        ,
            Action         *>(&InverseTransform::renderLeave));
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

InverseTransform::InverseTransform(void) :
    Inherited(),
    _invWorld()
{
}

InverseTransform::InverseTransform(const InverseTransform &source) :
    Inherited(source),
    _invWorld(source._invWorld)
{
}

InverseTransform::~InverseTransform(void)
{
}

/*----------------------------- class specific ----------------------------*/

void InverseTransform::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void InverseTransform::dump(      UInt32    uiIndent,
                            const BitVector bvFlags ) const
{
    Inherited::dump(uiIndent, bvFlags);
}

/*------------------------- volume update -------------------------------*/

void InverseTransform::adjustVolume( Volume & volume )
{
    volume.transform(_invWorld);
}

void InverseTransform::accumulateMatrix(Matrix &result)
{
    result.mult(_invWorld);
}

/*------------------------- calc matrix ---------------------------------*/

void InverseTransform::calcMatrix(      DrawActionBase * OSG_CHECK_ARG(pAction),
                                  const Matrix         &mToWorld,
                                        Matrix         &mResult)
{
    mResult.invertFrom(mToWorld);

    _invWorld = mResult;    // remember dynamically set matrix field
}

void InverseTransform::initMatrix(const Matrix &mToWorld)
{
    _invWorld.invertFrom(mToWorld);
}

/*-------------------------------------------------------------------------*/
/*                               Draw                                      */

Action::ResultE InverseTransform::drawEnter(Action *action)
{
    DrawAction *da = dynamic_cast<DrawAction *>(action);
    Matrix mMat;

    calcMatrix(da, da->getActNode()->getToWorld(), mMat);

    // should use the chunk, but it's not updated yet
    glPushMatrix ();
    glMultMatrixf(mMat.getValues());

    return Action::Continue;
}

Action::ResultE InverseTransform::drawLeave(Action * OSG_CHECK_ARG(action))
{
    glPopMatrix();

    return Action::Continue;
}

/*-------------------------------------------------------------------------*/
/*                            Intersect                                    */

Action::ResultE InverseTransform::intersectEnter(Action *action)
{
    IntersectAction *ia = dynamic_cast<IntersectAction *>(action);
    Matrix           m(_invWorld);

    m.invert();

    Pnt3f pos;
    Vec3f dir;

#ifndef OSG_2_PREP
    m.multFullMatrixPnt(ia->getLine().getPosition (), pos);
    m.multMatrixVec    (ia->getLine().getDirection(), dir);
#else
    m.multFull(ia->getLine().getPosition (), pos);
    m.mult    (ia->getLine().getDirection(), dir);
#endif

    ia->setLine(Line(pos, dir), ia->getMaxDist());
    ia->scale(dir.length());

    return Action::Continue;
}

Action::ResultE InverseTransform::intersectLeave(Action *action)
{
    IntersectAction *ia = dynamic_cast<IntersectAction *>(action);
    Matrix           m(_invWorld);

    Pnt3f pos;
    Vec3f dir;

#ifndef OSG_2_PREP
    m.multFullMatrixPnt(ia->getLine().getPosition (), pos);
    m.multMatrixVec    (ia->getLine().getDirection(), dir);
#else
    m.multFull(ia->getLine().getPosition (), pos);
    m.mult    (ia->getLine().getDirection(), dir);
#endif

    ia->setLine(Line(pos, dir), ia->getMaxDist());
    ia->scale(dir.length());

    return Action::Continue;
}

/*-------------------------------------------------------------------------*/
/*                                Render                                   */

Action::ResultE InverseTransform::renderEnter(Action *action)
{
    RenderAction *pAction = dynamic_cast<RenderAction *>(action);
    Matrix mMat;    // will be set to World^-1

    calcMatrix(pAction, pAction->top_matrix(), mMat);

    pAction->push_matrix(mMat);

    return Action::Continue;
}

Action::ResultE InverseTransform::renderLeave(Action *action)
{
    RenderAction *pAction = dynamic_cast<RenderAction *>(action);

    pAction->pop_matrix();

    return Action::Continue;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGInverseTransform.cpp,v 1.2 2004/10/13 14:51:34 a-m-z Exp $";
    static Char8 cvsid_hpp       [] = OSGINVERSETRANSFORMBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGINVERSETRANSFORMBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGINVERSETRANSFORMFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif
