
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


/***************************************************************************\
*                             Includes                                    *
\***************************************************************************/

#include <OSGMergeGraphOp.h>
#include <OSGDirectionalLight.h>
#include <OSGSpotLight.h>
#include <OSGLight.h>
#include <OSGPointLight.h>
#include <OSGSwitch.h>
#include <OSGDistanceLOD.h>
#include <OSGBillboard.h>
#include <OSGMaterialGroup.h>
#include <OSGComponentTransform.h>
#include <OSGPrimitiveIterator.h>
#include <OSGGeometry.h>

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::MergeGraphOp
    \ingroup GrpSystemNodeCoresDrawablesGeometry
    
A class used to optimize geometries a bit.

*/

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*------------- constructors & destructors --------------------------------*/

MergeGraphOp::MergeGraphOp(const char* name): GraphOp(name)
{
}

MergeGraphOp::~MergeGraphOp(void)
{
}

GraphOp* MergeGraphOp::create()
{
    MergeGraphOp* inst = new MergeGraphOp();
    return inst;
}

UInt32 countNodes(const NodePtr& node)
{
    if (node == NullFC)
        return 0;

    UInt32 total = 1;
    for (UInt32 i = 0; i < node->getNChildren(); ++i)
        total += countNodes(node->getChild(i));
    return total;
}

bool MergeGraphOp::traverse(NodePtr& node)
{
    // This is a hack and should be treated as such.
    // The fact that it helps means there is something wrong with
    // the merger. FIXME!!!
    UInt32 next = countNodes(node);
    SINFO << "MergeGraphOp::traverse: Number of nodes before merge: " 
          << next << endLog;
    bool result = true;
    UInt32 current;
    do {
        current = next;
        result &= mergeOnce(node);
        if (!result)
            break;
        next = countNodes(node);
    } while (next < current);

    SINFO << "MergeGraphOp::traverse: Number of nodes after merge: " 
          << current << endLog;

    return result;
}

void MergeGraphOp::setParams(const std::string)
{
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

bool MergeGraphOp::mergeOnce(NodePtr& node)
{
    std::list<NodePtr> tempList;
    tempList.clear();
    tempList.splice(tempList.end(),_excludeListNodes);
    makeExcludeList(node);
    bool result = GraphOp::traverse(node);
    _excludeListNodes.clear();    
    _excludeListNodes.splice(_excludeListNodes.end(),tempList);
    return result;
}

void MergeGraphOp::makeExcludeList(NodePtr& node)
{
    ::traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef<Action::ResultE,
        MergeGraphOp,
        NodePtr>(this,&MergeGraphOp::excludeListEnter),
        osgTypedMethodFunctor2ObjPtrCPtrRef<Action::ResultE,
        MergeGraphOp,
        NodePtr,
        Action::ResultE>(this,&MergeGraphOp::excludeListLeave));
}


Action::ResultE MergeGraphOp::excludeListEnter(NodePtr& node)
{
    if (node==NullFC) ; else ;    
    return Action::Continue;
}

Action::ResultE MergeGraphOp::excludeListLeave(NodePtr& node, Action::ResultE res)
{
    DirectionalLightPtr dlight = DirectionalLightPtr::dcast(node->getCore());
    if (dlight!=NullFC)
        addToExcludeList(dlight->getBeacon());
    
    LightPtr light = LightPtr::dcast(node->getCore());
    if (light!=NullFC)
        addToExcludeList(light->getBeacon());    
    
    PointLightPtr plight = PointLightPtr::dcast(node->getCore());
    if (plight!=NullFC)
        addToExcludeList(plight->getBeacon());
    
    SpotLightPtr slight = SpotLightPtr::dcast(node->getCore());
    if (slight!=NullFC)
        addToExcludeList(slight->getBeacon());    
    
    return res;
}

Action::ResultE MergeGraphOp::traverseEnter(NodePtr& node)
{
    SwitchPtr switch_ = SwitchPtr::dcast(node->getCore());
    if (switch_!=NullFC) return Action::Skip;
    
    DistanceLODPtr dlod = DistanceLODPtr::dcast(node->getCore());
    if (dlod!=NullFC) return Action::Skip;
    
    //leaf, don't enter, cause no job here
    if (isLeaf(node)) return Action::Skip;
    
    return Action::Continue;    
}

Action::ResultE MergeGraphOp::traverseLeave(NodePtr& node, Action::ResultE res)
{
    processGroups(node);
    processTransformations(node);
    processGeometries(node);
    return res;
}

bool MergeGraphOp::isLeaf(NodePtr& node)
{
    if (node->getMFChildren()->getValues().begin()==
        node->getMFChildren()->getValues().end()) return true;
    else return false;    
}

