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

#ifndef _OSGPRINTNAMEACTOR_H_
#define _OSGPRINTNAMEACTOR_H_
#ifdef __sgi
#pragma once
#endif

//----------------------------------------------------------------------------
//    Includes
//----------------------------------------------------------------------------

#include <OSGConfig.h>

#include <OSGPrintNameActorBase.h>

OSG_BEGIN_NAMESPACE

class OSG_SYSTEMLIB_DLLMAPPING PrintNameActor : public PrintNameActorBase
{
    /*====  PUBLIC  =========================================================*/
  public:
    /*-----------------------------------------------------------------------*/
    /*! \name    Types                                                       */
    /*! \{                                                                   */

    typedef BasicActorBase::ResultE ResultE;
    typedef BasicActorBase::Functor Functor;

    /*! \}                                                                   */
    /*-----------------------------------------------------------------------*/
    /*! \name    Create                                                      */
    /*! \{                                                                   */

    static PrintNameActor *create(void);

    /*! \}                                                                   */
    /*-----------------------------------------------------------------------*/
    /*! \name    Destructor                                                  */
    /*! \{                                                                   */

    virtual ~PrintNameActor(void);

    /*! \}                                                                   */
    /*-----------------------------------------------------------------------*/
    /*! \name    Enter/Leave                                                 */
    /*! \{                                                                   */


    /*! \}                                                                   */
    /*==== PROTECTED ========================================================*/
  protected:
    /*-----------------------------------------------------------------------*/
    /*! \name    Constructor                                                 */
    /*! \{                                                                   */

    PrintNameActor(void);

    /*! \}                                                                   */
    /*==== PRIVATE ==========================================================*/
  private:
    typedef PrintNameActorBase Inherited;
};

OSG_END_NAMESPACE

#include <OSGPrintNameActor.inl>

#define OSGPRINTNAMEACTOR_HEADER_CVSID "@(#)$Id:"

#endif /* _OSGPRINTNAMEACTOR_H_ */
