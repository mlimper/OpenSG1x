/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *               Copyright (C) 2000-2002 by the OpenSG Forum                 *
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
#include <OSGTypedFunctors.h>
#include <OSGQuaternion.h>
#include <OSGDrawAction.h>
#include <OSGMultiPassMaterial.h>

#include <OSGMatrix.h>
#include <OSGMatrixUtility.h>

#include <OSGBackground.h>
#include <OSGForeground.h>
#include <OSGImage.h>
#include <OSGMaterialGroup.h>
#include <OSGGeometry.h>

#include <OSGLight.h>

#include "OSGShadowViewport.h"
#include "OSGStdShadowMap.h"
#include "OSGPerspectiveShadowMap.h"
#include "OSGDitherShadowMap.h"
#include "OSGPCFShadowMap.h"
#include "OSGPCSSShadowMap.h"


//--------------------------------------------------------------------
#ifndef GL_CLAMP_TO_EDGE
#   define GL_CLAMP_TO_EDGE    0x812F
#endif
#ifndef GL_CLAMP_TO_BORDER_ARB
#   define GL_CLAMP_TO_BORDER_ARB  0x812D
#endif
#ifndef GL_ARB_depth_texture
#   define GL_DEPTH_COMPONENT16_ARB    0x81A5
#   define GL_DEPTH_COMPONENT24_ARB    0x81A6
#   define GL_DEPTH_COMPONENT32_ARB    0x81A7
#   define GL_TEXTURE_DEPTH_SIZE_ARB   0x884A
#   define GL_DEPTH_TEXTURE_MODE_ARB   0x884B
#endif
#ifndef GL_ARB_shadow
#   define GL_TEXTURE_COMPARE_MODE_ARB 0x884C
#   define GL_TEXTURE_COMPARE_FUNC_ARB 0x884D
#   define GL_COMPARE_R_TO_TEXTURE_ARB 0x884E
#endif
#ifndef GL_SGIX_depth_texture
#   define GL_DEPTH_COMPONENT16_SGIX   0x81A5
#   define GL_DEPTH_COMPONENT24_SGIX   0x81A6
#   define GL_DEPTH_COMPONENT32_SGIX   0x81A7
#endif
#ifndef GL_SGIX_shadow
#   define GL_TEXTURE_COMPARE_SGIX             0x819A
#   define GL_TEXTURE_COMPARE_OPERATOR_SGIX    0x819B
#   define GL_TEXTURE_LEQUAL_R_SGIX            0x819C
#   define GL_TEXTURE_GEQUAL_R_SGIX            0x819D
#endif
#ifndef GL_DEPTH_COMPONENT_ARB
#   define GL_DEPTH_COMPONENT_ARB            0x1902
#endif

//--------------------------------------------------------------------
OSG_USING_NAMESPACE

UInt32 ShadowViewport::_depth_texture_extension;
UInt32 ShadowViewport::_shadow_extension;

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

/***************************************************************************\
 *                           Class methods                                 *
\***************************************************************************/

void ShadowViewport::initMethod (void)
{
}


/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  private                                                                 -
\*-------------------------------------------------------------------------*/

/*----------------------- constructors & destructors ----------------------*/

ShadowViewport::ShadowViewport(void) :
    Inherited(),
    _mapRenderSize(128),
    _mapSizeChanged(false),
    _texGen(NullFC),
    _poly(NullFC),
    _dummy(NullFC),
    _silentBack(NullFC),
    _lights(),
    _oldLights(),
    _lightCameras(),
    _lightCamTrans(),
    _lightCamBeacons(),
    _lightStates(),
    _shadowImages(),
    _texChunks(),
    _trigger_update(false)
{
    _depth_texture_extension = Window::registerExtension("GL_ARB_depth_texture");
    _shadow_extension = Window::registerExtension("GL_ARB_shadow");
	treeRenderer = 0;
}

