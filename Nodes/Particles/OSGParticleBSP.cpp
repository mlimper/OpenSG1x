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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#define OSG_COMPILEPARTICLEBSPINST

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>

#include <OSGBaseFunctions.h>

#include "OSGParticleBSP.h"

#include "OSGParticles.h"

OSG_USING_NAMESPACE

/*! \class osg::ParticleBSPTree

A very simple BSP tree, optimized for particle sorting.
*/

/*! \class osg::ParticleBSPNode

A node of the ParticleBSPTree.
*/

/*----------------------- constructors & destructors ----------------------*/

ParticleBSPNode::ParticleBSPNode(void) :
    _axis()
{
}
    
ParticleBSPNode::ParticleBSPNode(const ParticleBSPNode &source) :
    _axis(source._axis)
{
    if(isLeaf())
    {
        _value = source._value;
    }
    else
    {
        _splitvalue = source._splitvalue;        
    }
}

ParticleBSPNode::ParticleBSPNode(UInt32 value) :
    _axis(Leaf),
    _value(value)
{
}

ParticleBSPNode::ParticleBSPNode(UInt8 axis, Real32 splitvalue) :
    _axis(axis),
    _splitvalue(splitvalue)
{
}

ParticleBSPNode::~ParticleBSPNode(void)
{
}

/*---------------------------------- output -------------------------------*/

void ParticleBSPNode::dump(      UInt32    OSG_CHECK_ARG(uiIndent), 
                           const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    static const char *axisname = "XYZL";
    
    Real32 v = isLeaf()?_value:_splitvalue;
    
    PLOG << "(" << axisname[_axis] << " " << v << ")";
}


/*-------------------------------------------------------------------------*/
/*---------------------------- BSP Tree -----------------------------------*/
/*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

ParticleBSPTree::ParticleBSPTree(void)
{
}

ParticleBSPTree::~ParticleBSPTree(void)
{
} 

/*---------------------------------- output -------------------------------*/

void ParticleBSPTree::dump(      UInt32    OSG_CHECK_ARG(uiIndent), 
                           const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    PLOG << "ParticleBSPTree(";

    if(!_tree.empty())
    {
        for(vector<ParticleBSPNode>::const_iterator i = _tree.begin() + 1;
            i != _tree.end(); ++i )
        {
            i->dump();
        }
    }   
    
    PLOG << ")" << endl;
}
    
void ParticleBSPTree::putToString(string &outVal) const
{
    typedef TypeConstants<UInt32> TypeConst;
    
    outVal.assign(TypeConstants<UInt32>::putToString(_tree.size()));
    outVal.append(";");
    
    if(! _tree.empty())
    {
        for(vector<ParticleBSPNode>::const_iterator i = _tree.begin() + 1;
            i != _tree.end(); ++i )
        {
            outVal.append(TypeConstants<UInt8>::putToString(i->getAxis()));
            outVal.append(":");
            if(i->isLeaf())
            {
                outVal.append(TypeConstants<Int32>::putToString(i->getValue()));
            }
            else
            {
                outVal.append(TypeConstants<Real32>::putToString(
                                                        i->getSplitValue()
                             )                                  );
            }
            outVal.append(";");
        }   
    }
}
    
bool ParticleBSPTree::getFromString(const Char8 *&inVal)
{
    UInt32 size = TypeConstants<UInt32>::getFromString(inVal);
 
    const Char8 *c = strchr(inVal, ';');
    
    if(!c)
        return false;
    c++;
    
    _tree.resize(size);
    
    for(UInt32 i = 1; i < size; ++i)
    {
        UInt8 axis = TypeConstants<UInt8>::getFromString(c);
        c = strchr(c, ':');
        if(!c)
            return false;
        c++;
        
        if(axis == ParticleBSPNode::Leaf)
        {
            Int32 value = TypeConstants<Int32>::getFromString(c);
            _tree[i].setValue(value);
        }
        else
        {
            Real32 value = TypeConstants<Real32>::getFromString(c);
            _tree[i].setSplit(axis, value);           
        }
        c = strchr(c, ';');
        if(!c)
            return false;
        c++;
    }
    
    return true;
}

    
UInt32 ParticleBSPTree::getBinSize(void) const
{
    return sizeof(UInt32) + _tree.size() * sizeof(ParticleBSPNode);
}
    
void ParticleBSPTree::copyToBin(BinaryDataHandler &pMem) const
{
    UInt32 size = _tree.size();
    pMem.put(&size, sizeof(UInt32));

    pMem.put(&_tree[0], _tree.size() * sizeof(ParticleBSPNode));
}
    
void ParticleBSPTree::copyFromBin(BinaryDataHandler &pMem)
{
    UInt32 size = _tree.size();
    pMem.get(&size, sizeof(UInt32));

    _tree.resize(size);
    pMem.get(&_tree[0], _tree.size() * sizeof(ParticleBSPNode));
}


/*-------------------------------- traversal ------------------------------*/

Int32 *ParticleBSPTree::traverse(const Pnt3f &refPoint, UInt32 &length, 
                                 Int32 *order) const
{
    if(_tree.empty())
        return NULL;
        
    if(order == NULL)
    {
        order = new Int32 [length];
    }
    
    length = doTraverse(refPoint,1,0,order);
    
    return order;
}

UInt32 ParticleBSPTree::doTraverse(const Pnt3f &refPoint, UInt32 index, 
                                   UInt32 length, Int32 *order) const
{
    const ParticleBSPNode *n = &_tree[index];
    
    if(n->isLeaf())
    {
        order[length] = n->getValue();
        return ++length;
    }
    else
    {
        if(refPoint[n->getAxis()] > n->getSplitValue())
        {
            length = doTraverse(refPoint, index * 2    , length, order);
            length = doTraverse(refPoint, index * 2 + 1, length, order);
        }
        else
        {
            length = doTraverse(refPoint, index * 2 + 1, length, order);
            length = doTraverse(refPoint, index * 2    , length, order);
        }
    }
    return length;
}

