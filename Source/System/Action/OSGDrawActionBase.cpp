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
#include <OSGWindow.h>
#include <OSGCamera.h>
#include <OSGViewport.h>
#include <OSGGeometry.h>
#include <OSGDrawable.h>

#include <OSGGL.h>
#include <OSGVolumeDraw.h>

#include <OSGDrawActionBase.h>

OSG_USING_NAMESPACE


/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::DrawActionBase
    \ingroup GrpSystemAction

The base class for all rendering-related actions.

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

char DrawActionBase::cvsid[] = "@(#)$Id: $";

StatElemDesc<StatTimeElem>  
DrawActionBase::statTravTime ("travTime", "time for traversal");

StatElemDesc<StatIntElem>   
DrawActionBase::statCullTestedNodes("cullTestedNodes", "nodes tested");

StatElemDesc<StatIntElem>   
DrawActionBase::statCulledNodes("culledNodes", "nodes culled from frustum");

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

DrawActionBase::DrawActionBase(void) :
     Inherited     (     ),
    _camera        (NULL ),
    _background    (NULL ),
    _window        (NULL ),
    _viewport      (NULL ),
    _statistics    (NULL ),
    _ownStat       (false),
    _frustumCulling(true ),
    _volumeDrawing (false),
    _autoFrustum   (true ),
    _frustum       (     ),
    _mCameraToWorld(     )
{
}


DrawActionBase::DrawActionBase(const DrawActionBase &source) :
     Inherited     (source                 ),
    _camera        (source._camera         ),
    _background    (source._background     ),
    _window        (source._window         ),
    _viewport      (source._viewport       ),
    _statistics    (source._statistics     ),
    _ownStat       (source._ownStat        ),
    _frustumCulling(source._frustumCulling ),
    _volumeDrawing (source._volumeDrawing  ),
    _autoFrustum   (source._autoFrustum    ),
    _frustum       (source._frustum        ),
    _mCameraToWorld(source._mCameraToWorld )
{
}

/** \brief Destructor
 */

DrawActionBase::~DrawActionBase(void)
{
#if 0 // Altered for last frame time

#else
  if (_ownStat) {
     delete _statistics;
  }
#endif
}

/*------------------------------ start -----------------------------------*/


Action::ResultE DrawActionBase::start(void)
{
    if(getFrustumCulling() == true &&
       getAutoFrustum   () == true &&
       getCamera        () != NULL &&
       getViewport      () != NULL)
    {
        getCamera()->getFrustum( _frustum, *getViewport() );
//_frustum.dump();
    }

    if(_camera != NULL && getViewport() != NULL)
    {
        _camera->getViewing( _mCameraToWorld,
                             getViewport()->getPixelWidth(),
                             getViewport()->getPixelHeight() );
        _mCameraToWorld.invert();
    }

//    cerr << "DA::start" << endl;

//    cerr << _mCameraToWorld << endl << endl;
    

#if 0 // Altered for last frame time
    if(_statistics == NULL)
    {
        _statistics = StatCollector::create();
        _ownStat = true;
    }
    else
    {
        _ownStat = false;        
    }
#else
    if(_statistics == NULL)
    {
        _statistics = StatCollector::create();
        _ownStat = true;
    }
#endif

    getStatistics()->getElem(statTravTime)->start();
    getStatistics()->getElem(statCullTestedNodes)->reset();
    getStatistics()->getElem(statCulledNodes)->reset();
   
    // this really doesn't belong here, but don't know a better place to put it
    if(getStatistics()->getElem(Drawable::statNTriangles,false))
    {
        getStatistics()->getElem(Drawable::statNTriangles)->set(0);
        getStatistics()->getElem(Drawable::statNLines)->set(0);
        getStatistics()->getElem(Drawable::statNPoints)->set(0);
        getStatistics()->getElem(Drawable::statNVertices)->set(0);
    }

//fprintf(stderr,"%p: start\n", Thread::getCurrent());
        
    return Action::Continue;
}

Action::ResultE DrawActionBase::stop(Action::ResultE res)
{
    if ( getVolumeDrawing() )
        drawVolume( _frustum );  
 
    getStatistics()->getElem(statTravTime)->stop();
  
#if 0 // Altered for last frame time
    if(_ownStat)
    {
        delete _statistics;
        _statistics = NULL;
    }
    else
    {
        _ownStat = false;        
    }
#endif

    return res; 
}

/*------------------------------ access -----------------------------------*/

void DrawActionBase::setViewport(Viewport *viewport)
{
    _viewport = viewport;
}

void DrawActionBase::setCamera(Camera *cam)
{
    _camera = cam;
}

void DrawActionBase::setBackground(Background *background)
{
    _background = background;
}

void DrawActionBase::setWindow(Window *window)
{
    _window = window;
}

void DrawActionBase::setStatistics(StatCollector *statistics)
{
#if 0 // Altered for last frame time
    _statistics = statistics;
    _ownStat = false;
#else
    if (_ownStat) {
       delete _statistics;
    }
    _statistics = statistics;
    _ownStat = false;
#endif
}


// do frustum culling at all?
// default true

void DrawActionBase::setFrustumCulling(bool frustumCulling)
{
    _frustumCulling = frustumCulling;
}

// automatically calc the frustum at the beginning of the traversal
// default true

void DrawActionBase::setAutoFrustum(bool autoFrustum)
{
    _autoFrustum = autoFrustum;
}

// draw the tested volumes
// default false

void DrawActionBase::setVolumeDrawing(bool volumeDrawing)
{
    _volumeDrawing = volumeDrawing;
}

// explicitly set the frustum

void DrawActionBase::setFrustum(FrustumVolume &frustum)
{
    _frustum = frustum;
}

// select all visible nodes
UInt32 DrawActionBase::selectVisibles(void)
{
    if(getFrustumCulling() == false)
        return getNNodes();

    useNodeList();

    Color3f col;
    
    UInt32 count = 0;
    for ( UInt32 i = 0; i < getNNodes(); i++ )
    {
        if ( isVisible( getNode(i).getCPtr() ) )
        {
            col.setValuesRGB(0,1,0);
            addNode( getNode(i) );
            ++count;
        }
        else
            col.setValuesRGB(1,0,0);
        
        if(getVolumeDrawing())
        {
            dropVolume(this, getNode(i), col);
        }
    }

    return count;
}


