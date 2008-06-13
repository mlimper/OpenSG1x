/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2003 by the OpenSG Forum                 *
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

#include "OSGDXFEntitiesEntry.h"

#include "OSGGeometry.h"
#include "OSGSimpleAttachments.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class DXFEntitiesEntry
    \ingroup GrpSystemFileIO

  This class implements the evaluation of group codes common for all entries of
  the ENTITIES and BLOCKS sections.

 */

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*================================ PUBLIC =================================*/

/*=============================== PROTECTED ===============================*/

/*================================ PRIVATE ================================*/


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*================================ PUBLIC =================================*/

/*=============================== PROTECTED ===============================*/

/*! Evaluate records common for all entities inside ENTITIES section
 *  with the following group codes:
 *  - 8 -- Layer name 
 *  - 6 -- Linetype name
 *  - 62 -- Color number
 *  - 48 -- Linetype scale
 *  - 39 -- Thickness
 *  - 210, 220, 230 -- Extrusion direction
 */
DXFResult DXFEntitiesEntry::evalRecord(void)
{
	DXFResult state = DXFStateContinue;
	switch( DXFRecord::getGroupCode() )
	{
		case 8:
			_layerName = DXFRecord::getValueStr();
			break;
		case 6:
			_linetypeName = DXFRecord::getValueStr();
			break;
		case 62:
			_colorNumber = DXFRecord::getValueInt();
			break;
		case 48:
			_linetypeScale = DXFRecord::getValueDbl();
			break;
		case 39:
			_thickness = DXFRecord::getValueDbl();
			break;
		case 210:
			_extrusionDirection[0] = DXFRecord::getValueDbl();
			break;
		case 220:
			_extrusionDirection[1] = DXFRecord::getValueDbl();
			break;
		case 230:
			_extrusionDirection[2] = DXFRecord::getValueDbl();
			break;
		default:
			state = DXFStateUnknown;
	}
	if(state == DXFStateUnknown)
		state = Inherited::evalRecord();
	return state;
}

DXFResult DXFEntitiesEntry::beginEntity(void)
{	
	DXFResult state;
	state = Inherited::beginEntity();

	_layerName = "";
	_linetypeName = "";
	_colorNumber = 256;
	_linetypeScale = 1.0;
	_thickness = 0.0;
	_extrusionDirection.setValues(0.0, 0.0, 1.0);

	return state;
}

NodePtr DXFEntitiesEntry::getParentNode(void)
{
	NodePtr parentNode = NullFC;

	if(std::string("BLOCK") != _parent->getEntityTypeName())
	{
		StringToNodePtrMap::iterator itr = _layersMapP->find(_layerName);
		if(itr == _layersMapP->end())
		{
			// Reference to an unknown LAYER.
			FWARNING(("DXF Loader: in line %d: "
					  "LAYER '%s' not found for new %s\n",
					  DXFRecord::getLineNumber(),
					  _layerName.c_str(),
					  getEntityTypeName()
					 ));
#if 1 // DEBUG
			for( itr =  _layersMapP->begin();
				 itr != _layersMapP->end();
				 ++ itr);
			{
				FDEBUGx(("Known Layer: %s\n",
						itr->first.c_str()));
			}
#endif
			// TODO: Alternatively add to default layer?
		}
		else
		{
			parentNode  = itr->second;
		}
	}
	else
	{
		parentNode = _parent->getNode();
	}

	return parentNode;
}

/*! Implements the arbitrary axis algorithm of AutoCAD.
 *
 *  See 
 *  http://www.autodesk.com/techpubs/autocad/acad2000/dxf/arbitrary_axis_algorithm_dxf_ab.htm
 */
Matrix DXFEntitiesEntry::arbitraryAxisAlgorithm(const Vec3f &normal)
{
	Vec3f aX, aY, aZ;
	const Vec3f WY(0.0, 1.0, 0.0);
	const Vec3f WZ(0.0, 0.0, 1.0);
	
	const Real32 Bound = 1.0/64.0;

	aZ = normal;
	aZ.normalize();
	
	if(osgabs(aZ[0]) < Bound && osgabs(aZ[1]) < Bound)
		aX = WY % aZ;
	else
		aX = WZ % aZ;	
	aX.normalize();

	aY = aZ % aX;
	aY.normalize();

	
	return Matrix(aX, aY, aZ);
}



