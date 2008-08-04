// OpenSG example: testSHL
//
// Demonstrates the use of the SHLChunk
// Implements a simple bumpmapping via vertex and fragment shader.

// Headers
#include <OSGGLUT.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
#include <OSGGLUT.h>
#include <OSGGLUTWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGAction.h>
#include <OSGSFSysTypes.h>
#include <OSGSceneFileHandler.h>
#include <OSGBaseFunctions.h>

#include <OSGNode.h>
#include <OSGGroup.h>
#include <OSGTransform.h>
#include <OSGPointLight.h>

#include <OSGImage.h>
#include <OSGChunkMaterial.h>
#include <OSGMaterialChunk.h>
#include <OSGTextureChunk.h>
#include <OSGSHLChunk.h>

// vertex shader program for bump mapping in surface local coordinates
static std::string _vp_program =
"uniform bool OSGLight0Active;\n"
"uniform bool OSGLight1Active;\n"
"uniform bool OSGLight2Active;\n"
"vec4 Ambient;\n"
"vec4 Diffuse;\n"
"vec4 Specular;\n"
"\n"
"\n"
"    void pointLight(in int i, in vec3 normal, in vec3 eye, in vec3 ecPosition3)\n"
"    {\n"
"        float nDotVP;         // normal . light direction\n"
"        float nDotHV;         // normal . light half vector\n"
"        float pf;                   // power factor\n"
"        float attenuation;    // computed attenuation factor\n"
"        float d;                    // distance from surface to light source\n"
"        vec3  VP;               // direction from surface to light position\n"
"        vec3  halfVector;    // direction of maximum highlights\n"
"\n"
"        // Compute vector from surface to light position\n"
"        VP = vec3 (gl_LightSource[i].position) - ecPosition3;\n"
"\n"
"        // Compute distance between surface and light position\n"
"        d = length(VP);\n"
"\n"
"        // Normalize the vector from surface to light position\n"
"        VP = normalize(VP);\n"
"\n"
"        // Compute attenuation\n"
"        attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +\n"
"                      gl_LightSource[i].linearAttenuation * d +\n"
"                      gl_LightSource[i].quadraticAttenuation * d * d);\n"
"        halfVector = normalize(VP + eye);\n"
"        nDotVP = max(0.0, dot(normal, VP));\n"
"        nDotHV = max(0.0, dot(normal, halfVector));\n"
"\n"
"        if (nDotVP == 0.0)\n"
"            pf = 0.0;\n"
"        else\n"
"            pf = pow(nDotHV, gl_FrontMaterial.shininess);\n"
"\n"
"        Ambient  += gl_LightSource[i].ambient * attenuation;\n"
"        Diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;\n"
"        Specular += gl_LightSource[i].specular * pf * attenuation;\n"
"    }\n"
"\n"
"    vec3 fnormal(void)\n"
"    {\n"
"        //Compute the normal\n"
"        vec3 normal = gl_NormalMatrix * gl_Normal;\n"
"        normal = normalize(normal);\n"
"        return normal;\n"
"    }\n"
"\n"
"    void flight(in vec3 normal, in vec4 ecPosition, float alphaFade)\n"
"    {\n"
"        vec4 color;\n"
"        vec3 ecPosition3;\n"
"        vec3 eye;\n"
"\n"
"        ecPosition3 = (vec3 (ecPosition)) / ecPosition.w;\n"
"        eye = vec3 (0.0, 0.0, 1.0);\n"
"\n"
"        // Clear the light intensity accumulators\n"
"        Ambient  = vec4 (0.0);\n"
"        Diffuse  = vec4 (0.0);\n"
"        Specular = vec4 (0.0);\n"
"\n"
"       if(OSGLight0Active)\n"
"           pointLight(0, normal, eye, ecPosition3);\n"
"\n"
"       if(OSGLight1Active)\n"
"           pointLight(1, normal, eye, ecPosition3);\n"
"\n"
"       if(OSGLight2Active)\n"
"           pointLight(2, normal, eye, ecPosition3);\n"
"\n"
"        color = gl_FrontLightModelProduct.sceneColor +\n"
"                    Ambient  * gl_FrontMaterial.ambient +\n"
"                    Diffuse  * gl_FrontMaterial.diffuse;\n"
"        color += Specular * gl_FrontMaterial.specular;\n"
"        color = clamp( color, 0.0, 1.0 );\n"
"        gl_FrontColor = color;\n"
"        gl_FrontColor.a *= alphaFade;\n"
"    }\n"
"\n"
"    void main(void)\n"
"    {\n"
"        vec3  transformedNormal;\n"
"        float alphaFade = 1.0;\n"
"\n"
"        // Eye-coordinate position of vertex, needed in various calculations\n"
"        vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;\n"
"\n"
"        // Do fixed functionality vertex transform\n"
"        gl_Position = ftransform();\n"
"        transformedNormal = fnormal();\n"
"        flight(transformedNormal, ecPosition, alphaFade);\n"
"    }\n";

