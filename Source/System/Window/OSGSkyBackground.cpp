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

#include <OSGDrawActionBase.h>
#include <OSGCamera.h>
#include <OSGViewport.h>
#include <OSGTextureChunk.h>

#include "OSGSkyBackground.h"

#include <OSGGL.h>

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::SkyBackground
    \ingroup GrpSystemWindowBackgrounds

A Sky/Ground/Skybox background, inspired by VRML, see \ref
PageSystemWindowBackgroundSky for a description.

The sky is defined by the _mfSkyAngle and _mfSkyColor fields, the ground by the
_mfGroundAngle and _mfGround Color fields. The resolution of the sky sphere can
be influenced by the _sfSphereRes field. The sky box is defined by the
_sfBackTexture, _sfFrontTexture, _sfLeftTexture, _sfRightTexture, _sfTopTexture
and _sfBottomTexture fields.

*/

/*----------------------- constructors & destructors ----------------------*/

SkyBackground::SkyBackground(void) :
    Inherited()
{
}

SkyBackground::SkyBackground(const SkyBackground &source) :
    Inherited(source)
{
}

SkyBackground::~SkyBackground(void)
{
}

/*----------------------------- class specific ----------------------------*/

void SkyBackground::initMethod (void)
{
}

void SkyBackground::changed(BitVector whichField, UInt32 origin)
{
    Inherited::changed(whichField, origin);
}

void SkyBackground::dump(     UInt32    , 
                         const BitVector) const
{
    SLOG << "Dump SkyBackground NI" << std::endl;
}

/*-------------------------- drawing ---------------------------------*/

void SkyBackground::drawFace(DrawActionBase *action, 
                             TextureChunkPtr tex, StateChunk* &oldtex, 
                             const Pnt3f &p1, const Pnt3f &p2, 
                             const Pnt3f &p3, const Pnt3f &p4)
{
    
    if(tex != NullFC)
    {
        if(oldtex != NULL)
        {
            tex->changeFrom(action, oldtex);
        }
        else
        {
            tex->activate(action);
        }
        
        if(tex->isTransparent())
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
        }
        
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3fv((GLfloat*) p1.getValues());
        glTexCoord2f(1, 0);
        glVertex3fv((GLfloat*) p2.getValues());
        glTexCoord2f(1, 1);
        glVertex3fv((GLfloat*) p3.getValues());
        glTexCoord2f(0, 1);
        glVertex3fv((GLfloat*) p4.getValues());
        glEnd();
        
        if(tex->isTransparent())
        {
            glDisable(GL_BLEND);
        }
        
        oldtex = tex.getCPtr();
    }
   
}

