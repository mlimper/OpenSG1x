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

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include <OSGGL.h>

#include <OSGDrawAction.h>
#include <OSGRenderAction.h>

#include "OSGSpotLight.h"

OSG_USING_NAMESPACE

/*! \class osg::SpotLight
    SpotLight is a located lightsource. The position of the light source
    (in the beacon's coordinate system) is defined by the \c position 
    attribute, its direction by the \c direction attribute. The spot has an 
    exponential fallof, controlled by the \c spotExponent attribute and a 
    maximum opening angle, defined by the \c spotCutOff attribute.
    The influence of the light diminishes with distance, controlled
    by the \c constantAttenuation, \c linearAttenuation and \c
    quadraticAttenuation attributes. 
*/


/*----------------------------- class variables ---------------------------*/

StatElemDesc<StatIntElem>  SpotLight::statNSpotLights(
"NSpotLights", "number of spot light sources");

/*-------------------------------------------------------------------------*/
/*                             Changed                                     */

void SpotLight::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------------------------------------------------------*/
/*                             Chunk                                       */

void SpotLight::makeChunk(void)
{
    Inherited::makeChunk();

    Vec4f pos(_sfPosition.getValue ());

    pos[3] = 1;

    _pChunk->setPosition (pos                     );
    _pChunk->setDirection(getDirection         () );

    _pChunk->setExponent (getSpotExponent      () );
    _pChunk->setCutoff   (rad2deg(getSpotCutOff()));
}

/*-------------------------------------------------------------------------*/
/*                                Dump                                     */

void SpotLight::dump(      UInt32    uiIndent, 
                     const BitVector bvFlags) const
{
   Inherited::dump(uiIndent, bvFlags);
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

SpotLight::SpotLight(void) :
    Inherited()
{
}

SpotLight::SpotLight(const SpotLight &source) :
    Inherited(source)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

SpotLight::~SpotLight(void)
{
}

/*-------------------------------------------------------------------------*/
/*                               Drawing                                   */

Action::ResultE SpotLight::drawEnter(Action *action)
{   
    if(getOn() == false)
        return Action::Continue;

    DrawAction *da    = dynamic_cast<DrawAction *>(action);
    GLenum      light = GL_LIGHT0 + da->getLightCount();
    
    Light::drawEnter(action);

    Vec4f  dir(_sfDirection.getValue());
    Vec4f  pos(_sfPosition .getValue());

    pos[3] = 1;
    dir[3] = 0;

    Real32 deg = rad2deg(_sfSpotCutOff.getValue());

    if(deg > 90.f) 
        deg = 180.f;

    glLightfv(light, GL_POSITION      ,  pos.getValues()          );
    glLightfv(light, GL_SPOT_DIRECTION,  dir.getValues()          );

    glLightf (light, GL_SPOT_CUTOFF   ,  deg                      );
    glLightf (light, GL_SPOT_EXPONENT , _sfSpotExponent.getValue());

    glPopMatrix();

    da->getStatistics()->getElem(SpotLight::statNSpotLights)->inc();

    return Action::Continue;
}
    
Action::ResultE SpotLight::drawLeave(Action *action)
{
    if(getOn() == false)
        return Action::Continue;

    return Light::drawLeave(action);
}

/*-------------------------------------------------------------------------*/
/*                             Rendering                                   */

Action::ResultE SpotLight::renderEnter(Action *action)
{
    if(getOn() == false)
        return Action::Continue;

    DrawActionBase *da    = dynamic_cast<DrawActionBase *>(action);
    da->getStatistics()->getElem(SpotLight::statNSpotLights)->inc();

    return PointLight::renderEnter(action);
}

Action::ResultE SpotLight::renderLeave(Action *action)
{
    if(getOn() == false)
        return Action::Continue;

    return PointLightBase::renderLeave(action);
}

/*-------------------------------------------------------------------------*/
/*                               Init                                      */

void SpotLight::initMethod (void)
{
    DrawAction::registerEnterDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            SpotLightPtr    , 
            CNodePtr        ,  
            Action         *>(&SpotLight::drawEnter));

    DrawAction::registerLeaveDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            SpotLightPtr    , 
            CNodePtr        ,  
            Action         *>(&SpotLight::drawLeave));

    RenderAction::registerEnterDefault(
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            SpotLightPtr    , 
            CNodePtr        ,  
            Action         *>(&SpotLight::renderEnter));

    RenderAction::registerLeaveDefault( 
        getClassType(), 
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            SpotLightPtr    , 
            CNodePtr        ,  
            Action         *>(&SpotLight::renderLeave));
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
    static Char8 cvsid_hpp[] = OSGSPOTLIGHT_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGSPOTLIGHT_INLINE_CVSID;
}
