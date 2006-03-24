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

//-------------------------------
//      Includes                                    
//-------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>

#include <OSGConfig.h>

#include <iostream>
#include <fstream>

#include <OSGLog.h>
#include <OSGImageFileHandler.h>
#include <OSGPathHandler.h>
#include <OSGFileSystem.h>

#include "OSGHDRImageFileType.h"

OSG_USING_NAMESPACE

#define MINELEN 8           // minimum scanline length for encoding
#define MAXELEN 0x7fff      // maximum scanline length for encoding
#define MINRUN  4           // minimum run length
#define RED 0
#define GRN 1
#define BLU 2
#define EXP 3
#define COLXS 128

// copy source -> dest
#define copy_rgbe(c1, c2) (c2[RED]=c1[RED],c2[GRN]=c1[GRN],c2[BLU]=c1[BLU],c2[EXP]=c1[EXP])

/*! \class osg::HDRImageFileType 
    \ingroup GrpSystemImage

Image File Type to read/write and store/restore Image objects as
HDR data.

All the type specific code is included in the class. Does
not depend on external libs.
    
 */

/*****************************
 *   Types
 *****************************/
// Static Class Varible implementations: 

static const Char8 *suffixArray[] = 
{
    "hdr"
};

HDRImageFileType HDRImageFileType::_the( "hdr",
                                         suffixArray, sizeof(suffixArray),
                                         OSG_READ_SUPPORTED | 
                                         OSG_WRITE_SUPPORTED );


/*****************************
 *    Classvariables
 *****************************/


/********************************
 *    Class methodes
 *******************************/

//-------------------------------------------------------------------------
/*!
Class method to get the singleton Object
*/
HDRImageFileType& HDRImageFileType::the (void)
{
  return _the;
}

/*******************************
*public
*******************************/

//-------------------------------------------------------------------------
/*!
Tries to fill the image object with the data read from
the given input stream. Returns true on success.
*/
bool HDRImageFileType::read(ImagePtr &image, std::istream &is, const std::string &mimetype)
{
    int width, height;
    
    if (!checkHDR(is, width, height))
    {
        FWARNING(("No valid RADIANCE picture format\n"));
        return false;
    }

    image->set (Image::OSG_RGB_PF, width, height, 1, 1, 1, 0.0, 0, Image::OSG_FLOAT32_IMAGEDATA);
    image->clear();

    Real32 *data = ((Real32 *)(image->getData()));

    return radiance2fp(is, data, width, height);
}

//-------------------------------------------------------------------------
/*!
Tries to write the image object to the given output stream.
Returns true on success.
*/
bool HDRImageFileType::write(const ImagePtr &image, std::ostream &os, const std::string &mimetype)
{
    if(image->getDataType() != Image::OSG_FLOAT32_IMAGEDATA)
    {
        FWARNING(("HDRImageFileType::write: Image has non float data type!\n"));
        return false;
    }
    
    if (!os.good())
        return false;

    int width = image->getWidth();
    int height = image->getHeight();

    os << "#?RADIANCE" << std::endl;
    os << "# Written with OpenSG" << std::endl;
    os << "FORMAT=32-bit_rle_rgbe" << std::endl;
    os << "EXPOSURE=" << 1.0f << std::endl << std::endl;
    os << "-Y " << height << " +X " << width << std::endl;

    RGBE *rgbe_scan = new RGBE[width];

    Real32 *data = ((Real32 *)(image->getData()));

    //upside down !!!
    for(int y=height-1;y>=0;y--)
    {
        if (fwritecolrs(os, &data[y * width * 3], rgbe_scan, width, height) < 0)
        {
            delete [] rgbe_scan;
            return false;
        }
    }

    delete [] rgbe_scan;
    return true;
}

//-------------------------------------------------------------------------
/*!
Tries to fill the image object with the data read from
the given fileName. Returns true on success.
*/
bool HDRImageFileType::read (      ImagePtr &image,
                                   const Char8 *fileName )
{
    FILE *file = fopen(fileName, "rb");
    
    if(file == NULL && ImageFileHandler::the().getPathHandler())
    {
        // Try to find the file in the search path
        PathHandler *ph = ImageFileHandler::the().getPathHandler();
        file = fopen(ph->findFile(fileName).c_str(), "rb");
    }

    if(file == NULL)
        return false;

    int width, height;
    if(!checkHDR(file, width, height))
    {
        fclose(file);
        return false;
    }

    image->set (Image::OSG_RGB_PF, width, height, 1, 1, 1, 0.0, 0, Image::OSG_FLOAT32_IMAGEDATA);
    image->clear();

    Real32 *data = ((Real32 *)(image->getData()));

    bool ok = radiance2fp(file, data, width, height);
    fclose(file);
    return ok;
}

