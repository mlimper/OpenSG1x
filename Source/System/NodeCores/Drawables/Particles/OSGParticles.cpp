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
#include <OSGGLEXT.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGRenderAction.h>
#include <OSGMaterial.h>
#include <OSGGeoPropPtrs.h>
#include <OSGCamera.h>
#include <OSGWindow.h>

#include "OSGParticles.h"

#include <algorithm>

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::Particles
    \ingroup GrpSystemNodeCoresDrawablesParticles
    
See \ref PageSystemParticles for details.

*/


/***************************************************************************\
 *                           File variables                                *
\***************************************************************************/

static UInt32 _extMultitexture;

static UInt32 _funcglMultiTexCoord3fvARB;

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void Particles::initMethod (void)
{
    DrawAction::registerEnterDefault( getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
                                          ParticlesPtr  ,
                                          CNodePtr      ,
                                          Action       *>
                                       (&MaterialDrawable::drawActionEnterHandler));
                                       
    DrawAction::registerLeaveDefault( getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
                                          ParticlesPtr  ,
                                          CNodePtr      ,
                                          Action       *>
                                       (&MaterialDrawable::drawActionLeaveHandler));

    RenderAction::registerEnterDefault( getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
                                          ParticlesPtr  ,
                                          CNodePtr      ,
                                          Action       *>
                                       (&MaterialDrawable::renderActionEnterHandler));
    
    RenderAction::registerLeaveDefault( getClassType(),
        osgTypedMethodFunctor2BaseCPtrRef<Action::ResultE,
                                          ParticlesPtr  ,
                                          CNodePtr      ,
                                          Action       *>
                                       (&MaterialDrawable::renderActionLeaveHandler));
}

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

Particles::Particles(void) :
    Inherited()
{
    _extMultitexture        = 
        Window::registerExtension("GL_ARB_multitexture");

    _funcglMultiTexCoord3fvARB = Window::registerFunction(
                            OSG_DLSYM_UNDERSCORE"glMultiTexCoord3fvARB",
                            _extMultitexture);          
}

Particles::Particles(const Particles &source) :
    Inherited(source)
{
}

Particles::~Particles(void)
{
    ParticlesPtr thisP(*this);

    if(_sfPositions.getValue() != NullFC)
    {
        beginEditCP(_sfPositions.getValue(), Attachment::ParentsFieldMask);
        {
            _sfPositions.getValue()->subParent(thisP);
        }
        endEditCP(_sfPositions.getValue(), Attachment::ParentsFieldMask);

        subRefCP(_sfPositions.getValue());
    }

    if(_sfSecPositions.getValue() != NullFC)
    {
        beginEditCP(_sfSecPositions.getValue(), Attachment::ParentsFieldMask);
        {
            _sfSecPositions.getValue()->subParent(thisP);
        }
        endEditCP(_sfSecPositions.getValue(), Attachment::ParentsFieldMask);

        subRefCP(_sfSecPositions.getValue());
    }

    if(_sfColors.getValue() != NullFC)
    {
        beginEditCP(_sfColors.getValue(), Attachment::ParentsFieldMask);
        {
            _sfColors.getValue()->subParent(thisP);
        }
        endEditCP(_sfColors.getValue(), Attachment::ParentsFieldMask);

        subRefCP(_sfColors.getValue());
    }

    if(_sfNormals.getValue() != NullFC)
    {
        beginEditCP(_sfNormals.getValue(), Attachment::ParentsFieldMask);
        {
            _sfNormals.getValue()->subParent(thisP);
        }
        endEditCP(_sfNormals.getValue(), Attachment::ParentsFieldMask);

        subRefCP(_sfNormals.getValue());
    }

    subRefCP(_sfMaterial.getValue());
}

/*------------------------------- Sync -----------------------------------*/

void Particles::changed(BitVector whichField, UInt32 origin)
{
    if(whichField & PositionsFieldMask)
    {
        for(UInt32 i = 0; i < _parents.size(); i++)
        {
            _parents[i]->invalidateVolume();
        }
        editBsp().destroy();

        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrCheckValid)
            {
                ParticlesPtr thisP(*this);

                if(_sfPositions.getValue()                    != NullFC &&
                   _sfPositions.getValue()->findParent(thisP) ==     -1 )
                {
                    GeoPositionsPtr pPos = _sfPositions.getValue();

                    _sfPositions.setValue(NullFC);

                    setPositions(pPos);
                }
            }
            else if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfPositions.getValue());
            }
            else
            {
                GeoPositionsPtr pPos = _sfPositions.getValue();

                _sfPositions.setValue(NullFC);

                setPositions(pPos);
            }
        }
    }

    if(whichField & SecPositionsFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrCheckValid)
            {
                ParticlesPtr thisP(*this);

                if(_sfSecPositions.getValue()                    != NullFC &&
                   _sfSecPositions.getValue()->findParent(thisP) ==     -1 )
                {
                    GeoPositionsPtr pPos = _sfSecPositions.getValue();

                    _sfSecPositions.setValue(NullFC);

                    setSecPositions(pPos);
                }
            }
            else if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfSecPositions.getValue());
            }
            else
            {
                GeoPositionsPtr pPos = _sfSecPositions.getValue();

                _sfSecPositions.setValue(NullFC);

                setSecPositions(pPos);
            }
        }
    }

    if(whichField & ColorsFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrCheckValid)
            {
                ParticlesPtr thisP(*this);

                if(_sfColors.getValue()                    != NullFC &&
                   _sfColors.getValue()->findParent(thisP) ==     -1 )
                {
                    GeoColorsPtr pCol = _sfColors.getValue();

                    _sfColors.setValue(NullFC);

                    setColors(pCol);
                }
            }
            else if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfColors.getValue());
            }
            else
            {
                GeoColorsPtr pCol = _sfColors.getValue();

                _sfColors.setValue(NullFC);

                setColors(pCol);
            }
        }
    }

    if(whichField & NormalsFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrCheckValid)
            {
                ParticlesPtr thisP(*this);

                if(_sfNormals.getValue()                    != NullFC &&
                   _sfNormals.getValue()->findParent(thisP) ==     -1 )
                {
                    GeoNormalsPtr pNorm = _sfNormals.getValue();

                    _sfNormals.setValue(NullFC);

                    setNormals(pNorm);
                }
            }
            else if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfNormals.getValue());
            }
            else
            {
                GeoNormalsPtr pNorm = _sfNormals.getValue();

                _sfNormals.setValue(NullFC);

                setNormals(pNorm);
            }
        }
    }

    if(whichField & MaterialFieldMask)
    {
        if(origin & ChangedOrigin::Abstract)
        {
            if(origin & ChangedOrigin::AbstrIncRefCount)
            {
                addRefCP(_sfMaterial.getValue());
            }
            else
            {
                MaterialPtr pMat = _sfMaterial.getValue();
                
                _sfMaterial.setValue(NullFC);
                
                setMaterial(pMat);
            }
        }
    }

    Inherited::changed(whichField, origin);
}

/*------------------------------ Output ----------------------------------*/

void Particles::dump(      UInt32    ,
                         const BitVector ) const
{
    SLOG << "Dump Particles NI" << std::endl;
}


void Particles::adjustVolume( Volume & volume )
{
    GeoPositionsPtr pos = getPositions();

    if ( pos == NullFC )
        return;                 // Node has no particles, no volume

    volume.setValid();
    volume.setEmpty();

    const MFVec3f *sizes = getMFSizes();

    if(sizes->size() == pos->size())
    {
        Vec3f p;
        Real32 s;

        for(UInt32 i = 0; i < pos->size(); i++)
        {
            pos->getValue(p, i);
            // make the size bigger to accomodate rotations
            s=(*sizes)[i][0]*Sqrt2;

            p[0]+=s/2;
            p[1]+=s/2;
            p[2]+=s/2;
            volume.extendBy(p);
            p[0]-=s;
            volume.extendBy(p);
            p[1]-=s;
            volume.extendBy(p);
            p[0]+=s;
            volume.extendBy(p);
            p[2]-=s;
            volume.extendBy(p);
            p[0]-=s;
            volume.extendBy(p);
            p[1]+=s;
            volume.extendBy(p);
            p[0]+=s;
            volume.extendBy(p);
        }

    }
    else if(sizes->size() == 1)
    {
        Vec3f p;
        Real32 s,s2;
        // make the size bigger to accomodate rotations
        s=(*sizes)[0][0]*Sqrt2;
        s2=s/2;

        for(UInt32 i = 0; i < pos->size(); i++)
        {
            pos->getValue(p, i);

            p[0]+=s2;
            p[1]+=s2;
            p[2]+=s2;
            volume.extendBy(p);
            p[0]-=s;
            volume.extendBy(p);
            p[1]-=s;
            volume.extendBy(p);
            p[0]+=s;
            volume.extendBy(p);
            p[2]-=s;
            volume.extendBy(p);
            p[0]-=s;
            volume.extendBy(p);
            p[1]+=s;
            volume.extendBy(p);
            p[0]+=s;
            volume.extendBy(p);
        }

    }
    else
    {
        Vec3f p;

        for(UInt32 i = 0; i < pos->getSize(); i++)
        {
            pos->getValue(p, i);

            volume.extendBy(p);
        }
    }
}

