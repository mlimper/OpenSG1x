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

#ifndef _SOCKETADDRESS_H_
#define _SOCKETADDRESS_H_
#ifdef __sgi
#pragma once
#endif

#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------
#include <string>
#include "OSGConfig.h"
#include "OSGBase.h"

OSG_BEGIN_NAMESPACE

class OSG_BASE_DLLMAPPING SocketAddress {
    /*==========================  PUBLIC  =================================*/
  public:

    /*! \enum SocketAddress type */
    enum Type { ANY,BROADCAST,MULTICAST };

#if   defined(WIN32)
    typedef          char       SocketAddrT;
#elif defined(darwin) || defined(__hpux)
    typedef          const char SocketAddrT;
#else
    typedef struct ::socketaddr SocketAddrT;
#endif

    static const char *getClassname(void) { return "Address"; }
 
    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    SocketAddress(const char *host,int port=0);
    SocketAddress(Type type=ANY,int port=0);
    SocketAddress(const SocketAddress &source);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructor                                 */
    /*! \{                                                                 */

    virtual ~SocketAddress();
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Get/Set                                    */
    /*! \{                                                                 */

    sockaddr       *getSockAddr    (                       ) const;
    int             getSockAddrSize(                       ) const;
    void            setPort        (int port               );
    void            setHost        (const std::string &host);
    int             getPort        (                       ) const;
    std::string     getHost        (                       ) const;
    std::string     getHostByName  (                       ) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Comparision                                */
    /*! \{                                                                 */

    bool operator == (const SocketAddress &other) const;
    bool operator != (const SocketAddress &other) const;
    bool operator < (const SocketAddress &other) const;

    /*! \}                                                                 */
    /*==========================  PROTECTED ===============================*/
  protected:
    /*---------------------------------------------------------------------*/
    /*! \name                   member                                     */
    /*! \{                                                                 */

    struct sockaddr_in _sockaddr;

    /*! \}                                                                 */
};

OSG_END_NAMESPACE

#define OSG_SOCKET_ADDRESS_HEADER_CVSID "@(#)$Id: $"

#endif