// fragment shader program for bump mapping in surface local coordinates
static std::string _fp_program =
"void main (void)\n"
"{\n"
"    vec4 color;\n"
"    color = gl_Color;\n"
"    gl_FragColor = color;\n"
"}\n";

// Activate the OpenSG namespace
OSG_USING_NAMESPACE


// ------------------- global vars ----------------------
//
// The SimpleSceneManager to manage simple applications
static SimpleSceneManager *_mgr;
// The scene
static NodePtr _scene;
static PointLightPtr _point1_core;
static PointLightPtr _point2_core;
static PointLightPtr _point3_core;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Shows how to add your own parameter callbacks.

typedef void   (OSG_APIENTRY * OSGGLUNIFORMMATRIXFVARBPROC) (GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

static void updateSpecialParameter(SHLChunk::OSGGLGETUNIFORMLOCATIONARBPROC getUniformLocation,
                                   DrawActionBase *action, GLuint program)
{
    if(action->getCamera() == NULL || action->getViewport() == NULL)
    {
        FWARNING(("updateSpecialParameter : Can't update OSGSpecialParameter"
                  "parameter, camera or viewport is NULL!\n"));
        return;
    }

    // uploads the camera orientation.
    Matrix m;
    action->getCamera()->getViewing(m,
                                action->getViewport()->getPixelWidth(),
                                action->getViewport()->getPixelHeight());
    m.invert();
    m[3].setValues(0, 0, 0, 1);

    //std::cout << "uploading matrix " << m << std::endl;

    // get "glUniformMatrix4fvARB" function pointer
    OSGGLUNIFORMMATRIXFVARBPROC uniformMatrix4fv = (OSGGLUNIFORMMATRIXFVARBPROC)
        action->getWindow()->getFunction(SHLChunk::getFuncUniformMatrix4fv());
    GLint location = getUniformLocation(program, "OSGSpecialParameter");
    if(location != -1)
        uniformMatrix4fv(location, 1, GL_FALSE, m.getValues());
}

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    printf("Press key '1', '2', or '3' to toggle the light sources.\n");
    // OSG init
    osgInit(argc,argv);

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->setSize( 800, 800 );
    gwin->init();

    // Create the shader material

    ChunkMaterialPtr cmat = ChunkMaterial::create();

    MaterialChunkPtr matc = MaterialChunk::create();
    beginEditCP(matc);
        matc->setAmbient(Color4f(0.1, 0.1, 0.1, 1.0));
        matc->setDiffuse(Color4f(0.3, 0.3, 0.3, 1.0));
        matc->setSpecular(Color4f(0.8, 0.8, 0.8, 1.0));
        matc->setShininess(100);
        matc->setLit(true);
    endEditCP(matc);

    SHLChunkPtr shl = SHLChunk::create();
    beginEditCP(shl);
        shl->setVertexProgram(_vp_program);
        shl->setFragmentProgram(_fp_program);
        shl->setUniformParameter("OSGLight0Active", 0);
        shl->setUniformParameter("OSGLight1Active", 0);
        shl->setUniformParameter("OSGLight2Active", 0);
        // The OSGSpecialParameter is not used in the shader just shows
        // how to add your own parameter callbacks!
        shl->addParameterCallback("OSGSpecialParameter", updateSpecialParameter);
    endEditCP(shl);

    beginEditCP(cmat);
        cmat->addChunk(matc);
        cmat->addChunk(shl);
    endEditCP(cmat);

    // create root node
    _scene = Node::create();

    // create two light sources.
    TransformPtr point1_trans;
    NodePtr point1 = makeCoredNode<PointLight>(&_point1_core);
    NodePtr point1_beacon = makeCoredNode<Transform>(&point1_trans);
    beginEditCP(point1_trans);
        point1_trans->editMatrix().setTranslate(-10.0, 5.0, 5.0);
    endEditCP(point1_trans);

    beginEditCP(_point1_core);
        _point1_core->setAmbient(0.0f, 0.0f, 0.0f , 1.0f);
        _point1_core->setDiffuse(1.0f, 0.0f, 0.0f, 1.0f);
        _point1_core->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
        _point1_core->setBeacon(point1_beacon);
        _point1_core->setOn(true);
    endEditCP(_point1_core);

    TransformPtr point2_trans;
    NodePtr point2 = makeCoredNode<PointLight>(&_point2_core);
    NodePtr point2_beacon = makeCoredNode<Transform>(&point2_trans);
    beginEditCP(point2_trans);
        point2_trans->editMatrix().setTranslate(10.0, 5.0, 5.0);
    endEditCP(point2_trans);

    beginEditCP(_point2_core);
        _point2_core->setAmbient(0.0f, 0.0f, 0.0f, 1.0f);
        _point2_core->setDiffuse(0.0f, 1.0f, 0.0f, 1.0f);
        _point2_core->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
        _point2_core->setBeacon(point2_beacon);
        _point2_core->setOn(true);
    endEditCP(_point2_core);

    beginEditCP(point1);
        point1->addChild(point2);
    endEditCP(point1);
    
    TransformPtr point3_trans;
    NodePtr point3 = makeCoredNode<PointLight>(&_point3_core);
    NodePtr point3_beacon = makeCoredNode<Transform>(&point3_trans);
    beginEditCP(point3_trans);
        point3_trans->editMatrix().setTranslate(0.0, -12.0, 5.0);
    endEditCP(point3_trans);

    beginEditCP(_point3_core);
        _point3_core->setAmbient(0.0f, 0.0f, 0.0f, 1.0f);
        _point3_core->setDiffuse(0.5f, 0.0f, 1.0f, 1.0f);
        _point3_core->setSpecular(1.0f, 1.0f, 1.0f, 1.0f);
        _point3_core->setBeacon(point3_beacon);
        _point3_core->setOn(true);
    endEditCP(_point3_core);

    beginEditCP(point2);
        point2->addChild(point3);
    endEditCP(point2);

    // create a sphere.
    GeometryPtr geo = makeLatLongSphereGeo (100, 100, 1.0);
    beginEditCP(geo, Geometry::MaterialFieldMask);
        geo->setMaterial(cmat);
    endEditCP(geo, Geometry::MaterialFieldMask);

    NodePtr sphere = OSG::makeNodeFor(geo);

    beginEditCP(point3);
        point3->addChild(sphere);
    endEditCP(point3);

    beginEditCP(_scene);
        _scene->setCore(Group::create());
        _scene->addChild(point1);
    endEditCP(_scene);

    // create the SimpleSceneManager helper
    _mgr = new SimpleSceneManager;

    // tell the manager what to manage
    _mgr->setWindow(gwin );
    _mgr->setRoot(_scene);

    _mgr->turnHeadlightOff();

    // show the whole scene
    _mgr->showAll();

    // enable local lights.
    RenderAction *ract = dynamic_cast<RenderAction *>(_mgr->getAction());
    ract->setLocalLights(true);

    // GLUT main loop
    glutMainLoop();

    return 0;
}

