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

#ifndef _OSGDYNAMICVOLUME_H_
#define _OSGDYNAMICVOLUME_H_

#include <OSGBaseTypes.h>
#include <OSGBaseFunctions.h>

#include <OSGVolume.h>

OSG_BEGIN_NAMESPACE

/*! 3D dynamic volume.
  \ingroup GrpBaseBaseVolume  
*/

class OSG_BASE_DLLMAPPING DynamicVolume : public Volume
{
    /*==========================  PUBLIC  =================================*/

  public:

    enum Type 
    { 
        BOX_VOLUME, 
        SPHERE_VOLUME 
    };

    /*---------------------------------------------------------------------*/
    /*! \name                   Constructors                               */
    /*! \{                                                                 */

    DynamicVolume(      Type           type = BOX_VOLUME);
    DynamicVolume(const DynamicVolume &obj              );

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                   Destructors                                */
    /*! \{                                                                 */

    virtual ~DynamicVolume(void);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Class Specific                            */
    /*! \{                                                                 */

    const Volume &getInstance    (void     ) const;
          Volume &getInstance    (void     );
          void    instanceChanged(void     );


          Type    getType        (void     ) const;
          void    setVolumeType  (Type type);
          void    morphToType    (Type type);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Get                                    */
    /*! \{                                                                 */

    virtual void   getCenter      (Pnt3f &center           ) const;
    virtual Real32 getScalarVolume(void                    ) const;
    virtual void   getBounds      (Pnt3f &min,   Pnt3f &max) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Extend                                 */
    /*! \{                                                                 */

    virtual void extendBy(const Pnt3f  &pt    );
    virtual void extendBy(const Volume &volume);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                    Intersection                              */
    /*! \{                                                                 */

    virtual bool intersect  (const Pnt3f  &point ) const;
    virtual bool intersect  (const Line   &line  ) const;
    virtual bool intersect  (const Line   &line,
                                   Real32 &enter, 
                                   Real32 &exit  ) const;
    virtual bool intersect  (const Volume &volume) const;
    virtual bool isOnSurface(const Pnt3f  &point ) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Transform                              */
    /*! \{                                                                 */

    virtual void transform (const Matrix &matrix);

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Output                                 */
    /*! \{                                                                 */

    virtual void dump(      UInt32    uiIndent = 0,
                      const BitVector bvFlags  = 0) const;

    /*! \}                                                                 */
    /*---------------------------------------------------------------------*/
    /*! \name                       Operators                              */
    /*! \{                                                                 */

    bool           operator ==(const DynamicVolume &other ) const;

    DynamicVolume &operator = (const DynamicVolume &source);

    /*! \}                                                                 */
    /*=========================  PROTECTED  ===============================*/

  protected:

    /*==========================  PRIVATE  ================================*/

  private:

    Type   _type;
    UChar8 _volumeMem[32];
};

OSG_BASE_DLLMAPPING
std::ostream &operator <<(      std::ostream  &outStream,
                          const DynamicVolume &vol);

typedef DynamicVolume* DynamicVolumeP;

OSG_END_NAMESPACE

#include "OSGDynamicVolume.inl"

#endif // _OSGDYNAMICVOLUME_H_