//-------------------------------------------------------------------------
/*!
Tries to write the image object to the given fileName.
Returns true on success.
*/
bool HDRImageFileType::write(const ImagePtr &image, 
                             const Char8 *fileName)
{
    if(image->getDataType() != Image::OSG_FLOAT32_IMAGEDATA)
    {
        FWARNING(("HDRImageFileType::write: Image has non float data type!\n"));
        return false;
    }

    FILE *file = fopen(fileName, "wb");
    
    if(file == NULL)
        return false;

    int width = image->getWidth();
    int height = image->getHeight();

    fprintf(file, "#?RADIANCE");
    fputc(10, file);
    fprintf(file, "# %s", "Written with OpenSG");
    fputc(10, file);
    fprintf(file, "FORMAT=32-bit_rle_rgbe");
    fputc(10, file);
    fprintf(file, "EXPOSURE=%25.13f", 1.0);
    fputc(10, file);
    fputc(10, file);
    fprintf(file, "-Y %d +X %d", height, width);
    fputc(10, file);

    RGBE *rgbe_scan = new RGBE[width];

    Real32 *data = ((Real32 *)(image->getData()));

    //upside down !!!
    for(int y=height-1;y>=0;y--)
    {
        if (fwritecolrs(file, &data[y * width * 3], rgbe_scan, width, height) < 0)
        {
            fclose(file);
            delete [] rgbe_scan;
            return false;
        }
    }
    fclose(file);
    delete [] rgbe_scan;
    return true;
}


//-------------------------------------------------------------------------
/*!
Tries to restore the image data from the given memblock.
Returns the amount of data read.
*/
UInt64 HDRImageFileType::restoreData(      ImagePtr &image, 
                                     const UChar8   *buffer,
                                           Int32   OSG_CHECK_ARG(memSize) )
{
    image->setData(buffer);

    return image->getSize();
}

//-------------------------------------------------------------------------
/*!
Tries to store the image data to the given memblock.
Returns the amount of data written.
*/
UInt64 HDRImageFileType::storeData(const ImagePtr &image, 
                                         UChar8   *buffer,
                                         Int32     OSG_CHECK_ARG(memSize))
{
    UInt32 dataSize = image->getSize();
    const UChar8 *src = image->getData();

    if ( dataSize && src && buffer )
        memcpy( buffer, src, dataSize);
  
    return dataSize;
} 


//-------------------------------------------------------------------------
/*!
Constructor used for the singleton object
*/
HDRImageFileType::HDRImageFileType ( const Char8 *mimeType,
                                     const Char8 *suffixArray[], 
                                     UInt16 suffixByteCount,
                                     UInt32 flags  )
    : ImageFileType ( mimeType, suffixArray, suffixByteCount, flags )
{
    FLOG(( "HDRImageFileType: %s %d\n", mimeType, flags ));
}

//-------------------------------------------------------------------------
/*!
Destructor
*/
HDRImageFileType::~HDRImageFileType(void)
{
}

// check header to see if this is really a HDR file
// if so get the resolution information
bool HDRImageFileType::checkHDR(FILE *file, int &width, int &height)
{
    char cs[256], st1[80], st2[80];
    bool resok = false;
    bool HDRok = false;
    while(!feof(file) && !resok)
    {
        fgets(cs, 255, file);
        if(strstr(cs, "32-bit_rle_rgbe"))
            HDRok = true;
        if(strcmp(cs, "\n") == 0)
        {
            // empty line found, next is resolution info, format: -Y N +X N
            // directly followed by data
            fgets(cs, 255, file);
            sscanf(cs, "%s %d %s %d", (char*)&st1, &height, (char*)&st2, &width);
            resok = true;
        }
    }
    return HDRok;
}

// check header and get resolution (streaming type)
bool HDRImageFileType::checkHDR(std::istream &is, int &width, int &height)
{
    char cs[256], st1[80], st2[80];
    bool resok = false;
    bool HDRok = false;
    int i = 0;

    while (!is.eof() && !resok)
    {
        is.getline(cs, 255);

        if (strstr(cs, "32-bit_rle_rgbe"))
            HDRok = true;

        if (HDRok && (cs[0] == '\r' || cs[0] == '\n' || cs[0] == '\0'))
        {
            // empty line found, next is resolution info, format: -Y N +X N
            // directly followed by data
            is.getline(cs, 255);
        
            i = sscanf(cs, "%s %d %s %d", st1, &height, st2, &width);
            if (i == 4) 
                resok = true;
        }
    }
    
    return HDRok;
}

// convert radiance hdr to float image
bool HDRImageFileType::radiance2fp(FILE *file, Real32 *data, int width, int height)
{
    int x,y,yx;
    RGBE *sline = new RGBE[width];

    for(y=height-1;y>=0;y--)
    {
        yx = y*width;
        if (!freadcolrs(file, sline, width))
            return false;
        Real32 *fcol = &data[yx * 3];
        for (x=0;x<width;x++)
        {
            RGBE2Float(sline[x], fcol);
            fcol += 3;
        }
    }
    delete[] sline;
    return true;
}

