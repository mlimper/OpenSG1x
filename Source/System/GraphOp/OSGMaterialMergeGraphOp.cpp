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

#include <OSGMaterialMergeGraphOp.h>
#include "OSGImage.h"
#include <stdio.h>


OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::MaterialMergeGraphOp
    \ingroup GrpSystemNodeCoresDrawablesGeometry
    
Merges equivalent materials in a scene.
*/

MaterialMergeGraphOp::MaterialMergeGraphOp(const char* name)
    : GraphOp(name)
{
}

GraphOp* MaterialMergeGraphOp::create()
{
    return new MaterialMergeGraphOp();
}


// Similar to boost's next iterator function.  (I think it's from
// boost, at least.)
template<typename T>
T next(T t) { // Iterator passed by value.
    ++t;
    return t;
}

bool isEqualImages(const osg::FieldContainerPtr& a, const osg::FieldContainerPtr& b)
{
	ImagePtr imageA = ImagePtr::dcast(a);
	ImagePtr imageB = ImagePtr::dcast(b);

	if(imageA == NullFC || imageB == NullFC )
	{	return false;	}

	if( imageA->getBpp() != imageB->getBpp() ) return false;
	if( imageA->getForceAlphaBinary() != imageB->getForceAlphaBinary() ) return false;
	if( imageA->getForceAlphaChannel() != imageB->getForceAlphaChannel() ) return false;
	if( imageA->getForceColorChannel() != imageB->getForceColorChannel() ) return false;
	if( imageA->getFrameCount() != imageB->getFrameCount() ) return false;
	if( imageA->getPixelFormat() != imageB->getPixelFormat() ) return false;

	// check if images are the same height and width
	if( imageA->getWidth() == imageB->getWidth() && imageA->getHeight() == imageB->getHeight() )
	{
		unsigned int width  = imageA->getWidth();
		unsigned int height = imageA->getHeight();

		const UInt8* dataA = imageA->getData();
		const UInt8* dataB = imageB->getData();

		// check if every pixel is the same
		for(unsigned int i=0; i<width*height; i++)
		{
			if( dataA[i] != dataB[i] )
			{
				return false; // return that images are NOT equal if the first different pixel is found
			}
		}

		return true; // images are equal
	}
	else // image size is not equal
		return false;
}

bool isEqual(const osg::FieldContainerPtr& a, const osg::FieldContainerPtr& b)
{
    using std::string;

    // Compare the pointers.
    if(a == b)
        return true;

    if(a == NullFC || b == NullFC)
        return false;

    if(a->getType() != b->getType())
        return false;
	
	// check if field is an image
	if(a->getType() == Image::getClassType() )
	{
		return isEqualImages(a,b);
	}

    //printf("comparing: %s\n", a->getType().getName().str());
    
    const FieldContainerType &type = a->getType();
    UInt32 fcount = type.getNumFieldDescs();
    
    for(UInt32 i=1;i <= fcount;++i)
    {
        const FieldDescription* fdesc = type.getFieldDescription(i);
    
        if(fdesc->isInternal())
            continue;
    
        // ignore attachments
        if(strcmp(fdesc->getCName(), "attachments") == 0)
            continue;
    
        // ignore parents
        if(strcmp(fdesc->getCName(), "parents") == 0)
            continue;
            
        Field *a_field = a->getField(i);
        Field *b_field = b->getField(i);
    
        const FieldType &a_ftype = a_field->getType();
        const FieldType &b_ftype = b_field->getType();
    
        //printf("comparing field %s\n", a_ftype.getCName());
    
        if(a_ftype != b_ftype)
            return false;
    
        if(strstr(a_ftype.getCName(), "Ptr") == NULL)
        {
            // This would be nice but there is no virtual compare method :-(
            // if(*a_field != *b_field)
            //     return false;
    
            // This is very slow with multi fields!!!!
            string av, bv;
            a_field->getValueByStr(av);
            b_field->getValueByStr(bv);			

            if(av != bv)
                return false;
        }
        else
        {
            if(a_field->getCardinality() == FieldType::SINGLE_FIELD)
            {
                if(!isEqual(static_cast<SFFieldContainerPtr *>(a_field)->getValue(),
                            static_cast<SFFieldContainerPtr *>(b_field)->getValue()))
                    return false;
            }
            else if(a_field->getCardinality() == FieldType::MULTI_FIELD)
            {
                if(static_cast<MFFieldContainerPtr*>(a_field)->size() !=
                   static_cast<MFFieldContainerPtr*>(b_field)->size())
                    return false;
    
                for(UInt32 j=0;j < static_cast<MFFieldContainerPtr*>(a_field)->size();++j)
                {
                    if(!isEqual((*(static_cast<MFFieldContainerPtr *>(a_field)))[j],
                                (*(static_cast<MFFieldContainerPtr *>(b_field)))[j]))
                        return false;
                }
            }
        }
    }
    return true;
}


bool equal(MaterialPtr a, MaterialPtr b) {
    // It's too bad Material::operator== doesn't work.
    //return *a == *b;
    return isEqual(a, b);
}


bool MaterialMergeGraphOp::traverse(NodePtr& node)
{
    // Find the materials.
    if (!GraphOp::traverse(node)) {
        return false;
    }

    SINFO << "Number of materials before merge: " << _materialObjects.size() << std::endl;

    // Now do the merge.
    MaterialObjectMap::iterator itr = _materialObjects.begin();
    for (; itr != _materialObjects.end(); ++itr)
    {
        MaterialPtr current = itr->first;
        MaterialObjectList& currentList = itr->second;

        MaterialObjectMap::iterator walker = ::next(itr);
        while (walker != _materialObjects.end()) {
            // Store the next iterator in case we have to delete
            // 'walker' from the map.
            MaterialObjectMap::iterator nextStep = ::next(walker);

            if (equal(current, walker->first)) {
                // Set the new objects to have the current material,
                // and move the objects to the current list.
                MaterialObjectList::iterator i = walker->second.begin();
                for (; i != walker->second.end(); ++i) {
                    i->setMaterial(current);
                    currentList.push_back(*i);
                }
                _materialObjects.erase(walker);
            }

            walker = nextStep;
        }
    }

    SINFO << "Number of materials after merge: " << _materialObjects.size() << std::endl;
    return true;
}


void MaterialMergeGraphOp::setParams(const std::string params)
{
    ParamSet ps(params);   
    
    std::string out = ps.getUnusedParams();
    if(out.length())
    {
        FWARNING(("MaterialMergeGraphOp doesn't have parameters '%s'.\n",
                out.c_str()));
    }
}

std::string MaterialMergeGraphOp::usage(void)
{
    return 
    "MaterialMerge: merge Materials in given subtree\n"
    "  Tries to find and merge equiavlent Materials to reduce the number\n"
    "  of Materials used.\n"
    ;
}

Action::ResultE MaterialMergeGraphOp::traverseEnter(NodePtr& node)
{
    GeometryPtr geo = GeometryPtr::dcast(node->getCore());
    if (geo != NullFC)
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

Action::ResultE MaterialMergeGraphOp::traverseLeave(NodePtr& node, Action::ResultE res)
{
    return res;
}

void MaterialMergeGraphOp::addObject(MaterialObject m)
{
    MaterialPtr mat = m.getMaterial();
    if (mat == osg::NullFC)
        return;

    _materialObjects[mat].push_back(m);
}

