/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *                 Copyright (C) 2000 by the OpenSG Forum                    *
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

// System declarations

#ifdef WIN32
//#include <iosfwd>
#endif

//---------------------------------------------------------------------------
//  Includes
//---------------------------------------------------------------------------

#include "OSGConfig.h"

#include <OSGLog.h>

#include <iostream>

#include <assert.h>

#include <OSGLine.h>
#include <OSGMatrix.h>

#include "OSGFrustumVolume.h"
#include "OSGPlane.h"

OSG_USING_NAMESPACE

/*-------------------------------- get ------------------------------------*/

void FrustumVolume::getCenter(Pnt3f &center) const
{
    Pnt3f vertices[8];
    Line  lines   [4];

    _planeVec[5].intersect(_planeVec[3],lines[3]);
    _planeVec[3].intersect(_planeVec[4],lines[2]);
    _planeVec[4].intersect(_planeVec[2],lines[0]);
    _planeVec[2].intersect(_planeVec[5],lines[1]);
    
    for(Int32 i = 0; i < 4; i++)
    {
        _planeVec[0].intersectInfinite(lines[i],vertices[    i]);
        _planeVec[1].intersectInfinite(lines[i],vertices[4 + i]);
    }    
    
    center = Pnt3f(0.f, 0.f ,0.f);

    for(Int32 i = 0; i < 8; i++)
    {        
        center = center + vertices[i].subZero();
    }

    center /= 8.f;
}


Real32 FrustumVolume::getScalarVolume() const
{
    const Int32 faces[6][4] =
    {
        {0,1,3,2},
        {4,5,7,6},
        {0,4,5,1},
        {2,6,7,3},
        {2,6,4,0},
        {1,5,7,3}
    };

    Pnt3f vertices[8];
    Line  lines   [4];

    _planeVec[5].intersect(_planeVec[3], lines[3]); 
    _planeVec[3].intersect(_planeVec[4], lines[2]); 
    _planeVec[4].intersect(_planeVec[2], lines[0]); 
    _planeVec[2].intersect(_planeVec[5], lines[1]); 
    
    for(Int32 i = 0; i < 4; i++)
    {
        _planeVec[0].intersectInfinite(lines[i], vertices[    i]);
        _planeVec[1].intersectInfinite(lines[i], vertices[4 + i]);
    }
    

    Pnt3f center = Pnt3f(0.f, 0.f, 0.f);

    for(Int32 i = 0; i < 8; i++)
    {
        center = center + vertices[i].subZero();
    }

    center /= 8.f;
    
    Real32 volume = .0f;

    for(Int32 i = 0; i < 6; i++)
    {
        Real32 height;
        Real32 area;

        height = 
            _planeVec[i].getNormal().dot(center) - 
            _planeVec[i].getDistanceFromOrigin();
        
        Vec3f main_diag = vertices[faces[i][0]] - vertices[faces[i][2]];
        Vec3f sec_diag  = vertices[faces[i][1]] - vertices[faces[i][3]];

        area = osgabs((main_diag.cross(sec_diag)).length() / 2.f);        

        volume += osgabs((height*area)) / 3.f;
    }

    return volume;
}


/*! 
  \warning NOT IMPLEMENTED 
  \brief   NOT IMPLEMENTED 
 */

void FrustumVolume::getBounds(Pnt3f &OSG_CHECK_ARG(minPnt), 
                              Pnt3f &OSG_CHECK_ARG(maxPnt)) const
{
}

/*------------------------------ feature ----------------------------------*/

void FrustumVolume::setPlanes(const Plane &pnear, const Plane &pfar,
                              const Plane &left,  const Plane &right,
                              const Plane &top,   const Plane &bottom)
{
    _planeVec[0] = pnear;
    _planeVec[1] = pfar;
    _planeVec[2] = left;
    _planeVec[3] = right;
    _planeVec[4] = top;
    _planeVec[5] = bottom;
}


void FrustumVolume::setPlanes(const Pnt3f &nlt, const Pnt3f &nlb,
                              const Pnt3f &nrt, const Pnt3f &nrb,
                              const Pnt3f &flt, const Pnt3f &flb,
                              const Pnt3f &frt, const Pnt3f &frb)
{

    Plane pnear  (nlb,nlt,nrb);
    Plane pfar   (frb,frt,flb);
    Plane pleft  (flb,flt,nlb);
    Plane pright (nrb,nrt,frb);
    Plane ptop   (frt,nrt,flt);
    Plane pbottom(nlb,nrb,flb);


    _planeVec[0] = pnear;
    _planeVec[1] = pfar;
    _planeVec[2] = pleft;
    _planeVec[3] = pright;
    _planeVec[4] = ptop;
    _planeVec[5] = pbottom;

}


