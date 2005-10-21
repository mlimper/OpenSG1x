/*---------------------------------------------------------------------------*\
 *                                                                     *
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

#include "OSGPhysicsHandler.h"

#include <OSGMatrix.h>
#include <OSGQuaternion.h>
#include <OSGVector.h>
#include <OSGBaseFunctions.h>
#include <OSGAction.h>
#include <OSGTransform.h>
#include <OSGGroup.h>
//////////////////////////////////////////////////////////////////////////
//! collision handling
//////////////////////////////////////////////////////////////////////////
#include <OSGPhysics.h>


OSG_USING_NAMESPACE
/************************************************************************/
/* init ode stuff!call this once when you have loaded a ode scene from
    file                                                                */
/************************************************************************/
Action::ResultE initOde(NodePtr& node)
{   
    AttachmentPtr a;
    a = node->findAttachment(PhysicsWorld::getClassType());
    if(a!=NullFC)
    {
        PhysicsWorldPtr world = PhysicsWorldPtr::dcast(a);
        world->initWorld();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsHashSpace::getClassType());
    if(a!=NullFC)
    {
        PhysicsHashSpacePtr space = PhysicsHashSpacePtr::dcast(a);
        space->initHashSpace();

        return Action::Continue;
    }

    a = node->findAttachment(PhysicsBoxGeom::getClassType());
    if(a!=NullFC)
    {
        PhysicsBoxGeomPtr box = PhysicsBoxGeomPtr::dcast(a);
        box->initBoxGeom();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsSphereGeom::getClassType());
    if(a!=NullFC)
    {
        PhysicsSphereGeomPtr sphere = PhysicsSphereGeomPtr::dcast(a);
        sphere->initSphereGeom();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsPlaneGeom::getClassType());
    if(a!=NullFC)
    {
        PhysicsPlaneGeomPtr sphere = PhysicsPlaneGeomPtr::dcast(a);
        sphere->initPlaneGeom();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsCCylinderGeom::getClassType());
    if(a!=NullFC)
    {
        PhysicsCCylinderGeomPtr ccylinder = PhysicsCCylinderGeomPtr::dcast(a);
        ccylinder->initCCylinderGeom();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsRayGeom::getClassType());
    if(a!=NullFC)
    {
        PhysicsRayGeomPtr ray = PhysicsRayGeomPtr::dcast(a);
        ray->initRayGeom();

        return Action::Continue;
    }

    a = node->findAttachment(PhysicsBody::getClassType());
    if(a!=NullFC)
    {
        PhysicsBodyPtr body = PhysicsBodyPtr::dcast(a);
        body->initBody();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsTriMeshGeom::getClassType());
    if(a!=NullFC)
    {
        PhysicsTriMeshGeomPtr tri = PhysicsTriMeshGeomPtr::dcast(a);
        tri->initTriMeshGeom();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsAMotorJoint::getClassType());
    if(a!=NullFC)
    {
        PhysicsAMotorJointPtr joint = PhysicsAMotorJointPtr::dcast(a);
        joint->initAMotorJoint();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsSliderJoint::getClassType());
    if(a!=NullFC)
    {
        PhysicsSliderJointPtr joint = PhysicsSliderJointPtr::dcast(a);
        joint->initSliderJoint();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsHinge2Joint::getClassType());
    if(a!=NullFC)
    {
        PhysicsHinge2JointPtr joint = PhysicsHinge2JointPtr::dcast(a);
        joint->initHinge2Joint();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsHingeJoint::getClassType());
    if(a!=NullFC)
    {
        PhysicsHingeJointPtr joint = PhysicsHingeJointPtr::dcast(a);
        joint->initHingeJoint();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsUniversalJoint::getClassType());
    if(a!=NullFC)
    {
        PhysicsUniversalJointPtr joint = PhysicsUniversalJointPtr::dcast(a);
        joint->initUniversalJoint();

        return Action::Continue;
    }
    a = node->findAttachment(PhysicsBallJoint::getClassType());
    if(a!=NullFC)
    {
        PhysicsBallJointPtr joint = PhysicsBallJointPtr::dcast(a);
        joint->initBallJoint();

        return Action::Continue;
    }
    return Action::Continue;

}


