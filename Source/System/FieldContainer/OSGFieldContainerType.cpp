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

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include <iostream>

#include <algorithm>

#include "OSGFieldContainerType.h"
#include "OSGFieldContainerFactory.h"

#include "OSGFieldDescription.h"

#include "OSGNodePtr.h"
#include "OSGAttachmentPtr.h"

#include "OSGNodeCore.h"
#include "OSGAttachment.h"

#if 0
#include "OSGNodeCore.h"
#endif

OSG_USING_NAMESPACE

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

/*-------------------------------------------------------------------------*/
/*                              Register                                   */

inline
void FieldContainerType::registerType(const Char8 *szGroupName)
{
    FieldContainerFactory::the()->registerType (this);

    _uiGroupId = FieldContainerFactory::the()->registerGroup(
        szGroupName != NULL ? szGroupName : _szName.str());
}

/*! \class osg::FieldContainerType
 */

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

FieldContainerType::FieldContainerType(const Char8    *szName,
                                   const Char8        *szParentName,
                                   const Char8        *szGroupName,
                                   PrototypeCreateF    fPrototypeCreate,
                                   InitContainerF      fInitMethod,
                                   FieldDescription  **pDesc,
                                   UInt32              uiDescByteCounter,
                                   bool                bDescsAddable) :
     Inherited        (szName, 
                       szParentName     ),
    _uiGroupId        (0                ),

    _bInitialized     (false            ),
    _bDescsAddable    (bDescsAddable    ),

    _baseType         (IsFieldContainer ),

    _pParent          (NULL             ),

    _szParentName     (szParentName     ),
    _szGroupName      (szGroupName      ),

    _pPrototype       (NullFC           ),
    _fPrototypeCreate (fPrototypeCreate ),

    _pDesc            (pDesc            ),
    _uiDescByteCounter(uiDescByteCounter),

    _mDescMap         (                 ),
    _vDescVec         (0                ),
    _bCopy            (false            )
{
    registerType(szGroupName);

    if(fInitMethod != NULL)
        fInitMethod();
}