// convert radiance hdr to float image (streaming type)
bool HDRImageFileType::radiance2fp(std::istream &is, Real32 *data, int width, int height)
{
    int x,y,yx;
    RGBE *sline = new RGBE[width];

    if (!sline)
        return false;

    for(y=height-1;y>=0;y--)
    {
        yx = y*width;
        if (!freadcolrs(is, sline, width))
            return false;
        Real32 *fcol = &data[yx * 3];
        for (x=0;x<width;x++)
        {
            RGBE2Float(sline[x], fcol);
            fcol += 3;
        }
    }
    delete[] sline;
    
    return true;
}

// read and possibly RLE decode a rgbe scanline
bool HDRImageFileType::freadcolrs(FILE *file, RGBE *scan, int width)
{
    int i,j,code,val,size;

    if((width < MINELEN) | (width > MAXELEN))
        return oldreadcolrs(file, scan, width);

    if((i = getc(file)) == EOF)
        return false;

    if(i != 2)
    {
        ungetc(i, file);
        return oldreadcolrs(file, scan, width);
    }

    scan[0][GRN] = (unsigned char)getc(file);
    scan[0][BLU] = (unsigned char)getc(file);

    if((i = getc(file)) == EOF)
        return false;

    size = ((int)scan[0][BLU]) << 8;
    if ( (size | i) != width )
        return false;

    for(i=0;i<4;i++)
    {
        for (j=0;j<width;)
        {
            if ((code = getc(file)) == EOF)
                return false;
            if (code > 128)
            {
                code &= 127;
                val = getc(file);
                while (code--)
                    scan[j++][i] = (unsigned char)val;
            }
            else
            {
                while (code--)
                  scan[j++][i] = (unsigned char)getc(file);
            }
        }
    }
    return feof(file) ? false : true;
}

// read and decode a rgbe scanline (streaming type)
bool HDRImageFileType::freadcolrs(std::istream &is, RGBE *scan, int width)
{
    int i,j,code,val,size;
    unsigned char byte;

    if ((width < MINELEN) | (width > MAXELEN))
    {
        FWARNING(("Sorry, format probably too old\n"));
        return false;
    }

    byte = (unsigned char)is.get();
    if (is.eof())
        return false;

    byte = (unsigned char)is.get();
    scan[0][GRN] = byte;

    byte = (unsigned char)is.get();
    scan[0][BLU] = byte;

    size = ((int)scan[0][BLU]) << 8;
    i = is.get();

    if ( (size | i) != width )
        return false;

    for(i=0;i<4;i++)
    {
        for (j=0;j<width;)
        {
            if (is.eof())
                return false;
            
            code = is.get();
                
            if (code > 128)
            {
                code &= 127;
                val = is.get();
                
                while (code--)
                    scan[j++][i] = (unsigned char)val;
            }
            else
            {
                while (code--) 
                  scan[j++][i] = is.get();
            }
        }
    }
    
    return is.eof() ? false : true;
}

// old format
bool HDRImageFileType::oldreadcolrs(FILE *file, RGBE *scan, int width)
{
    int i, rshift = 0, len = width;
    while (len > 0)
    {
        scan[0][RED] = (unsigned char)getc(file);
        scan[0][GRN] = (unsigned char)getc(file);
        scan[0][BLU] = (unsigned char)getc(file);
        scan[0][EXP] = (unsigned char)getc(file);
        if(feof(file) || ferror(file))
            return false;
        if(scan[0][RED] == 1 && scan[0][GRN] == 1 && scan[0][BLU] == 1)
        {
            for (i=scan[0][EXP]<<rshift;i>0;i--)
            {
                copy_rgbe(scan[-1], scan[0]);
                scan++;
                len--;
            }
            rshift += 8;
        }
        else
        {
            scan++;
            len--;
            rshift = 0;
        }
    }
    return true;
}

//rgbe -> float color
void HDRImageFileType::RGBE2Float(RGBE rgbe, Real32 *fcol)
{
    if (rgbe[EXP] == 0)
    {
        *(fcol + RED) = *(fcol + GRN) = *(fcol + BLU) = 0;
    }
    else
    {
        Real32 f = ldexp(1., rgbe[EXP]-(COLXS+8));
        *(fcol + RED) = (rgbe[RED]+.5)*f;
        *(fcol + GRN) = (rgbe[GRN]+.5)*f;
        *(fcol + BLU) = (rgbe[BLU]+.5)*f;
    }
}