/*! checks whether a node is a group and nothing else
*/
bool MergeGraphOp::isGroup(NodePtr& node)
{
    if(  node->getCore()->getType().isDerivedFrom( Group::getClassType()              ) &&
        !node->getCore()->getType().isDerivedFrom( Transform::getClassType()          ) &&
        !node->getCore()->getType().isDerivedFrom( ComponentTransform::getClassType() ) &&
        !node->getCore()->getType().isDerivedFrom( Switch::getClassType()             ) &&
        !node->getCore()->getType().isDerivedFrom( MaterialGroup::getClassType()      ) &&
        !node->getCore()->getType().isDerivedFrom( DistanceLOD::getClassType()        ) &&
        !node->getCore()->getType().isDerivedFrom( Billboard::getClassType()          )) 
        return true;
    else return false;
}

void MergeGraphOp::processGroups(NodePtr& node)
{
    std::vector<NodePtr>::iterator it = node->getMFChildren()->getValues().begin();
    std::vector<NodePtr>::iterator en = node->getMFChildren()->getValues().end  ();
    std::vector<NodePtr> toAdd;
    std::vector<NodePtr> toSub;
    
    for ( ; it != en; ++it )
    {
        bool special=isInExcludeList(*it);
        bool leaf=isLeaf(*it);
        
        if (isGroup(*it))
        {
            if (!leaf && !special)
            {
                std::vector<NodePtr>::iterator it2 = (*it)->getMFChildren()->getValues().begin();
                std::vector<NodePtr>::iterator en2 = (*it)->getMFChildren()->getValues().end  ();
                
                for ( ; it2 != en2; ++it2 )
                {
                    toAdd.push_back(*it2);
                }                
            }
            
            if (!special)
            {
                toSub.push_back(*it);
                continue;
            }
            
            if (leaf && special)
            {
                //what to do?
            }
            if (!leaf && special)
            {
                //what to do?
            }
            continue;
        }
        else if ((*it)->getCore()->getType().isDerivedFrom( MaterialGroup::getClassType() ))
        {
            MaterialGroupPtr mg=MaterialGroupPtr::dcast((*it)->getCore());
            
            std::vector<NodePtr>::iterator it2 = (*it)->getMFChildren()->getValues().begin();
            std::vector<NodePtr>::iterator en2 = (*it)->getMFChildren()->getValues().end  ();
            
            bool empty=true;
            
            for ( ; it2 != en2; ++it2 )
            {
                if (!isInExcludeList(*it2))
                {
                    //check if geometry
                    if ((*it2)->getCore()->getType().isDerivedFrom(Geometry::getClassType()))
                    {
                        if(!isLeaf(*it2))
                        {
                            //hmm...bad tree...
                            empty=false;
                        }
                        else
                        {                                
                            //it is a leaf geometry, so apply the transformation
                            GeometryPtr geo = GeometryPtr::dcast((*it2)->getCore());
                            beginEditCP(geo);
                            geo->setMaterial(mg->getMaterial());
                            endEditCP(geo);
                            toAdd.push_back(*it2);                            
                        }
                    } else empty=false;
                } else empty=false;                
            }
            
            if (empty) toSub.push_back(*it);
            
        }
    }
    
    it = toAdd.begin();
    en = toAdd.end  ();
    
    for ( ; it != en; ++it )
    {
        beginEditCP(node, Node::ChildrenFieldMask);
        node->addChild(*it);
        endEditCP  (node, Node::ChildrenFieldMask);
    }
    
    it = toSub.begin();
    en = toSub.end  ();
    
    for ( ; it != en; ++it )
    {
        beginEditCP(node, Node::ChildrenFieldMask);
        node->subChild(*it);
        endEditCP  (node, Node::ChildrenFieldMask);
    }
}