//
// GLUT callback functions
//

// redraw the window
void display(void)
{
  // render scene
  _mgr->redraw();
}

// react to size changes
void reshape(int w, int h)
{
    _mgr->resize(w, h);
    glutPostRedisplay();
}

// react to mouse button presses
void mouse(int button, int state, int x, int y)
{
    if (state)
        _mgr->mouseButtonRelease(button, x, y);
    else
        _mgr->mouseButtonPress(button, x, y);

    glutPostRedisplay();
}

// react to mouse motions with pressed buttons
void motion(int x, int y)
{
    _mgr->mouseMove(x, y);
    glutPostRedisplay();
}

// react to keys
void keyboard(unsigned char k, int x, int y)
{
    switch(k)
    {
        case 27:
        case 'q':
            exit(1);
        break;
        case 'w':
            SceneFileHandler::the().write(_scene, "scene.osb.gz", true);
            printf("wrote scene.osb.gz\n");
        break;
        case '1':
        {
            beginEditCP(_point1_core);
                if(_point1_core->getOn() == false)
                    _point1_core->setOn(true);
                else
                    _point1_core->setOn(false);
            endEditCP(_point1_core);
            break;
        }

        case '2':
        {
            beginEditCP(_point2_core);
                if(_point2_core->getOn() == false)
                    _point2_core->setOn(true);
                else
                    _point2_core->setOn(false);
            endEditCP(_point2_core);
            break;
        }

        case '3':
        {
            beginEditCP(_point3_core);
                if(_point3_core->getOn() == false)
                    _point3_core->setOn(true);
                else
                    _point3_core->setOn(false);
            endEditCP(_point3_core);
            break;
        }
    }

    glutPostRedisplay();
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    int winid = glutCreateWindow("OpenSG CG Shader");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    return winid;
}


