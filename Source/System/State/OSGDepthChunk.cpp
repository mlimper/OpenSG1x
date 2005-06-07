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

#include "OSGDrawActionBase.h"
#include "OSGWindow.h"

#include "OSGDepthChunk.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::DepthChunk
The depth chunk contains the parameters that are specific for depth control. 	

The parameters of the following functions are wrapped here: glDepthFunc
(osg::DepthChunk::_sfFunc), glDepthRange (osg::DepthChunk::_sfNear,
osg::DepthChunk::_sfFar), glEnable(GL_DEPTH_TEST)
(osg::DepthChunk::_sfEnable).
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

StateChunkClass DepthChunk::_class("Depth");

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void DepthChunk::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

DepthChunk::DepthChunk(void) :
    Inherited()
{
}

DepthChunk::DepthChunk(const DepthChunk &source) :
    Inherited(source)
{
}

DepthChunk::~DepthChunk(void)
{
}

/*------------------------- Chunk Class Access ---------------------------*/

const StateChunkClass *DepthChunk::getClass(void) const
{
    return &_class;
}

/*----------------------------- class specific ----------------------------*/

void DepthChunk::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void DepthChunk::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump DepthChunk NI" << std::endl;
}


/*------------------------------ State ------------------------------------*/

void DepthChunk::activate(DrawActionBase *, UInt32)
{
    if(_sfFunc.getValue() != GL_NONE)
    {
        glDepthFunc(_sfFunc.getValue());
    }
    
    if(getNear() >= 0 && getFar() >= 0)
    {   
        glDepthRange(getNear(), getFar());
    }
    
    if(getEnable())
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthMask(!getReadOnly());
}

void DepthChunk::changeFrom( DrawActionBase *act, StateChunk * old_chunk, UInt32 index )
{
    old_chunk->deactivate( act, index );
    activate( act, index );
}

void DepthChunk::deactivate ( DrawActionBase *, UInt32 )
{
    if(_sfFunc.getValue() != GL_NONE)
    {
        glDepthFunc(GL_LEQUAL);
    }
    
    if(getNear() >= 0 && getFar() >= 0)
    {   
        glDepthRange(0, 1);
    }
    
    if(!getEnable())
    {
        glEnable(GL_DEPTH_TEST);
    }

    glDepthMask(GL_TRUE);
}

/*-------------------------- Comparison -----------------------------------*/

Real32 DepthChunk::switchCost(StateChunk *)
{
    return 0;
}

/** \brief assignment
 */

bool DepthChunk::operator < (const StateChunk &other) const
{
    return this < &other;
}

/** \brief equal
 */

bool DepthChunk::operator == (const StateChunk &other) const
{
    DepthChunk const *tother = dynamic_cast<DepthChunk const*>(&other);

    if(!tother)
        return false;

    if(tother == this)
        return true;

    if(getEnable()          != tother->getEnable()  ||
       getFunc()            != tother->getFunc()    ||
       getFar()             != tother->getFar()     ||
       getNear()            != tother->getNear()    ||   
       getReadOnly()        != tother->getReadOnly() )
        return false;

    return true;
}

/** \brief unequal
 */

bool DepthChunk::operator != (const StateChunk &other) const
{
    return ! (*this == other);
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGDepthChunk.cpp,v 1.2 2005/06/07 09:35:15 yjung Exp $";
    static Char8 cvsid_hpp       [] = OSGDEPTHCHUNKBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGDEPTHCHUNKBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGDEPTHCHUNKFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