#if !defined(OSG_DO_DOC) || OSG_DOC_LEVEL > 1  // remove from user docu

/*---------------------------- pumps -----------------------------------*/

/*! \name Drawing Pumps                                                 */
/*! \{                                                                  */

/*! The pumps use a trait system to create specialized code for important
    cases. The general idea is to keep decision out of the inner loops
    and make special versions for the most common data types. Not for
    everything, as that creates big time code bloat.
*/

/*! Base class for the particle rendering traits. Does nothing, just needed to 
    have a common base class.
*/
 
class ParticleTraits
{
};

/*! Color trait base, keep the static data needed for color handling */

struct ColTraitBase : public ParticleTraits
{
    typedef void (OSG_APIENTRY *pumpFunc)( GLubyte * data );
    static const int formatBase;
    enum { numFormats = GL_DOUBLE - GL_BYTE + 1 };

    static char *formatNames[];

    static pumpFunc ColorFuncs[numFormats][4]; 
};

/*! The smallest enum for data types, used as a base for the formatNames map. 
*/
const int ColTraitBase::formatBase = GL_BYTE;

/*! A map from the OpenGL data type to the corresponding name 
*/
char *ColTraitBase::formatNames[] = 
{   "GL_BYTE", "GL_UNSIGNED_BYTE", "GL_SHORT", "GL_UNSIGNED_SHORT", 
    "GL_INT", "GL_UNSIGNED_INT", "GL_FLOAT", "GL_2_BYTES", 
    "GL_3_BYTES", "GL_4_BYTES", "GL_DOUBLE"
};

/*! The pump functions for colors, indexed by data type and dimensionality 
*/
ColTraitBase::pumpFunc 
ColTraitBase::ColorFuncs[ColTraitBase::numFormats][4] = {
    { NULL, 
      NULL, 
      reinterpret_cast<pumpFunc>(glColor3bv),
      reinterpret_cast<pumpFunc>(glColor4bv) },   // GL_BYTE
    { NULL, 
      NULL, 
      reinterpret_cast<pumpFunc>(glColor3ubv),
      reinterpret_cast<pumpFunc>(glColor4ubv) },  // GL_UNSIGNED_BYTE
    { NULL, 
      NULL, 
      reinterpret_cast<pumpFunc>(glColor3sv),
      reinterpret_cast<pumpFunc>(glColor4sv) },   // GL_SHORT
    { NULL, 
      NULL,  
      reinterpret_cast<pumpFunc>(glColor3usv),
      reinterpret_cast<pumpFunc>(glColor4usv) },  // GL_UNSIGNED_SHORT
    { NULL, 
      NULL,  
      reinterpret_cast<pumpFunc>(glColor3iv),
      reinterpret_cast<pumpFunc>(glColor4iv) },   // GL_INT
    { NULL, 
      NULL,  
      reinterpret_cast<pumpFunc>(glColor3uiv),
      reinterpret_cast<pumpFunc>(glColor4uiv) },  // GL_UNSIGNED_INT
    { NULL, 
      NULL,  
      reinterpret_cast<pumpFunc>(glColor3fv),
      reinterpret_cast<pumpFunc>(glColor4fv) },   // GL_FLOAT
    { NULL, NULL, NULL, NULL },                       // GL_2_BYTES
    { NULL, NULL, NULL, NULL },                       // GL_3_BYTES
    { NULL, NULL, NULL, NULL },                       // GL_4_BYTES
    { NULL, 
      NULL,  
      reinterpret_cast<pumpFunc>(glColor3dv),
      reinterpret_cast<pumpFunc>(glColor4dv) },   // GL_DOUBLE
};

struct ColTraitNone : public ColTraitBase
{
    typedef UInt8 dataType; // no data needed, but can't instantiate void
    
    static inline void init(Particles *, DrawActionBase *, dataType &)
    {
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline void vertex(dataType &, UInt32, UInt32)
    {
    }
};

struct ColTraitSingle : public ColTraitBase
{
    typedef UInt8 dataType; // no data needed, but can't instantiate void
    
    static inline void init(Particles *part, DrawActionBase *, dataType &)
    {
        GeoColorsPtr col = part->getColors();
 
        if(col != NullFC)
        {
            pumpFunc col_func;

            col_func = ColorFuncs[ col->getFormat() - formatBase ]
                                 [ col->getDimension() - 1 ];

            if(col_func == NULL)
            {
                SWARNING << "Particles " << part << "have illegal "
                         << "colors: " << col->getDimension()
                         << "D " << formatNames[col->getFormat() - formatBase]
                         << "!" << std::endl;
                return;
            }
            else
            {
                col_func(const_cast<GLubyte*>(col->getData()));
            }
        }
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }

    static inline void vertex(dataType &, UInt32, UInt32)
    {
    }
};

struct ColTraitParticle : public ColTraitBase
{
    typedef struct
    {
        UInt8    *data;
        UInt32    stride;
        pumpFunc  func;
    } 
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        GeoColorsPtr col = part->getColors();

        data.data = col->getData();
        if((data.stride = col->getStride()) == 0)
            data.stride = col->getFormatSize() * col->getDimension();
        
        data.func = ColorFuncs[ col->getFormat() - formatBase ]
                              [ col->getDimension() - 1 ];
                             
        if(data.func == NULL)
        {
            SWARNING << "Particles " << part << "have illegal "
                     << "colors: " << col->getDimension()
                     << "D " << formatNames[ col->getFormat() - formatBase ]
                     << "!" << std::endl;
            return;
        }
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {   
        data.func(const_cast<GLubyte*>(data.data + particle * data.stride));
        return false;
    }
    
    static inline void vertex(dataType &, UInt32, UInt32)
    {
    }
};

struct ColTraitGeneric : public ColTraitBase
{
    typedef struct
    {
        UInt8    *data;
        UInt32    stride;
        pumpFunc  func;
        bool      perParticle;
    } 
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        GeoColorsPtr col = part->getColors();
        
        data.perParticle = false;
        data.data        = NULL;
        data.stride      = 0;
        data.func        = NULL;

        if(col != NullFC)
        {
            data.data = col->getData();
            if((data.stride = col->getStride()) == 0)
                data.stride = col->getFormatSize() * col->getDimension();

            data.func = ColorFuncs[ col->getFormat() - formatBase ]
                                  [ col->getDimension() - 1 ];

            if(data.func == NULL)
            {
                SWARNING << "Particles " << part << "have illegal "
                         << "colors: " << col->getDimension()
                         << "D " << formatNames[ col->getFormat() - formatBase ]
                         << "!" << std::endl;
                return;
            }
            
            if(col->getSize() == 1)
            {
                data.func(const_cast<GLubyte*>(col->getData()));
            }
            else if(col->getSize() == part->getPositions()->getSize())
            {
                data.perParticle = true;
            }
        }
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {   
        if(data.perParticle == true)
            data.func(const_cast<GLubyte*>(data.data + particle * data.stride));
        return false;
    }
        