void FrustumVolume::setPlanes(const Matrix &objectClipMat)
{
    Vec4f  planeEquation[6];
    Real32 vectorLength;
    Vec3f  normal;

    planeEquation[0][0] = objectClipMat[0][3] - objectClipMat[0][0];
    planeEquation[0][1] = objectClipMat[1][3] - objectClipMat[1][0];
    planeEquation[0][2] = objectClipMat[2][3] - objectClipMat[2][0];
    planeEquation[0][3] = objectClipMat[3][3] - objectClipMat[3][0];

    planeEquation[1][0] = objectClipMat[0][3] + objectClipMat[0][0];
    planeEquation[1][1] = objectClipMat[1][3] + objectClipMat[1][0];
    planeEquation[1][2] = objectClipMat[2][3] + objectClipMat[2][0];
    planeEquation[1][3] = objectClipMat[3][3] + objectClipMat[3][0];

    planeEquation[2][0] = objectClipMat[0][3] + objectClipMat[0][1];
    planeEquation[2][1] = objectClipMat[1][3] + objectClipMat[1][1];
    planeEquation[2][2] = objectClipMat[2][3] + objectClipMat[2][1];
    planeEquation[2][3] = objectClipMat[3][3] + objectClipMat[3][1];

    planeEquation[3][0] = objectClipMat[0][3] - objectClipMat[0][1];
    planeEquation[3][1] = objectClipMat[1][3] - objectClipMat[1][1];
    planeEquation[3][2] = objectClipMat[2][3] - objectClipMat[2][1];
    planeEquation[3][3] = objectClipMat[3][3] - objectClipMat[3][1];

    planeEquation[4][0] = objectClipMat[0][3] + objectClipMat[0][2];
    planeEquation[4][1] = objectClipMat[1][3] + objectClipMat[1][2];
    planeEquation[4][2] = objectClipMat[2][3] + objectClipMat[2][2];
    planeEquation[4][3] = objectClipMat[3][3] + objectClipMat[3][2];

    planeEquation[5][0] = objectClipMat[0][3] - objectClipMat[0][2];
    planeEquation[5][1] = objectClipMat[1][3] - objectClipMat[1][2];
    planeEquation[5][2] = objectClipMat[2][3] - objectClipMat[2][2];
    planeEquation[5][3] = objectClipMat[3][3] - objectClipMat[3][2];

    for(Int32  i = 0; i < 6; i++) 
    {
        vectorLength = 
            osgsqrt(planeEquation[i][0] * planeEquation[i][0] +
                    planeEquation[i][1] * planeEquation[i][1] +
                    planeEquation[i][2] * planeEquation[i][2]);
 
        planeEquation[i][0] /= -vectorLength;
        planeEquation[i][1] /= -vectorLength;
        planeEquation[i][2] /= -vectorLength;
        planeEquation[i][3] /=  vectorLength;
    }

  // right
  _planeVec[3].set(planeEquation[0]);

  // left
  _planeVec[2].set(planeEquation[1]);

  // bottom
  _planeVec[5].set(planeEquation[2]);

  // top
  _planeVec[4].set(planeEquation[3]);

  // near
  _planeVec[0].set(planeEquation[4]);

  // far
  _planeVec[1].set(planeEquation[5]);
}



/*-------------------------- extending ------------------------------------*/

/*! 
  \warning NOT IMPLEMENTED 
  \brief   NOT IMPLEMENTED 
 */

void FrustumVolume::extendBy(const Pnt3f &OSG_CHECK_ARG(pt))
{
}

/*!
  \brief extend the volume to enclose the given one.
 */

void FrustumVolume::extendBy(const Volume &volume)
{
    osg::extend(*this, volume);
}


/*-------------------------- intersection ---------------------------------*/

bool FrustumVolume::intersect(const Pnt3f &point) const
{
    bool retCode = true;

    for(Int32 i = 0; i < 6; i++) 
    {
        if((_planeVec[i].getNormal().x() * point.x() +
            _planeVec[i].getNormal().y() * point.x() +
            _planeVec[i].getNormal().z() * point.x() +
            _planeVec[i].getDistanceFromOrigin()     ) < 0.f) 
        {
            retCode = false;
            break;
        }
    }
    
    return retCode;
}