Int32 *ParticleBSPTree::traverse(const Vec3f &refVec, UInt32 &length, 
                                 Int32 *order) const
{
    if(order == NULL)
    {
        order = new Int32 [length];
    }
    
    length = doTraverse(refVec,1,0,order);
    
    return order;
}

UInt32 ParticleBSPTree::doTraverse(const Vec3f &refVec, UInt32 index, 
                                   UInt32 length, Int32 *order) const
{
    const ParticleBSPNode *n = &_tree[index];
    
    if(n->isLeaf())
    {
        order[length] = n->getValue();
        return ++length;
    }
    else
    {
        if(refVec[n->getAxis()] > 0.f)
        {
            length = doTraverse(refVec, index * 2    , length, order);
            length = doTraverse(refVec, index * 2 + 1, length, order);
        }
        else
        {
            length = doTraverse(refVec, index * 2 + 1, length, order);
            length = doTraverse(refVec, index * 2    , length, order);
        }
    }
    return length;
}

/*--------------------------------- creation ------------------------------*/

void ParticleBSPTree::build(Particles *core)
{
    _tree.clear();
    
    if(core == NULL)
    {
        FWARNING(("ParticleBSP::build: no core!!\n"));
        return;
    }
    
    const GeoPositionsPtr pos = core->getPositions();
    
    if(pos == NullFC)
        return;

    const MFInt32 *indices = core->getMFIndices();
        
    // 1. create list for particles
 
    vector<Int32> order;
    order.reserve(pos->getSize());
    
    for(UInt32 i = 0; i < pos->getSize(); ++i )
    {     
        if(indices->size() == pos->getSize())
        {        
            order.push_back((*indices)[i]);
        }
        else
        {
            order.push_back(i);            
        }
    }
    
    // reserve mem for tree
    
    _tree.resize(osgnextpower2(order.size()) * 2);
    
    // 2. recursively build the tree
    
    UInt32 nnodes = doBuild(order.begin(), order.end(), 1, pos);
    
    // 3. remove the unneeded elements from the end
    
    if(nnodes < _tree.size())
        _tree.erase( _tree.begin() + nnodes, _tree.end());

    // done
}

struct ParticleCompare : public binary_function<Int32, Int32, bool> 
{
    ParticleCompare(GeoPositionsPtr pos, UInt8 axis) : _pos(pos), _axis(axis)
    {}
    
    bool operator()(Int32 x, Int32 y) 
    { 
        Pnt3f px,py;
        _pos->getValue(px, x);
        _pos->getValue(py, y);
        
        return px[_axis] < py[_axis]; 
    }
    
    GeoPositionsPtr _pos;
    UInt8 _axis;
};
    
UInt32 ParticleBSPTree::doBuild(vector<Int32>::iterator begin, 
                            vector<Int32>::iterator end,
                            UInt32 nodeindex,
                            GeoPositionsPtr pos)
{
    // reached a leaf?
    
    if(begin + 1 == end)
    {
        _tree[nodeindex].setValue(*begin);
        return nodeindex + 1;
    }
    
    // find the bounding volume of the group
    
    BoxVolume b;
    Pnt3f p;
    
    b.setEmpty();
    
    for(vector<Int32>::iterator i = begin; i != end; ++i)
    {
        pos->getValue(p,*i);     
        b.extendBy(p);
    }
    
    // find the axis with the longest extension
    
    Vec3f d = b.getMax() - b.getMin();
    
    UInt8 axis = ParticleBSPNode::X;
    Real32 maxval = d[0];
    
    if(d[1] > maxval)
    {
        axis = ParticleBSPNode::Y;
        maxval = d[1];
    }
    if(d[2] > maxval)
    {
        axis = ParticleBSPNode::Z;
        maxval = d[2];
    }

    // sort in that axis
    ParticleCompare comp(pos, axis);
    
    sort(begin,end,comp);
    
    // find median value
    vector<Int32>::iterator mid = begin + (end - begin) / 2;
    
    Pnt3f p2;
    pos->getValue(p ,*mid);
    pos->getValue(p2,(*mid)-1);
    _tree[nodeindex].setSplit(axis, (p[axis] + p2[axis]) / 2.f);
    
    return osgMax( doBuild(begin, mid, nodeindex * 2    , pos),
                   doBuild(  mid, end, nodeindex * 2 + 1, pos) );   
}   


void ParticleBSPTree::destroy()
{
    _tree.clear();
}


#include <OSGMFieldTypeDef.inl>
#include <OSGSFieldTypeDef.inl>

OSG_BEGIN_NAMESPACE

/*-------------------------- field instantiations -------------------------*/

DataType FieldDataTraits<ParticleBSPTree>::_type("ParticleBSPTree", 
        "None");

#if defined(__sgi)

#pragma instantiate SField<ParticleBSPTree>::_fieldType

#else

OSG_DLLEXPORT_SFIELD_DEF1(ParticleBSPTree, OSG_SYSTEMLIB_DLLTMPLMAPPING)

#endif

OSG_END_NAMESPACE

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
    static char cvsid_cpp[] = "@(#)$Id: OSGParticleBSP.cpp,v 1.10 2002/05/20 02:45:54 vossg Exp $";
    static char cvsid_hpp[] = OSGPARTICLEBSP_HEADER_CVSID;
    static char cvsid_inl[] = OSGPARTICLEBSP_INLINE_CVSID;
}