    static inline void vertex(dataType &, UInt32, UInt32)
    {
    }
};

/*! Position Particle Traits */

struct PosTraitNone : public ParticleTraits
{
    typedef struct
    {
    }
    dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &data,
        GeoPositionsPtr pos)
    {
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {
        return false;
    }
    
    static inline Pnt3f &position(dataType &data)
    {
        static Pnt3f null(0,0,0);
        return null;
    }
    
    static inline void vertex(dataType &data, UInt32 , Vec4f &dir, 
                              Real32 s)
    {
    }
    
    static inline void vertex(dataType &data)
    {
    }
};

struct PosTraitGeneric : public ParticleTraits
{
    typedef struct
    {
        GeoPositionsPtr pos;
        Pnt3f p;
    }
    dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &data,
        GeoPositionsPtr pos)
    {
        data.pos = pos;
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {
        data.pos->getValue(data.p, particle);
             
        return false;
    }
    
    static inline Pnt3f &position(dataType &data)
    {
        return data.p;
    }
    
    static inline void vertex(dataType &data, UInt32 , Vec4f &dir, 
                              Real32 s)
    {
        glVertex3f( data.p[0] + dir[0] * s, 
                    data.p[1] + dir[1] * s, 
                    data.p[2] + dir[2] * s);             
    }
    
    static inline void vertex(dataType &data)
    {
        glVertex3fv( static_cast<const GLfloat*>(data.p.getValues()) );
    }
};

struct PosTrait3f : public ParticleTraits
{
    typedef struct
    {
        MFPnt3f *pos;
        Pnt3f   *p;
    }
    dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &data,
        GeoPositionsPtr pos)
    {
        GeoPositions3fPtr pos3f = GeoPositions3fPtr::dcast(pos);
 
        data.pos  = pos3f->getFieldPtr();
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {
        data.p = & (*(data.pos))[particle];
        return false;
    }
    
    static inline Pnt3f &position(dataType &data)
    {
        return *data.p;
    }
    
    static inline void vertex(dataType &data, UInt32 , Vec4f &dir, Real32 s)
    {
        glVertex3f( (*data.p)[0] + dir[0] * s, 
                    (*data.p)[1] + dir[1] * s, 
                    (*data.p)[2] + dir[2] * s);             
    }
    
    static inline void vertex(dataType &data)
    {
        glVertex3fv( static_cast<const GLfloat*>((*data.p).getValues()) );
    }
};

/*! Particle Size Traits */

struct SizeTraitGeneric : public ParticleTraits
{
    typedef struct
    {
        const MFVec3f *sizes;
        Vec3f    s;
        bool     perParticle;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.sizes = part->getMFSizes();
        
        if(data.sizes != NULL)
        {
            if(data.sizes->size() == 1)
            {
                data.s = (*(data.sizes))[0];
                data.perParticle = false;
            }
            else if(data.sizes->size() == part->getPositions()->size())
            {
                data.perParticle = true;
            }
            else
            {
                data.s.setValues(1,1,1);
                data.perParticle = false;
            }           
        }
        else
        {
            data.s.setValues(1,1,1);
            data.perParticle = false;
        }           
    }
   
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline const Vec3f &size(dataType &data, UInt32 particle)
    {
        if(data.perParticle)
            return (*(data.sizes))[particle];
        return data.s;
    }
};

struct SizeTraitSingle : public ParticleTraits
{
    typedef struct
    {
        Vec3f    s;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.s = part->getSizes(0);
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline Vec3f &size(dataType &data, UInt32)
    {
        return data.s;
    }
};

struct SizeTraitParticle : public ParticleTraits
{
    typedef struct
    {
        const MFVec3f *sizes;
        Vec3f    s;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.sizes = part->getMFSizes();
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline const Vec3f &size(dataType &data, UInt32 particle)
    {
        return (*(data.sizes))[particle];
    }
};

struct SizeTraitNone : public ParticleTraits
{
    typedef UInt8 dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &)
    {
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline Vec3f &size(dataType &, UInt32 )
    {
        static Vec3f s(1,1,1);
        return s;
    }
};

/*! Particle Texture Traits */

struct TexTraitGeneric : public ParticleTraits
{
    typedef struct
    {
        const MFReal32 *texzs;
        Real32    z;
        bool      perParticle;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.texzs = part->getMFTextureZs();
        
        data.perParticle = false;
        data.z           = 0.f;

        if(data.texzs != NULL)
        {
            if(data.texzs->size() == 1)
            {
                data.z = (*(data.texzs))[0];
             }
            else if(data.texzs->size() == part->getPositions()->size())
            {
                data.perParticle = true;
            }
            else
            {
                data.z = 0;
            }           
        }
        else
        {
            data.z = 0;
        }
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {       
        if(data.perParticle)
        {
            data.z = (*(data.texzs))[particle];
        }
        return false;
    }
    
    static inline void vertex(dataType &data)
    {
        glTexCoord1f(data.z);
    }
    
    static inline void vertex(dataType &data, UInt32 , Real32 u,
                                Real32 v)
    {
        glTexCoord3f(u, v, data.z);
    }
};

struct TexTraitParticle : public ParticleTraits
{
    typedef struct
    {
        const MFReal32 *texzs;
        Real32    z;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.texzs = part->getMFTextureZs();
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {       
        data.z = (*(data.texzs))[particle];
        return false;
    }
    
    static inline void vertex(dataType &data)
    {
        glTexCoord1f(data.z);
    }
    
    static inline void vertex(dataType &data, UInt32 , Real32 u,
                                Real32 v)
    {
        glTexCoord3f(u, v, data.z);
    }
};

struct TexTraitSingle : public ParticleTraits
{
    typedef struct
    {
        Real32    z;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.z = (*(part->getMFTextureZs()))[0];
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline void vertex(dataType &data)
    {
        glTexCoord1f(data.z);
    }
    
    static inline void vertex(dataType &data, UInt32 , Real32 u,
                                Real32 v)
    {
        glTexCoord3f(u, v, data.z);
    }
};

struct TexTraitNone : public ParticleTraits
{
    typedef UInt8 dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &)
    {
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline void vertex(dataType &, UInt32 )
    {
        glTexCoord1f(0);
    }
   
    static inline void vertex(dataType &, UInt32, Real32 u, Real32 v)
    {
        glTexCoord2f(u, v);
    }
};


/*! Normal Particle Traits */

struct NormalTraitGeneric : public ParticleTraits
{
    typedef struct
    {
        GeoNormalsPtr norms;
        Vec3f         n;
        bool          perParticle;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        data.norms = part->getNormals();
        
        data.perParticle = false;
        
        if(data.norms != NullFC)
        {
            if(data.norms->size() == 1)
            {
                data.n = data.norms->getValue(0);
            }
            else if(data.norms->getSize() == part->getPositions()->getSize())
            {
                data.perParticle = true;
            }
            else
            {
                data.n.setValues(0,1,0);
            }           
        }
        else
        {
            data.n.setValues(0,1,0);
        }
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {
        if(data.perParticle)
            data.norms->getValue(data.n, particle);
             
        return false;
    }
    
    static inline Vec3f &normal(dataType &data)
    {
        return data.n;
    }
    
    static inline void normal(dataType &data, UInt32 )
    {
        glNormal3fv( static_cast<GLfloat*>(data.n.getValues()) );            
    }
};

struct NormalTraitGeneric3f : public ParticleTraits
{
    typedef struct
    {
        MFVec3f *norms;
        Vec3f   *n;
        bool     perParticle;
        Vec3f    const_n;
    }
    dataType;
    
    static inline void init(Particles *part, DrawActionBase *, dataType &data)
    {
        GeoNormals3fPtr norms3f = GeoNormals3fPtr::dcast(part->getNormals());
        
        data.norms = norms3f->getFieldPtr();
        
        data.perParticle = false;
        
        if(norms3f != NullFC)
        {
            if(norms3f->getSize() == 1)
            {
                data.n = &(*(data.norms))[0];
            }
            else if(data.norms->size() == part->getPositions()->size())
            {
                data.perParticle = true;
            }
            else
            {
                data.const_n.setValues(0,1,0);
                data.n = &data.const_n;
            }           
        }
        else
        {
            data.const_n.setValues(0,1,0);
            data.n = &data.const_n;
        }
    }
    
    static inline bool particle(dataType &data, UInt32 particle)
    {
        if(data.perParticle)
            data.n = &(*(data.norms))[particle];
             
        return false;
    }
    
    static inline Vec3f &normal(dataType &data)
    {
        return *data.n;
    }
    
    static inline void normal(dataType &data, UInt32 )
    {
        glNormal3fv( static_cast<GLfloat*>(data.n->getValues()) );            
    }
};

/* ParticlesDrawer base classes, define the common interface(s) */

struct OSG::ParticlesDrawer
{
    virtual void draw(Particles *part, DrawActionBase *action, 
                             UInt32 length) = 0;

    virtual void drawIndexed(Particles *part, 
                             DrawActionBase *action,
                             const Int32 *index, UInt32 length) = 0;
    
    virtual ~ParticlesDrawer()
    {
    }
};

/* View-Dir aligned, indexed quad rendering */

template <class posTrait, class colTrait, class texTrait, class sizeTrait>
struct drawViewDirQuads : public ParticlesDrawer 
{
    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {
        // get ModelView matrix to define the direction vectors
        Matrix camera,toworld;
        camera = action->getCameraToWorld();

        RenderAction *ra = dynamic_cast<RenderAction *>(action);
        
        if(ra != NULL)
        {
            toworld = ra->top_matrix();
        }
        else
        {
            action->getActNode()->getToWorld(toworld);
        }

        // normalize them, we don't want to neutralize scales in toworld
        toworld[0].normalize();
        toworld[1].normalize();
        toworld[2].normalize();       
        toworld.transpose();
        camera.multLeft(toworld);

        // Direction vectors
        Vec4f  d1, d2, d3, d4;

        d1 = -1.0f * camera[0] + -1.0f * camera[1];
        d2 =  1.0f * camera[0] + -1.0f * camera[1];
        d3 =  1.0f * camera[0] +  1.0f * camera[1];
        d4 = -1.0f * camera[0] +  1.0f * camera[1];

        // some variables for faster access
        GeoPositionsPtr  pos  = part->getPositions();

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        glBegin(GL_QUADS);

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle (texData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            Real32 s = sizeTrait::size(sizeData, i)[0] / 2.f;

            if(s == 0)
                continue;

            colTrait::vertex(colData, i, 0);
            texTrait::vertex(texData, 0, 0, 0);
            posTrait::vertex(posData, 0, d1, s);

            colTrait::vertex(colData, i, 1);
            texTrait::vertex(texData, 1, 1, 0);
            posTrait::vertex(posData, 1, d2, s);

            colTrait::vertex(colData, i, 2);
            texTrait::vertex(texData, 2, 1, 1);
            posTrait::vertex(posData, 2, d3, s);

            colTrait::vertex(colData, i, 3);
            texTrait::vertex(texData, 3, 0, 1);
            posTrait::vertex(posData, 3, d4, s);
        }

        glEnd();
    }

    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        // get ModelView matrix to define the direction vectors
        Matrix camera,toworld;

        camera = action->getCameraToWorld();

