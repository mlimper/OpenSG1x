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

#define OSG_COMPILEBASELIB

#include <stdlib.h>
#include <stdio.h>

#include "OSGConfig.h"
#include "OSGLog.h"
#include "OSGBinaryDataHandler.h"
#include "OSGBaseFunctions.h"

OSG_USING_NAMESPACE

namespace 
{
    static char cvsid_cpp[] = "@(#)$Id: $";
    static char cvsid_hpp[] = OSGBINARYDATAHANDLER_HEADER_CVSID;
    static char cvsid_inl[] = OSGBINARYDATAHANDLER_INLINE_CVSID;
}

/***************************************************************************\
 *                               Types                                     *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/

/*------------- constructors & destructors --------------------------------*/

/** \brief Constructor
 */
BinaryDataHandler::BinaryDataHandler(UInt32 zeroCopyThreshold) :
    _readBuffers(),
    _writeBuffers(),
    _zeroCopyBuffers(),
    _zeroCopyThreshold(zeroCopyThreshold),
    _freeMem(),
    _currentReadBuffer(),
    _currentReadBufferPos(0),
    _currentWriteBuffer(),
    _currentWriteBufferPos(0)
{
}

/** \brief Destructor
 */
BinaryDataHandler::~BinaryDataHandler(void)
{
    freeMem();
}

/*------------------------------ access -----------------------------------*/

/*---------------------------- properties ---------------------------------*/

/*-------------------------- your_category---------------------------------*/

/** \brief but binary data
 */
void BinaryDataHandler::put(void const *src,UInt32 size)
{
    UInt8 const *data=static_cast<UInt8 const *>(src);

    if(_zeroCopyThreshold && size>=_zeroCopyThreshold)
    {
        if(_zeroCopyThreshold==1)
        {
            write(const_cast<MemoryHandle>(data),size);
        }
        else
        {
            UInt8 tag=1;
            // we have to write a tag, to indicate the membership
            // of this zero copy block to the current data block
            put(&tag,sizeof(tag));
            _zeroCopyBuffers.push_back(MemoryBlock(
                const_cast<MemoryHandle>(data),size,size));
        }
    }
    else
    {
        int copySize;
        while(size)
        {
            if(_currentWriteBuffer == writeBufEnd())
            {
                pushBuffer();
            }
            copySize=osgMin(_currentWriteBuffer->getSize()    -
                            _currentWriteBufferPos,
                            size);
            memcpy(_currentWriteBuffer->getMem() + 
                   _currentWriteBufferPos,
                   data,
                   copySize);
            size                    -=copySize;
            _currentWriteBufferPos  +=copySize;
            data                    +=copySize;
            // skip to next buffer if current buffer is full
            if(_currentWriteBufferPos == _currentWriteBuffer->getSize())
            {
                _currentWriteBuffer->setDataSize(_currentWriteBufferPos);
                _currentWriteBuffer++;
                _currentWriteBufferPos=0;
            }
        }
    }
}

/** Put data from dynamic allocated block
 *
 * The caller doesn't know, when the block will be written. So we are
 * responsible for freeing this block.
 */
void BinaryDataHandler::putAndFree(MemoryHandle src,UInt32 size)
{
    put(src,size);
    if(_zeroCopyThreshold && size>_zeroCopyThreshold)
    {
        _freeMem.push_back(src);
    }
    else
    {
        delete [] src;
    }
}

/** \brief get binary data
 */
void BinaryDataHandler::get(void *dst,UInt32 size)
{
    MemoryHandle data=static_cast<MemoryHandle>(dst);

    if(_zeroCopyThreshold && size>=_zeroCopyThreshold)
    {
        if(_zeroCopyThreshold>1)
        {
            UInt8 tag;
            // we have to read the tag, to force reading of data blocks
            // if the first data field was zero copied
            get(&tag,sizeof(tag));
        }
        // read direct into destination
        read(data,size);
    }
    else
    {
        int copySize;
        while(size)
        {
            // read new data if nothing left
            if(_currentReadBuffer == readBufEnd())
            {
                pullBuffer();
            }
            // num bytes to copy
            copySize=osgMin(_currentReadBuffer->getDataSize() -
                            _currentReadBufferPos,
                            size);
            // no data in buffer ?
            if(copySize)
            {
                memcpy(data,
                       _currentReadBuffer->getMem()+_currentReadBufferPos,
                       copySize);
                size                 -=copySize;
                _currentReadBufferPos+=copySize;
                data                 +=copySize;
            }
            // skip to next buffer if current buffer is full
            if(_currentReadBufferPos == _currentReadBuffer->getDataSize())
            {
                _currentReadBuffer++;
                _currentReadBufferPos=0;
            }
        }
    }
}

/** read data in a dynamic allocated block
 *
 * If data was compressed or converted then it is not possible to 
 * write it into the destination memory. BinaryDataHandler doesn't
 * store data in continous memory areas. If the caller needs a
 * continuous memory then we provide a dynamic allocated block.
 * The caller is responsible to free this block.
 */