void MergeGraphOp::processTransformations(NodePtr& node)
{
    std::vector<NodePtr>::iterator it = node->getMFChildren()->getValues().begin();
    std::vector<NodePtr>::iterator en = node->getMFChildren()->getValues().end  ();
    std::vector<NodePtr> toAdd;
    std::vector<NodePtr> toSub;
    
    for ( ; it != en; ++it )
    {
        bool special=isInExcludeList(*it);
        bool leaf=isLeaf(*it);
        bool empty=true;
        
        //if a transformation:
        if ((*it)->getCore()->getType().isDerivedFrom(Transform::getClassType()))
        {        
            if (!leaf && !special)
            {
                //try to apply it to children geometries
                //move all "moveable" children one level up
                //if empty after that, delete it
                std::vector<NodePtr>::iterator it2 = (*it)->getMFChildren()->getValues().begin();
                std::vector<NodePtr>::iterator en2 = (*it)->getMFChildren()->getValues().end  ();
                
                for ( ; it2 != en2; ++it2 )
                {
                    if (!isInExcludeList(*it2))
                    {
                        //check if geometry
                        if ((*it2)->getCore()->getType().isDerivedFrom(Geometry::getClassType()))
                        {
                            if(!isLeaf(*it2))
                            {
                                //hmm...bad tree...
                                empty=false;
                            }
                            else
                            {                                
                                //it is a leaf geometry, so apply the transformation
                                GeometryPtr geo_old = GeometryPtr::dcast((*it2)->getCore());
                                GeometryPtr geo = geo_old->clone();
                                TransformPtr  t = TransformPtr::dcast((*it)->getCore());
                                GeoPositionsPtr pos  = geo->getPositions();
                                GeoNormalsPtr   norm = geo->getNormals();
                                Matrix m=t->getMatrix();
                                if (pos!=NullFC) 
                                {
                                    beginEditCP( pos );
                                    for (UInt32 i=0; i<pos->getSize(); i++)
                                    {
                                        Pnt3f p=pos->getValue(i);
                                        m.multFullMatrixPnt(p);
                                        pos->setValue(p,i);
                                    }
                                    endEditCP( pos );
                                }
                                
                                if (norm!=NullFC)
                                {
                                    beginEditCP( norm );
                                    for (UInt32 i=0; i<norm->getSize(); i++)
                                    {
                                        Vec3f n=norm->getValue(i);
                                        m.multMatrixVec(n);
                                        n.normalize();
                                        norm->setValue(n,i);
                                    }
                                    endEditCP( norm );
                                }
                                beginEditCP(*it2);
                                (*it2)->setCore(geo);
                                endEditCP(*it2);
                                toAdd.push_back(*it2);
                            }
                        } else empty=false;
                    } else empty=false;
                }                
            }
            
            //now check whether we have to remove it
            if ((empty||leaf) && !special)
            {
                toSub.push_back(*it);
                continue;
            }
            
            if (leaf && special)
            {
                //what to do?
            }
            if (!leaf && special)
            {
                //what to do?
            }
            continue;
        }
    }
    
    it = toAdd.begin();
    en = toAdd.end  ();
    
    for ( ; it != en; ++it )
    {
        beginEditCP(node, Node::ChildrenFieldMask);
        node->addChild(*it);
        endEditCP  (node, Node::ChildrenFieldMask);
    }
    
    it = toSub.begin();
    en = toSub.end  ();
    
    for ( ; it != en; ++it )
    {
        beginEditCP(node, Node::ChildrenFieldMask);
        node->subChild(*it);
        endEditCP  (node, Node::ChildrenFieldMask);
    }
}

void MergeGraphOp::processGeometries(NodePtr& node)
{
    std::vector<NodePtr>::iterator it = node->getMFChildren()->getValues().begin();
    std::vector<NodePtr>::iterator en = node->getMFChildren()->getValues().end  ();
    std::vector<NodePtr> toSub;
    std::vector<NodePtr> toAdd;
    
    for ( ; it != en; ++it )
    {
        bool special=isInExcludeList(*it);
        
        if ((*it)->getCore()->getType().isDerivedFrom(Geometry::getClassType()))
        {
            GeometryPtr geo = GeometryPtr::dcast((*it)->getCore());
            //if a geometry, try to merge it in another geometry
            //if successfull, delete it.
            //check also if it is added for exclusion

            bool inSubList=false;

            std::vector<NodePtr>::iterator it3=toSub.begin();
            std::vector<NodePtr>::iterator en3=toSub.end();

            for ( ; it3 != en3; ++it3 ) 
                if (*it3==*it) { inSubList=true; break; }

            if (!special && !inSubList)
            {
                //ok, try
                std::vector<NodePtr>::iterator it2=it+1;
                GeometryPtr new_geo=NullFC;
                for ( ; it2!=en; ++it2)
                {
                    if (!isInExcludeList(*it2) && (*it2)->getCore()->getType().isDerivedFrom(Geometry::getClassType()))
                    {
                        GeometryPtr geo2 = GeometryPtr::dcast((*it2)->getCore());
                        if (geo->isMergeable(geo2))
                        {
                            if (new_geo==NullFC)
                            {
                                new_geo=Geometry::create();
                                if (new_geo->merge(geo))
                                    toSub.push_back(*it);
                                else FWARNING(("MergeGraphOp: processGeometries problem 1\n"));

                                if (new_geo->merge(geo2))
                                    toSub.push_back(*it2);
                                else FWARNING(("MergeGraphOp: processGeometries problem 2\n"));
                            }
                            else
                            {
                                if (new_geo->merge(geo2))
                                    toSub.push_back(*it2);
                            }
                        }                        
                    }
                }
                if (new_geo!=NullFC)
                {
                    NodePtr new_node=Node::create();
                    beginEditCP(new_node);
                    new_node->setCore(new_geo);
                    endEditCP  (new_node);

                    toAdd.push_back(new_node);
                }
            }
            else
            {
                //hmm...have to skip it
            }
        }
    }
    
    it = toAdd.begin();
    en = toAdd.end  ();
    
    for ( ; it != en; ++it )
    {
        beginEditCP(node, Node::ChildrenFieldMask);
        node->addChild(*it);
        endEditCP  (node, Node::ChildrenFieldMask);
    }

    it = toSub.begin();
    en = toSub.end  ();
    
    for ( ; it != en; ++it )
    {
        beginEditCP(node, Node::ChildrenFieldMask);
        node->subChild(*it);
        endEditCP  (node, Node::ChildrenFieldMask);
    }
}
