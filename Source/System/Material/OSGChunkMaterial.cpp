/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright(C) 2000-2002 by the OpenSG Forum                   *
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

#include <OSGAttachmentContainerPtr.h>
#include <OSGState.h>
#include <OSGDrawActionBase.h>
#include <OSGGeometry.h>

#include "OSGChunkMaterial.h"

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::ChunkMaterial
    \ingroup GrpSystemMaterial

The chunk material class. See \ref PageSystemMaterialChunkMaterial for a
description.

Chunks can be attached and detached from the material using
osg::ChunkMaterial::addChunk() and osg::ChunkMaterial::subChunk(). For more
complex manipulation of the chunk list use the osg::ChunkMaterial::_mfChunks
field's functions.

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void ChunkMaterial::initMethod(void)
{
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*------------- constructors & destructors --------------------------------*/

ChunkMaterial::ChunkMaterial(void) :
    Inherited()
{
}

ChunkMaterial::ChunkMaterial(const ChunkMaterial &source) :
    Inherited(source)
{
}

ChunkMaterial::~ChunkMaterial(void)
{
    clearChunks();
}

void ChunkMaterial::changed(BitVector whichField, UInt32 origin)
{
    if(origin & ChangedOrigin::Abstract)
    {
        MFStateChunkPtr::iterator matIt  = _mfChunks.begin();
        MFStateChunkPtr::iterator matEnd = _mfChunks.end ();
        
        while(matIt != matEnd)
        {
            addRefCP(*matIt);
           
            ++matIt;
        }
    }

    Inherited::changed(whichField, origin);
}


/*-------------------------- your_category---------------------------------*/

/*! Add the given chunk to the material. It is possible to specify
which slot this chunk should be associated with. See \ref StateChunkClass for a
general description of the slots concept. The default slot is
State::AutoSlotReplace. */

bool ChunkMaterial::addChunk(StateChunkPtr chunk, Int32 slot)
{
    if(slot != State::AutoSlotReplace)
    {
        while(_mfSlots.size() < _mfChunks.size())
            _mfSlots.push_back(State::AutoSlotReplace);
    }
    
    _mfChunks.push_back(chunk);
    
    if(slot != State::AutoSlotReplace)
        _mfSlots.push_back(slot);
        
    addRefCP(chunk);

    return true;
}

/*! Remove the given chunk from the material. If the slot is not
State::AutoSlotReplace, only the given slot will be searched and
removed if found. Returns true if the chunk couldn't be found. */

bool ChunkMaterial::subChunk(StateChunkPtr chunk, Int32 slot)
{
    UInt32 i;
    
    for(i = 0; i < _mfChunks.size(); ++i)
    {
        if(_mfChunks[i] == chunk &&
           ((i < _mfSlots.size() && _mfSlots[i] == slot) ||
            slot == State::AutoSlotReplace))
        {
            subRefCP(chunk);
            _mfChunks.erase(_mfChunks.begin() + i);
            if(i < _mfSlots.size())
                _mfSlots.erase(_mfSlots.begin() + i);
            return false;
        }
    }

    SWARNING << "ChunkMaterial::subChunk(" << this << ") has no chunk "
             << chunk << " with slot " << slot << std::endl;
             
    return true;
}

/*! Search the list of chunks for the given chunk. Returns its index, -1
if the chunk is not used in the material.
*/

Int32 ChunkMaterial::find(StateChunkPtr chunk)
{
    UInt32 i;
    
    for(i = 0; i < _mfChunks.size(); ++i)
    {
        if(_mfChunks[i] == chunk)
            return i;
    }
             
    return -1;
}

/*! Search the list of chunks for a chunk of the given type and the given
slot. If slot is State::AutoSlotReplace, all slots are searched and the
first found chunk is returned.
For searching a specific chunk with a AutoSlotReplace slot,
the slot parameter is interpreted as a index.
*/

StateChunkPtr ChunkMaterial::find(const FieldContainerType &type, 
                                    Int32 slot)
{
    UInt32 index = 0;
    for(UInt32 i = 0; i < _mfChunks.size(); ++i)
    {
        StateChunkPtr p = _mfChunks[i];
        Int32 s = State::AutoSlotReplace;

        if(i < getSlots().size())
            s = getSlots(i);

        if(s == State::AutoSlotReplace)
            s = index;

        if(p->getType() == type)
        {           
            if(slot == State::AutoSlotReplace || slot == s)
                return (p);
            ++index;
        }
    }
    return NullFC;
}

void ChunkMaterial::clearChunks(void)
{
    MFStateChunkPtr::iterator matIt  = _mfChunks.begin();
    MFStateChunkPtr::iterator matEnd = _mfChunks.end ();
    
    while(matIt != matEnd)
    {
        subRefCP(*matIt);
        ++matIt;
    }
    _mfChunks.clear();
}

bool ChunkMaterial::operator==(const Material& other)
{
    Material& thisBase = *this;
    if (!(thisBase == other))
        return false;

    const ChunkMaterial* othercm = dynamic_cast<const ChunkMaterial*>(&other);
    if (!othercm)
        return false;

    // ?
    return false;
}

/*! Draw the geometry with this material.
*/

void ChunkMaterial::draw(Geometry* geo, DrawActionBase * action)
{
    StatePtr state = makeState();

    addRefCP(state);

    state->activate(action);

    geo->drawPrimitives(action);

    state->deactivate(action);

    subRefCP(state); // kill it
}

/*! Draw the function behind the functor with this material. The functored
    function should be very careful changing OpenGL state, better just use
    glBegin(), glEnd() and the standard stuff in between.
*/

void ChunkMaterial::draw(DrawFunctor& func, DrawActionBase * action)
{
    StatePtr state = makeState();

    addRefCP(state);

    state->activate(action);

    func.call(action);

    state->deactivate(action);

    subRefCP(state); // kill it
}

/*! Add chunks to the given state. Needed for ordering in the drived
materials.
*/

void ChunkMaterial::addChunks(StatePtr state)
{
    UInt32 i;
    
    for(i = 0; i < _mfChunks.size(); ++i)
        state->addChunk(_mfChunks[i], 
                        (i < _mfSlots.size()) ? _mfSlots[i]
                                                : State::AutoSlotReplace);
}

/*! Create a osg::State that represents this Material and return it.
*/

StatePtr ChunkMaterial::makeState(void)
{
    StatePtr state = State::create();
    
    addChunks(state);

    return state;
}

/*! Rebuild the internal State. Just collects the chunks in the State.
*/

void ChunkMaterial::rebuildState(void)
{
    if(_pState != NullFC)
    {
        _pState->clearChunks();
    }
    else
    {
        _pState = State::create();

        addRefCP(_pState);
    }
        
    addChunks(_pState);
}

/*! Check if the Material (i.e. any of its chunks) is transparent..
*/

bool ChunkMaterial::isTransparent(void) const
{
    Int32 tm = getTransparencyMode();
    if(tm != Material::TransparencyAutoDetection)
    {
        return (tm == Material::TransparencyForceTransparent);
    }

    bool returnValue = false;

    MFStateChunkPtr::const_iterator it        = _mfChunks.begin();
    MFStateChunkPtr::const_iterator chunksEnd = _mfChunks.end();

    for(; it != chunksEnd && returnValue == false; ++it)
    {
        returnValue =(*it)->isTransparent();
    }

    return returnValue;
}

/*------------------------------- dump ----------------------------------*/

void ChunkMaterial::dump(      UInt32    OSG_CHECK_ARG(uiIndent),
                         const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    SLOG << "ChunkMaterial at " << this << std::endl;
    SLOG << "Chunks: " << std::endl;

    UInt32 i;
    
    for(i = 0; i < _mfChunks.size(); ++i)
        SLOG << _mfChunks[i] << " "
             << static_cast<Int32>((i < _mfSlots.size()) 
                                   ? _mfSlots[i]
                                   :   State::AutoSlotReplace)
             << std::endl;
}


/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: FCTemplate_cpp.h,v 1.13 2002/06/01 10:37:25 vossg Exp $";
    static Char8 cvsid_hpp       [] = OSGCHUNKMATERIAL_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGCHUNKMATERIAL_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGCHUNKMATERIALFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif


