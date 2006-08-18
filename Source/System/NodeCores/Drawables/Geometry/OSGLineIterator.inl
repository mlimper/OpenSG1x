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
#include "OSGGeometry.h"

OSG_BEGIN_NAMESPACE

/*! Return the index of the current face. The index runs from 0 to the
    number of triangles in the geometry. Its main use is as an input to 
    osg::LineIterator::seek.
*/
inline       
Int32 LineIterator::getIndex(void) const
{
    return _lineIndex;
}

inline       
Int32 LineIterator::getPositionIndex(Int32 which) const
{
    return Inherited::getPositionIndex(_linePntIndex[which]);
}

inline       
Pnt3f LineIterator::getPosition(Int32 which) const
{
    Int32 ind = getPositionIndex(which);
    
    return getGeometry()->getPositions()->getValue(ind);
}

inline       
Int32 LineIterator::getNormalIndex(Int32 which) const
{
    return Inherited::getNormalIndex(_linePntIndex[which]);
}

inline 
Vec3f LineIterator::getNormal(Int32 which) const
{   
    Int32 ind = getNormalIndex(which);
    
    if(ind < 0)
        return Vec3f::Null;
        
    return getGeometry()->getNormals()->getValue(ind);
}

inline       
Int32 LineIterator::getColorIndex(Int32 which) const
{
    return Inherited::getColorIndex(_linePntIndex[which]);
}

inline 
Color3f LineIterator::getColor(Int32 which) const
{   
    Int32 ind = getColorIndex(which);
    
    if(ind < 0)
        return Color3f::Null;
        
    return getGeometry()->getColors()->getValue(ind);
}

inline       
Int32 LineIterator::getSecondaryColorIndex(Int32 which) const
{
    return Inherited::getSecondaryColorIndex(_linePntIndex[which]);
}

inline 
Color3f LineIterator::getSecondaryColor(Int32 which) const
{   
    Int32 ind = getSecondaryColorIndex(which);
    
    if(ind < 0)
        return Color3f::Null;
        
    return getGeometry()->getSecondaryColors()->getValue(ind);
}


inline       
Int32 LineIterator::getTexCoordsIndex(Int32 which) const
{
    return Inherited::getTexCoordsIndex(_linePntIndex[which]);
}

inline 
Vec2f LineIterator::getTexCoords(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords()->getValue(ind);
}


inline       
Int32 LineIterator::getTexCoordsIndex1(Int32 which) const
{
    return Inherited::getTexCoordsIndex1(_linePntIndex[which]);
}

inline 
Vec2f LineIterator::getTexCoords1(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex1(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords1()->getValue(ind);
}


inline       
Int32 LineIterator::getTexCoordsIndex2(Int32 which) const
{
    return Inherited::getTexCoordsIndex2(_linePntIndex[which]);
}

inline 
Vec2f LineIterator::getTexCoords2(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex2(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords2()->getValue(ind);
}


inline
Int32 LineIterator::getTexCoordsIndex3(Int32 which) const
{
    return Inherited::getTexCoordsIndex3(_linePntIndex[which]);
}

inline
Vec2f LineIterator::getTexCoords3(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex3(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords3()->getValue(ind);
}

inline
Int32 LineIterator::getTexCoordsIndex4(Int32 which) const
{
    return Inherited::getTexCoordsIndex4(_linePntIndex[which]);
}

inline
Vec2f LineIterator::getTexCoords4(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex4(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords4()->getValue(ind);
}

inline
Int32 LineIterator::getTexCoordsIndex5(Int32 which) const
{
    return Inherited::getTexCoordsIndex5(_linePntIndex[which]);
}

inline
Vec2f LineIterator::getTexCoords5(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex5(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords5()->getValue(ind);
}

inline
Int32 LineIterator::getTexCoordsIndex6(Int32 which) const
{
    return Inherited::getTexCoordsIndex6(_linePntIndex[which]);
}

inline
Vec2f LineIterator::getTexCoords6(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex6(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords6()->getValue(ind);
}

inline
Int32 LineIterator::getTexCoordsIndex7(Int32 which) const
{
    return Inherited::getTexCoordsIndex7(_linePntIndex[which]);
}

inline
Vec2f LineIterator::getTexCoords7(Int32 which) const
{   
    Int32 ind = getTexCoordsIndex7(which);
    
    if(ind < 0)
        return Vec2f::Null;
        
    return getGeometry()->getTexCoords7()->getValue(ind);
}

inline       
Int32 LineIterator::getIndexIndex(Int32 which) const
{
    if(_linePntIndex[which] >= 0)
        return Inherited::getIndexIndex(_linePntIndex[which]);
    else 
        return -1;
}


OSG_END_NAMESPACE