void SkyBackground::clear(DrawActionBase *action, Viewport *viewport)
{
    bool light = glIsEnabled(GL_LIGHTING);
    if (light)  
        glDisable(GL_LIGHTING);

    GLint fill[2];
    glGetIntegerv(GL_POLYGON_MODE, fill);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    GLint depth;
    glGetIntegerv(GL_DEPTH_FUNC, &depth);
    glDepthFunc(GL_ALWAYS);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    Matrix m,pt;
    action->getCamera()->getViewing(m, viewport->getPixelWidth(),
                                        viewport->getPixelHeight());
    action->getCamera()->getProjectionTranslation(pt, 
                                        viewport->getPixelWidth(),
                                        viewport->getPixelHeight());
    m.multLeft(pt);
    
    m[3][0] = m[3][1] = m[3][2] = 0;
    glLoadMatrixf(m.getValues());         
    Real32 viewscale = (m[0].length() + m[1].length() + m[2].length()) / 3.f;
    float scale = (action->getCamera()->getFar() + 
                    action->getCamera()->getNear()) / 2 / viewscale;
    glScalef(scale, scale, scale);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    action->getCamera()->getProjection(m, viewport->getPixelWidth(),
                                           viewport->getPixelHeight());
    glLoadMatrixf(m.getValues());         
    
    UInt32 i, j;
    UInt32 sr = _sfSphereRes.getValue() + 1;      // sphere resolution
    
    // These should be stored somewhere, not recalced.
    Real32 *cosval = new Real32 [ sr ];
    Real32 *sinval = new Real32 [ sr ];
    
    Real32  vcos1,vsin1,vcos2,vsin2;
    
    Real32 da = 2 * Pi / (sr - 1);    
    for(i = 0; i < sr; ++i)
    {
        cosval[i] = osgcos(i * da);
        sinval[i] = osgsin(i * da);     
    }
    
    if(_mfSkyAngle.size() > 0)
    {
        vcos1 = osgcos(_mfSkyAngle[0]);
        vsin1 = osgsin(_mfSkyAngle[0]);

        glBegin(GL_TRIANGLE_FAN);
        glColor3fv((GLfloat*) _mfSkyColor[0].getValuesRGB());
        glVertex3f(0, 1, 0);
        glColor3fv((GLfloat*) _mfSkyColor[1].getValuesRGB());

        for(i = 0; i < sr; ++i)
        {
            glVertex3f(vsin1 * sinval[i], vcos1, vsin1 * cosval[i]);
        }

        glEnd();


        for(j = 0; j < _mfSkyAngle.size() - 1; ++j)
        {
            Color3f c1, c2;

            c1 = _mfSkyColor[j+1];
            c2 = _mfSkyColor[j+2];

            vcos1 = osgcos(_mfSkyAngle[j  ]);
            vsin1 = osgsin(_mfSkyAngle[j  ]);
            vcos2 = osgcos(_mfSkyAngle[j+1]);
            vsin2 = osgsin(_mfSkyAngle[j+1]);

            glBegin(GL_TRIANGLE_STRIP);

            for(i = 0; i < sr; ++i)
            {
                glColor3fv((GLfloat*) c1.getValuesRGB());
                glVertex3f(vsin1 * sinval[i], vcos1, vsin1 * cosval[i]);
                glColor3fv((GLfloat*) c2.getValuesRGB());
                glVertex3f(vsin2 * sinval[i], vcos2, vsin2 * cosval[i]);
            }
            glEnd();
        }

        if(osgabs(_mfSkyAngle[j] - Pi) > Eps)
        {
            glBegin(GL_TRIANGLE_FAN);
            glColor3fv((GLfloat*) _mfSkyColor[j+1].getValuesRGB());
            glVertex3f(0, -1, 0);
            vcos1 = osgcos(_mfSkyAngle[j]);
            vsin1 = osgsin(_mfSkyAngle[j]);

            for(i = 0; i < sr; ++i)
            {
                glVertex3f(vsin1 * sinval[i], vcos1, vsin1 * cosval[i]);
            }

            glEnd();
        }
    }
    else // no angles, just fill single color
    {
        if(_mfSkyColor.size() > 0)
        {
            glClearColor(_mfSkyColor[0][0], _mfSkyColor[0][1], 
                         _mfSkyColor[0][2], 0);
        }
        else
        {
            glClearColor(0, 0, 0, 0);
        }
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // Draw the ground.
    // It's possible to be smarter about this, but for now just overdraw.

    if(_mfGroundAngle.size() > 0)
    {    
        vcos1 = -osgcos(_mfGroundAngle[0]);
        vsin1 =  osgsin(_mfGroundAngle[0]);

        glBegin(GL_TRIANGLE_FAN);
        glColor3fv((GLfloat*) _mfGroundColor[0].getValuesRGB());
        glVertex3f(0, -1, 0);
        glColor3fv((GLfloat*) _mfGroundColor[1].getValuesRGB());

        for(i = 0; i < sr; ++i)
        {
            glVertex3f(vsin1 * sinval[i], vcos1, vsin1 * cosval[i]);
        }

        glEnd();


        for(j = 0; j < _mfGroundAngle.size() - 1; ++j)
        {
            Color3f c1, c2;

            c1 = _mfGroundColor[j+1];
            c2 = _mfGroundColor[j+2];

            vcos1 = -osgcos(_mfGroundAngle[j  ]);
            vsin1 =  osgsin(_mfGroundAngle[j  ]);
            vcos2 = -osgcos(_mfGroundAngle[j+1]);
            vsin2 =  osgsin(_mfGroundAngle[j+1]);

            glBegin(GL_TRIANGLE_STRIP);

            for(i = 0; i < sr; ++i)
            {
                glColor3fv((GLfloat*) c1.getValuesRGB());
                glVertex3f(vsin1 * sinval[i], vcos1, vsin1 * cosval[i]);
                glColor3fv((GLfloat*) c2.getValuesRGB());
                glVertex3f(vsin2 * sinval[i], vcos2, vsin2 * cosval[i]);
            }
            glEnd();
        }
    }
    
    // now draw the textures, if set
    StateChunk *tchunk = NULL;
    
    drawFace(action, getBackTexture(),   tchunk,
                                         Pnt3f(0.5, -0.5,  0.5),
                                         Pnt3f(-0.5, -0.5,  0.5),
                                         Pnt3f(-0.5,  0.5,  0.5),
                                         Pnt3f(0.5,  0.5,  0.5));
    
    drawFace(action, getFrontTexture(),  tchunk,
                                         Pnt3f(-0.5, -0.5, -0.5),
                                         Pnt3f(0.5, -0.5, -0.5),
                                         Pnt3f(0.5,  0.5, -0.5),
                                         Pnt3f(-0.5,  0.5, -0.5));
    
    drawFace(action, getBottomTexture(), tchunk,
                                         Pnt3f(-0.5, -0.5,  0.5),
                                         Pnt3f(0.5, -0.5,  0.5),
                                         Pnt3f(0.5, -0.5, -0.5),
                                         Pnt3f(-0.5, -0.5, -0.5));
    
    drawFace(action, getTopTexture(),    tchunk,
                                         Pnt3f(-0.5,  0.5, -0.5),
                                         Pnt3f(0.5,  0.5, -0.5),
                                         Pnt3f(0.5,  0.5,  0.5),
                                         Pnt3f(-0.5,  0.5,  0.5));
    
    drawFace(action, getLeftTexture(),   tchunk,
                                         Pnt3f(-0.5, -0.5,  0.5),
                                         Pnt3f(-0.5, -0.5, -0.5),
                                         Pnt3f(-0.5,  0.5, -0.5),
                                         Pnt3f(-0.5,  0.5,  0.5));
    
    drawFace(action, getRightTexture(),  tchunk,
                                         Pnt3f(0.5, -0.5, -0.5),
                                         Pnt3f(0.5, -0.5,  0.5),
                                         Pnt3f(0.5,  0.5,  0.5),
                                         Pnt3f(0.5,  0.5, -0.5));
    
    if(tchunk != NULL)
        tchunk->deactivate(action);
    
    glClear(GL_DEPTH_BUFFER_BIT);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glDepthFunc(depth);
    glPolygonMode(GL_FRONT, fill[0]);
    glPolygonMode(GL_BACK , fill[1]);
    if (light)  
        glEnable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);

    delete [] sinval;
    delete [] cosval;
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
    static char cvsid_cpp[] = "@(#)$Id: $";
    static char cvsid_hpp[] = OSGSKYBACKGROUND_HEADER_CVSID;
    static char cvsid_inl[] = OSGSKYBACKGROUND_INLINE_CVSID;
}