        RenderAction *ra = dynamic_cast<RenderAction *>(action);
        
        if(ra != NULL)
        {
            toworld = ra->top_matrix();
        }
        else
        {
            action->getActNode()->getToWorld(toworld);
        }

        // normalize them, we don't want to neutralize scales in toworld
        toworld[0].normalize();
        toworld[1].normalize();
        toworld[2].normalize();       
        toworld.transpose();
        camera.multLeft(toworld);

        // Direction vectors
        Vec4f  d1, d2, d3, d4;

        d1 = -1.0f * camera[0] + -1.0f * camera[1];
        d2 =  1.0f * camera[0] + -1.0f * camera[1];
        d3 =  1.0f * camera[0] +  1.0f * camera[1];
        d4 = -1.0f * camera[0] +  1.0f * camera[1];

        // some variables for faster access
        GeoPositionsPtr pos = part->getPositions();

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        glBegin(GL_QUADS);
       
        for(UInt32 i = 0; i < length; ++i)
        {
            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle (texData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            Real32 s = sizeTrait::size(sizeData, i)[0] / 2.f;

            if(s == 0)
                continue;
                
            colTrait::vertex(colData, i, 0);
            texTrait::vertex(texData, 0, 0, 0);
            posTrait::vertex(posData, 0, d1, s);

            colTrait::vertex(colData, i, 1);
            texTrait::vertex(texData, 1, 1, 0);
            posTrait::vertex(posData, 1, d2, s);

            colTrait::vertex(colData, i, 2);
            texTrait::vertex(texData, 2, 1, 1);
            posTrait::vertex(posData, 2, d3, s);

            colTrait::vertex(colData, i, 3);
            texTrait::vertex(texData, 3, 0, 1);
            posTrait::vertex(posData, 3, d4, s);
        }

        glEnd();
    }
};


/* Viewer aligned, indexed quad rendering */

template <class posTrait, class colTrait, class texTrait, class sizeTrait>
struct drawViewerQuads : public ParticlesDrawer 
{
    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {
        // get ModelView matrix to define the direction vectors
        Matrix camera,toworld;

        camera = action->getCameraToWorld();

        RenderAction *ra = dynamic_cast<RenderAction *>(action);
        
        if(ra != NULL)
        {
            toworld = ra->top_matrix();
        }
        else
        {
            action->getActNode()->getToWorld(toworld);
        }

        // normalize them, we don't want to neutralize scales in toworld
        toworld[0].normalize();
        toworld[1].normalize();
        toworld[2].normalize();       
        toworld.invert();
        camera.multLeft(toworld);

        // Viewer position & up
        Pnt3f  vpos(camera[3]);
        Vec3f  vup (camera[1]);
        vup.normalize();
        Vec3f  vdir,vnorm;

        // direction vector
        Vec4f d;
        
        // some variables for faster access
        GeoPositionsPtr  pos  = part->getPositions();

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        glBegin(GL_QUADS);

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle (texData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            Real32 s = sizeTrait::size(sizeData, i)[0] / 2.f;

            if(s == 0)
                continue;

            // calc viewer-relative coord system
            
            Pnt3f p = pos->getValue(i);  
            vdir = vpos - p;  
            vdir.normalize();         
            vnorm = vup.cross(vdir);
 
            d.setValues( -vnorm[0] - vup[0],
                         -vnorm[1] - vup[1],
                         -vnorm[2] - vup[2],
                         1 );
            colTrait::vertex(colData, i, 0);
            texTrait::vertex(texData, 0, 0, 0);
            posTrait::vertex(posData, 0, d, s);

            d.setValues(  vnorm[0] - vup[0],
                          vnorm[1] - vup[1],
                          vnorm[2] - vup[2],
                         1 );
            colTrait::vertex(colData, i, 1);
            texTrait::vertex(texData, 1, 1, 0);
            posTrait::vertex(posData, 1, d, s);

            d.setValues(  vnorm[0] + vup[0],
                          vnorm[1] + vup[1],
                          vnorm[2] + vup[2],
                         1 );
            colTrait::vertex(colData, i, 2);
            texTrait::vertex(texData, 2, 1, 1);
            posTrait::vertex(posData, 2, d, s);

            d.setValues( -vnorm[0] + vup[0],
                         -vnorm[1] + vup[1],
                         -vnorm[2] + vup[2],
                         1 );
            colTrait::vertex(colData, i, 3);
            texTrait::vertex(texData, 3, 0, 1);
            posTrait::vertex(posData, 3, d, s);
        }

        glEnd();
    }

    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        // get ModelView matrix to define the direction vectors
        Matrix camera,toworld;

        camera = action->getCameraToWorld();

        RenderAction *ra = dynamic_cast<RenderAction *>(action);
        
        if(ra != NULL)
        {
            toworld = ra->top_matrix();
        }
        else
        {
            action->getActNode()->getToWorld(toworld);
        }


        // normalize them, we don't want to neutralize scales in toworld
        toworld[0].normalize();
        toworld[1].normalize();
        toworld[2].normalize();       
        toworld.invert();
        camera.multLeft(toworld);

        // Viewer position & up
        Pnt3f  vpos(camera[3]);
        Vec3f  vup (camera[1]);
        vup.normalize();
        Vec3f  vdir,vnorm;

        // direction vector
        Vec4f d;
        
        // some variables for faster access
        GeoPositionsPtr pos = part->getPositions();

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        glBegin(GL_QUADS);
       
        for(UInt32 i = 0; i < length; ++i)
        {
            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle (texData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            Real32 s = sizeTrait::size(sizeData, i)[0] / 2.f;

            if(s == 0)
                continue;

            // calc viewer-relative coord system
            
            Pnt3f p = pos->getValue(i);  
            vdir = vpos - p;  
            vdir.normalize();         
            vnorm = vup.cross(vdir);
 
            d.setValues( -vnorm[0] - vup[0],
                         -vnorm[1] - vup[1],
                         -vnorm[2] - vup[2],
                         1 );
            colTrait::vertex(colData, i, 0);
            texTrait::vertex(texData, 0, 0, 0);
            posTrait::vertex(posData, 0, d, s);

            d.setValues(  vnorm[0] - vup[0],
                          vnorm[1] - vup[1],
                          vnorm[2] - vup[2],
                         1 );
            colTrait::vertex(colData, i, 1);
            texTrait::vertex(texData, 1, 1, 0);
            posTrait::vertex(posData, 1, d, s);

            d.setValues(  vnorm[0] + vup[0],
                          vnorm[1] + vup[1],
                          vnorm[2] + vup[2],
                         1 );
            colTrait::vertex(colData, i, 2);
            texTrait::vertex(texData, 2, 1, 1);
            posTrait::vertex(posData, 2, d, s);

            d.setValues( -vnorm[0] + vup[0],
                         -vnorm[1] + vup[1],
                         -vnorm[2] + vup[2],
                         1 );
            colTrait::vertex(colData, i, 3);
            texTrait::vertex(texData, 3, 0, 1);
            posTrait::vertex(posData, 3, d, s);
        }

        glEnd();
    }
};



/* Single color lines between secPos and pos */

template <class posTrait, class colTrait, class texTrait, class sizeTrait>
struct drawLines : public ParticlesDrawer 
{
    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        // some variables for faster access
        GeoPositionsPtr  pos  = part->getPositions();

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename posTrait::dataType secPosData;
        posTrait::init(part, action, secPosData, part->getSecPositions());

        Real32 s = sizeTrait::size(sizeData, 0)[0];

        glPushAttrib(GL_LINE_BIT);
        glLineWidth(s);
          
        glBegin(GL_LINES);
        
        for(UInt32 i = 0; i < length; ++i)
        {
            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle (texData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(posTrait::particle(secPosData, i))
                continue;

            posTrait::vertex(posData);
            posTrait::vertex(secPosData);
        }

        glEnd();
    
        glPopAttrib();
    }

    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {
        // some variables for faster access
        GeoPositionsPtr  pos  = part->getPositions();

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename posTrait::dataType secPosData;
        posTrait::init(part, action, secPosData, part->getSecPositions());

        Real32 s = sizeTrait::size(sizeData, 0)[0];

        glPushAttrib(GL_LINE_BIT);
        glLineWidth(s);
          
        glBegin(GL_LINES);

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle (texData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(posTrait::particle(secPosData, i))
                continue;

            posTrait::vertex(posData);
            posTrait::vertex(secPosData);
        }

        glEnd();
    
        glPopAttrib();
    }
};

/*  Points at pos */

template <class posTrait, class colTrait, class texTrait, class sizeTrait>
struct drawPoints : public ParticlesDrawer 
{
    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        // some variables for faster access
        GeoPositionsPtr  pos  = part->getPositions();

        // init traits
        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        sizeTrait::particle(sizeData, 0);
        Real32 s = sizeTrait::size(sizeData, 0)[0];

        glPushAttrib(GL_POINT_BIT);
        glPointSize(s);
          
        glBegin(GL_POINTS);
        
