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
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "OSGConfig.h"
#include "OSGLog.h"
#include "OSGBaseThread.h"
#include "OSGSocketSelection.h"
#include "OSGBinaryMessage.h"
#include "OSGPointSockPipeline.h"
#include "OSGGroupSockPipeline.h"
#include "OSGConnectionType.h"
#include "OSGBinaryMessage.h"

OSG_USING_NAMESPACE

/** \class osg::PointSockPipeline
 *  \ingroup GrpBaseNetwork
 **/

/*-------------------------------------------------------------------------*/
/*                            constructor destructor                       */

/*! Constructor
 */

PointSockPipeline::PointSockPipeline():
    Inherited(),
    _initialized(false)
{
    _prev.open();
    _next.open();
    printf("point pipe\n");
}

/*! Destructor
 */
PointSockPipeline::~PointSockPipeline(void)
{
    _prev.close();
    _next.close();
}

/*! get connection type
 */
const ConnectionType *PointSockPipeline::getType()
{
    return &_type;
}

/*-------------------------------------------------------------------------*/
/*                            connection                                   */

/*! connect to the given group. If timeout is reached, -1 is
    returned
 */
Connection::Channel PointSockPipeline::connectGroup(
    const std::string &address,
    Time               timeout)
{
    Channel channel = Inherited::connectGroup(address,timeout);
    return channel;
}

/*! disconnect the given channel
 */
void PointSockPipeline::disconnect(void)
{
    _socket.close();
}

/*! accept an icomming grop connection. If timeout is reached,
    -1 is returned. If timeout is -1 then wait without timeout
 */
Connection::Channel PointSockPipeline::acceptGroup(Time timeout)
{
    Channel channel = Inherited::acceptGroup(timeout);
    return channel;
}

/*-------------------------------------------------------------------------*/
/*                              channel handling                           */

/*! select the next channel for reading. If timeout is not -1
    then -1 is returned if timeout is reached
*/
Connection::Channel PointSockPipeline::selectChannel(Time timeout)
    throw (ReadError)
{
    if(!_initialized)
        initialize();
    try
    {
        if(_prev.waitReadable(timeout))
            return 0;
    }
    catch(SocketError &e)
    {
        throw ReadError(e.what());
    }
    return -1;
}

/*-------------------------- create ---------------------------------------*/

/** \brief create conneciton
 */

PointConnection *PointSockPipeline::create(void)
{
    return new PointSockPipeline();
}

/*-------------------------------------------------------------------------*/
/*                              read write                                 */

/** Read data into given memory
 *
 * Read data form the current read socket. The read socket is that
 * socket, that was selectet in selectChannel.
 *
 **/

void PointSockPipeline::read(MemoryHandle mem,UInt32 size)
{
    int len;

    if(!_initialized)
        initialize();
    // read data
    len=_prev.recv(mem,size);
    if(len==0)
    {
        throw ReadError("read got 0 bytes!");
    }
    // send to next in chain
    if(!_last)
        _next.send(mem,size);
}

/** Read next data block
 *
 * The stream connection uses only BinaryDataHandler buffer. If more
 * then one buffer is present, then this methode must be changed!
 *
 */

void PointSockPipeline::readBuffer()
{
    int size;
    int len;

    if(!_initialized)
        initialize();

    // read buffer header
    len=_prev.recv(&_socketReadBuffer[0],sizeof(SocketBufferHeader));
    //printf("recv1 %d\n",len);
    if(len==0)
        throw ReadError("peek got 0 bytes!");
    // read remaining data
    size=ntohl(((SocketBufferHeader*)&_socketReadBuffer[0])->size);
    len=_prev.recv(&_socketReadBuffer[sizeof(SocketBufferHeader)],
                   size);
    //printf("recv2 %d\n",len);
    if(len==0)
        throw ReadError("read got 0 bytes!");
    readBufBegin()->setDataSize(size);
    // send to next in chain
    if(!_last)
        _next.send(&_socketReadBuffer[0],
                   sizeof(SocketBufferHeader)+size);
}    

/*-------------------------------------------------------------------------*/
/*                              private helpers                            */

/*! initialize pipeline
 */
void PointSockPipeline::initialize(void)
{
    BinaryMessage message;
    StreamSocket  sock;
    UInt32        nextPort;
    std::string   nextHost;
    UInt32        len;
    char          localhost[256];
    std::string   interf;

    // get local host name
    gethostname(localhost,255);
    if(!getInterface().empty())
        interf = getInterface();
    else
        interf = localhost;

    sock.open();
    sock.bind(SocketAddress(interf.c_str(),0));
    sock.listen();

    printf("init point\n");

    // send my own address
    message.putString(interf);
    message.putUInt32(sock.getAddress().getPort());
    _socket.send(message);
    // accept prev
    _prev = sock.accept();
    sock.close();

    len = _socket.recv(message);
    if(len == 0)
        throw ReadError("Channel closed\n");
    _last = message.getUInt32();
    printf("last %d\n",_last);
    if(!_last)
    {
        nextHost = message.getString();
        nextPort = message.getUInt32();
        for(;;)
        {
            try
            {
                _next.connect(SocketAddress(nextHost.c_str(),
                                            nextPort));
                break;
            }
            catch(...)
            {
            }
        }
    }

    _initialized = true;
    printf("init ok\n");
}

/*-------------------------------------------------------------------------*/
/*                              static type                                */

ConnectionType PointSockPipeline::_type(
    &PointSockPipeline::create,
    "SockPipeline");

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
    static Char8 cvsid_cpp       [] = "@(#)$Id: $";
    static Char8 cvsid_hpp       [] = OSG_GROUPSOCKPIPELINE_HEADER_CVSID;
}