ShadowViewport::ShadowViewport(const ShadowViewport &source) :
    Inherited(source),
    _mapRenderSize(source._mapRenderSize),
    _mapSizeChanged(source._mapSizeChanged),
    _texGen(source._texGen),
    _poly(source._poly),
    _dummy(source._dummy),
    _silentBack(source._silentBack),
    _lights(source._lights),
    _oldLights(source._oldLights),
    _lightCameras(source._lightCameras),
    _lightCamTrans(source._lightCamTrans),
    _lightCamBeacons(source._lightCamBeacons),
    _lightStates(source._lightStates),
    _shadowImages(source._shadowImages),
    _texChunks(source._texChunks),
    _trigger_update(source._trigger_update)
{
    _depth_texture_extension = Window::registerExtension("GL_ARB_depth_texture");
    _shadow_extension = Window::registerExtension("GL_ARB_shadow");
	treeRenderer = 0;
}

ShadowViewport::~ShadowViewport(void)
{
}

/*----------------------------- class specific ----------------------------*/

void ShadowViewport::changed(BitVector whichField, UInt32 origin)
{
	if(whichField & OffBiasFieldMask ||
       whichField & OffFactorFieldMask)
    {
        FDEBUG(("ShadowViewport::changed : ofsset bias/factor changed.\n"));
        // Setting up Polygon-Chunk with Depth-Offset
        beginEditCP(_poly);
        {
            _poly->setOffsetBias(getOffBias());
            _poly->setOffsetFactor(getOffFactor());
            _poly->setOffsetFill(true);
        }
        endEditCP(_poly);
    }

    if(whichField & MapSizeFieldMask)
    {
        FDEBUG(("ShadowViewport::changed : map size changed.\n"));
        _mapSizeChanged = true;
		printf("Mapsize set to %d\n", getMapSize());
    }

    if(whichField & LightNodesFieldMask)
    {
        FDEBUG(("ShadowViewport::changed : light nodes changed.\n"));
        _lights.clear();
        for(UInt32 i=0;i<getLightNodes().getSize();++i)
            _lights.push_back(LightPtr::dcast(getLightNodes()[i]->getCore()));
    }

    if(whichField & MapAutoUpdateFieldMask)
    {
        _trigger_update = true;
    }

	if(whichField & QualityModeFieldMask)
	{
		if (getQualityMode())
		printf("Quality Mode on\n");
		else printf("Quality Mode off\n");
	}

	if(whichField & RangeFieldMask)
	{
		if (getRange() < 0.0)
		{
			printf("No Range < 0 allowed!\n");
			setRange(0.0);
		}
		printf("Range set to %f\n",getRange());
	}

    if(whichField & ShadowModeFieldMask)
    {
		delete treeRenderer;
		treeRenderer = 0;

		switch (getShadowMode())
		{

		case NO_SHADOW:
		{
			printf("No Shadows\n");
			//Viewport::render(action);
		}
		break;
	
		case STD_SHADOW_MAP:
		{
			printf("using standard Shadow Mapping...\n");
			treeRenderer = new StdShadowMap(this);			
		}
		break;

		case PERSPECTIVE_SHADOW_MAP:
		{
			printf("using Lisp Perspective Shadow Mapping...\n");
			treeRenderer = new PerspectiveShadowMap(this);
			
		}
		break;

		case DITHER_SHADOW_MAP:
		{
			printf("using Dither Shadow Mapping...\n");
			treeRenderer = new DitherShadowMap(this);
		}
		break;

		case PCF_SHADOW_MAP:
		{
			printf("using PCF Shadow Mapping...\n");
			treeRenderer = new PCFShadowMap(this);
		}
		break;

		case PCSS_SHADOW_MAP:
		{
			printf("using PCSS Shadow Mapping...\n");
			treeRenderer = new PCSSShadowMap(this);
		}
		break;

		default: break;
		}
    }

    Inherited::changed(whichField, origin);
}