bool FrustumVolume::intersect(const Line &line) const
{
    return line.intersect(*this);
}


bool FrustumVolume::intersect(const Line   &line,
                                    Real32 &minDist, 
                                    Real32 &maxDist) const
{
    return line.intersect(*this, minDist, maxDist);
}


bool FrustumVolume::intersect(const Volume &volume) const
{
    return osg::intersect(*this, volume);
}

/*! 
  \warning NOT IMPLEMENTED 
  \brief   NOT IMPLEMENTED 
 */

bool FrustumVolume::isOnSurface(const Pnt3f &OSG_CHECK_ARG(point)) const
{
    return false;
}

/*! 
  \warning NOT IMPLEMENTED 
  \brief   NOT IMPLEMENTED 
 */

void FrustumVolume::transform(const Matrix &OSG_CHECK_ARG(m))
{
}

const FrustumVolume &FrustumVolume::operator =(const FrustumVolume &b1)
{
    for(Int32 i = 0; i < 5; i++)
    {
        _planeVec[i] = b1._planeVec[i];
    }

    _state = b1._state;

    return *this;
}

void FrustumVolume::dump(      UInt32    OSG_CHECK_ARG(uiIndent), 
                         const BitVector OSG_CHECK_ARG(bvFlags )) const
{
#if 1
fprintf(stderr,"Frustum:(%f %f %f:%f)(%f %f %f:%f)(%f %f %f:%f)"
                "(%f %f %f:%f)(%f %f %f:%f)(%f %f %f:%f)\n",
                _planeVec[0].getNormal()[0],
                _planeVec[0].getNormal()[1],
                _planeVec[0].getNormal()[2],
                _planeVec[0].getDistanceFromOrigin(),
                _planeVec[1].getNormal()[0],
                _planeVec[1].getNormal()[1],
                _planeVec[1].getNormal()[2],
                _planeVec[1].getDistanceFromOrigin(),
                _planeVec[2].getNormal()[0],
                _planeVec[2].getNormal()[1],
                _planeVec[2].getNormal()[2],
                _planeVec[2].getDistanceFromOrigin(),
                _planeVec[3].getNormal()[0],
                _planeVec[3].getNormal()[1],
                _planeVec[3].getNormal()[2],
                _planeVec[3].getDistanceFromOrigin(),
                _planeVec[4].getNormal()[0],
                _planeVec[4].getNormal()[1],
                _planeVec[4].getNormal()[2],
                _planeVec[4].getDistanceFromOrigin(),
                _planeVec[5].getNormal()[0],
                _planeVec[5].getNormal()[1],
                _planeVec[5].getNormal()[2],
                _planeVec[5].getDistanceFromOrigin() );
                

#else 
    PLOG << "Frustum("
             << _planeVec[0].getNormal() << ":" 
             << _planeVec[0].getDistanceFromOrigin() << "|"
             << _planeVec[1].getNormal() << ":" 
             << _planeVec[1].getDistanceFromOrigin() << "|"
             << _planeVec[2].getNormal() << ":" 
             << _planeVec[2].getDistanceFromOrigin() << "|"
             << _planeVec[3].getNormal() << ":" 
             << _planeVec[3].getDistanceFromOrigin() << "|"
             << _planeVec[4].getNormal() << ":" 
             << _planeVec[4].getDistanceFromOrigin() << "|"
             << _planeVec[5].getNormal() << ":" 
             << _planeVec[5].getDistanceFromOrigin() << "|"
             << ")";
#endif
}


OSG_BEGIN_NAMESPACE

OSG_BASE_DLLMAPPING
bool operator ==(const FrustumVolume &b1, const FrustumVolume &b2)
{
    return ((static_cast<const Volume &>(b1) == b2 ) &&
            (b1.getPlanes()[0] == b2.getPlanes()[0]) &&
            (b1.getPlanes()[1] == b2.getPlanes()[1]) &&
            (b1.getPlanes()[2] == b2.getPlanes()[2]) &&
            (b1.getPlanes()[3] == b2.getPlanes()[3]) &&
            (b1.getPlanes()[4] == b2.getPlanes()[4]) &&
            (b1.getPlanes()[5] == b2.getPlanes()[5]));
}

OSG_END_NAMESPACE
