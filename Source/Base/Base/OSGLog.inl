/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
 *                                                                           *
 *                            www.opensg.org                                 *
 *                                                                           *
 *         contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de    *
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

OSG_BEGIN_NAMESPACE

//---------------------------------------------------------------------------
//  LogStream
//---------------------------------------------------------------------------

inline
LogOStream::LogOStream(std::streambuf *buf) : 
    std::ostream(buf) 
{
}


inline
LogOStream::~LogOStream(void)
{
}

inline
void LogOStream::setrdbuf(std::streambuf *buf) 
{
#ifdef OSG_STREAM_RDBUF_HAS_PARAM
    std::ostream::rdbuf(buf); 
#else
    bp = buf; 
#endif
}

//---------------------------------------------------------------------------
//  LogBuf
//---------------------------------------------------------------------------

inline
LogBuf::Chunk::Chunk(void) : 
    data(NULL), 
    size(   0) 
{
}


inline
LogBuf::Chunk::~Chunk(void)
{
    delete data; 
}


inline
bool LogBuf::getEnabled(void)
{
    return _enabled; 
}


inline
void LogBuf::setEnabled(bool value)
{
    _enabled = value; 
}

//---------------------------------------------------------------------------
//  Log
//---------------------------------------------------------------------------

inline 
bool Log::checkLevel(LogLevel level)
{
    return (_logLevel >= level) ? true : false;
}

inline
Time Log::getRefTime(void)
{
    return _refTime;
}

inline
void Log::setRefTime(Time refTime)
{
    _refTime = refTime;
}

inline
void Log::resetRefTime(void)
{
    _refTime = getSystemTime();
}


inline
Log::Module::Module(void) : 
    name(NULL), 
    isStatic(true) 
{
}

inline
LogBuf & Log::getLogBuf(void)
{
  return _logBuf;
}

inline
std::ostream &Log::stream(LogLevel level)
{
    return *(_streamVec[level]); 
}

inline
std::ostream &Log::nilstream(void)
{
    return *_nilstreamP;
}

inline 
std::ostream &Log::doHeader(      LogLevel  level, 
                            const Char8    *module, 
                            const Char8    *file, 
                                  UInt32    line)
{
    LogOStream &sout = *(_streamVec[level]);

    if(_headerElem) 
    {
        if(_headerElem & LOG_BEGIN_NEWLINE_HEADER)
            sout << std::endl;   
        
        if(_headerElem & LOG_TYPE_HEADER)
            sout << _levelName[level] << ":";

        if(_headerElem & LOG_TIMESTAMP_HEADER) 
            sout << " ts: " << (getSystemTime() - _refTime);

        if(module && *module && (_headerElem & LOG_MODULE_HEADER))
            sout << " mod: " << module;

        if(file && *file && (_headerElem & LOG_FILE_HEADER)) 
        {
            sout << " file: " << file;

            if(_headerElem & LOG_LINE_HEADER)
                sout << ':' << line;
        }
        else
        {
            if(_headerElem & LOG_LINE_HEADER)
                sout << " line:" << line;
        }

        if(_headerElem & LOG_END_NEWLINE_HEADER)
            sout << std::endl;   
        else
            sout << ' ';
    }

    return sout;
}

inline 
void initLog(void) 
{
    if(osgLogP == NULL)
        doInitLog();
}

inline 
Log &osgLog(void) 
{
    initLog();

    return *osgLogP;
}

inline 
std::ostream &osgStartLog(      bool      logHeader,
                                LogLevel  level, 
                          const Char8    *module,
                          const Char8    *file, 
                                UInt32    line)
{
    initLog();

    osgLogP->lock();

    if(osgLogP->checkModule(module)) 
    {
        if(logHeader)
            osgLogP->doHeader(level,module,file,line);

        return osgLogP->stream(level);
    }
    else
        return osgLogP->nilstream();
}

inline  
std::ostream &endLog(std::ostream &strm)
{
    initLog();

    osgLogP->unlock();
    
    strm << std::endl;
    return strm;
}

inline 
void indentLog(UInt32 indent, std::ostream &stream)
{
    for(UInt32 i = 0; i < indent; i++)
    {
        stream << " ";
    }
}

OSG_END_NAMESPACE