/*! Set up GeoProperties for derived entity types which fetch geometry data.
 */

DXFResult DXFEntitiesEntry::beginGeometry(void)
{
	// prepare geometry properties 
	// TODO: moeglicherweise k�nnen diese Pointer gleich auf was gescheites
	// gesetzt werden: Geometrieknoten des Layers!
	_pointsP       = GeoPositions3d::create();
	_pointIndicesP = GeoIndicesUI32::create();	// for meshes
	_faceLengthP   = GeoPLengthsUI32::create();
	_faceTypeP     = GeoPTypesUI8::create();

	addRefCP(_pointsP);
	addRefCP(_pointIndicesP);	
	addRefCP(_faceLengthP);
	addRefCP(_faceTypeP);

	beginEditCP(_pointsP);
	beginEditCP(_pointIndicesP);
	beginEditCP(_faceLengthP);
	beginEditCP(_faceTypeP);

	_uiNumFacePoints = 0;
	_uiCurrentFaceType = -1;	// no current GL primitive

    return DXFStateContinue;
}


/*! Clear GeoProperties.
 */

DXFResult DXFEntitiesEntry::endGeometry(void)
{
	// The corresponding beginEditCPs are performed in beginEntity()	
	endEditCP(_faceTypeP);
	endEditCP(_faceLengthP);
	endEditCP(_pointIndicesP);
	endEditCP(_pointsP);

	// Clean up: perform subRefCP() for all FCPtrs there was an addRefCP in
	// beginEntity() or this method before. Unused FCs will be destroyed then,
	// the others will survive!
	subRefCP(_pointsP);
	subRefCP(_pointIndicesP);
	subRefCP(_faceTypeP);
	subRefCP(_faceLengthP);

	_pointsP       = NullFC;
	_pointIndicesP = NullFC;
	_faceTypeP     = NullFC;
	_faceLengthP   = NullFC;

    return DXFStateContinue;
}


/*! Creates new geometry node and core and sets the properties from the fetched
 *  data (this is enforced by setting \c forceNewNode to \c true) or appends
 *  fetched geometry data to existing Geometry Node. The distinction is made as
 *  follows: if there is a GeoIndices property set, a new Node is
 *  created. Otherwise the data is appended.
 *  
 */