void BinaryDataHandler::getAndAlloc(MemoryHandle &src,UInt32 size)
{
    src=new UInt8 [size];
    get(src,size);
}

/** \brief write data not written 
 */
void BinaryDataHandler::flush()
{
    if(_currentWriteBuffer != writeBufEnd())
    {
        // mark rest of buffer as empty
        _currentWriteBuffer->setDataSize(_currentWriteBufferPos);
        _currentWriteBuffer++;
        while(_currentWriteBuffer!=writeBufEnd())
        {
            _currentWriteBuffer->setDataSize(0);
            _currentWriteBuffer++;
        }
    }
    pushBuffer();
}

/*-------------------------- assignment -----------------------------------*/

/*-------------------------- comparison -----------------------------------*/

/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/

void BinaryDataHandler::readBufAdd(MemoryHandle mem,
                                   UInt32       size,
                                   UInt32       dataSize)
{
    _readBuffers.push_back(MemoryBlock(mem,size,dataSize));
    _currentReadBuffer=readBufEnd();
}

void BinaryDataHandler::readBufClear( void )
{
    _readBuffers.clear();
    _currentReadBuffer=readBufEnd();
}

void BinaryDataHandler::writeBufAdd(MemoryHandle mem,
                                    UInt32       size,
                                    UInt32       dataSize)
{
    _writeBuffers.push_back(MemoryBlock(mem,size,dataSize));
    _currentWriteBuffer    = writeBufBegin();
    _currentWriteBufferPos = 0;
}

void BinaryDataHandler::writeBufClear( void )
{
    _writeBuffers.clear();
    _currentWriteBuffer=writeBufEnd();
}

/** \brief default buffer read
 *
 * Use direct read to implement buffer read. First read buffer size
 * and then read rest of buffer
 *
 * @return buffer iterator points behind the last buffer containing data
 */
void BinaryDataHandler::read()
{
    BuffersT::iterator i;
    OSG::UInt32 size,readSize;

    // read buffer size
    read((MemoryHandle)&size,sizeof(size));
    // read rest of buffer
    for(i=readBufBegin();size;++i)
    {
        if(i==readBufEnd())
        {
            SFATAL << "Read buffer is to small. " << size
                   << "bytes missing" << endl;
            throw ReadError("Read buffer to small for whole block");
        }
        readSize=osgMin(size, i->getSize());
        read(i->getMem(),readSize);
        i->setDataSize(readSize);
        size-=readSize;
    }
    for(;i!=readBufEnd();++i)
    {
        i->setDataSize(0);
    }
}

/** \brief direct buffer read
 *
 * \param mem    destination 
 * \param size   destination size
 *
 * write data into given buffer
 *
 */
void BinaryDataHandler::read(MemoryHandle mem,UInt32 size)
{
    SWARNING << "BinaryDataHandler::read(MemoryHandle mem,int size) called" 
             << endl;
}

/** \brief default buffer write
 *
 * Use direct write to implement buffer write.
 *
 */
void BinaryDataHandler::write()
{
    BuffersT::iterator i;
    UInt32 size=0;

    // calculate blocksize
    for(i=writeBufBegin(); i!=writeBufEnd(); ++i)
        size+=i->getDataSize();
    // write buffer size
    write((MemoryHandle)&size,sizeof(size));
    // write buffers
    for(i=writeBufBegin(); i!=writeBufEnd(); ++i)
    {
        if(i->getDataSize())
        {
            write(i->getMem(),i->getDataSize());
        }
    }
}

/** \brief direct buffer write
 *
 * \param mem    source
 * \param size   source size
 *
 * write data into given buffer
 *
 */
void BinaryDataHandler::write(MemoryHandle mem,UInt32 size)
{
    SWARNING << "BinaryDataHandler::write(MemoryHandle mem,int size) called" 
             << endl;
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

void BinaryDataHandler::pushBuffer()
{
    BuffersT::iterator i,writeEnd;

    // write buffers
    write();
    // direct write zero copy buffers
    for(i =_zeroCopyBuffers.begin();
        i!=_zeroCopyBuffers.end();++i)
        write(i->getMem(),i->getDataSize());
    _zeroCopyBuffers.clear();
    // remove buffers given with getAndFree
    freeMem();
    // reset buffer pos
    _currentWriteBuffer          =writeBufBegin();
    _currentWriteBufferPos       =0;
    _currentWriteBuffer->setDataSize(0);
}

void BinaryDataHandler::pullBuffer()
{
    // read buffers
    read();
    _currentReadBuffer    = readBufBegin();
    _currentReadBufferPos = 0;
}

void BinaryDataHandler::freeMem()
{
    for(FreeMemT::iterator i=_freeMem.begin();i!=_freeMem.end();++i)
        delete [] *i;
    _freeMem.clear();
}






