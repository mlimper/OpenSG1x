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

#ifndef _OSGNFIOBASE_H_
#define _OSGNFIOBASE_H_
#ifdef __sgi
#pragma once
#endif

#include <string>
#include <map>
#include <list>
#include <set>
#include <iostream>

#include <OSGSystemDef.h>
#include <OSGNode.h>
#include <OSGBinaryDataHandler.h>
#include <OSGSFFieldContainerPtr.h>
#include <OSGMFFieldContainerPtr.h>
#include <OSGSFAttachmentMap.h>
#include <OSGNFIOOptions.h>

OSG_BEGIN_NAMESPACE

/* \brief NFIOBase class.
          Base class for native file io.
*/

class OSG_SYSTEMLIB_DLLMAPPING NFIOBase
{
    /*==========================  PUBLIC  =================================*/
  public:

    /*---------------------------------------------------------------------*/
    /*! \name                  Constructor                                 */
    /*! \{                                                                 */

    NFIOBase                     (const char *name);
  
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Destructor                                */
    /*! \{                                                                 */

    virtual ~NFIOBase            (void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Read/Write                                */
    /*! \{                                                                 */
  
    static NodePtr  read    (std::istream &is,
                             const std::string &options);
    static bool     write   (const NodePtr &node, std::ostream &os,
                             const std::string &options);
  
    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/
    
protected:

    /*---------------------------------------------------------------------*/
    /*! \name                      Options                                 */
    /*! \{                                                                 */

    static const NFIOOptions &getOptions(void);

    /*! \}                                                                 */
    /*! \name                      Read                                    */
    /*! \{                                                                 */

    virtual FieldContainerPtr readFC(const std::string &typeName) = 0;
    
    static std::string readFCFields    (const FieldContainerPtr &fc,
                                        const std::string &exclude = "",
                                        const std::string &endMarkers = "");
    static void readSFFieldContainerPtr(const FieldContainerPtr &fc,
                                        const BitVector &mask,
                                        Field *field                );
    static void readMFFieldContainerPtr(const FieldContainerPtr &fc,
                                        const BitVector &mask,
                                        Field *field                );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Write                                     */
    /*! \{                                                                 */

    virtual void writeFC(const FieldContainerPtr &fc) = 0;

    static void writeFCFields           (const FieldContainerPtr &fc,
                                         const std::string &exclude = "",
                                         bool endMarker = true);
    static void writeSFFieldContainerPtr(SFFieldContainerPtr *field  );
    static void writeMFFieldContainerPtr(MFFieldContainerPtr *field  );
    static void writeSFAttachmentMap    (SFAttachmentMap *amap       );
    
    static void writeEndMarker          (void                        );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Skip                                      */
    /*! \{                                                                 */
    
    static void skipFCFields(void);
        
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                 Binary Read/Write Handler                    */
    /*! \{                                                                 */

    class BinaryReadHandler : public BinaryDataHandler
    {
      public:
        BinaryReadHandler(std::istream &is);
        virtual ~BinaryReadHandler();

        void read(MemoryHandle mem, UInt32 size);
        void skip(UInt32 size);

      private:

        std::vector<UInt8>  _readMemory;
        std::istream &_is;
      
        BinaryReadHandler(const BinaryReadHandler &source);
        void operator =(const BinaryReadHandler &source);
    };

    class BinaryWriteHandler : public BinaryDataHandler
    {
      public:
        BinaryWriteHandler(std::ostream &os);
        virtual ~BinaryWriteHandler();
    
        void write(MemoryHandle mem, UInt32 size);
    
      private:
    
        std::vector<UInt8> _writeMemory;
        std::ostream &_os;
    
        BinaryWriteHandler(const BinaryWriteHandler &source);
        void operator =(const BinaryWriteHandler &source);
    };
    
    static BinaryReadHandler    *_in;
    static BinaryWriteHandler   *_out;

    UInt16 _version;

    /*! \}                                                                 */ 
    /*==========================  PRIVATE  ================================*/
private:
    
    /*---------------------------------------------------------------------*/
    /*! \name                      Read                                    */
    /*! \{                                                                 */

    static FieldContainerPtr readFieldContainer(void);

    struct fcInfo
    {
        fcInfo(const FieldContainerPtr &fc, const BitVector &mask,
               UInt32 id, Field *field                            );
        fcInfo(const FieldContainerPtr &fc, const BitVector &mask,
               Field *field                                       );
        
        bool isMultiField(void) const;
        
        FieldContainerPtr   _fc;
        BitVector           _mask;
        UInt32              _id;
        Field               *_field;
        std::list<UInt32>   _ids;
    };
    
    static void addReadFieldContainer  (const FieldContainerPtr &fc,
                                        UInt32 id                   );
    static void chargeFieldPtr         (const fcInfo &info          );
    
    typedef std::map<UInt32, FieldContainerPtr> fcMap;
    
    static fcMap                _fcMap;
    static std::list<fcInfo>    _fieldList;
    
    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Write                                  */
    /*! \{                                                                 */
    
    static void writeFieldContainer (const FieldContainerPtr &fc);
    static void writeFCId           (const FieldContainerPtr &fc);
    
    static std::list<FieldContainerPtr>  _fcList;
    static std::set<UInt32>              _fcSet;
    
    static NFIOOptions _options;

};

OSG_END_NAMESPACE

#define OSGNFIOBASE_HEADER_CVSID "@(#)$Id: OSGNFIOBase.h,v 1.1 2004/01/08 18:00:04 a-m-z Exp $"

#endif /* _OSGNFIOBASE_H_ */
