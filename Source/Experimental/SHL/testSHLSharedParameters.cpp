// OpenSG example: testSHL
//
// Demonstrates the use of the SHLChunk
// Ok this creates 1 CGChunk with different parameter sets for each geometry.

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

#include <OSGGradientBackground.h>
#include <OSGNode.h>
#include <OSGGroup.h>
#include <OSGTransform.h>
#include <OSGPointLight.h>
#include <OSGMaterialGroup.h>

#include <OSGImage.h>
#include <OSGChunkMaterial.h>
#include <OSGMaterialChunk.h>
#include <OSGTextureChunk.h>
#include <OSGSHLChunk.h>
#include <OSGSHLParameterChunk.h>

// vertex shader program for 
static std::string _vp_program =
"varying vec3 DiffuseColor;\n"
"varying vec3 SpecularColor;\n"
"\n"
"void main(void)\n"
"{\n"
"	vec3 LightColor = vec3(1.0);\n"
"	vec3 Specular	= vec3(1.0);\n"
"	vec3 Ambient	= vec3(0.2);\n"
"	float Kd		= 0.3;\n"
"\n"
"	vec3 LightPosition = gl_LightSource[0].position.xyz;\n"
"\n"
"	vec3 ecPosition	= vec3(gl_ModelViewMatrix * gl_Vertex);\n"
"\n"
"	vec3 tnorm		= normalize(gl_NormalMatrix * gl_Normal);\n"
"\n"
"	vec3 lightVec	= normalize(LightPosition - ecPosition);\n"
"\n"
"	vec3 hvec		= normalize(lightVec - ecPosition);\n"
"\n"
"	float spec		= clamp(dot(hvec, tnorm), 0.0, 1.0);\n"
"	spec			= pow(spec, 16.0);\n"
"\n"
"	DiffuseColor	= LightColor * vec3(Kd * dot(lightVec, tnorm));\n"
"	DiffuseColor	= clamp(Ambient + DiffuseColor, 0.0, 1.0);\n"
"	SpecularColor	= clamp((LightColor * Specular * spec), 0.0 ,1.0);\n"
"\n"
"	gl_TexCoord[0]	= gl_MultiTexCoord0;\n"
"	gl_Position		= ftransform();\n"
"}\n";


// fragment shader program for
static std::string _fp_program =
"varying vec3 DiffuseColor;\n"
"varying vec3 SpecularColor;\n"
"\n"
"uniform vec2 Scale;\n"
"uniform vec2 Threshold;\n"
"uniform vec3 SurfaceColor;\n"
"\n"
"void main (void)\n"
"{\n"
"	float ss = fract(gl_TexCoord[0].s * Scale.s);\n"
"	float tt = fract(gl_TexCoord[0].t * Scale.t);\n"
"\n"
"	if ((ss > Threshold.s) && (tt > Threshold.t))\n"
"		discard;\n"
"\n"
"	vec3 finalColor = SurfaceColor * DiffuseColor + SpecularColor;\n"
"	gl_FragColor = vec4(finalColor, 1.0);\n"
"}\n";


// Activate the OpenSG namespace
OSG_USING_NAMESPACE


// ------------------- global vars ----------------------
//
// The SimpleSceneManager to manage simple applications
static SimpleSceneManager *_mgr;
// The scene
static NodePtr _scene;

static Int32 _animation = 1;
static SHLParameterChunkPtr _shlparameter = NullFC;


// forward declaration so we can have the interesting stuff upfront
int setupGLUT( int *argc, char *argv[] );

// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin = GLUTWindow::create();
    beginEditCP(gwin);
        gwin->setId(winid);
        gwin->setSize( 800, 800 );
        gwin->init();
    endEditCP(gwin);

    // create root node
    _scene = makeCoredNode<Group>();

    GeometryPtr geo = makeBoxGeo(0.5, 0.5, 0.5, 1, 1, 1);

    // share the chunk
    SHLChunkPtr shl = SHLChunk::create();
    beginEditCP(shl);
        shl->setVertexProgram(_vp_program);
        shl->setFragmentProgram(_fp_program);
        // These parameters are the same for all geometries so we
        // keep them in here.
        shl->setUniformParameter("Scale", Vec2f(20.0f, 20.0f));
        shl->setUniformParameter("Threshold", Vec2f(0.7f, 0.7f));
    endEditCP(shl);

    Int32 size = 4;
    
    // start color
    Vec3f sc(0.0, 0.0, 0.0);
    
    // end color
    Vec3f ec(1.0, 1.0, 1.0);

    Real32 sr = (ec[0] - sc[0]) / Real32((size*2));
    Real32 sg = (ec[1] - sc[1]) / Real32((size*2));
    Real32 sb = (ec[2] - sc[2]) / Real32((size*2));
    
    Vec3f color(sc);

    Int32 x = - size;
    Int32 y = - size;
    Int32 z = - size;

    UInt32 iterations = size*2 * size*2 * size*2;

    printf("Creating %u cubes ...\n", iterations);
    for(UInt32 i=0;i<iterations;++i)
    {
        ChunkMaterialPtr cmat = ChunkMaterial::create();

        // ok use one SHLChunk and n SHLParameterChunks
        // Assing a different "SurfaceColor" parameter to each geometry.
        SHLParameterChunkPtr shlparameter = SHLParameterChunk::create();
        beginEditCP(shlparameter);
            shlparameter->setSHLChunk(shl);
            shlparameter->setUniformParameter("SurfaceColor", color);
        endEditCP(shlparameter);
        _shlparameter = shlparameter;

        beginEditCP(cmat);
            cmat->addChunk(shl);
            cmat->addChunk(shlparameter);
        endEditCP(cmat);
    
        TransformPtr trans;
        NodePtr trans_node = makeCoredNode<Transform>(&trans);
        beginEditCP(trans);
            trans->getMatrix().setTranslate(Real32(x), Real32(y), Real32(z));
        endEditCP(trans);

        MaterialGroupPtr mg;
        NodePtr mg_node = makeCoredNode<MaterialGroup>(&mg);
        beginEditCP(mg, MaterialGroup::MaterialFieldMask);
            mg->setMaterial(cmat);
        endEditCP(mg, MaterialGroup::MaterialFieldMask);

        NodePtr geonode = Node::create();
        beginEditCP(geonode, Node::CoreFieldMask);
            geonode->setCore(geo);
        endEditCP(geonode, Node::CoreFieldMask);
        
        beginEditCP(mg_node);
            mg_node->addChild(geonode);
        endEditCP(mg_node);

        beginEditCP(trans_node);
            trans_node->addChild(mg_node);
        endEditCP(trans_node);
    
        // add to scene
        beginEditCP(_scene);
            _scene->addChild(trans_node);
        endEditCP(_scene);
        
        // ----
        ++x;
        color[0] += sr;

        if(x == size)
        {
            x = - size;
            ++y;
            color[0] = sc[0];
            color[1] += sg;
            if(y == size)
            {
                y = - size;
                ++z;
                color[1] = sc[1];
                color[2] += sb;
            }
        }
    }


    // create the SimpleSceneManager helper
    _mgr = new SimpleSceneManager;

    // tell the manager what to manage
    _mgr->setWindow(gwin );
    _mgr->setRoot(_scene);

    // show the whole scene
    _mgr->showAll();

    // create a gradient background.
    GradientBackgroundPtr gback = GradientBackground::create();
    beginEditCP(gback, GradientBackground::LineFieldMask);
        gback->clearLines();
        gback->addLine(Color3f(0.7, 0.7, 0.8), 0);
        gback->addLine(Color3f(0.0, 0.1, 0.3), 1);
    endEditCP(gback, GradientBackground::LineFieldMask);

    WindowPtr win = _mgr->getWindow();
    beginEditCP(win);
        for(int i=0;i<win->getPort().size();++i)
        {
            ViewportPtr vp = win->getPort()[i];
            beginEditCP(vp);
                vp->setBackground(gback);
            endEditCP(vp);
        }
    endEditCP(win);
        
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
    Real64 t = OSG::getSystemTime();
    // render scene
    _mgr->redraw();
    t = OSG::getSystemTime() - t;
    if(t > 0.0)
        printf("fps: %f\r", 1.0f / Real32(t));
    else
        printf("fps: very fast ...\r");
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
    static Real32 season = 0.0f; 
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
        case 'a':
            _animation = 1 - _animation;
        break;
        case 'c':
            if(_shlparameter != NullFC)
            {
                beginEditCP(_shlparameter);
                    _shlparameter->setUniformParameter("SurfaceColor", Vec3f(1.0f, 1.0f, 1.0f));
                endEditCP(_shlparameter);
            }
        break;
    }

    glutPostRedisplay();
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    int winid = glutCreateWindow("OpenSG SHL Shader");

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    glutIdleFunc(display);

    return winid;
}


