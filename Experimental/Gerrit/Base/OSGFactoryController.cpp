/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *                 Copyright (C) 2000 by the OpenSG Forum                    *
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

#include <iostream>

#include "OSGFactoryController.h"
#include <OSGFactoryBase.h>
#include <OSGTypeFactory.h>
#include <OSGLog.h>

OSG_USING_NAMESPACE

#ifdef __sgi
#pragma set woff 1174
#endif

namespace 
{
    static Char8 cvsid_cpp[] = "@(#)$Id: $";
    static Char8 cvsid_hpp[] = OSGFACTORYCONTROLLER_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif

//---------------------------------------------------------------------------
//  Class
//---------------------------------------------------------------------------

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

FactoryController *FactoryController::_the = NULL;

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

FactoryController *FactoryController::the(void)
{
    if(_the == NULL)
        _the = new FactoryController;

    return _the;
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

FactoryController::FactoryController(void) :
    _pTypeFactory(NULL),
    _mFactoryMap (    )
{
}

FactoryController::~FactoryController(void)
{
}

bool FactoryController::initialize(void)
{
    bool returnValue = true;

    if(_pTypeFactory != NULL)
        returnValue = _pTypeFactory->initialize();

    if(returnValue == false)
        return returnValue;

    FactoryMapIt mapIt  = _mFactoryMap.begin();
    FactoryMapIt mapEnd = _mFactoryMap.end  ();

    while(mapIt != mapEnd)
    {
        returnValue &= mapIt->second->initialize();

        if(returnValue == false)
            break;

        mapIt++;
    }

    return returnValue;
}

bool FactoryController::terminate(void)
{
    bool returnValue = true;

    FactoryMapIt mapIt  = _mFactoryMap.begin();
    FactoryMapIt mapEnd = _mFactoryMap.end  ();

    while(mapIt != mapEnd)
    {
        returnValue &= mapIt->second->terminate();

        if(returnValue == false)
            break;

        mapIt++;
    }

    if(_pTypeFactory != NULL)
        returnValue = _pTypeFactory->terminate();

    return returnValue;
}

bool FactoryController::onLoadInitialize(void)
{
    bool returnValue = true;

    if(_pTypeFactory != NULL)
        returnValue = _pTypeFactory->onLoadInitialize();

    if(returnValue == false)
        return returnValue;

    FactoryMapIt mapIt  = _mFactoryMap.begin();
    FactoryMapIt mapEnd = _mFactoryMap.end  ();

    while(mapIt != mapEnd)
    {
        returnValue &= mapIt->second->onLoadInitialize();

        if(returnValue == false)
            break;

        mapIt++;
    }

    return returnValue;
}

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

bool FactoryController::registerTypeFactory(TypeFactory *pFactory)
{
    if(pFactory == NULL)
    {
		SWARNING << "no type factory given" << endl;

		return false;        
    }

	if(pFactory->getName().isEmpty() == true) 
    {
		SWARNING << "TypeFactory without name" << endl;

		return false;
	}

    if(findFactory(pFactory->getCName()) != NULL || _pTypeFactory != NULL)
    {
        SWARNING << "ERROR: Can't add a second "
                 << "type factory with the name " 
                 << pFactory->getCName()
                 << endl;
        
        return false;
    }

    _pTypeFactory = pFactory;

    PINFO << "Registered type factory " << pFactory->getCName() 
          << endl;

    return true;
}

bool FactoryController::registerFactory(FactoryBase *pFactory)
{
    if(pFactory == NULL)
    {
		SWARNING << "no factory given" << endl;

		return false;        
    }

	if(pFactory->getName().isEmpty() == true) 
    {
		SWARNING << "FactoryBase without name" << endl;

		return false;
	}

    if(findFactory(pFactory->getCName()) != NULL)
    {
        SWARNING << "ERROR: Can't add a second "
                 << "type with the name " << pFactory->getCName() << endl;

        return false;
    }

    _mFactoryMap[pFactory->getName()] = pFactory;
    
    PINFO << "Registered factory " << pFactory->getCName() 
          << endl;

    return true;
}

FactoryBase *FactoryController::findFactory(const Char8 *szName)
{
	FactoryMapConstIt factIt;

    if(szName == NULL)
        return NULL;

    factIt = _mFactoryMap.find(IDStringLink(szName));

    return ((factIt == _mFactoryMap.end()) ? NULL : (*factIt).second);
}
