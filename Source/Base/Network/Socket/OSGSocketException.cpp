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

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <OSGBase.h>
#include <OSGSocket.h>
#include <OSGSocketException.h>

OSG_BEGIN_NAMESPACE

/** \class osg::SocketException
 *  \ingroup GrpBaseNetwork
 *  \brief Base class for socket exceptions
 *  \author Marcus Roth
 **/

/** \brief Constructor
 */
SocketException::SocketException():
    Exception()
{
    _what+="SocketLib: ";
}

/** \class osg::SocketError
 *  \ingroup GrpBaseNetwork
 *  \brief Execption caused by socket error return codes
 *  \author Marcus Roth
 **/

/** \brief Constructor
 */
SocketError::SocketError(const char *function):SocketException()
{
    char estr[6];
    
    sprintf(estr,"%5d",Socket::getError());
    _what=_what + function + " " + estr + " " + Socket::getErrorStr();
}

/** \class osg::SocketHostError
 *  \ingroup GrpBaseNetwork
 *  \brief Execption caused by host name resolution
 *  \author Marcus Roth
 **/

/** \brief Constructor
 */
SocketHostError::SocketHostError(const char *function):SocketException()
{
    char estr[6];

    sprintf(estr,"%5d",Socket::getHostError());
    _what=_what + function + " " + estr + " " + Socket::getHostErrorStr(); 
}

/** \class osg::SocketInUse
 *  \ingroup GrpBaseNetwork
 *  \brief Execption caused by reusage of ports
 *  \author Marcus Roth
 **/

/** \brief Constructor
 */
SocketInUse::SocketInUse(const char *function):
    SocketError(function)
{
}

/** \class osg::SocketConnReset
 *  \ingroup GrpBaseNetwork
 *  \brief Execption caused by connection reset of remote endpoint
 *  \author Marcus Roth
 **/

/** \brief Constructor
 */
SocketConnReset::SocketConnReset(const char *function):
    SocketError(function)
{
}
OSG_END_NAMESPACE






