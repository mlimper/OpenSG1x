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

#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include <OSGNode.h>
#include <OSGBaseFunctions.h>
#include <OSGFieldContainer.h>
#include <OSGSFFieldContainerPtr.h>
#include <OSGMFFieldContainerPtr.h>
#include <OSGIDString.h>
#include <OSGBinaryDataHandler.h>

#include <OSGConfig.h>
#include "OSGBINWriter.h"

OSG_USING_NAMESPACE

/*! \class osg::OSGBINWriter
    OSGBINWriter documentation,
 */


/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */


BINWriter::BINWriter(FILE *file) :
           _outFileHandler(file)
{
}

BINWriter::FCInfo::FCInfo() :
        type(osg::IDString()),
        ptr (NullFC),
        mask(FieldBits::NoField)
{
}
 


BINWriter::BinaryFileHandler::BinaryFileHandler(FILE *file)
		: OSG::BinaryDataHandler(0), _file(file)
{
    _readMemory .resize(10000);
    _writeMemory.resize(10000);
    readBufAdd  (&_readMemory [0], _readMemory.size());
    writeBufAdd (&_writeMemory[0], _writeMemory.size());
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor documentation
 */
BINWriter::~BINWriter(void)
{
}

BINWriter::BinaryFileHandler::~BinaryFileHandler(void)
{
}

void BINWriter::addToIdMap(FieldContainerPtr fcPtr)
{
    if (fcPtr==NullFC)
	{
		SWARNING<<"ERROR in BINWriter::addToIdMap : NullFC"<<endl;
		return;
	}
	FCTypeIdMap::iterator iIdMap;

	//add new ContainerType
	if (_fcIdMap.find(fcPtr->getType().getCName())==_fcIdMap.end())
	{
		vector<UInt32> idVec;
		idVec.push_back(0);
		if (!(_fcIdMap.insert(pair<std::string, vector<UInt32> >
                        (fcPtr->getType().getCName(), idVec)).second))
        {
                SWARNING<<"BINWriter::addToIdMap(FieldContainerPtr):"<<endl
                        <<"ERROR while inserting into _fcIdMap"<<endl;
        };
	}
	
	//add container to the vector of appropriate type
    iIdMap = _fcIdMap.find(fcPtr->getType().getCName());
	iIdMap->second.push_back(fcPtr.getFieldContainerId());

	//increase the 0th entry of the vector (number of entries)
	iIdMap->second[0]++;
}

/*-------------------------------------------------------------------------*/
/*Write a file header containing the following:                            */
/*-------------------------------------------------------------------------*/
/* Number (k) of roots (osg::UInt32)                                       */
/*   {                                                                     */
/*       root IDs (osg::UInt32)                                            */
/*   } (k times)                                                           */
/* Number(i) of different containertypes (osg::UInt32)                     */
/*   {                                                                     */
/*   containertype (std::string)                                           */
/*   number(j) of appropriate containers (osg::UInt32)                     */
/*     {                                                                   */
/*     container Ids (osg::UInt32)                                         */
/*     } (j times)                                                         */
/*                                                                         */
/*   } (i times)                                                           */
/*-------------------------------------------------------------------------*/

void BINWriter::writeFileHeader()
{
	FCTypeIdMap    ::iterator iIdMap = _fcIdMap.begin();
	vector<NodePtr>::iterator iRoots = _vec_pRootNodes.begin();
    UInt32 mapSize    = _fcIdMap.size();
	UInt32 numOfRoots = _vec_pRootNodes.size();
    UInt32 i = 0, j = 0;
    
    _outFileHandler.putValue(numOfRoots);
    for(i=0; i!=numOfRoots; ++i, ++iRoots)
    {
       _outFileHandler.putValue(iRoots->getFieldContainerId());
    }

    _outFileHandler.putValue(mapSize);
   	for(i=0; i<mapSize ; ++i, ++iIdMap)
	{
		_outFileHandler.putValue(iIdMap->first);

        vector<UInt32>::iterator intIt  = iIdMap->second.begin();
        vector<UInt32>::iterator intEnd = iIdMap->second.end();

        while(intIt != intEnd)
        {
			_outFileHandler.putValue(*intIt);

            ++intIt;
		}
	}
}


void BINWriter::BinaryFileHandler
		::read(OSG::MemoryHandle mem, OSG::UInt32 size)
{
    fread(mem, size, 1, _file);
}

void BINWriter::BinaryFileHandler
		::write(OSG::MemoryHandle mem, OSG::UInt32 size)
{
    fwrite(mem, size, 1, _file);
}


void BINWriter::write(NodePtr node)
{
	_fcMap.clear();
	_fcIdMap.clear();
	_vec_pRootNodes.clear();
    _vec_pRootNodes.push_back(node);
	//traverse and index containers to write
	doIndexFC(node);
	//write the header with number and type of containers
	writeFileHeader();
	//write the containers and flush the filehandler
	doWriteIndexedFC();
	_outFileHandler.flush();
}

void BINWriter::write(vector<NodePtr> nodes)
{
    //does the same as write(NodePtr) for every Node in the vector
	_fcMap.clear();
	_fcIdMap.clear();
    _vec_pRootNodes.clear();
    _vec_pRootNodes = nodes;

    vector<NodePtr>::const_iterator iter = nodes.begin();
    for(; iter != nodes.end(); ++iter)
    {
        doIndexFC( *iter );
    }
	writeFileHeader();
	doWriteIndexedFC();
	_outFileHandler.flush();
}

void BINWriter::doIndexFC(FieldContainerPtr fieldConPtr)
{
    // traverse
	if(fieldConPtr == NullFC)
	{
		// nothing to do
        return;
	}

    FieldContainerType &fcType = fieldConPtr->getType();
	UInt32 i;

    // insert into map
    pair< map<UInt32, FCInfo>::iterator, bool>
        element = _fcMap.insert(pair<UInt32, FCInfo>(
            fieldConPtr.getFieldContainerId(),
            FCInfo()));
    // avoid loop. Element was already in the map
    if(element.second==false)
    {
        return;
    }
    // get inserted element
    FCInfo &fcInfo = element.first->second;

	//go through all fields
	for(i = 1; i <= fcType.getNumFieldDescs(); ++i)
    {
		FieldDescription *fDesc    = fcType.getFieldDescription(i);
        Field            *fieldPtr = fieldConPtr->getField(i);
        const FieldType  &fType    = fieldPtr->getType();

        // process all containers
        if(!fDesc->isInternal())
        { 
            //detect referenced containers #DIRTY-HACK#
			if( strstr(fType.getCName(), "Ptr") != NULL )
            { 
                //traverse the referenced SF/MF-containers
				if(fieldPtr->getCardinality() == FieldType::SINGLE_FIELD)
                { 
					doIndexFC(
                    ((SFFieldContainerPtr *) fieldPtr)->getValue());

                }
                else if(fieldPtr->getCardinality() == 
                        FieldType::MULTI_FIELD       )
                { 
					UInt32 j;
                    for(j = 0;
                        j < ((MFFieldContainerPtr*)fieldPtr)->size();
                        ++j)
                    { 
						doIndexFC((*(((MFFieldContainerPtr*)fieldPtr)))[j]);
                    }

                }

			}
            //attachments
			if( strcmp(fDesc->getCName(), "attachments") == 0 )
            { 
                AttachmentMap::const_iterator mapIt  = 
                    ((SFAttachmentMap*)fieldPtr)->getValue().begin();
                AttachmentMap::const_iterator mapEnd = 
                    ((SFAttachmentMap*)fieldPtr)->getValue().end();
                for(; mapIt != mapEnd; ++mapIt)
                {
                    doIndexFC(mapIt->second);
                }
            }
        }

        // create field mask

        // ignore internal fields except parent fields
        if(fDesc->isInternal() &&
           strcmp(fDesc->getCName(),"parent")!=0 &&
           strcmp(fDesc->getCName(),"parents")!=0 )
        {
            continue;
        }

        // ignore node volume
        if(fcType == Node::getClassType() &&
           fcType.getFieldDescription(i)->getFieldMask() == 
           Node::VolumeFieldMask)
        {
            continue;
        }

        // ignore empty mfields
        if(fieldPtr->getCardinality() == FieldType::MULTI_FIELD &&
           fieldPtr->isEmpty() == true)
        {
            continue;
        }

        // add field to fieldmask
        fcInfo.mask = (fcInfo.mask | fcType.getFieldDescription(i)
                       ->getFieldMask());

	}
	// add container to the Map of containers to write
    fcInfo.ptr  = fieldConPtr;
    fcInfo.type = fieldConPtr->getType().getName();
    // add container id to index
    addToIdMap(fcInfo.ptr);
}


void BINWriter::doWriteIndexedFC()
{
	FCInfoMap::const_iterator i=_fcMap.begin();
	UInt32 mapSize = _fcMap.size(),count = 1;

    for( ; i != _fcMap.end(); ++i, ++count)
	{
        SINFO<<"writing container "<<setw(4)
             <<count<<"/"<<mapSize<<"..."<<endl;
        //for each entry in _fcMap
	    //write ID
        _outFileHandler.putValue(i->first);
        //write FieldMask
        _outFileHandler.putValue(i->second.mask);
	    //write data
	    i->second.ptr->copyToBin(_outFileHandler, i->second.mask);
	}
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
    static Char8 cvsid_cpp[] = "@(#)$Id: $";
    static Char8 cvsid_hpp[] = OSGBINWRITER_HEADER_CVSID;
}