void ShadowViewport::dump(      UInt32    , 
                         const BitVector ) const
{
    SLOG << "Dump ShadowViewport NI" << std::endl;
}

void ShadowViewport::setVPSize(UInt32 a,UInt32 b, UInt32 c, UInt32 d)
{
	beginEditCP(getPtr(), LeftFieldMask | RightFieldMask |
                          BottomFieldMask | TopFieldMask);
    {
        this->setSize(a,b,c,d);
    }
    endEditCP(getPtr(), LeftFieldMask | RightFieldMask |
                        BottomFieldMask | TopFieldMask);
}

void ShadowViewport::triggerMapUpdate(void)
{
    ShadowViewportPtr tmpPtr(*this);
    beginEditCP(tmpPtr, MapAutoUpdateFieldMask);
    endEditCP(tmpPtr, MapAutoUpdateFieldMask);
}


void ShadowViewport::onCreate(const ShadowViewport *OSG_CHECK_ARG(source))
{
    // if we're in startup this is the prototype ...
    if(OSG::GlobalSystemState == OSG::Startup)
        return;

	treeRenderer = 0;

    _mapRenderSize = 128;

    _dummy = makeCoredNode<Group>();
    addRefCP(_dummy);

    _texGen = TexGenChunk::create();
    addRefCP(_texGen);
    beginEditCP(_texGen);//------Setting up TexGen--------------
    {
        _texGen->setSBeacon(_dummy);
        _texGen->setTBeacon(_dummy);
        _texGen->setRBeacon(_dummy);
        _texGen->setQBeacon(_dummy);
            
        _texGen->setGenFuncS(GL_EYE_LINEAR);
        _texGen->setGenFuncT(GL_EYE_LINEAR);
        _texGen->setGenFuncR(GL_EYE_LINEAR);
        _texGen->setGenFuncQ(GL_EYE_LINEAR);
    }
    endEditCP(_texGen);

    _poly = PolygonChunk::create();
    addRefCP(_poly);

	_offset = PolygonChunk::create();
    addRefCP(_offset);

    _silentBack = PassiveBackground::create();
    addRefCP(_silentBack);
}

void ShadowViewport::onDestroy(void)
{
    clearLights(_lights.size());
    subRefCP(_silentBack);
    subRefCP(_poly);
    subRefCP(_texGen);
    subRefCP(_dummy);
}

void ShadowViewport::render(RenderActionBase* action)
{
	if (treeRenderer == 0) Viewport::render(action);
	else
	{
    if(getCamera() == NullFC)
    {
        SWARNING << "ShadowViewport::render: no camera!" << std::endl;
        return;
    }
    if(getBackground() == NullFC)
    {
        SWARNING << "ShadowViewport::render: no Background!" << std::endl;
        return;
    }
    if(getRoot() == NullFC)
    {
        SWARNING << "ShadowViewport::render: no root!" << std::endl;
        return;
    }

    if(!getShadowOn())
    {
        Viewport::render(action);
        return;
    }


	if(!extensionCheck())
    {
        SWARNING << "No Shadowmap-Extensions available!" << endLog;
    }
    else
    {

		if(getSceneRoot() == NullFC)
        {
            //beginEditCP(getPtr(), SceneRootFieldMask);
                setSceneRoot(getRoot());
            //endEditCP(getPtr(), SceneRootFieldMask);
        }


        action->setCamera    (getCamera().getCPtr());
        action->setBackground(_silentBack.getCPtr());
        action->setViewport  (this);
        action->setTravMask  (getTravMask());

        checkMapResolution();
		checkLights(action);

		// find transparent nodes
        _transparent.clear();
        traverse(getRoot(), osgTypedMethodFunctor1ObjPtrCPtrRef
             <Action::ResultE, ShadowViewport, NodePtr>
             (this, &ShadowViewport::findTransparent));
	}

	_windowW = getParent()->getWidth();
    _windowH = getParent()->getHeight();

	treeRenderer->render(action);
	}
}

