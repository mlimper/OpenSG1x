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

#include <OSGConfig.h>

#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGCamera.h>
#include <OSGRenderAction.h>

//just for debug
#include <iostream>
//just for debug

#include "OSGDistanceLOD.h"

OSG_USING_NAMESPACE

/*! \class osg::DistanceLOD

  This Node manages the different levels of detail available for a Geometry
  and decides which one should be rendered, according to the distance from the
  current camera. The details of the selection process are taken from VRML97
  standard. 

*/

/*-------------------------------------------------------------------------*/
/*                               Sync                                      */

void DistanceLOD::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*-------------------------------------------------------------------------*/
/*                               Dump                                      */

void DistanceLOD::dump(      UInt32    OSG_CHECK_ARG(uiIndent), 
                       const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    SLOG << "Dump DistanceLOD NI" << endl;
}

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

DistanceLOD::DistanceLOD(void) :
    Inherited()
{
}

DistanceLOD::DistanceLOD(const DistanceLOD &source) :
    Inherited(source)
{
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

DistanceLOD::~DistanceLOD(void)
{
}

/*-------------------------------------------------------------------------*/
/*                                Draw                                     */

Action::ResultE DistanceLOD::draw(Action *action)
{
    DrawActionBase *da        = dynamic_cast<DrawActionBase *>(action);

    UInt32          numLevels = action->getNNodes();
    UInt32          numRanges = getMFRange()->size();

    UInt32          limit     = osgMin(numLevels, numRanges); 
    
    Int32           index     = -1;

    Pnt3f            eyepos(0.f, 0.f, 0.f);
    Pnt3f            objpos;

    da->getCamera()->getBeacon()->getToWorld().mult(eyepos);

    da->getActNode()            ->getToWorld().mult(getCenter(), 
                                                    objpos);
        
    Real32 dist = osgsqrt((eyepos[0] - objpos[0])*(eyepos[0] - objpos[0]) +
                          (eyepos[1] - objpos[1])*(eyepos[1] - objpos[1]) +
                          (eyepos[2] - objpos[2])*(eyepos[2] - objpos[2]));
    
    da->useNodeList();
    
    if(numRanges != 0)
    {
        if(dist < (*(getMFRange()))[0])
        {
            index = 0;
        } 
        else if(dist >= (*(getMFRange()))[numRanges-1])
        {
            index =  limit-1;
        }
        else
        {
            UInt32 i = 1;

            while( (i < numRanges) && 
                  !( ((*(getMFRange()))[i-1] <= dist) && 
                     (dist < (*(getMFRange()))[i]   )   ) )
            {
                i++;
            }
            
            index = osgMin(i, limit-1);
        } 
        
        if(da->isVisible(action->getNode(index).getCPtr()))
        {
            da->addNode(action->getNode(index));
        }
    }

    return Action::Continue;
}

/*-------------------------------------------------------------------------*/
/*                               Init                                      */

void DistanceLOD::initMethod (void)
{
    DrawAction::registerEnterDefault( 
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            DistanceLODPtr  ,
            CNodePtr        ,
            Action         *>(&DistanceLOD::draw));

    RenderAction::registerEnterDefault(
        getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<
            Action::ResultE,
            DistanceLODPtr  ,
            CNodePtr        ,
            Action         *>(&DistanceLOD::draw));
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
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGDistanceLOD.cpp,v 1.22 2002/06/30 05:04:22 vossg Exp $";
    static Char8 cvsid_hpp[] = OSGDISTANCELOD_HEADER_CVSID;
    static Char8 cvsid_inl[] = OSGDISTANCELOD_INLINE_CVSID;
}

    