        for(UInt32 i = 0; i < length; ++i)
        {
            if(colTrait::particle(colData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(texTrait::particle(texData, i))
                continue;

            texTrait::vertex(texData);
            posTrait::vertex(posData);
        }

        glEnd();
    
        glPopAttrib();
    }

    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {
        // some variables for faster access
        GeoPositionsPtr  pos  = part->getPositions();

        // init traits
        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        sizeTrait::particle(sizeData, 0);
        Real32 s = sizeTrait::size(sizeData, 0)[0];

        glPushAttrib(GL_POINT_BIT);
        glPointSize(s);
          
        glBegin(GL_POINTS);

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(colTrait::particle(colData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            posTrait::vertex(posData);
        }

        glEnd();
    
        glPopAttrib();
    }
};

/* Geometry traits for drawing objects */

struct GeoTraitArrow : public ParticleTraits
{
    typedef UInt8 dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &)
    {
    }
    
    static inline void exit(Particles *, DrawActionBase *, dataType &)
    {
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline void draw(dataType &, Pnt3f &p, Pnt3f &,
                            Vec3f &dx, Vec3f &dy, Vec3f &dz,
                            const Vec3f &s)
    {
        dz*=s[2];
        dx*=s[0];
       
        glNormal3fv(static_cast<GLfloat*>(dy.getValues()));
        
        glBegin(GL_TRIANGLE_FAN);
        
        glVertex3fv(static_cast<GLfloat*>(p.getValues()));
        
        glVertex3f(p[0] + dz[0] * .5f + dx[0]      ,
                   p[1] + dz[1] * .5f + dx[1]      ,
                   p[2] + dz[2] * .5f + dx[2]      );

        glVertex3f(p[0] + dz[0] * .5f + dx[0] * .5f ,
                   p[1] + dz[1] * .5f + dx[1] * .5f ,
                   p[2] + dz[2] * .5f + dx[2] * .5f );

        glVertex3f(p[0] + dz[0]       + dx[0] * .5f ,
                   p[1] + dz[1]       + dx[1] * .5f ,
                   p[2] + dz[2]       + dx[2] * .5f );

        glVertex3f(p[0] + dz[0]       - dx[0] * .5f ,
                   p[1] + dz[1]       - dx[1] * .5f ,
                   p[2] + dz[2]       - dx[2] * .5f );

        glVertex3f(p[0] + dz[0] * .5f - dx[0] * .5f ,
                   p[1] + dz[1] * .5f - dx[1] * .5f ,
                   p[2] + dz[2] * .5f - dx[2] * .5f );

        glVertex3f(p[0] + dz[0] * .5f - dx[0]      ,
                   p[1] + dz[1] * .5f - dx[1]      ,
                   p[2] + dz[2] * .5f - dx[2]      );
       
        glEnd();
    }
};


/* Geometry traits for drawing rectangles */

struct GeoTraitRectangle : public ParticleTraits
{
    typedef UInt8 dataType;
    
    static inline void init(Particles *, DrawActionBase *, dataType &)
    {
        glBegin(GL_QUADS);
    }
    
    static inline void exit(Particles *, DrawActionBase *, dataType &)
    {
        glEnd();
    }
    
    static inline bool particle(dataType &, UInt32)
    {
        return false;
    }
    
    static inline void draw(dataType &, Pnt3f &p, Pnt3f &sp,
                            Vec3f &dx, Vec3f &dy, Vec3f &,
                            const Vec3f &s)
    {
        dx *= s[0] * .5f;
       
        glNormal3fv(static_cast<GLfloat*>(dy.getValues()));

        glVertex3f( p[0] - dx[0],
                    p[1] - dx[1],
                    p[2] - dx[2]);

        glVertex3f( p[0] + dx[0],
                    p[1] + dx[1],
                    p[2] + dx[2]);

        glVertex3f(sp[0] + dx[0],
                   sp[1] + dx[1],
                   sp[2] + dx[2]);

        glVertex3f(sp[0] - dx[0],
                   sp[1] - dx[1],
                   sp[2] - dx[2]);
    }
};



/* Objects using the pos/secpos/normal coordinate system for rendering */

template <class posTrait, class colTrait, class sizeTrait, class normalTrait, 
          class geoTrait>
struct drawObjects : public ParticlesDrawer 
{
    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        // some variables for faster access
        GeoPositionsPtr pos = part->getPositions();

        // init traits
        typename geoTrait::dataType geoData;
        geoTrait::init(part, action, geoData);

        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename normalTrait::dataType normalData;
        normalTrait::init(part, action, normalData);

        typename posTrait::dataType posData, secPosData;
        posTrait::init(part, action, posData   , part->getPositions());
        posTrait::init(part, action, secPosData, part->getSecPositions());
        
        for(UInt32 i = 0; i < length; ++i)
        {
            if(geoTrait::particle (geoData,  i))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(normalTrait::particle(normalData, i))
                continue;

            if(posTrait::particle (posData,  i))
                continue;

            if(posTrait::particle (secPosData,  i))
                continue;

            // calc the coordinate system
            
            Pnt3f &p  = posTrait   ::position(posData   );
            Pnt3f &sp = posTrait   ::position(secPosData);
            Vec3f &n  = normalTrait::normal  (normalData);
           
            Vec3f dz(sp - p);
            dz.normalize();

            Vec3f dx(n.cross(dz));
            if(dx.isZero())
            {
                dx=Vec3f(1,0,0).cross(dz);
                if(dx.isZero())
                {
                    dx=Vec3f(0,1,0).cross(dz);
                    if(dx.isZero())
                    {
                        dx=Vec3f(0,0,1).cross(dz);
                    }
                    else
                    {
                        dx.setValues(0,0,0);
                    }
                }
            }
            dx.normalize();

            Vec3f dy(dx.cross(dz));

            // now draw the geometry;
            geoTrait::draw(geoData, p, sp, dx, dy, dz, 
                           sizeTrait::size(sizeData, i));
        }
        
        geoTrait::exit(part, action, geoData);
    }
    
    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {
        // some variables for faster access
        GeoPositionsPtr pos = part->getPositions();

        // init traits
        typename geoTrait::dataType geoData;
        geoTrait::init(part, action, geoData);

        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename normalTrait::dataType normalData;
        normalTrait::init(part, action, normalData);

        typename posTrait::dataType posData, secPosData;
        posTrait::init(part, action, posData   , part->getPositions());
        posTrait::init(part, action, secPosData, part->getSecPositions());

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(geoTrait::particle (geoData,  i))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(normalTrait::particle(normalData, i))
                continue;

            if(posTrait::particle (posData,  i))
                continue;

            if(posTrait::particle (secPosData,  i))
                continue;

            // calc the coordinate system
            
            Pnt3f &p  = posTrait   ::position(posData   );
            Pnt3f &sp = posTrait   ::position(secPosData);
            Vec3f &n  = normalTrait::normal  (normalData);
           
            Vec3f dz(sp - p);
            dz.normalize();

            Vec3f dx(n.cross(dz));
            if(dx.isZero())
            {
                dx=Vec3f(1,0,0).cross(dz);
                if(dx.isZero())
                {
                    dx=Vec3f(0,1,0).cross(dz);
                    if(dx.isZero())
                    {
                        dx=Vec3f(0,0,1).cross(dz);
                    }
                    else
                    {
                        dx.setValues(0,0,0);
                    }
                }
            }
            dx.normalize();

            Vec3f dy(dx.cross(dz));

            // now draw the geometry;
            geoTrait::draw(geoData, p, sp, dx, dy, dz, 
                           sizeTrait::size(sizeData, i));
        }
        
        geoTrait::exit(part, action, geoData);
    }
};


/* Objects using the pos/secpos and viewer-directed coordinate system for 
   rendering */

template <class posTrait, class colTrait, class sizeTrait, 
          class geoTrait>
struct drawViewerObjects : public ParticlesDrawer 
{
    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        // get ModelView matrix to define the direction vectors
        Matrix camera,toworld;
        action->getCamera()->getBeacon()->getToWorld(camera);
        action->getActNode()->getToWorld(toworld);
        // normalize them, we don't want to neutralize scales in toworld
        toworld[0].normalize();
        toworld[1].normalize();
        toworld[2].normalize();       
        toworld.invert();
        camera.multLeft(toworld);

        // Viewer position & up
        Pnt3f  vpos(camera[3]);

        // some variables for faster access
        GeoPositionsPtr pos = part->getPositions();

        // init traits
        typename geoTrait::dataType geoData;
        geoTrait::init(part, action, geoData);

        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData, secPosData;
        posTrait::init(part, action, posData   , part->getPositions());
        posTrait::init(part, action, secPosData, part->getSecPositions());
        
        for(UInt32 i = 0; i < length; ++i)
        {
            if(geoTrait::particle (geoData,  i))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle (posData,  i))
                continue;

            if(posTrait::particle (secPosData,  i))
                continue;

            // calc the coordinate system
            
            Pnt3f &p  = posTrait::position(posData   );
            Pnt3f &sp = posTrait::position(secPosData);
            Vec3f  n  = vpos - p;
           
            Vec3f dz(sp - p);
            dz.normalize();