//Checks if the needed OpenGL-Extensions are supported
bool ShadowViewport::extensionCheck()
{
    if(!this->getParent()->hasExtension(_depth_texture_extension))
    {
        SWARNING << "I need ARB_depth_texture-Extension!" << endLog;
    }
    else if(!this->getParent()->hasExtension(_shadow_extension))
    {
        SWARNING << "I need ARB_shadow-Extension!" << endLog;
    }
    else
    {
        FDEBUG((" Extensions-Check done!\n"));
        return true;
    }
    
    return false;
}

// Checks window-Size and determines best size of ShadowMap. Default is 128
void ShadowViewport::checkMapResolution()
{
    UInt32 minSize = 0;
    
    if(this->getParent() != NullFC)
    {
        //Checking for the smallest Window-Dimension
        minSize = this->getPixelWidth();

        if(this->getPixelHeight() < minSize)
            minSize = this->getPixelHeight();

        //Checking for biggest PowerOf2 that fits in smallest Window-Dimension
        _mapRenderSize = osgnextpower2(minSize + 1) / 2;
        if(_mapRenderSize == 0)
            _mapRenderSize = 128;
    }
    else
    {
        _mapRenderSize = 128;
        FDEBUG(("Could not find Window. Map will be set to 128\n"));
    }

    UInt32 mapSize = osgnextpower2(getMapSize() + 1) / 2;
    if(mapSize < _mapRenderSize)
        mapSize = _mapRenderSize;

    GLint max_tex_size = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_tex_size);
    
    if(mapSize > max_tex_size)
        mapSize = max_tex_size;
    
    beginEditCP(getPtr(), ShadowViewport::MapSizeFieldMask);
        setMapSize(mapSize);
    beginEditCP(getPtr(), ShadowViewport::MapSizeFieldMask);
}

Action::ResultE ShadowViewport::findLight(NodePtr& node)
{
    if(node->getCore()->getType().isDerivedFrom(Light::getClassType()))
         _allLights.push_back(LightPtr::dcast(node->getCore()));
    return Action::Continue;
}

Action::ResultE ShadowViewport::findTransparent(NodePtr& node)
{
    if(node->getCore() != NullFC)
    {
        if(node->getCore()->getType() == Geometry::getClassType() ||
           node->getCore()->getType() == MaterialGroup::getClassType())
        {
            MaterialPtr mat;
            MultiPassMaterialPtr multiMat;
            osg::GeometryPtr geo = osg::GeometryPtr::dcast(node->getCore());
            osg::MaterialGroupPtr matGroup = osg::MaterialGroupPtr::dcast(node->getCore());
            if(geo != NullFC)
                mat = geo->getMaterial();
            if(matGroup != NullFC)
                mat = matGroup->getMaterial();
            if(mat != NullFC)
            {
                /* isTransparent in MultiPassMaterial returns false,
                   if one Material is not transparent. Here we need
                   to know if one Material is transparent so we can't
                   use isTransparent for MultiPassMaterials. */
                multiMat = MultiPassMaterialPtr::dcast(mat);
                if(multiMat != NullFC)
                {
                    MFMaterialPtr::const_iterator it = multiMat->getMaterials().begin();
                    MFMaterialPtr::const_iterator matsEnd = multiMat->getMaterials().end();
                    for(; it != matsEnd; ++it)
                    {
                        if((*it) == NullFC)
                            continue;
                        if((*it)->isTransparent())
                        {
                            _transparent.push_back(node);
                            break;
                        }
                    }
                }
                else
                {
                    if(mat->isTransparent())
                        _transparent.push_back(node);
                }
            }

        }

        if(node->getCore()->getType() == Geometry::getClassType())
        {
            osg::GeometryPtr geo = osg::GeometryPtr::dcast(node->getCore());
            if(geo->getMaterial() != NullFC &&
               geo->getMaterial()->isTransparent())
                _transparent.push_back(node);
        }
        else if(node->getCore()->getType() == MaterialGroup::getClassType())
        {
            osg::MaterialGroupPtr matGroup = osg::MaterialGroupPtr::dcast(node->getCore());
            if(matGroup->getMaterial() != NullFC &&
               matGroup->getMaterial()->isTransparent())
                _transparent.push_back(node);
        }
    }
    return Action::Continue;
}



