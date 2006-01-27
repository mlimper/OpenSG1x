/*!
 * \class OSGRenderOptions
 * \brief
 * \author Andreas Zieringer AMZ GmbH
 */

#include <stdlib.h>
#include <stdio.h>

#include <OSGConfig.h>
#include <OSGGLEXT.h>

#include "OSGRenderOptions.h"
#include "OSGRenderAction.h"

OSG_USING_NAMESPACE


/*!
 * \brief
 * \param
 * \return
 */
RenderOptions::RenderOptions(void) :
    Inherited(),
    _changed(0),
    _last_changed(0),
    _gl_version(0.0f),
    _polygon_mode(GL_FILL),
    _backface_culling(false),
    _two_sided_lighting(false),
    _spec_tex_lighting(false)
{
}

/*!
 * \brief
 * \param
 * \return
 */
RenderOptions::RenderOptions(const RenderOptions &source) :
    Inherited(source),
    _changed(source._changed),
    _last_changed(source._last_changed),
    _gl_version(source._gl_version),
    _polygon_mode(source._polygon_mode),
    _backface_culling(source._backface_culling),
    _two_sided_lighting(source._two_sided_lighting),
    _spec_tex_lighting(source._spec_tex_lighting)
{
}

/*!
 * \brief
 * \param
 * \return
 */
RenderOptions::~RenderOptions(void)
{
}

/*!
 * \brief
 * \param
 * \return
 */
void RenderOptions::initMethod (void)
{
}

/*!
 * \brief
 * \param
 * \return
 */
void RenderOptions::changed(BitVector whichField, UInt32 origin)
{
    _changed |= whichField;
    Inherited::changed(whichField, origin);
}

void RenderOptions::setWireframe( bool value )
{
    if(value)
        setPolygonMode(GL_LINE);
    else
        setPolygonMode(GL_FILL);
}

bool RenderOptions::getWireframe( void )
{
    return getPolygonMode() == GL_LINE;
}

BitVector RenderOptions::getChanged( void )
{
    return _changed;
}

BitVector RenderOptions::getLastChanged( void )
{
    return _last_changed;
}

void RenderOptions::activateOptions(RenderAction *action)
{
    if(_gl_version == 0.0f)
    {
        // detect OpenGL version.
        std::string vstr = (const char *) glGetString(GL_VERSION);
        // remove last .x
        vstr = vstr.substr(0, 3);
        _gl_version = atof(vstr.c_str());
    }

    if(_changed & PolygonModeFieldMask)
        _polygon_mode = getPolygonMode();

    if(_changed & BackfaceCullingFieldMask)
        _backface_culling = getBackfaceCulling();

    if(_changed & CorrectTwoSidedLightingFieldMask)
        action->setCorrectTwoSidedLighting(getCorrectTwoSidedLighting());

    if(_changed & TwoSidedLightingFieldMask)
        _two_sided_lighting = getTwoSidedLighting();
    
    if(_changed & SortTransFieldMask)
        action->setSortTrans(getSortTrans());

    if(_changed & ZWriteTransFieldMask)
        action->setZWriteTrans(getZWriteTrans());

    if(_changed & LocalLightsFieldMask)
        action->setLocalLights(getLocalLights());

    if(_changed & SpecTexLightingFieldMask)
        _spec_tex_lighting = getSpecTexLighting();

    if(_changed & OcclusionCullingFieldMask)
        action->setOcclusionCulling(getOcclusionCulling());

    if(_changed & SmallFeatureCullingFieldMask)
        action->setSmallFeatureCulling(getSmallFeatureCulling());

    if(_changed & SmallFeaturePixelsFieldMask)
        action->setSmallFeaturePixels(getSmallFeaturePixels());

    if(_changed & SmallFeatureThresholdFieldMask)
        action->setSmallFeatureThreshold(getSmallFeatureThreshold());

    // we update the gl stuff each frame.
    glPolygonMode(GL_FRONT_AND_BACK, _polygon_mode);

    if(_backface_culling)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } 

    if(_two_sided_lighting)
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    else
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    if(_gl_version >= 1.2f)
    {
        if(_spec_tex_lighting)
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
        else
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
    } 

    _last_changed = _changed;
    _changed = 0;
}