            Vec3f dx(n.cross(dz));
            if(dx.isZero())
            {
                dx=Vec3f(1,0,0).cross(dz);
                if(dx.isZero())
                {
                    dx=Vec3f(0,1,0).cross(dz);
                    if(dx.isZero())
                    {
                        dx=Vec3f(0,0,1).cross(dz);
                    }
                    else
                    {
                        dx.setValues(0,0,0);
                    }
                }
            }
            dx.normalize();

            Vec3f dy(dx.cross(dz));

            // now draw the geometry;
            geoTrait::draw(geoData, p, sp, dx, dy, dz, 
                           sizeTrait::size(sizeData, i));
        }
         
        geoTrait::exit(part, action, geoData);
   }
    
    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {
        // get ModelView matrix to define the direction vectors
        Matrix camera,toworld;
        action->getCamera()->getBeacon()->getToWorld(camera);
        action->getActNode()->getToWorld(toworld);
        // normalize them, we don't want to neutralize scales in toworld
        toworld[0].normalize();
        toworld[1].normalize();
        toworld[2].normalize();       
        toworld.invert();
        camera.multLeft(toworld);

        // Viewer position & up
        Pnt3f  vpos(camera[3]);

        // some variables for faster access
        GeoPositionsPtr pos = part->getPositions();

        // init traits
        typename geoTrait::dataType geoData;
        geoTrait::init(part, action, geoData);

        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename posTrait::dataType posData, secPosData;
        posTrait::init(part, action, posData   , part->getPositions());
        posTrait::init(part, action, secPosData, part->getSecPositions());

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(geoTrait::particle (geoData,  i))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(posTrait::particle (posData,  i))
                continue;

            if(posTrait::particle (secPosData,  i))
                continue;

            // calc the coordinate system
            
            Pnt3f &p  = posTrait   ::position(posData   );
            Pnt3f &sp = posTrait   ::position(secPosData);
            Vec3f  n  = vpos - p;
            
            Vec3f dz(sp - p);
            dz.normalize();

            Vec3f dx(n.cross(dz));
            if(dx.isZero())
            {
                dx=Vec3f(1,0,0).cross(dz);
                if(dx.isZero())
                {
                    dx=Vec3f(0,1,0).cross(dz);
                    if(dx.isZero())
                    {
                        dx=Vec3f(0,0,1).cross(dz);
                    }
                    else
                    {
                        dx.setValues(0,0,0);
                    }
                }
            }
            dx.normalize();

            Vec3f dy(dx.cross(dz));

            // now draw the geometry;
            geoTrait::draw(geoData, p, sp, dx, dy, dz, 
                           sizeTrait::size(sizeData, i));
        }
        
