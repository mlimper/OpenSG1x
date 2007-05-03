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

#include <OSGConfig.h>

#include <OSGNodePtr.h>
#include <OSGImage.h>

#include <OSGTextTXFFace.h>
#include <OSGTextTXFGlyph.h>
#include <OSGTextLayoutParam.h>
#include <OSGTextLayoutResult.h>

#include <OSGViewport.h>

#include "OSGSimpleStatisticsForeground.h"

#include "OSGStatisticsDefaultFont.h"

OSG_USING_NAMESPACE

/*! \class osg::SimpleStatisticsForeground
    \ingroup GrpSystemWindowForegroundsStatistics

SimpleStatisticsForeground displays the Statistics info as simple text lines.
See \ref PageSystemWindowForegroundStatisticsSimple for a description.

The format string for the given elements are stored in the _mfFormats Field,
the size and color used for all lines in _sfSize and _sfColor.
*/

/*----------------------- constructors & destructors ----------------------*/
SimpleStatisticsForeground::SimpleStatisticsForeground(void) :
    Inherited(), _face(0), _texchunk()
{
}

SimpleStatisticsForeground::SimpleStatisticsForeground(const SimpleStatisticsForeground &source) :
    Inherited(), _face(source._face), _texchunk(source._texchunk)
{
    if (_face != 0)
        addRefP(_face);
    if (_texchunk != NullFC)
        addRefCP(_texchunk);
}

/* */
SimpleStatisticsForeground::~SimpleStatisticsForeground(void)
{
    if (_face != 0)
        subRefP(_face);
    if (_texchunk != NullFC)
        subRefCP(_texchunk);
}

/*----------------------------- class specific ----------------------------*/
void SimpleStatisticsForeground::initMethod(void)
{
}

/* */
void SimpleStatisticsForeground::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

/* */
void SimpleStatisticsForeground::dump(UInt32, const BitVector) const
{
    SLOG << "Dump SimpleStatisticsForeground NI" << std::endl;
}

/*! Convenience function to add an element and format.
*/
void SimpleStatisticsForeground::addElement(StatElemDescBase &desc,
                                            const char *format)
{
    getElementIDs().push_back(desc.getID());
    getFormats().push_back(format ? format : "");
}

/*! Convenience function to add an element and format.
*/
void SimpleStatisticsForeground::addElement(Int32 id, const char *format)
{
    getElementIDs().push_back(id);
    getFormats().push_back(format ? format : "");
}

/*! Convenience function to add a line of text.
*/
void SimpleStatisticsForeground::addText( const char *text )
{
    addElement( -1, text );
}

/*! Convenience function to clear all elements.
*/
void SimpleStatisticsForeground::clearElems( void )
{
    getElementIDs().clear();
    getFormats().clear();
    getCollector().clearElems();
}

/*! Initialize the text used. It is compiled into the library as
    StatisticsDefaultFontData and used as a TXF font.
*/
void SimpleStatisticsForeground::initText(const std::string &family, Real32 size)
{
    // Cleanup
    if (_face != 0)
        subRefP(_face);
    if (_texchunk != NullFC)
        subRefCP(_texchunk);

    // Create the font
    if (family.empty() == false)
    {
        TextTXFParam param;
        param.size = static_cast<UInt32>(size);
        _face = TextTXFFace::create(family, TextFace::STYLE_PLAIN, param);
        if (_face != 0)
        {
            _texchunk = TextureChunk::create();
            beginEditCP(_texchunk);
            {
                ImagePtr texture = _face->getTexture();
                _texchunk->setImage(texture);
                _texchunk->setWrapS(GL_CLAMP);
                _texchunk->setWrapT(GL_CLAMP);
                _texchunk->setMinFilter(GL_NEAREST);
                _texchunk->setMagFilter(GL_NEAREST);
                _texchunk->setEnvMode(GL_MODULATE);
            }
            endEditCP(_texchunk);
        }
    }

    // We failed to create the font - fallback to the default font
    if (_face == 0)
    {
        _face = getStatisticsDefaultFont();
        _texchunk = getStatisticsDefaultFontTexture();
    }

    // Increment reference counters
    addRefP(_face);
    addRefCP(_texchunk);
}

