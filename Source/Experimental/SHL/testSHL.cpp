// OpenSG example: testSHL
//
// Demonstrates the use of the SHLChunk
// Implements a simple bumpmapping via vertex and fragment shader.

// Headers
#include <GL/glut.h>
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
"varying vec3 lightDir;    // interpolated surface local coordinate light direction\n"
"varying vec3 viewDir;     // interpolated surface local coordinate view direction\n"

"void main(void)\n"
"{\n"
"    vec3 b;\n"
"    vec3 n;\n"
"    vec3 t;\n"
"    vec3 pos;\n"
"    vec3 lightVec;\n"
"    vec3 r;\n"
"    vec3 v;\n"

"    // Do standard vertex stuff\n"

"    gl_Position  = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"

"    // Compute the binormal\n"

"    n = normalize(gl_NormalMatrix * gl_Normal);\n"
"    t = normalize(gl_NormalMatrix * vec3 (gl_Color));\n"
"    b = cross(n, t);\n"

"    // Transform light position into surface local coordinates\n"

"    vec3 LightPosition = gl_LightSource[0].position.xyz;\n"

"    v.x = dot(LightPosition, t);\n"
"    v.y = dot(LightPosition, b);\n"
"    v.z = dot(LightPosition, n);\n"

"    lightDir = normalize(v);\n"

"    pos      = vec3 (gl_ModelViewMatrix * gl_Vertex);\n"

"    v.x = dot(pos, t);\n"
"    v.y = dot(pos, b);\n"
"    v.z = dot(pos, n);\n"

"    viewDir = normalize(v);\n"
"\n"
"}\n";

// fragment shader program for bump mapping in surface local coordinates
static std::string _fp_program =
"uniform sampler2D sampler2d; // value of sampler2d = 3\n"
"varying vec3 lightDir;       // interpolated surface local coordinate light direction\n"
"varying vec3 viewDir;        // interpolated surface local coordinate view direction\n"

"const float diffuseFactor  = 0.7;\n"
"const float specularFactor = 0.7;\n"
"vec3 basecolor = vec3 (0.8, 0.7, 0.3);\n"

"void main (void)\n"
"{\n"
"    vec3 norm;\n"
"    vec3 r;\n"
"    vec3 color;\n"
"    float intensity;\n"
"    float spec;\n"
"    float d;\n"
"    // Fetch normal from normal map\n"
"    norm = vec3(texture2D(sampler2d, vec2 (gl_TexCoord[0])));\n"
"    norm = (norm - 0.5) * 2.0;\n"
"    norm.y = -norm.y;\n"
"    intensity = max(dot(lightDir, norm), 0.0) * diffuseFactor;\n"
"    // Compute specular reflection component\n"
"    d = 2.0 * dot(lightDir, norm);\n"
"    r = d * norm;\n"
"    r = lightDir - r;\n"
"    spec = pow(max(dot(r, viewDir), 0.0) , 6.0) * specularFactor;\n"
"    intensity += min (spec, 1.0);\n"
"     // Compute final color value\n"
"    color = clamp(basecolor * intensity, 0.0, 1.0);\n"
"    // Write out final fragment color\n"
"    gl_FragColor = vec4 (color, 1.0);\n"
"\n"
"}\n";


// Activate the OpenSG namespace
OSG_USING_NAMESPACE


// ------------------- global vars ----------------------
//
// The SimpleSceneManager to manage simple applications
static SimpleSceneManager *_mgr;
// The scene
static NodePtr _scene;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    printf("Usage: testCGShader [normal map filename]\n");
    char *normal_map_img_name = "3dlabsbump.png";

    Color4f tmp;

    if( argc > 1 )
        normal_map_img_name = argv[1];

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

    // Read the image for the normal texture
    ImagePtr normal_map_img = Image::create();
    if(!normal_map_img->read(normal_map_img_name))
    {
        fprintf(stderr, "Couldn't read normalmap texture '%s'!\n", normal_map_img_name);
        return 1;
    }

    ChunkMaterialPtr cmat = ChunkMaterial::create();

    MaterialChunkPtr matc = MaterialChunk::create();
    beginEditCP(matc);
        matc->setAmbient(Color4f(0.1, 0.1, 0.1, 1.0));
        matc->setDiffuse(Color4f(0.3, 0.3, 0.3, 1.0));
        matc->setSpecular(Color4f(0.8, 0.8, 0.8, 1.0));
        matc->setShininess(100);
        matc->setLit(true);
    endEditCP(matc);

    // we use the glstate in the cg program so we force
    // to use the CG_PROFILE_ARBVP1 and CG_PROFILE_ARBFP1 extensions.

    SHLChunkPtr shl = SHLChunk::create();
    beginEditCP(shl);
        shl->setVertexProgram(_vp_program);
        shl->setFragmentProgram(_fp_program);
    endEditCP(shl);

    TextureChunkPtr tex_normal_map = TextureChunk::create();
    beginEditCP(tex_normal_map);
        tex_normal_map->setImage(normal_map_img);
        tex_normal_map->setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
        tex_normal_map->setMagFilter(GL_LINEAR);
        tex_normal_map->setWrapS(GL_REPEAT);
        tex_normal_map->setWrapT(GL_REPEAT);
        tex_normal_map->setEnvMode(GL_MODULATE);
    endEditCP(tex_normal_map);

    beginEditCP(cmat);
        cmat->addChunk(matc);
        cmat->addChunk(shl);
        cmat->addChunk(tex_normal_map);
    endEditCP(cmat);


    // create root node
    _scene = Node::create();

    // create torus
    GeometryPtr geo = makeTorusGeo(.8, 1.8, 128, 128);
    beginEditCP( geo, Geometry::MaterialFieldMask);
        geo->setMaterial(cmat);
    endEditCP(geo, Geometry::MaterialFieldMask);

    NodePtr torus = Node::create();
    beginEditCP(torus, Node::CoreFieldMask);
        torus->setCore(geo);
    endEditCP(torus, Node::CoreFieldMask);

    // add torus to scene
    GroupPtr group = Group::create();
    beginEditCP(_scene);
        _scene->setCore(group);
        _scene->addChild(torus);
    endEditCP(_scene);

    // create the SimpleSceneManager helper
    _mgr = new SimpleSceneManager;

    // tell the manager what to manage
    _mgr->setWindow(gwin );
    _mgr->setRoot(_scene);

    /*
    // create point headlight
    _mgr->turnHeadlightOff();
    NodePtr headlight = _mgr->getHighlight();
    PointLightPtr light    = PointLight::create();
    beginEditCP(light);
        light->setAmbient  (.3, .3, .3, 1);
        light->setDiffuse  ( 1,  1,  1, 1);
        light->setSpecular ( 1,  1,  1, 1);
        light->setBeacon   (_mgr->getCamera()->getBeacon());
    endEditCP(light);
    beginEditCP(_scene);
        _scene->setCore(light);
    endEditCP(_scene);
    */

    // show the whole scene
    _mgr->showAll();

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