int HDRImageFileType::fwritecolrs(FILE *file, Real32 *scan, RGBE *rgbe_scan,
                                  int width, int height)
{
    int i, j, beg, c2, cnt=0;
    // convert scanline
    for (i=0;i<width;i++)
    {
        float2RGBE(scan, rgbe_scan[i]);
        scan += 3;
    }

    if ((width < MINELEN) | (width > MAXELEN))	// OOBs, write out flat
          return (fwrite((char *)rgbe_scan, sizeof(RGBE), width, file) - width);

    // put magic header
    putc(2, file);
    putc(2, file);
    putc((unsigned char)(width>>8), file);
    putc((unsigned char)(width&255), file);

    // put components seperately
    for (i=0;i<4;i++)
    {
        for (j=0;j<width;j+=cnt)
        {	// find next run
            for (beg=j;beg<width;beg+=cnt)
            {
                for(cnt=1;(cnt<127) && ((beg+cnt)<width) && (rgbe_scan[beg+cnt][i] == rgbe_scan[beg][i]); cnt++);
                if (cnt>=MINRUN)
                    break;   // long enough
            }
            if (((beg-j)>1) && ((beg-j) < MINRUN))
            {
                c2 = j+1;
                while (rgbe_scan[c2++][i] == rgbe_scan[j][i])
                if (c2 == beg)
                {        // short run
                    putc((unsigned char)(128+beg-j), file);
                    putc((unsigned char)(rgbe_scan[j][i]), file);
                    j = beg;
                    break;
                }
            }
            while (j < beg)
            {     // write out non-run
                if ((c2 = beg-j) > 128)
                    c2 = 128;
                putc((unsigned char)(c2), file);
                while (c2--)
                    putc(rgbe_scan[j++][i], file);
            }
            if (cnt >= MINRUN)
            {      // write out run
                putc((unsigned char)(128+cnt), file);
                putc(rgbe_scan[beg][i], file);
            }
            else
            {
                cnt = 0;
            }
        }
    }
    return(ferror(file) ? -1 : 0);
}

int HDRImageFileType::fwritecolrs(std::ostream &os, Real32 *scan, RGBE *rgbe_scan,
                                  int width, int height)
{
    int i, j, beg, c2, cnt=0;
    
    // convert scanline
    for (i=0;i<width;i++)
    {
        float2RGBE(scan, rgbe_scan[i]);
        scan += 3;
    }

    if ((width < MINELEN) | (width > MAXELEN))
    {
        // OOBs, write out flat
        os.write((char *)rgbe_scan, width);
        return 0;
    }

    // put magic header
    os << (unsigned char)2;
    os << (unsigned char)2;
    os << (unsigned char)(width>>8);
    os << (unsigned char)(width&255);

    // put components seperately
    for (i=0;i<4;i++)
    {
        for (j=0;j<width;j+=cnt)
        {
            // find next run
            for (beg=j;beg<width;beg+=cnt)
            {
                for(cnt=1; (cnt<127)&&((beg+cnt)<width)&&(rgbe_scan[beg+cnt][i]==rgbe_scan[beg][i]); cnt++)
                    ;
                if (cnt>=MINRUN)
                    break;
                    // long enough
            }
            if (((beg-j)>1) && ((beg-j) < MINRUN))
            {
                c2 = j+1;
                while (rgbe_scan[c2++][i] == rgbe_scan[j][i])
                {
                    if (c2 == beg)
                    {
                        // short run
                        os << (unsigned char)(128+beg-j);
                        os << (unsigned char)(rgbe_scan[j][i]);
                        j = beg;
                        break;
                    }
                }
            }
            while (j < beg)
            {
                // write out non-run
                if ((c2 = beg-j) > 128)
                    c2 = 128;
                os << (unsigned char)(c2);
                
                while (c2--)
                    os << rgbe_scan[j++][i];
            }
            if (cnt >= MINRUN)
            {
                // write out run
                os << (unsigned char)(128+cnt);
                os << rgbe_scan[beg][i];
            }
            else
            {
                cnt = 0;
            }
        }
    }
    return (os.fail() ? -1 : 0);
}

//float color -> rgbe
void HDRImageFileType::float2RGBE(Real32 *fcol, RGBE rgbe)
{
    Real32 d = (*(fcol + RED) > *(fcol + GRN)) ? *(fcol + RED) : *(fcol + GRN);
    
    if(*(fcol + BLU) > d)
        d = *(fcol + BLU);
    if(d <= 1e-32f)
    {
        rgbe[RED] = rgbe[GRN] = rgbe[BLU] = rgbe[EXP] = 0;
    }
    else
    {
        int e;
        d = frexp(d, &e) * 256.f / d;
        rgbe[RED] = (unsigned char)(*(fcol + RED) * d);
        rgbe[GRN] = (unsigned char)(*(fcol + GRN) * d);
        rgbe[BLU] = (unsigned char)(*(fcol + BLU) * d);
        rgbe[EXP] = (unsigned char)(e + COLXS);
    }
}