/*! Draw the statistics lines.
*/
void SimpleStatisticsForeground::draw(DrawActionBase *action, Viewport *port)
{
    if (getActive() == false)
        return;
	
    if (_face == 0)
        initText(getFamily(), getSize());

    if (!getCollector().getNumOfElems() && !getElementIDs().size())
        return; // nothing to do

    Real32  pw = Real32(port->getPixelWidth ());
    Real32  ph = Real32(port->getPixelHeight());

    if (pw < 1 || ph < 1)
        return;

    glPushAttrib(GL_LIGHTING_BIT | GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Set viewport. We want to map one unit to one pixel on the
    // screen. Some sources in the internet say that we should
    // add an offset of -0.375 to prevent rounding errors. Don't
    // know if that is true, but it seems to work.
    glOrtho(0 - 0.375, pw - 0.375, 0 - 0.375, ph - 0.375, 0, 1);

    // retrieve text
    std::vector < std::string > stat;

    StatCollector   *col = &getCollector();
    StatElem        *el;

    if(getElementIDs().size() != 0)
    {
        for(UInt32 i = 0; i < getElementIDs().size(); ++i)
        {
            Int32 id(getElementIDs()[i]);
            el = ((id >= 0) ? col->getElem(id) : 0);

            stat.resize(stat.size() + 1);
            std::vector < std::string >::iterator str = stat.end() - 1;

            const char  *format = NULL;
            if(i < getFormats().size() && getFormats()[i].length())
            {
              format = getFormats()[i].c_str();
            }

            if (el)
                el->putToString(*str, format);
            else
                *str = format;
        }
    }
    else    // fallback, show all elements
    {
        for(UInt32 i = 0; i < col->getNumOfElems(); ++i)
        {
            el = col->getElem(i, false);
            if(el)
            {
                std::string desc(el->getDesc()->getName().str()), eltext;

                el->putToString(eltext);
                desc = desc + " : " + eltext;

                stat.resize(stat.size() + 1);
                std::vector < std::string >::iterator str = stat.end() - 1;
                *str = desc;
            }
        }
    }

    TextLayoutParam layoutParam;
    layoutParam.spacing = 1.1;
    layoutParam.majorAlignment = TextLayoutParam::ALIGN_BEGIN;
    layoutParam.minorAlignment = TextLayoutParam::ALIGN_BEGIN;

    TextLayoutResult layoutResult;
    _face->layout(stat, layoutParam, layoutResult);

    Real32 scale = 1 / _face->getScale();
    Real32 size = _face->getParam().size;
    Real32 textWidth = layoutResult.textBounds.x() * scale + size + getTextMargin().x() * 2.0f;
    Real32 textHeight = layoutResult.textBounds.y() * scale + size + getTextMargin().y() * 2.0f;

    // Let's do some simple form of layout management
    Real32 orthoX = 0, orthoY = ph;

    switch ( getHorizontalAlign() )
    {
        case Right:
            orthoX = pw - textWidth;
            break;
        case Middle:
            orthoX = (pw - textWidth) * 0.5;
            break;
        case Left:
        default:
            break;
    }

    switch ( getVerticalAlign() )
    {
        case Bottom:
            orthoY = textHeight;
            break;
        case Center:
            orthoY = (ph - textHeight) * 0.5 + textHeight;
            break;
        case Top:
        default:
            break;
    }

    glTranslatef(orthoX, orthoY, 0.0);

    // draw background
    glColor4fv((GLfloat*)getBgColor().getValuesRGBA());
    glBegin(GL_QUADS);
        glVertex2f(0, -textHeight);
        glVertex2f(textWidth, -textHeight);
        glVertex2f(textWidth, 0);
        glVertex2f(0, 0);
    glEnd();

    // draw border
    if(getBorderColor().red() >= 0.0f)
    {
        glColor4fv((GLfloat*)getBorderColor().getValuesRGBA());
        glBegin(GL_LINE_LOOP);
            glVertex2f(getBorderOffset().x(), -textHeight + 1 + getBorderOffset().y());
            glVertex2f(textWidth - 1 - getBorderOffset().x(), -textHeight + 1 + getBorderOffset().y());
            glVertex2f(textWidth - 1 - getBorderOffset().x(), -1 - getBorderOffset().y());
            glVertex2f(getBorderOffset().x(), -1 - getBorderOffset().y());
        glEnd();
    }

    // draw text
    glTranslatef(0.5 * size + getTextMargin().x(), -0.5 * size - getTextMargin().y(), 0.0);

    _texchunk->activate(action);

    glColor4fv((GLfloat*)getShadowColor().getValuesRGBA());
    glPushMatrix();
    glTranslatef(getShadowOffset().x(), getShadowOffset().y(), 0);
    glScalef(scale, scale, 1);
    drawCharacters(layoutResult);

    glColor4fv((GLfloat*)getColor().getValuesRGBA());
    glPopMatrix();
    glScalef(scale, scale, 1);
    drawCharacters(layoutResult);

    _texchunk->deactivate(action);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glPopAttrib();
}

void SimpleStatisticsForeground::drawCharacters(const TextLayoutResult &layoutResult) const
{
    glBegin(GL_QUADS);

    UInt32 i, numGlyphs = layoutResult.getNumGlyphs();
    Real32 width, height;
    for(i = 0; i < numGlyphs; ++i)
    {
        const TextTXFGlyph &glyph = _face->getTXFGlyph(layoutResult.indices[i]);
        width = glyph.getWidth();
        height = glyph.getHeight();
        // No need to draw invisible glyphs
        if ((width <= 0.f) || (height <= 0.f))
            continue;

        // Calculate coordinates
        const Vec2f &pos = layoutResult.positions[i];
        Real32 posLeft = pos.x();
        Real32 posTop = pos.y();
        Real32 posRight = pos.x() + width;
        Real32 posBottom = pos.y() - height;
        Real32 texCoordLeft = glyph.getTexCoord(TextTXFGlyph::COORD_LEFT);
        Real32 texCoordTop = glyph.getTexCoord(TextTXFGlyph::COORD_TOP);
        Real32 texCoordRight = glyph.getTexCoord(TextTXFGlyph::COORD_RIGHT);
        Real32 texCoordBottom = glyph.getTexCoord(TextTXFGlyph::COORD_BOTTOM);

        // lower left corner
        glTexCoord2f(texCoordLeft, texCoordBottom);
        glVertex2f(posLeft, posBottom);

        // lower right corner
        glTexCoord2f(texCoordRight, texCoordBottom);
        glVertex2f(posRight, posBottom);

        // upper right corner
        glTexCoord2f(texCoordRight, texCoordTop);
        glVertex2f(posRight, posTop);

        // upper left corner
        glTexCoord2f(texCoordLeft, texCoordTop);
        glVertex2f(posLeft, posTop);
    }

    glEnd();
}

/*-------------------------------------------------------------------------*/
/*                              cvs id's                                   */
#ifdef __sgi
#pragma set woff 1174
#endif
#ifdef OSG_LINUX_ICC
#pragma warning(disable : 177)
#endif
namespace
{
static char cvsid_cpp[] = "@(#)$Id: OSGSimpleStatisticsForeground.cpp,v 1.11 2002/08/07 04:04:13 vossg Exp $";
static char cvsid_hpp[] = OSGSIMPLESTATISTICSFOREGROUND_HEADER_CVSID;
static char cvsid_inl[] = OSGSIMPLESTATISTICSFOREGROUND_INLINE_CVSID;
}