FieldContainerType::FieldContainerType(const FieldContainerType &obj) :

     Inherited        (obj                   ),
    _uiGroupId        (obj._uiGroupId        ),

    _bInitialized     (false                 ),
    _bDescsAddable    (obj._bDescsAddable    ),

    _baseType         (obj._baseType         ),

    _pParent          (obj._pParent          ),

    _szParentName     (obj._szParentName     ),
    _szGroupName      (obj._szGroupName      ),

    _pPrototype       (obj._pPrototype       ),
    _fPrototypeCreate (obj._fPrototypeCreate ),

    _pDesc            (obj._pDesc            ),
    _uiDescByteCounter(obj._uiDescByteCounter),

    _mDescMap         (                      ),
    _vDescVec         (0                     ),
    _bCopy            (true                  )
{
    if(_pPrototype != NullFC)
        addRefCP(_pPrototype);

    initFields();
    initParentFields();

    _bInitialized = true;
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

FieldContainerType::~FieldContainerType(void)
{
    if(GlobalSystemState != Shutdown)
    {
        terminate();
        if(_bCopy == false)
        {
            FieldContainerFactory::the()->unregisterType(this);
        }
    }
}

/*-------------------------------------------------------------------------*/
/*                            Add / Sub                                    */

UInt32 FieldContainerType::addDescription(const FieldDescription &desc)
{
    UInt32            returnValue = 0;
    DescMapConstIt    descIt;
    DescVecIt         descVIt;

    FieldDescription  *pDesc;
    FieldDescription  *pNullDesc = NULL;

    if(_bDescsAddable == false)
        return returnValue;

    descIt = _mDescMap.find(IDStringLink(desc.getCName()));

    if(desc.isValid())
    {
        if(descIt == _mDescMap.end())
        {
            pDesc = new FieldDescription(desc);

            _mDescMap[IDStringLink(pDesc->getCName())] = pDesc;

            descVIt = std::find(_vDescVec.begin(),
                                _vDescVec.end(),
                                 pNullDesc);

            if(descVIt == _vDescVec.end())
            {
                _vDescVec.push_back(pDesc);

                returnValue = _vDescVec.size();
            }
            else
            {
                (*descVIt) = pDesc;

                returnValue  = descVIt - _vDescVec.begin();
                returnValue += 1;
            }
        }
        else
        {
            SWARNING << "ERROR: Double field description "
                        << "in " << _szName.str() << " from "
                        << desc.getCName() << " (id:"
                        << desc.getTypeId() << ")" << std::endl;
        }
    }
    else
    {
        SWARNING << "ERROR: Invalid field description "
                    << "in " << _szName.str() << " from "
                    << desc.getTypeId() << std::endl;
    }

    return returnValue;
}

bool FieldContainerType::subDescription(UInt32 uiFieldId)
{
    FieldDescription  *pDesc = getFieldDescription(uiFieldId);
    DescMapIt          descMIt;
    DescVecIt          descVIt;
    bool               returnValue = true;

    if(pDesc == NULL || _bDescsAddable == false)
        return false;

    descMIt = _mDescMap.find(IDStringLink(pDesc->getCName()));

    if(descMIt != _mDescMap.end())
    {
        _mDescMap.erase(descMIt);
    }
    else
    {
        returnValue = false;
    }

    descVIt = std::find(_vDescVec.begin(), _vDescVec.end(), pDesc);

    if(descVIt != _vDescVec.end())
    {
        (*descVIt) = NULL;

        returnValue &= true;
    }
    else
    {
        returnValue = false;
    }

    delete pDesc;

    return returnValue;
}


/*-------------------------------------------------------------------------*/
/*                               Create                                    */

FieldContainerPtr FieldContainerType::createFieldContainer(void) const
{
    FieldContainerPtr fc;

    if(isAbstract()       == false)
    {
        fc = _pPrototype->shallowCopy();
    }

    return fc;

}

NodePtr  FieldContainerType::createNode(void) const
{
    NodePtr fc;

    if(isAbstract() == false &&
       isNode()     == true)
    {
        fc = NodePtr::dcast(_pPrototype->shallowCopy());
    }

    return fc;
}

NodeCorePtr FieldContainerType::createNodeCore(void) const
{
    NodeCorePtr fc;

    if(isAbstract() == false &&
       isNodeCore() == true)
    {
        fc = NodeCorePtr::dcast(_pPrototype->shallowCopy());
    }

    return fc;
}

AttachmentPtr FieldContainerType::createAttachment(void) const
{
    AttachmentPtr fc;

    if(isAbstract()   == false &&
       isAttachment() == true)
    {
        fc = AttachmentPtr::dcast(_pPrototype->shallowCopy());
    }

    return fc;
}


/*-------------------------------------------------------------------------*/
/*                                Dump                                     */

void FieldContainerType::dump(      UInt32    OSG_CHECK_ARG(uiIndent),
                              const BitVector OSG_CHECK_ARG(bvFlags )) const
{
    SLOG << "FieldContainerType: "
         << getCName()
         << ", Id: "       
         << getId()
         << ", parentP: " 
         << (_pParent ? _pParent->getCName() : "NONE")
         << ", groupId: "  
         << _uiGroupId
         << ", abstract: "
         << ((_pPrototype != NullFC) ? "false" : "true")
         << " "
         << _vDescVec.size()
         << std::endl;

    for(UInt32 i = 0; i < _vDescVec.size(); i++)
    {
        SLOG << "Desc : " << _vDescVec[i]->getCName() << std::endl;
    }
}


/*-------------------------------------------------------------------------*/
/*                                Init                                     */

bool FieldContainerType::initPrototype(void)
{
    _bInitialized = true;

    if(_fPrototypeCreate != NULL)
    {
        _pPrototype = _fPrototypeCreate();

        addRefCP(_pPrototype);
    }

    return _bInitialized;
}

bool FieldContainerType::initBaseType(void)
{
    if     (isDerivedFrom(NodeCore::getClassType())   == true)
    {
        _baseType = IsNodeCore;
    }
    else if(isDerivedFrom(Attachment::getClassType()) == true)
    {
        _baseType = IsAttachment;
    }
    else if(isDerivedFrom(Node::getClassType())       == true)
    {
        _baseType = IsNode;
    }

    return true;
}

bool FieldContainerType::initFields(void)
{
    UInt32    i;
    DescMapIt descIt;

    _bInitialized = true;

	if(_pDesc == NULL)
		return true;

    for(i = 0; i < _uiDescByteCounter / sizeof(FieldDescription *); i++)
    {
        if(_pDesc[i]->isValid())
        {
            descIt = _mDescMap.find(IDStringLink(_pDesc[i]->getCName()));

            if(descIt == _mDescMap.end())
            {
                _mDescMap[IDStringLink(_pDesc[i]->getCName())] = _pDesc[i];

                _vDescVec.push_back(_pDesc[i]);
            }
            else
            {
                SWARNING << "ERROR: Double field description "
                            << "in " << _szName.str() << " from "
                            << _pDesc[i]->getCName() << " (id:"
                            << _pDesc[i]->getTypeId() << ")" << std::endl;

                _bInitialized = false;
            }
        }
        else
        {
            SWARNING << "ERROR: Invalid field description "
                        << "in " << _szName.str() << "from "
                        << (_pDesc[i]?_pDesc[i]->getTypeId():0) << std::endl;

            _bInitialized = false;
        }

    }

    std::sort(_vDescVec.begin(), _vDescVec.end(), FieldDescriptionPLT());

    return _bInitialized;
}

bool FieldContainerType::initParentFields(void)
{
    DescMapIt dPIt;

    _bInitialized = true;

    if(_szParentName.str() != NULL)
    {
        _pParent =
            FieldContainerFactory::the()->findType(_szParentName.str());

        if(_pParent == NULL)
        {
            _pParent =
                FieldContainerFactory::the()->findUninitializedType(
                    _szParentName.str());
        }

        if(_pParent != NULL)
        {
            _bInitialized = _pParent->initialize();

            if(_bInitialized == false)
            {
                return _bInitialized;
            }

            for(  dPIt  = _pParent->_mDescMap.begin();
                  dPIt != _pParent->_mDescMap.end();
                ++dPIt)
            {
                if(_mDescMap.find((*dPIt).first) == _mDescMap.end())
                {
                    _mDescMap[(*dPIt).first] = (*dPIt).second;
                }
                else
                {
                    SWARNING << "ERROR: Can't add field "
                                << "description a second time: "
                                << (*dPIt).first.str() << std::endl;
                }
            }

            _vDescVec.insert(_vDescVec.end(),
                             _pParent->_vDescVec.begin(),
                             _pParent->_vDescVec.end());

        }
        else
        {
            SWARNING << "ERROR: Can't find type with "
                        << "name " << _szParentName.str()
                        << std::endl;

            _bInitialized = false;
        }
    }

    return _bInitialized;
}

bool FieldContainerType::initialize(void)
{
    if(_bInitialized == true)
        return _bInitialized;

    _bInitialized = initParentFields();

    if(_bInitialized == false)
        return _bInitialized;

    _bInitialized = initFields      ();

    if(_bInitialized == false)
        return _bInitialized;

    _bInitialized = initPrototype   ();

    if(_bInitialized == false)
        return _bInitialized;

    _bInitialized = initBaseType    ();

    FDEBUG ( ( "init FieldContainerType %s (%d)\n",
               _szName.str(), int(_bInitialized) ));

    if(_vDescVec.size() > sizeof(BitVector) * 8)
    {
        FWARNING(("FCType %s has %d (>%d) fields!\n", getCName(),
                    _vDescVec.size(), sizeof(BitVector) * 8));
    }
    
    return _bInitialized;
}

void FieldContainerType::terminate(void)
{
    UInt32 i;

    subRefCP(_pPrototype);

    _bInitialized = false;

    for(i = 0; i < _uiDescByteCounter / sizeof(FieldDescription *); i++)
    {
        delete _pDesc[i];
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
    static Char8 cvsid_hpp[] = OSGFIELDCONTAINERTYPE_HEADER_CVSID;
}