void DXFEntitiesEntry::flushGeometry(bool forceNewNode)
{
	bool doNewNode  = true;
	NodePtr     geoNode = NullFC;
	GeometryPtr	geoCore = NullFC;

	NodePtr parentNode = getParentNode();

	if(! forceNewNode)
	{
		for(MFNodePtr::const_iterator itr = parentNode->getMFChildren()->begin();
			itr != parentNode->getMFChildren()->end();
			++ itr)
		{
			geoNode = *itr;  // can't be NullFC (see Node::addChild())
			geoCore = GeometryPtr::dcast(geoNode->getCore());
			if(geoCore == NullFC)
				continue;
				
			// Look for a node with non indexed geometry TODO: here goes the
			// additional check for matching line/color attibutes
			if(geoCore->getIndices() == NullFC)
			{
				doNewNode = false;
				break;
			}
		}
	}

	if(doNewNode)
	{
		geoNode = Node::create();
		geoCore = Geometry::create();
	
		beginEditCP(geoNode);
		{
			geoNode->setCore(geoCore);
			setName(geoNode, _entityTypeName +"="+ _layerName +"-"+ _handle);
			FDEBUGx(("DXFEntitiesEntry::flushGeometry(): "
					 "%s attached name '%s'\n",
					 getEntityTypeName(),
					 getName(geoNode)
					));
		}
		endEditCP(geoNode);
		// put the geometry core together
		beginEditCP(geoCore);
		{
			geoCore->setPositions(_pointsP);
			geoCore->setLengths  (_faceLengthP);
			geoCore->setTypes    (_faceTypeP);
			if(_pointIndicesP->size() > 0)
				geoCore->setIndices(_pointIndicesP);	
		}
		endEditCP(geoCore);

		beginEditCP(parentNode, Node::ChildrenFieldMask);
		{
			parentNode->addChild(geoNode);
		}
		endEditCP(parentNode, Node::ChildrenFieldMask);	
	}
	else
	{
		// Append new vertex data to existing geometry core
		// Here we don't have indexed geometry, thus we only have to
		// copy the positions, lengths and types
		
		GeoPositions3dPtr pointsP =  
			GeoPositions3dPtr::dcast(geoCore->getPositions());
		MFPnt3d *pointsF =  pointsP->getFieldPtr();
		_pointsF         = _pointsP->getFieldPtr();
		
		GeoPLengthsUI32Ptr faceLengthP =  
			GeoPLengthsUI32Ptr::dcast(geoCore->getLengths());
		MFUInt32 *faceLengthF =  faceLengthP->getFieldPtr();
		_faceLengthF          = _faceLengthP->getFieldPtr();

		GeoPTypesUI8Ptr faceTypeP =  
			GeoPTypesUI8Ptr::dcast(geoCore->getTypes());
		MFUInt8 *faceTypeF =  faceTypeP->getFieldPtr();
		_faceTypeF         = _faceTypeP->getFieldPtr();
		
        
		beginEditCP(geoCore);
		{
#if 1 // TODO: remove when OSGMField.h/inl defines a range insert
            for(MFPnt3d::iterator itr = _pointsF->begin();
				itr != _pointsF->end();
				++itr)
                pointsF->push_back(*itr);
            
            for(MFUInt32::iterator itr = _faceLengthF->begin();
				itr != _faceLengthF->end();
				++itr)
                faceLengthF->push_back(*itr);
            
            for(MFUInt8::iterator itr = _faceTypeF->begin();
				itr != _faceTypeF->end();
				++itr)
                faceTypeF->push_back(*itr);
            
#else // TODO: use, when OSGMField.h/inl defines a range insert
			pointsF->insert(pointsF->end(),
							_pointsF->begin(), 
							_pointsF->end());
			faceLengthF->insert(faceLengthF->end(),
								_faceLengthF->begin(),
								_faceLengthF->end());
			faceTypeF->insert(faceTypeF->end(),
							  _faceTypeF->begin(),
							  _faceTypeF->end());
#endif
		}
		endEditCP(geoCore);
	}
}


/*================================ PRIVATE ================================*/

/*-------------------------------------------------------------------------*/

/*------------------------- constructors ----------------------------------*/

/*! Define derived entity types as of class ENTITY. Sets default
 *  values to common entity entries data.
 *
 * \see  beginEntity()
 * \todo
 * do registerToParentEntityType() here? Problem: can VERTEX be a direct
 * "child" of DXFEntities? How do we deal with prepare the geometry data? Will
 * this be done in beginEntity for all ENTITIES entries?
 */
DXFEntitiesEntry::DXFEntitiesEntry(void) :
    Inherited(),
	_layerName(),
	_linetypeName(),
	_colorNumber(256),
	_linetypeScale(1.0),
	_thickness(0.0),
	_extrusionDirection(0.0, 0.0, 1.0)
{
	_entityClassName = "ENTITY";

//	registerToParentEntityType("SECTION:ENTITIES");
//	registerToParentEntityType("BLOCKSENTRY:BLOCK");
}

/*-------------------------- destructor -----------------------------------*/

/*! Destructor documentation
 */
DXFEntitiesEntry::~DXFEntitiesEntry(void)
{
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
    static Char8 cvsid_cpp[] = "@(#)$Id: OSGDXFEntitiesEntry.cpp,v 1.3 2008/06/13 04:59:48 vossg Exp $";
    static Char8 cvsid_hpp[] = OSGDXFENTITIESENTRY_HEADER_CVSID;
//    static Char8 cvsid_inl[] = OSGDXFENTITIESENTRY_INLINE_CVSID;
}