void ShadowViewport::checkLights(RenderActionBase* action)
{
    //Finding lights by going through whole Scenegraph
    _allLights.clear();
    traverse(getRoot(), osgTypedMethodFunctor1ObjPtrCPtrRef
             <Action::ResultE, ShadowViewport, NodePtr>
             (this, &ShadowViewport::findLight));

    //shadow for all lights
    if(getLightNodes().getSize() == 0)
        _lights = _allLights;

    _lightStates.clear();
    bool changed = false;
    if(_lights.size() > 0 && _lights.size() == _oldLights.size())
    {
        for(UInt32 i=0;i<_lights.size();++i)
        {
            _lightStates.push_back(_lights[i]->getOn()); 
            if(_lights[i] != _oldLights[i])
                changed = true;
        }
    }
    else
    {
        changed = true;
    }

    if(!changed)
    {
        if(_mapSizeChanged)
            changed = true;
    }

    if(!changed)
    {
        updateLights();
        return;
    }

    _mapSizeChanged = false;

    initializeLights(action);
}

void ShadowViewport::updateLights(void)
{
    SpotLightPtr tmpSpot;
    DirectionalLightPtr tmpDir;
    PointLightPtr tmpPoint;
    Matrix tmpMatrix;
    bool isSpot, isDirect;
    Real32 sceneWidth = 0.0;
    Real32 sceneHeight = 0.0;

    for(UInt32 i = 0; i < _lights.size(); ++i)
    {
        //Giving new Camera Rotation and Position of the light it belongs to
        beginEditCP(_lightCamTrans[i]);
        {
            Quaternion q;

            tmpMatrix.setIdentity();

            //Is the Lightsource a Spotlight?
            if(_lights[i]->getType() == SpotLight::getClassType())
            {
                //Casting to Spotlight
                tmpSpot = SpotLightPtr::dcast(_lights[i]);
                FDEBUG(("Found Spotlight!\n"));
                isSpot = true;
                isDirect = false;

                Pnt3f lightpos = tmpSpot->getPosition();
                Vec3f lightdir = tmpSpot->getDirection();
                if(tmpSpot->getBeacon() != NullFC)
                {
                    Matrix m = tmpSpot->getBeacon()->getToWorld();
                    m.mult(lightpos);
                    m.mult(lightdir);
                }

                //<-- ???
                q.setValue(Vec3f(0,0,-1), lightdir);
                tmpMatrix.setTransform(Vec3f(lightpos),q);
            }
            else if(_lights[i]->getType() == DirectionalLight::getClassType())
            {
                Vec3f diff;
                Pnt3f center;

                tmpDir = DirectionalLightPtr::dcast(_lights[i]);
                FDEBUG(("Found Directionallight!\n"));
                isSpot = false;
                isDirect = true;

                diff = (getSceneRoot()->getVolume().getMax() -
                        getSceneRoot()->getVolume().getMin());

                sceneWidth = diff.length() * 0.5;
                // Not final values. May get tweaked in the future
                sceneHeight = diff.length() * 0.5;

                getSceneRoot()->getVolume().getCenter(center);

                Vec3f lightdir = tmpDir->getDirection();
                if(tmpDir->getBeacon() != NullFC)
                {
                    Matrix m = tmpDir->getBeacon()->getToWorld();
                    m.mult(lightdir);
                }

                MatrixLookAt(tmpMatrix,center + lightdir,
                             center,Vec3f(0,1,0));
                tmpMatrix.invert();
            }
            else // Preparation for PointLight -- In this version just a hack
            {
                Vec3f dir;
                Pnt3f center;

                tmpPoint = PointLightPtr::dcast(_lights[i]);
                FDEBUG(("Found PointLight!\n"));
                isSpot = false;
                isDirect = false;

                /* Direction of LightCam is a hack. 
                   It always looks at the center of the scene.
                   If the light is placed INSIDE of your Scene
                  (surrounded by Geometry, like a bulb in a Room)
                   the result of the Shadows may be not what you wanted.
                   So, please use the PointLight only outside of your 
                   Scene/outside the Bounding-Box of the lit geometry
                */

                Pnt3f lightpos = tmpPoint->getPosition();
                if(tmpPoint->getBeacon() != NullFC)
                {
                    Matrix m = tmpPoint->getBeacon()->getToWorld();
                    m.mult(lightpos);
                }

                getSceneRoot()->getVolume().getCenter(center);
                
                dir = lightpos - center;
                dir.normalize();
                dir.negate();

                q.setValue(Vec3f(0,0,-1),dir);

                tmpMatrix.setTransform(Vec3f(lightpos),q);

            }

            _lightCamTrans[i]->setMatrix(tmpMatrix);
        }
        endEditCP(_lightCamTrans[i]);

        //Feeding new Camera with paramters of original camera
        beginEditCP(_lightCameras[i]);
        {
            // Is the Lightsource a Spotlight?
            if(isSpot)
            {
				tmpSpot = SpotLightPtr::dcast(_lights[i]);

				_lightCameras[i]->setNear(getCamera()->getNear());

				
				Pnt3f lightpos = tmpSpot->getPosition();
                if(tmpSpot->getBeacon() != NullFC)
                {
                    Matrix m = tmpSpot->getBeacon()->getToWorld();
                    m.mult(lightpos);
                }

				Pnt3f center;
                getSceneRoot()->getVolume().getCenter(center);
                Vec3f dir = lightpos - center;
				Real32 dirLength = dir.length();

				Vec3f diff = (getSceneRoot()->getVolume().getMax() -
                        getSceneRoot()->getVolume().getMin());
				Real32 diffLength = diff.length();

				_lightCameras[i]->setFar(dirLength+diffLength);
                //Using Spot-angle of Spotlight as FOV for LightCamera
                PerspectiveCameraPtr::dcast(_lightCameras[i])->
                                            setFov(tmpSpot->getSpotCutOffDeg()*2);
            }
            // Is the Lightsource a Directional-Light? Setting up MatrixCamera
            else if(isDirect)
            {
                Matrix proMatrix,modMatrix;

                proMatrix.setIdentity();
                modMatrix.setIdentity();

                MatrixOrthogonal(proMatrix,-sceneWidth,sceneWidth,-sceneHeight,
                                 sceneHeight,-sceneWidth,sceneWidth);
        
                // Grabbing ModelView-Matrix from Light-Transformation
                modMatrix = _lightCamTrans[i]->getMatrix();

                MatrixCameraPtr::dcast(_lightCameras[i])->setProjectionMatrix(proMatrix);
                MatrixCameraPtr::dcast(_lightCameras[i])->setModelviewMatrix(modMatrix);
                
            }
            else // If none of above the Lightsource must be a PointLight
            {
                Vec3f dist,diff;
                Pnt3f center;
                Real32 angle;

                getSceneRoot()->getVolume().getCenter(center);
                Pnt3f lightpos = tmpPoint->getPosition();
                if(tmpPoint->getBeacon() != NullFC)
                {
                    Matrix m = tmpPoint->getBeacon()->getToWorld();
                    m.mult(lightpos);
                }
                
                dist =  (lightpos - center);
                diff = (getSceneRoot()->getVolume().getMax() -
                        getSceneRoot()->getVolume().getMin());

				Pnt3f sceneMin = getSceneRoot()->getVolume().getMin();
				Pnt3f sceneMax = getSceneRoot()->getVolume().getMax();

				Real32 distLength = dist.length();
				Real32 diffLength = diff.length();

				_lightCameras[i]->setNear(getCamera()->getNear());
				_lightCameras[i]->setFar(distLength+diffLength);

                //angle = atan((diff.length()* 0.5)/dist.length());
				//Scene Bounding Box Points

				Pnt3f bb[8];
				bb[0]=Pnt3f(sceneMin[0],sceneMin[1],sceneMin[2]);
				bb[1]=Pnt3f(sceneMax[0],sceneMin[1],sceneMin[2]);
				bb[2]=Pnt3f(sceneMax[0],sceneMax[1],sceneMin[2]);
				bb[3]=Pnt3f(sceneMin[0],sceneMax[1],sceneMin[2]);
				bb[4]=Pnt3f(sceneMin[0],sceneMin[1],sceneMax[2]);
				bb[5]=Pnt3f(sceneMax[0],sceneMin[1],sceneMax[2]);
				bb[6]=Pnt3f(sceneMax[0],sceneMax[1],sceneMax[2]);
				bb[7]=Pnt3f(sceneMin[0],sceneMax[1],sceneMax[2]);	
				
				angle = deg2rad(0);
				//Real32 angle2;
				Pnt3f maxAnglePnt1,maxAnglePnt2;

				for(UInt32 j = 0; j<8;j++)
				{
					Vec3f vector1(lightpos - center);
					Vec3f vector2(lightpos - bb[j]);
					Real32 newAngle = vector1.enclosedAngle(vector2);
					if(osgabs(rad2deg(newAngle)) > 180) newAngle = vector2.enclosedAngle(vector1);
					if(rad2deg(newAngle) > rad2deg(angle)) 
					{
						angle = newAngle;
						maxAnglePnt1 = bb[j];
					}
				}

				for(UInt32 j = 0; j<8;j++)
				{
					Vec3f vector1(lightpos - maxAnglePnt1);
					Vec3f vector2(lightpos - bb[j]);
					Real32 newAngle = vector1.enclosedAngle(vector2);
					if(osgabs(rad2deg(newAngle)) > 180) newAngle = vector2.enclosedAngle(vector1);
					if(rad2deg(newAngle) > rad2deg(angle)) 
					{
						//angle2 = newAngle;
						maxAnglePnt2 = bb[j];
					}
				}

				//printf("Maximal noetiger Winkel ist %f\n",rad2deg(angle2));

				if(rad2deg(2*angle) > 175) angle = deg2rad(175);

                PerspectiveCameraPtr::dcast(_lightCameras[i])->setFov(2*angle);
				//printf("Tatsaechlicher Winkel ist: %f\n",rad2deg(2*angle));
            }
            
             _lightCameras[i]->setBeacon(_lightCamBeacons[i]);
        }
        endEditCP(_lightCameras[i]);
    }
}

