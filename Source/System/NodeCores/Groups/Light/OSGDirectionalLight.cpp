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

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include <OSGGL.h>

#include "OSGNodePtr.h"
#include "OSGDirectionalLight.h"
#include <OSGDrawAction.h>
#include <OSGRenderAction.h>

OSG_USING_NAMESPACE

/*! \class osg::DirectionalLight
 *  DirectionalLight is an infinitely distant lightsource. Its only
 *  attribute is the light's direction.
 */

/*----------------------------- class variables ---------------------------*/

StatElemDesc<StatIntElem>  DirectionalLight::statNDirectionalLights(
"NDirectionalLights", "number of directional light sources");


/*-------------------------------------------------------------------------*/
/*                                Set                                      */

void DirectionalLight::setDirection(Real32 rX, Real32 rY, Real32 rZ)
{
    _sfDirection.getValue().setValues(rX, rY, rZ);
}

/*-------------------------------------------------------------------------*/
/*                             Changed                                     */

void DirectionalLight::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------------------------------------------------------*/
/*                             Chunk                                       */

void DirectionalLight::makeChunk(void)
{
    Inherited::makeChunk();

    Vec4f dir(_sfDirection.getValue());

    dir[3] = 0;
   
    _pChunk->setPosition(dir);
}

/*-------------------------------------------------------------------------*/
/*                                Dump                                     */

void DirectionalLight::dump(      UInt32    uiIndent, 
                            const BitVector bvFlags) const
{
   Inherited::dump(uiIndent, bvFlags);
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

DirectionalLight::DirectionalLight(void) :
    Inherited()
{
}

DirectionalLight::DirectionalLight(const DirectionalLight &source) :
    Inherited(source)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

DirectionalLight::~DirectionalLight(void)
{
}

/*-------------------------------------------------------------------------*/
/*                               Drawing                                   */

Action::ResultE DirectionalLight::drawEnter(Action *action)
{   
    if(getOn() == false)
        return Action::Continue;

    DrawAction *da = dynamic_cast<DrawAction *>(action);

    GLenum light = GL_LIGHT0 + da->getLightCount();
    
    Light::drawEnter(action);

    Vec4f dir(_sfDirection.getValue());

    dir[3] = 0;
    
    glLightfv(light, GL_POSITION   , dir.getValues());
    glLightf (light, GL_SPOT_CUTOFF, 180.f          );

    glPopMatrix();

    da->getStatistics()->getElem(DirectionalLight::statNDirectionalLights)->inc();

    return Action::Continue;
}
    
Action::ResultE DirectionalLight::drawLeave(Action *action)
{
    if(getOn() == false)
        return Action::Continue;

    return Light::drawLeave(action);
}

/*-------------------------------------------------------------------------*/
/*                             Rendering                                   */

Action::ResultE DirectionalLight::renderEnter(Action *action)
{
    if(getOn() == false)
        return Action::Continue;

    DrawActionBase *da    = dynamic_cast<DrawActionBase *>(action);
    da->getStatistics()->getElem(DirectionalLight::statNDirectionalLights)->inc();
    
    return Light::renderEnter(action);
}

Action::ResultE DirectionalLight::renderLeave(Action *action)
{
    if(getOn() == false)
        return Action::Continue;

    return Light::renderLeave(action);
}

/*-------------------------------------------------------------------------*/
/*                               Init                                      */

void DirectionalLight::initMethod (void)
{
    DrawAction::registerEnterDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            DirectionalLightPtr  , 
            CNodePtr             ,  
            Action              *>(&DirectionalLight::drawEnter));

    DrawAction::registerLeaveDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            DirectionalLightPtr  , 
            CNodePtr             ,  
            Action              *>(&DirectionalLight::drawLeave));

    RenderAction::registerEnterDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            DirectionalLightPtr  , 
            CNodePtr             ,  
            Action              *>(&DirectionalLight::renderEnter));

    RenderAction::registerLeaveDefault(
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            OSG::Action::ResultE,
            DirectionalLightPtr  , 
            CNodePtr             ,  
            Action              *>(&DirectionalLight::renderLeave));
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
    static Char8 cvsid_hpp[] = OSGDIRECTIONALLIGHT_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGDIRECTIONALLIGHT_INLINE_CVSID;
}
