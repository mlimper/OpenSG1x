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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include <OSGGL.h>

#include "OSGPolygonChunk.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::PolygonChunk
    \ingroup GrpSystemState

See \ref PageSystemPolygonChunk for details.

The parameters of the following functions are wrapped here: glCullFace()
(osg::PolygonChunk::_sfCullFace), glFrontFace()
(osg::PolygonChunk::_sfFrontFace), glPolygonMode() 
(osg::PolygonChunk::_sfFrontMode, osg::PolygonChunk::_sfBackMode),
glEnable(GL_POLYGON_SMOOTH) (osg::PolygonChunk::_sfSmooth), glPolygonOffset()
(osg::PolygonChunk::_sfOffsetFactor, osg::PolygonChunk::_sfOffsetBias),
glEnable(GL_POLYGON_OFFSET_POINT) (osg::PolygonChunk::_sfOffsetPoint),
glEnable(GL_POLYGON_OFFSET_LINE) (osg::PolygonChunk::_sfOffsetLine),
glEnable(GL_POLYGON_OFFSET_FILL) (osg::PolygonChunk::_sfOffsetFill),
glStipplePattern() and glEnable(GL_POLYGON_STIPPLE)
(osg::PolygonChunk::_sfStipple).
*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

StateChunkClass PolygonChunk::_class("Polygon");

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void PolygonChunk::initMethod (void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/


/*------------- constructors & destructors --------------------------------*/

PolygonChunk::PolygonChunk(void) :
    Inherited()
{
}

PolygonChunk::PolygonChunk(const PolygonChunk &source) :
    Inherited(source)
{
}

PolygonChunk::~PolygonChunk(void)
{
}

/*------------------------- Chunk Class Access ---------------------------*/

const StateChunkClass *PolygonChunk::getClass( void ) const
{
    return &_class;
}

/*------------------------------- Sync -----------------------------------*/

void PolygonChunk::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/*------------------------------ Output ----------------------------------*/

void PolygonChunk::dump(      UInt32    uiIndent,
                        const BitVector bvFlags) const
{
   Inherited::dump(uiIndent, bvFlags);
}


/*------------------------------ State ------------------------------------*/

void PolygonChunk::activate(DrawActionBase *, UInt32)
{
// cullFace

    if(_sfCullFace.getValue() != GL_NONE)
    {
        glCullFace(_sfCullFace.getValue());
        glEnable(GL_CULL_FACE);
    }

// frontFace

    if(_sfFrontFace.getValue() != GL_CCW)
        glFrontFace(_sfFrontFace.getValue());

// smooth

    if(_sfSmooth.getValue()) 
        glEnable(GL_POLYGON_SMOOTH);

// mode

    if(_sfFrontMode.getValue() != GL_FILL)
        glPolygonMode(GL_FRONT, _sfFrontMode.getValue());

    if(_sfBackMode.getValue() != GL_FILL)
        glPolygonMode(GL_BACK, _sfBackMode.getValue());

// offset
   
    if(_sfOffsetFactor.getValue() != 0.f || _sfOffsetBias.getValue() != 0.f)    
        glPolygonOffset(_sfOffsetFactor.getValue(), _sfOffsetBias.getValue());

    if(_sfOffsetPoint.getValue())    
        glEnable(GL_POLYGON_OFFSET_POINT);

    if(_sfOffsetLine.getValue()) 
        glEnable(GL_POLYGON_OFFSET_LINE);

    if(_sfOffsetFill.getValue()) 
        glEnable(GL_POLYGON_OFFSET_FILL);

// stipple

    if(_mfStipple.size() == 32)
    {
        glPolygonStipple((const GLubyte *) &(_mfStipple[0]));
        glEnable(GL_POLYGON_STIPPLE);
    }
}


void PolygonChunk::changeFrom(DrawActionBase *, StateChunk * old_chunk, UInt32)
{
    PolygonChunk const *old = dynamic_cast<PolygonChunk const*>(old_chunk);

    // change from me to me?
    // this assumes I haven't changed in the meantime. is that a valid assumption?
    if(old == this)
        return;

// cullFace

    if(_sfCullFace.getValue() != old->_sfCullFace.getValue())
    {
        if(_sfCullFace.getValue() != GL_NONE)
        {
            glCullFace(_sfCullFace.getValue());
            glEnable(GL_CULL_FACE);
        }
        else  
        {
            glDisable(GL_CULL_FACE);
        }
    }

// frontFace

    if(_sfFrontFace.getValue() != old->_sfFrontFace.getValue())
    {
        glFrontFace(_sfFrontFace.getValue());
    }

// smooth

    if(_sfSmooth.getValue() != old->_sfSmooth.getValue())
    {
        if(_sfSmooth.getValue())   
        {
            glEnable(GL_POLYGON_SMOOTH);
        }
        else
        {
            glDisable(GL_POLYGON_SMOOTH);
        }
    }

// mode

    if(_sfFrontMode.getValue() !=  old->_sfFrontMode.getValue())
        glPolygonMode(GL_FRONT, _sfFrontMode.getValue());

    if(_sfBackMode.getValue() !=  old->_sfBackMode.getValue())
        glPolygonMode(GL_BACK, _sfBackMode.getValue());

// offset

    if(_sfOffsetFactor.getValue() != old->_sfOffsetFactor.getValue() ||
         _sfOffsetBias.getValue()   != old->_sfOffsetBias.getValue())
    {
        glPolygonOffset(_sfOffsetFactor.getValue(), _sfOffsetBias.getValue());
    }

    if(_sfOffsetPoint.getValue() != old->_sfOffsetPoint.getValue())
    {
        if(_sfOffsetPoint.getValue())  
        {
            glEnable(GL_POLYGON_OFFSET_POINT);
        }
        else                            
        {
            glDisable(GL_POLYGON_OFFSET_POINT);
        }
    }

    if(_sfOffsetLine.getValue() != old->_sfOffsetLine.getValue())
    {
        if(_sfOffsetLine.getValue())  
        {
            glEnable(GL_POLYGON_OFFSET_LINE);
        }
        else                            
        {
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
    }

    if(_sfOffsetFill.getValue() != old->_sfOffsetFill.getValue())
    {
        if(_sfOffsetFill.getValue())  
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
        }
        else                            
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

// stipple

    if(_mfStipple.getValues() != old->_mfStipple.getValues())
    {
        if(_mfStipple.size() == 32)
        {
            glPolygonStipple((const GLubyte *) &(_mfStipple[0]));
            glEnable(GL_POLYGON_STIPPLE);
        }
        else glDisable(GL_POLYGON_STIPPLE);
    }

}

void PolygonChunk::deactivate(DrawActionBase *, UInt32)
{

// cullFace

    if(_sfCullFace.getValue() != GL_NONE)
        glDisable(GL_CULL_FACE);

// frontFace

    if(_sfFrontFace.getValue() != GL_CCW)
        glFrontFace(GL_CCW);

// smooth

    if(_sfSmooth.getValue())
        glDisable(GL_POLYGON_SMOOTH);

// mode

    if(_sfFrontMode.getValue() != GL_FILL)
        glPolygonMode(GL_FRONT, GL_FILL);

    if(_sfBackMode.getValue() != GL_FILL)
        glPolygonMode(GL_BACK, GL_FILL);

// offset

    if(_sfOffsetPoint.getValue())
        glDisable(GL_POLYGON_OFFSET_POINT);

    if(_sfOffsetLine.getValue())
        glDisable(GL_POLYGON_OFFSET_LINE);

    if(_sfOffsetFill.getValue())
        glDisable(GL_POLYGON_OFFSET_FILL);

// stipple

    if(_mfStipple.size() == 32)
        glDisable(GL_POLYGON_STIPPLE);

}


/*-------------------------- Comparison -----------------------------------*/

Real32 PolygonChunk::switchCost(StateChunk *OSG_CHECK_ARG(chunk))
{
    return 0;
}

bool PolygonChunk::operator <(const StateChunk &other) const
{
    return this < &other;
}

bool PolygonChunk::operator ==(const StateChunk &other) const
{
    PolygonChunk const *tother = dynamic_cast<PolygonChunk const*>(&other);

    if(!tother)
        return false;

    if(tother == this)
        return true;

    if(getCullFace()       != tother->getCullFace()      ||
       getFrontFace()      != tother->getFrontFace()     ||
       getSmooth()         != tother->getSmooth()        ||
       getFrontMode()      != tother->getFrontMode()     ||
       getBackMode()       != tother->getBackMode()      ||
       getOffsetPoint()    != tother->getOffsetPoint()   ||
       getOffsetLine()     != tother->getOffsetLine()    ||
       getOffsetFill()     != tother->getOffsetFill()    ||
       getOffsetFactor()   != tother->getOffsetFactor()  ||
       getOffsetBias()     != tother->getOffsetBias()    ||
       getStipple().size() != tother->getStipple().size()
      )
        return false;

    // would need to compare the whole stipple data
    // cheap trick: if != 0 take as different
    
    if(        getStipple().size() != 0 ||
       tother->getStipple().size() != 0 
      )
        return false;

    return true;
}

bool PolygonChunk::operator !=(const StateChunk &other) const
{
    return !(*this == other);
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
    static Char8 cvsid_cpp       [] = "@(#)$Id: FCTemplate_cpp.h,v 1.13 2002/06/01 10:37:25 vossg Exp $";
    static Char8 cvsid_hpp       [] = OSGPOLYGONCHUNK_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGPOLYGONCHUNK_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGPOLYGONCHUNKFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