void ShadowViewport::initializeLights(RenderActionBase *action)
{
    clearLights(_oldLights.size());

    FDEBUG(("Initialising lights.\n"));

    _oldLights = _lights;

    //Setting up Light-Cameras, ShadowMaps and TextureChunks
    for(UInt32 i = 0; i < _lights.size(); ++i)
    {
        // Remembering initial state of Lights
        _lightStates.push_back(_lights[i]->getOn());
        //Fill Transformation-List, so it can be used later on
        _lightCamTrans.push_back(NullFC);
        //Creation of Lightcam-Beacon
        _lightCamBeacons.push_back(makeCoredNode<Transform>(&_lightCamTrans[i]));
        
        //Giving new Camera Rotation and Position of the light it belongs to
        beginEditCP(_lightCamTrans[i]);
        {
            //Is the Lightsource a Spotlight?
            if(_lights[i]->getType() == SpotLight::getClassType())
            {
                //Creation of new Perspective-LightCam
                _lightCameras.push_back(PerspectiveCamera::create());
            }
            else if(_lights[i]->getType() == DirectionalLight::getClassType())
            {
                _lightCameras.push_back(MatrixCamera::create());
            }
			else // Preparation for PointLight -- In this version just a hack
            {
                _lightCameras.push_back(PerspectiveCamera::create());    
            }
        }
        endEditCP(_lightCamTrans[i]);

        addRefCP(_lightCameras[i]);

        if(getRoot() == NullFC)
        {
               SFATAL << "RootNode not found!" << endLog;
        }
        else
        {
           getRoot()->addChild(_lightCamBeacons[i]);
        }

        //----------Shadowtexture-Images and Texture-Chunks-----------

        _shadowImages.push_back(Image::create());

        // creates a image without allocating main memory.
        beginEditCP(_shadowImages[i]);
            _shadowImages[i]->set(Image::OSG_L_PF,getMapSize(), getMapSize(),
                                  1, 1, 1, 0, NULL,
                                  Image::OSG_UINT8_IMAGEDATA, false);
        endEditCP(_shadowImages[i]);

        _texChunks.push_back(TextureChunk::create());
        addRefCP(_texChunks[i]);

        // Preparation of Texture be a Depth-Texture
        beginEditCP(_texChunks[i]);
        {
            _texChunks[i]->setImage(_shadowImages[i]);
            _texChunks[i]->setInternalFormat(GL_DEPTH_COMPONENT_ARB);
            _texChunks[i]->setExternalFormat(GL_DEPTH_COMPONENT_ARB);
            _texChunks[i]->setMinFilter(GL_LINEAR);
            _texChunks[i]->setMagFilter(GL_LINEAR);
            _texChunks[i]->setWrapS(GL_CLAMP_TO_BORDER_ARB);
            _texChunks[i]->setWrapT(GL_CLAMP_TO_BORDER_ARB);
            _texChunks[i]->setEnvMode(GL_MODULATE);
            _texChunks[i]->setTarget(GL_TEXTURE_2D);
        }
        endEditCP(_texChunks[i]);

        // Just a Hack until TextureChunk is ready
        // to take the following arguments
        /*_texChunks[i]->activate(action, 3);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE_ARB,
                        GL_COMPARE_R_TO_TEXTURE_ARB);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC_ARB,GL_LEQUAL);
        glTexParameteri(GL_TEXTURE_2D,GL_DEPTH_TEXTURE_MODE_ARB,GL_LUMINANCE);
        _texChunks[i]->deactivate(action, 3);*/
    }

    updateLights();
}

