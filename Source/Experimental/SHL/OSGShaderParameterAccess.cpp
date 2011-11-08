/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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

#include <OSGConfig.h>

#include "OSGShaderParameterAccess.h"

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::ShaderParameterAccess

*/

/*----------------------- constructors & destructors ----------------------*/

ShaderParameterAccess::ShaderParameterAccess(MFShaderParameterPtr &parameters) :
    _parameters(parameters),
    _parametermap(),
    _mapsize(0)
{
}

ShaderParameterAccess::~ShaderParameterAccess(void)
{
}

void ShaderParameterAccess::operator =(const ShaderParameterAccess &/*source*/)
{
}

bool ShaderParameterAccess::subParameter(const char *name)
{
    if(name == NULL)
        return false;

    updateMap();
    parameterIt it = _parametermap.find(name);
    
    if(it == _parametermap.end())
        return false;

    subRefCP(_parameters[(*it).second]);
    _parameters.erase(_parameters.begin() + (*it).second);
    _parametermap.erase(it);
    updateMap();

    return true;
}

bool ShaderParameterAccess::hasParameter(const char *name)
{
    if(name == NULL)
        return false;

    updateMap();

    return (_parametermap.find(name) != _parametermap.end());
}

void ShaderParameterAccess::updateMap(void)
{
    if(_mapsize == _parameters.size())
        return;
    
    UInt32 size = _parameters.size();
    _parametermap.clear();
    for(UInt32 i=0;i<size;++i)
        _parametermap.insert(std::pair<std::string, UInt32>(_parameters[i]->getName(), i));
    _mapsize = size;
}
