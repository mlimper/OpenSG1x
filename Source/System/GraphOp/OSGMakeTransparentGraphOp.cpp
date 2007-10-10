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


#include <OSGBlendChunk.h>
#include <OSGMakeTransparentGraphOp.h>
#include <OSGPolygonChunk.h>
#include <OSGSimpleMaterial.h>

#include <OSGGLEXT.h>

OSG_USING_NAMESPACE

MakeTransparentGraphOp::MakeTransparentGraphOp(const char* name)
    : GraphOp(name),
      _transparency(0.5)
{
}

GraphOp* MakeTransparentGraphOp::create()
{
    return new MakeTransparentGraphOp();
}


// Similar to boost's next iterator function.  (I think it's from
// boost, at least.)
template<typename T>
T next(T t) // Iterator passed by value.
{
    return ++t;
}


bool MakeTransparentGraphOp::traverse(NodePtr& node)
{
    // Find the materials.
    if (!GraphOp::traverse(node)) {
        return false;
    }

    // Now do the merge.
    MaterialObjectMap::iterator itr = _materialObjects.begin();
    for (; itr != _materialObjects.end(); ++itr)
    {
        MaterialPtr oldMaterial = itr->first;
        MaterialPtr newMaterial = MaterialPtr::dcast(deepClone(oldMaterial));
        if (newMaterial != NullFC)
        {
            std::cout << "Applying transparency:  ";

            applyTransparency(newMaterial);

            // Put the new material in the objects in this subtree.
            MaterialObjectList& currentList = itr->second;
            MaterialObjectList::iterator i = currentList.begin();
            for (; i != currentList.end(); ++i)
            {
                i->setMaterial(newMaterial);
            }
        }
    }

    return true;
}


void MakeTransparentGraphOp::setParams(const std::string params)
{
    ParamSet ps(params);   

    ps("transparency",  _transparency);
   
    std::string out = ps.getUnusedParams();
    if(out.length())
    {
        FWARNING(("MakeTransparentGraphOp doesn't have parameters '%s'.\n",
                out.c_str()));
    }
}

std::string MakeTransparentGraphOp::usage(void)
{
    return 
    "MakeTransparent: make used Materials transparent\n"
    "  Based on MaterialMergeGraphOp, merges Materials and sets their\n"
    "  transparency.\n"
    "Params: name (type, default)\n"
    "  transparency (Real32, 0.5f): transparency value\n";    
}

Action::ResultE MakeTransparentGraphOp::traverseEnter(NodePtr& node)
{
    if(isInExcludeList(node))
        return Action::Continue;

    GeometryPtr geo = GeometryPtr::dcast(node->getCore());
    if(geo != NullFC)
    {
        addObject(MaterialObject(geo));
        return Action::Continue;
    }

    MaterialGroupPtr mg = MaterialGroupPtr::dcast(node->getCore());
    if (mg != NullFC)
    {
        addObject(MaterialObject(mg));
        return Action::Continue;
    }

    // Otherwise, keep looking.
    return Action::Continue;
}

Action::ResultE MakeTransparentGraphOp::traverseLeave(NodePtr& node, Action::ResultE res)
{
    return res;
}

void MakeTransparentGraphOp::addObject(MaterialObject m)
{
    MaterialPtr mat = m.getMaterial();
    if (mat == NullFC)
        return;

    _materialObjects[mat].push_back(m);
}


// Generally used to prevent templated functions from resolving to the
// same signature.
template<typename T>
struct Type2Type {
    typedef T Original;
};


template<typename Chunk>
typename Chunk::Ptr getOrAddChunk(ChunkMaterialPtr cm,
                                  Type2Type<Chunk> = Type2Type<Chunk>()) {
    osg::StateChunkPtr stateChunk = cm->find(Chunk::getClassType());
    typename Chunk::Ptr chunk = Chunk::Ptr::dcast(stateChunk);
    if (!chunk) {
        chunk = Chunk::create();
        beginEditCP(cm);
        cm->addChunk(chunk);
        endEditCP(cm);
    }
    return chunk;
}


void MakeTransparentGraphOp::applyTransparency(MaterialPtr m) {
 
    SimpleMaterialPtr sm = SimpleMaterialPtr::dcast(m);
    if (sm != NullFC) {
        std::cout << "SimpleMaterial" << std::endl;
        beginEditCP(sm);
        sm->setTransparency(1.0f - (1.0f - sm->getTransparency()) * 
                            _transparency);
        sm->setColorMaterial(GL_NONE);
        endEditCP(sm);

        PolygonChunkPtr polygonChunk = getOrAddChunk<PolygonChunk>(sm);
        beginEditCP(polygonChunk);
        polygonChunk->setCullFace(GL_BACK);
        endEditCP(polygonChunk);
        return;
    }

    ChunkMaterialPtr cm = ChunkMaterialPtr::dcast(m);
    if (cm != NullFC) {
        std::cout << "ChunkMaterial" << std::endl;
        BlendChunkPtr blendChunk = getOrAddChunk<BlendChunk>(cm);
        beginEditCP(blendChunk);
        blendChunk->setColor(Color4f(1, 1, 1, 1.f - _transparency));
        blendChunk->setSrcFactor(GL_CONSTANT_ALPHA);
        blendChunk->setDestFactor(GL_ONE_MINUS_CONSTANT_ALPHA);
        endEditCP(blendChunk);
        return;
    }
}