        geoTrait::exit(part, action, geoData);
    }
};


/* Static coordinates quad, pass all parameters to shader */

template <class posTrait, class secPosTrait, class colTrait, class texTrait, 
          class sizeTrait, class normalTrait>
struct drawShaderQuads : public ParticlesDrawer 
{
    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {   
        Window *win = action->getWindow();
        
        void (OSG_APIENTRY*_glMultiTexCoord3fvARB)(GLenum which, 
                                                   GLubyte * data)=
            reinterpret_cast<void (OSG_APIENTRY*) (GLenum which, 
                                                   GLubyte * data)>(
                win->getFunction(_funcglMultiTexCoord3fvARB));

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename normalTrait::dataType normalData;
        normalTrait::init(part, action, normalData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename secPosTrait::dataType secPosData;
        secPosTrait::init(part, action, secPosData, part->getSecPositions());

        GeoPositionsPtr pos = part->getPositions();
        
        glBegin(GL_QUADS);

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle(texData, i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(normalTrait::particle(normalData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(secPosTrait::particle(secPosData, i))
                continue;

            
            Vec3f s = sizeTrait::size(sizeData, i);

            if(s[0] == 0)
                continue;
            
            normalTrait::normal(normalData, 0);
            
            _glMultiTexCoord3fvARB(
                GL_TEXTURE1_ARB,
                reinterpret_cast<GLubyte*>(
                    posTrait::position(posData).getValues()));

            _glMultiTexCoord3fvARB(
                GL_TEXTURE2_ARB,
                reinterpret_cast<GLubyte*>(
                    secPosTrait::position(secPosData).getValues()));

            _glMultiTexCoord3fvARB(GL_TEXTURE3_ARB,
                                   reinterpret_cast<GLubyte*>(s.getValues()));
            
            texTrait::vertex(texData, 0, 0, 0);
            glVertex2f      (-.5f, -.5f);
            texTrait::vertex(texData, 0, 1, 0);
            glVertex2f      ( .5f, -.5f);
            texTrait::vertex(texData, 0, 1, 1);
            glVertex2f      ( .5f,  .5f);
            texTrait::vertex(texData, 0, 0, 1);
            glVertex2f      (-.5f,  .5f);
        }

        glEnd();
    }

    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        Window *win = action->getWindow();
        
        void (OSG_APIENTRY*_glMultiTexCoord3fvARB)(GLenum which, 
                                                   GLubyte * data)=
            reinterpret_cast<void (OSG_APIENTRY*) (GLenum which,
                                                   GLubyte * data)>(
                win->getFunction(_funcglMultiTexCoord3fvARB));

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename normalTrait::dataType normalData;
        normalTrait::init(part, action, normalData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename secPosTrait::dataType secPosData;
        secPosTrait::init(part, action, secPosData, part->getSecPositions());

        GeoPositionsPtr pos = part->getPositions();
        
        glBegin(GL_QUADS);

        Int32 i;
 
        for(UInt32 i = 0; i < length; ++i)
        {
            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle(texData, i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(normalTrait::particle(normalData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(secPosTrait::particle(secPosData, i))
                continue;

            
            Vec3f s = sizeTrait::size(sizeData, i);

            if(s[0] == 0)
                continue;
            
            normalTrait::normal(normalData, 0);
            
            _glMultiTexCoord3fvARB(
                GL_TEXTURE1_ARB,
                reinterpret_cast<GLubyte*>(
                    posTrait::position(posData).getValues()));

            _glMultiTexCoord3fvARB(
                GL_TEXTURE2_ARB,
                reinterpret_cast<GLubyte*>(
                    secPosTrait::position(secPosData).getValues()));

            _glMultiTexCoord3fvARB(GL_TEXTURE3_ARB,
                                   reinterpret_cast<GLubyte*>(s.getValues()));
            
            texTrait::vertex(texData, 0, 0, 0);
            glVertex2f      (-.5f, -.5f);
            texTrait::vertex(texData, 0, 1, 0);
            glVertex2f      ( .5f, -.5f);
            texTrait::vertex(texData, 0, 1, 1);
            glVertex2f      ( .5f,  .5f);
            texTrait::vertex(texData, 0, 0, 1);
            glVertex2f      (-.5f,  .5f);
        }

        glEnd();
    }
};


/* Static coordinates strips with y coordinates between -.5 and .5, split 
   into size[2] parts, pass all parameters to shader */

template <class posTrait, class secPosTrait, class colTrait, class texTrait, 
          class sizeTrait, class normalTrait>
struct drawShaderStrips : public ParticlesDrawer 
{
    virtual void drawIndexed(Particles *part, DrawActionBase *action, 
                             const Int32 *index, UInt32 length)
    {   
        Window *win = action->getWindow();
        
        void (OSG_APIENTRY*_glMultiTexCoord3fvARB)(GLenum which, 
                                                   GLubyte * data)=
            reinterpret_cast<void (OSG_APIENTRY*) (GLenum which, 
                                                   GLubyte * data)>(
                win->getFunction(_funcglMultiTexCoord3fvARB));

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename normalTrait::dataType normalData;
        normalTrait::init(part, action, normalData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename secPosTrait::dataType secPosData;
        secPosTrait::init(part, action, secPosData, part->getSecPositions());

        GeoPositionsPtr pos = part->getPositions();

        Int32 i;

        for(UInt32 ii = 0; ii < length; ++ii)
        {
            i = index[ii];

            if(i < 0 || i > Int32(pos->getSize()))
                continue;

            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle(texData, i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(normalTrait::particle(normalData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(secPosTrait::particle(secPosData, i))
                continue;

            
            Vec3f s = sizeTrait::size(sizeData, i);

            if(s[0] == 0)
                continue;
             
            normalTrait::normal(normalData, 0);
           
            _glMultiTexCoord3fvARB(
                GL_TEXTURE1_ARB,
                reinterpret_cast<GLubyte *>(
                    posTrait::position(posData).getValues()));

            _glMultiTexCoord3fvARB(
                GL_TEXTURE2_ARB,
                reinterpret_cast<GLubyte*>(
                    secPosTrait::position(secPosData).getValues()));
            
            _glMultiTexCoord3fvARB(GL_TEXTURE3_ARB, 
                                   reinterpret_cast<GLubyte*>(s.getValues()));
         
            glBegin(GL_QUAD_STRIP);
           
            UInt32 n = static_cast<UInt32>(s[2]);           
            Real32 step = 1.f / (n-1);
            Real32 v = 0.f;
            for(int y = 0; y < n; ++y, v += step)
            {             
                texTrait::vertex(texData, 0, 0, v);
                glVertex2f  (-.5f, v-.5f);
                texTrait::vertex(texData, 0, 1, v);
                glVertex2f  ( .5f, v-.5f);
            }
            
            glEnd();
        }

    }

    virtual void draw(Particles *part, DrawActionBase *action, UInt32 length)
    {
        Window *win = action->getWindow();
        
        void (OSG_APIENTRY*_glMultiTexCoord3fvARB)(GLenum which, 
                                                   GLubyte * data)=
            reinterpret_cast<void (OSG_APIENTRY*)(GLenum which, 
                                                  GLubyte * data)>(
                win->getFunction(_funcglMultiTexCoord3fvARB));

        // init traits
        typename colTrait::dataType colData;
        colTrait::init(part, action, colData);

        typename texTrait::dataType texData;
        texTrait::init(part, action, texData);

        typename sizeTrait::dataType sizeData;
        sizeTrait::init(part, action, sizeData);

        typename normalTrait::dataType normalData;
        normalTrait::init(part, action, normalData);

        typename posTrait::dataType posData;
        posTrait::init(part, action, posData, part->getPositions());

        typename secPosTrait::dataType secPosData;
        secPosTrait::init(part, action, secPosData, part->getSecPositions());

        GeoPositionsPtr pos = part->getPositions();

        Int32 i;
 
        for(UInt32 i = 0; i < length; ++i)
        {
            if(colTrait::particle (colData,  i))
                continue;

            if(texTrait::particle(texData, i))
                continue;

            if(sizeTrait::particle(sizeData, i))
                continue;

            if(normalTrait::particle(normalData, i))
                continue;

            if(posTrait::particle(posData, i))
                continue;

            if(secPosTrait::particle(secPosData, i))
                continue;

            
            Vec3f s = sizeTrait::size(sizeData, i);

            if(s[0] == 0)
                continue;
             
            normalTrait::normal(normalData, 0);
           
            _glMultiTexCoord3fvARB(
                GL_TEXTURE1_ARB,
                reinterpret_cast<GLubyte*>(
                    posTrait::position(posData).getValues()));

            _glMultiTexCoord3fvARB(
                GL_TEXTURE2_ARB,
                reinterpret_cast<GLubyte*>(
                    secPosTrait::position(secPosData).getValues()));

            _glMultiTexCoord3fvARB(GL_TEXTURE3_ARB, 
                                   reinterpret_cast<GLubyte*>(s.getValues()));
         
            glBegin(GL_QUAD_STRIP);
           
            UInt32 n = static_cast<UInt32>(s[2]);           
            Real32 step = 1.f / (n-1);
            Real32 v = 0.f;
            for(int y = 0; y < n; ++y, v += step)
            {
                texTrait::vertex(texData, 0, 0, v);
                glVertex2f  (-.5f, v-.5f);
                texTrait::vertex(texData, 0, 1, v);
                glVertex2f  ( .5f, v-.5f);
            }
            
            glEnd();
        }
    }
};




/* Sorting functions */


struct sorter
{
    sorter() {}

    sorter(Real32 value, Int32 index)
    {
        _value=value;
        _index=index;
    }

    bool operator<(const sorter &a) const
    {
        if(_value < a._value)
            return true;

        return false;
    }

    bool operator<=(const sorter &a) const
    {
        if(_value <= a._value)
            return true;

        return false;
    }

    bool operator>(const sorter &a) const
    {
        return ! (a < *this);
    }

    bool operator>=(const sorter &a) const
    {
        return ! (a <= *this);
    }

    Real32 _value;
    Int32  _index;
};

Int32 *Particles::calcIndex(DrawActionBase *action, UInt32 &len, 
                            Int32 *index)
{ 
    // some variables for faster access
    GeoPositionsPtr  pos     = getPositions();
    const MFInt32   *indices = getMFIndices();
 
    // get ModelView matrix to define the direction vectors
    Matrix camera,toworld;

    camera = action->getCameraToWorld();
    
    RenderAction *ra = dynamic_cast<RenderAction *>(action);
    
    if(ra != NULL)
    {
        toworld = ra->top_matrix();
    }
    else
    {
        action->getActNode()->getToWorld(toworld);
    }
    
    toworld.invert();
    camera.multLeft(toworld);

    Pnt3f refpoint(camera[3].getValues());
    
    if(getMode() == ViewDirQuads)
        camera.invert();
        
    Pnt3f p,q;
    UInt32 size;
    
    if(indices->size() > 0)
    {   
        if(getNumParticles() == -1)
        {
            size = indices->size();
        }
        else
        {
            size = getNumParticles();
        }
    }
    else
    {
        if(getNumParticles() == -1)
        {
            size = pos->size();
        }
        else
        {
            size = getNumParticles();
        }
    }
   
    std::vector<sorter> sorterList(size);
    
    len = 0;
    UInt32 i;
    
    if(getMode() == ViewDirQuads)
    {
        if(indices->size() > 0)
        {
            for(i = 0; i < size; i++)
            {
                if((*(indices))[i] < 0 || 
                   (*(indices))[i] > Int32(pos->size()))
                    continue;

                sorterList[len]._index = (*(indices))[i];

                pos->getValue(p,i);                
                camera.mult(p);      
                sorterList[len]._value = p[2];

                len++;
            }
        }
        else
        {
            for(i = 0; i < size; i++)
            {
                sorterList[len]._index = i;         

                pos->getValue(p,i);       
                camera.mult(p);      
                sorterList[len]._value = p[2];

                len++;
            }
        }
    }
    else
    {
        if(indices->size() > 0)
        {
            for(i = 0; i < size; i++)
            {
                if((*(indices))[i] < 0 || 
                   (*(indices))[i] > Int32(pos->size()))
                    continue;

                sorterList[len]._index = (*(indices))[i];

                pos->getValue(p,i);       
                sorterList[len]._value = - refpoint.dist2(p);

                len++;
            }
        }
        else
        {
            for(i = 0; i < size; i++)
            {
                sorterList[len]._index = i;         

                pos->getValue(p,i);       
                sorterList[len]._value = - refpoint.dist2(p);

                len++;
            }
        }
    }
    
    std::sort(sorterList.begin(), sorterList.begin() + len);
    
    if(index == NULL)
        index=new Int32[len];
    
    if(getDrawOrder() == FrontToBack)
    {
        for(i = 0; i<len; i++)
        {
            index[i] = sorterList[len - 1 - i]._index;
        }
    }
    else
    {
        for(i = 0; i<len; i++)
        {
            index[i] = sorterList[i]._index;
        }
    }
       
    return index;    
}

/*! \} */

#endif // remove from user docu

/*! Low-level Draw method that pumps OpenGL commands.
*/
Action::ResultE Particles::drawPrimitives(DrawActionBase * action)
{
    // some variables for faster access
    GeoPositionsPtr  pos  = getPositions();
    GeoColorsPtr     col  = getColors();
    GeoNormalsPtr    norm = getNormals();
    const MFVec3f   *size = getMFSizes();
 
    if((size   ->size() > 1 && size   ->size() != pos->getSize())  ||
       (col  != NullFC && col->getSize()  != 1 &&
                          col->getSize()  != pos->getSize())       ||
       (norm != NullFC && norm->getSize() != 1 &&
                          norm->getSize() != pos->getSize())
      )
    {
        FWARNING(("Particles::draw: inconsistent attributes "
                    "(p:%d s:%d c:%d)!\n",
                    pos->getSize(), size->size(),
                  (col != NullFC)? int(col->getSize()) : -1));
        return Action::Continue;
    }

    ParticlesDrawer *drawer = findDrawer();
    
    if(drawer == NULL)
    {
        FWARNING(("Particles 0x%lx: couldn't find drawer!\n", this));
        return Action::Continue;;
    }
    
    const Int32 *index = NULL;
    UInt32 length = 0;
    bool freeIndex = false;
    
    if(_sfDrawOrder.getValue()!=Particles::Any)
    {
        if(getDynamic())
        {
            index=calcIndex(action,length);
        }
        else
        {
            if(!getBsp().created())
            {
                editBsp().build(this);
            }
            Matrix modelview,toworld;

            modelview = action->getCameraToWorld();
    
            RenderAction *ra = dynamic_cast<RenderAction *>(action);
            
            if(ra != NULL)
            {
                toworld = ra->top_matrix();
            }
            else
            {
                action->getActNode()->getToWorld(toworld);
            }

//            action->getCamera()->getBeacon()->getToWorld(modelview);
//            action->getActNode()->getToWorld(toworld);

            toworld.invert();
            modelview.mult(toworld);

            Pnt3f ref(modelview[3][0],modelview[3][1],
                      modelview[3][2]);
            Vec3f refvec(  modelview[2][0],modelview[2][1],
                      modelview[2][2]);  

            index = getBsp().traverse(ref,length);
        }
        freeIndex = true;
    }
    else if (getMFIndices()->size() > 0)
    {
        index  = &getMFIndices()->getValues()[0];
        if(getNumParticles() == -1)
        {
            length = getMFIndices()->size();
        }
        else
        {
            length = getNumParticles();
        }
    }

    if(index != NULL)
    {
        if(length > 0)
            drawer->drawIndexed(this,action,index,length);
    }
    else
    {
        if(getNumParticles() == -1)
        {
            drawer->draw(this,action,pos->getSize());  
        }
        else if(getNumParticles() > 0)
        {
            drawer->draw(this,action,getNumParticles());         
        } 
    }
    
    if(freeIndex)
        delete [] index;
        
    return Action::Continue;
}

/*! find the drawer object for the actual configuration of parameters */

ParticlesDrawer *Particles::findDrawer(void)
{
    if(getPositions() == NullFC)
        return NULL;

    UInt8 mode;
    enum { part = 0, sing, none } size,normal,color,tex,secpos;
    
    mode = getMode();
    
    // find the parameters' use
    
    size =   (getMFSizes()->size()      == getPositions()->getSize()) ? part :
             (getMFSizes()->size()      == 1                        ) ? sing : 
                                                                        none;
    normal = (getNormals() != NullFC && 
              getNormals()->getSize()   == getPositions()->getSize()) ? part :
             (getNormals() != NullFC && 
              getNormals()->getSize()   == 1                        ) ? sing : 
                                                                        none;
    secpos = (getSecPositions() != NullFC && 
              getSecPositions()->getSize()== getPositions()->getSize())?part :
             (getSecPositions() != NullFC && 
              getSecPositions()->getSize()== 1                        )?sing : 
                                                                        none;
    color =  (getColors() != NullFC && 
              getColors()->getSize()    == getPositions()->getSize()) ? part :
             (getColors() != NullFC && 
              getColors()->getSize()    == 1                        ) ? sing : 
                                                                        none;
    tex =    (getMFTextureZs()->size()  == getPositions()->getSize()) ? part :
             (getMFTextureZs()->size()  == 1                        ) ? sing : 
                                                                        none;
    
    // check if the used types are common cases
    
/*
    bool normalIs3f = (getNormals()   != NullFC && 
                       getNormals()->getFormat()      == GL_FLOAT);
                        
    bool colorIs3f  = (getColors()    != NullFC && 
                       getColors()->getFormat()       == GL_FLOAT && 
                       getColors()->getDimension()    == 3);
 */
                        
    bool posIs3f    = (getPositions() != NullFC && 
                       getPositions()->getFormat()    == GL_FLOAT && 
                       getPositions()->getDimension() == 3);
    
    // now find the drawer
    
    // main difference: mode, as only a subset of the attributes is used
    // in the different modes
    
    // this is where to add fast pathes...
    // don't go overboard, as every path adds code, and code bloat is
    // easily obtained here.
    
    switch(mode)
    {
    case Points:
    {
        static ParticlesDrawer *fallback = 
                    new drawPoints<PosTraitGeneric,ColTraitGeneric,
                                   TexTraitGeneric,SizeTraitGeneric>;
        
        return fallback;
    }
        
    case Lines:
    {
        if(secpos == none)
        {
            FWARNING(("Particles::findDrawer: Need secondary positions for "
                      "type Lines!\n"));
            return NULL;
        }
        
        static ParticlesDrawer *fallback = 
                    new drawLines<PosTraitGeneric,ColTraitGeneric,
                                  TexTraitGeneric,SizeTraitGeneric>;
        
        return fallback;
    }
    
    case ViewDirQuads:
    {
        static ParticlesDrawer *fallback = 
                    new drawViewDirQuads<PosTraitGeneric,ColTraitGeneric,
                                         TexTraitGeneric,SizeTraitGeneric>;
        
        if(posIs3f && tex == none && size != none)
        {
            static ParticlesDrawer *pumps[] = {
              new drawViewDirQuads<PosTrait3f,ColTraitParticle,
                                   TexTraitNone,SizeTraitParticle>,
              new drawViewDirQuads<PosTrait3f,ColTraitSingle,
                                   TexTraitNone,SizeTraitParticle>,
              new drawViewDirQuads<PosTrait3f,ColTraitNone,
                                   TexTraitNone,SizeTraitParticle>,
                                   
              new drawViewDirQuads<PosTrait3f,ColTraitParticle,
                                   TexTraitNone,SizeTraitSingle>,
              new drawViewDirQuads<PosTrait3f,ColTraitSingle,
                                   TexTraitNone,SizeTraitSingle>,
              new drawViewDirQuads<PosTrait3f,ColTraitNone,
                                   TexTraitNone,SizeTraitSingle>,
              };
              
              return pumps[size * 3 + color];
        }
        
        return fallback;
    }
    
    case ViewerQuads:
    {
        static ParticlesDrawer *fallback = 
                    new drawViewerQuads<PosTraitGeneric,ColTraitGeneric,
                                         TexTraitGeneric,SizeTraitGeneric>;
        
        if(posIs3f && tex == none && size != none)
        {
            static ParticlesDrawer *pumps[] = {
              new drawViewerQuads<PosTrait3f,ColTraitParticle,
                                  TexTraitNone,SizeTraitParticle>,
              new drawViewerQuads<PosTrait3f,ColTraitSingle,
                                  TexTraitNone,SizeTraitParticle>,
              new drawViewerQuads<PosTrait3f,ColTraitNone,
                                  TexTraitNone,SizeTraitParticle>,
                                   
              new drawViewerQuads<PosTrait3f,ColTraitParticle,
                                  TexTraitNone,SizeTraitSingle>,
              new drawViewerQuads<PosTrait3f,ColTraitSingle,
                                  TexTraitNone,SizeTraitSingle>,
              new drawViewerQuads<PosTrait3f,ColTraitNone,
                                  TexTraitNone,SizeTraitSingle>,
              };
              
              return pumps[size * 3 + color];
        }
        
        return fallback;
    }
    
    case Arrows:
    {
        if(secpos == none)
        {
            FWARNING(("Particles::findDrawer: Need secondary positions for "
                      "type Arrows!\n"));
            return NULL;
        }
        
        if(normal == none)
        {
            FWARNING(("Particles::findDrawer: Need normals for "
                      "type Arrows!\n"));
            return NULL;
        }
        
        static ParticlesDrawer *fallback = 
                    new drawObjects<PosTraitGeneric,ColTraitGeneric, 
                                    SizeTraitGeneric,NormalTraitGeneric,
                                    GeoTraitArrow>;
        
        return fallback;
    }
    
    case ViewerArrows:
    {
        if(secpos == none)
        {
            FWARNING(("Particles::findDrawer: Need secondary positions for "
                      "type ViewerArrows!\n"));
            return NULL;
        }
        
        if(normal == none)
        {
            FWARNING(("Particles::findDrawer: Need normals for "
                      "type ViewerArrows!\n"));
            return NULL;
        }
        
        static ParticlesDrawer *fallback = 
                    new drawViewerObjects<PosTraitGeneric,ColTraitGeneric, 
                                          SizeTraitGeneric,GeoTraitArrow>;
        
        return fallback;
    }
    
    case Rectangles:
    {
        if(secpos == none)
        {
            FWARNING(("Particles::findDrawer: Need secondary positions for "
                      "type Rectangles!\n"));
            return NULL;
        }
        
        if(normal == none)
        {
            FWARNING(("Particles::findDrawer: Need normals for "
                      "type Rectangles!\n"));
            return NULL;
        }
        
        static ParticlesDrawer *fallback = 
                    new drawObjects<PosTraitGeneric,ColTraitGeneric, 
                                    SizeTraitGeneric,NormalTraitGeneric,
                                    GeoTraitRectangle>;
        
        return fallback;
    }
    
    case ShaderQuads:
    {        
        static ParticlesDrawer *fallback = 
                    new drawShaderQuads<PosTraitGeneric,PosTraitGeneric,
                                        ColTraitGeneric, TexTraitGeneric,
                                        SizeTraitGeneric,NormalTraitGeneric>,
                               *nosecfallback =  
                    new drawShaderQuads<PosTraitGeneric,PosTraitNone,
                                        ColTraitGeneric, TexTraitGeneric,
                                        SizeTraitGeneric,NormalTraitGeneric>;
        
        if(secpos == none)
        {
            return nosecfallback;
        }
        else
        {
            return fallback;
        }
    }
    
    case ShaderStrips:
    {
        static ParticlesDrawer *fallback = 
                    new drawShaderStrips<PosTraitGeneric,PosTraitGeneric,
                                         ColTraitGeneric, TexTraitGeneric,
                                         SizeTraitGeneric,NormalTraitGeneric>,
                               *nosecfallback =  
                    new drawShaderStrips<PosTraitGeneric,PosTraitNone,
                                         ColTraitGeneric, TexTraitGeneric,
                                         SizeTraitGeneric,NormalTraitGeneric>;
               
        if(secpos == none)
        {
            return nosecfallback;
        }
        else
        {
            return fallback;
        }
    }
    
    };

    
    if (normal) return NULL; // make picky compilers happy
    return NULL;
}
 
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
    static char cvsid_cpp[] = "@(#)$Id: OSGParticles.cpp,v 1.24 2002/06/30 05:04:22 vossg Exp $";
    static char cvsid_hpp[] = OSGPARTICLES_HEADER_CVSID;
    static char cvsid_inl[] = OSGPARTICLES_INLINE_CVSID;
}