void ShadowViewport::clearLights(UInt32 size)
{
    if(size > 0)
    {
        FDEBUG(("Clearing Lightcamera-Garbage!\n"));

        for(UInt32 i=0; i < size; ++i)
        {
            getRoot()->subChild(_lightCamBeacons[i]);
            subRefCP(_lightCameras[i]);
            subRefCP(_texChunks[i]);
        }

        _lightCameras.clear();
        _lightCamTrans.clear();
        _lightCamBeacons.clear();
        _lightStates.clear();
        _texChunks.clear();
        _shadowImages.clear();
    }
}

/*------------------------------------------------------------------------*/
/*                              cvs id's                                  */

#ifdef OSG_SGI_CC
#pragma set woff 1174
#endif

#ifdef OSG_LINUX_ICC
#pragma warning( disable : 177 )
#endif

namespace
{
    static Char8 cvsid_cpp       [] = "@(#)$Id: OSGShadowViewport.cpp,v 1.4 2006/04/18 12:41:12 yjung Exp $";
    static Char8 cvsid_hpp       [] = OSGSHADOWVIEWPORTBASE_HEADER_CVSID;
    static Char8 cvsid_inl       [] = OSGSHADOWVIEWPORTBASE_INLINE_CVSID;

    static Char8 cvsid_fields_hpp[] = OSGSHADOWVIEWPORTFIELDS_HEADER_CVSID;
}

#ifdef __sgi
#pragma reset woff 1174
#endif