//////////////////////////////////////////////////////////////////////////
//! this action traverses the graph to match the OpenSG representation 
//! to ODE
//////////////////////////////////////////////////////////////////////////
Action::ResultE updateOsgOde(NodePtr& node)
{   
    //SLOG << "entering " << node << endLog;
    TransformPtr t = TransformPtr::dcast(node->getCore());
    Matrix m,r;
    if(t!=NullFC)
    {
        //SLOG << "found a TransformNode " << endLog;
        AttachmentPtr a = node->findAttachment(PhysicsBody::getClassType());
        if(a!=NullFC)
        {
            //SLOG << "found a bodyNode " << endLog;
            PhysicsBodyPtr body = PhysicsBodyPtr::dcast(a);
            // If the object is moving normally, dampen the movement a bit to simulate low-level friction
            // (the amount of dampening should depend on whether the object is contacting another or not,
            // and a contact-type-specific dampening amount, but oh well, this generally works)
            CPEdit(body, PhysicsBody::ForceFieldMask | PhysicsBody::TorqueFieldMask);
            Vec3f vel = body->getLinearVel();
            body->addForce(vel * -0.01f);
            vel = body->getAngularVel();
            body->addTorque(vel * -0.01f);

            //update the position
            m.setIdentity();
            r.setIdentity();
            Vec3f p = body->getPosition();
            Quaternion q = body->getQuaternion();
            r.setRotate(q);
            m.setTransform(p);
            m.mult(r);
            CPEdit(t, Transform::MatrixFieldMask);
            t->setMatrix(m);
            //update BB
            CPEdit(node, Node::VolumeFieldMask);
            node->updateVolume();

            if(p.z() < -10)
            {
                //subRefCP(node);
            }
        }
    }

    return Action::Continue; 
}


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::PhysicsHandler

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void PhysicsHandler::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/
/*-------------------------------------------------------------------------*\
-  public                                                                 -
\*-------------------------------------------------------------------------*/
void PhysicsHandler::onCreate(const PhysicsHandler *)
{

    /*
    PhysicsHandlerPtr tmpPtr(*this);
    tmpPtr->physContactArray = new dContact[MAX_PHYS_CONTACTS];
    tmpPtr->physColJointGroupId = dJointGroupCreate(0);
    for (Int32 index = 0; index < MAX_PHYS_CONTACTS; index++)
    {
        tmpPtr->physContactArray[index].surface.mode = dContactApprox1 | dContactBounce;
        tmpPtr->physContactArray[index].surface.mu = 0.75;
        tmpPtr->physContactArray[index].surface.bounce = 0.14;
        tmpPtr->physContactArray[index].surface.bounce_vel = 0.1;
    }
    */
}

void PhysicsHandler::onDestroy()
{
    /*
    PhysicsHandlerPtr tmpPtr(*this);
    if(tmpPtr->physContactArray)
        delete tmpPtr->physContactArray;
        */
}
/***************************************************************************\
*                              Field Get	                               *
\***************************************************************************/

/***************************************************************************\
*                              Field Set	                               *
\***************************************************************************/

/***************************************************************************\
*                              Class Specific                              *
\***************************************************************************/

void PhysicsHandler::updateWorld(NodePtr node)
{
    //update matrices
    traverse(node, 
        osgTypedFunctionFunctor1CPtrRef<Action::ResultE,
        NodePtr        >(updateOsgOde));
}

void PhysicsHandler::odeInit(NodePtr node)
{
    traverse(node, 
        osgTypedFunctionFunctor1CPtrRef<Action::ResultE,
        NodePtr        >(initOde));
}
/************************************************************************/
/* the callback function for collision handling                         */
/************************************************************************/
/*
void PhysicsHandler::physCollisionCallback(void* somedata, dGeomID o1, dGeomID o2)
{
    PhysicsHandlerPtr app = PhysicsHandlerPtr(somedata);
    //PhysicsHandlerPtr tmpPtr(*this);
    //SLOG << hashSpace->GetNumGeoms() << endLog;
    Int32 numContacts = dCollide(o1, o2, 32, 
        &app->physContactArray[0].geom, sizeof(dContact));
    SLOG << "found contacts: " << numContacts << endLog;
    for (Int32 i=0; i < numContacts; i++)
    {
        dJointID jointId = dJointCreateContact(app->getPhysicsWorld()->getWorldID(), 
            app->physColJointGroupId, 
            &app->physContactArray[i]);

        dJointAttach(jointId, dGeomGetBody(o1), dGeomGetBody(o2));
    }
}
/************************************************************************/
/* this method should be called whenever you want to update the physics */
/************************************************************************/
/*
void PhysicsHandler::doPhysicsOnNode(NodePtr rootNode)
{
    PhysicsHandlerPtr tmpPtr(*this);
    //free contact Joints
    dJointGroupEmpty(tmpPtr->physColJointGroupId);
    //collide
    tmpPtr->getSpace()->Collide(0 , physCollisionCallback);
    //step the world
    tmpPtr->getPhysicsWorld()->worldQuickStep(0.02);

    //update matrices
    traverse(rootNode, 
        osgTypedFunctionFunctor1CPtrRef<Action::ResultE,
        NodePtr        >(updateOsgOde));
}
/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

PhysicsHandler::PhysicsHandler(void) :
    Inherited()
{
}

PhysicsHandler::PhysicsHandler(const PhysicsHandler &source) :
    Inherited(source)
{
}

PhysicsHandler::~PhysicsHandler(void)
{
}

/*----------------------------- class specific ----------------------------*/

void PhysicsHandler::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void PhysicsHandler::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump PhysicsHandler NI" << std::endl;
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGPhysicsHandler.cpp,v 1.1 2005/10/21 15:44:24 a-m-z Exp $";
    static Char8 cvsid_hpp       [] = OSGPHYSICSHANDLERBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGPHYSICSHANDLERBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGPHYSICSHANDLERFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

