/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *           Copyright (C) 2000,2001,2002 by the OpenSG Forum                *
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

#define OSG_COMPILESHADINGGROUPFIELDINST

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"

#include "OSGShadingGroupFields.h"

#include "OSGSFieldTypeDef.inl"
#include "OSGMFieldTypeDef.inl"

OSG_USING_NAMESPACE

DataType FieldDataTraits<ShadingGroup *>::_type("ShadingGroupP", 
                                                "ShadingType"  );

OSG_DLLEXPORT_SFIELD_DEF1(ShadingGroup *, OSG_SHADER_DLLMAPPING);

OSG_DLLEXPORT_MFIELD_DEF1(ShadingGroup *, OSG_SHADER_DLLMAPPING);


DataType FieldDataTraits<MFShadingGroupP>::_type("MFShadingGroupP", 
                                                 "ShadingType"  );

OSG_DLLEXPORT_SFIELD_DEF1(MFShadingGroupP, OSG_SHADER_DLLMAPPING);

OSG_DLLEXPORT_MFIELD_DEF1(MFShadingGroupP, OSG_SHADER_DLLMAPPING);


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
    static Char8 cvsid_hpp[] = OSGSHADINGGROUPFIELDS_HEADER_CVSID;
}

