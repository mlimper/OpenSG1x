// occlision culling.

// Three algorithms are implemented "stop an wait", "multi frame"
// and "hierarchical multi frame"

// --- stop and wait ---
// Renders the scene in front to back order. For each object (except for the
// front most object) a bounding box is drawn with an occlusion query.
// The result is fetched immediately afterwards and if the box was visible
// the corresponding object is drawn.
// This is quite slow because of the front to back sorted rendering and
// the occlusions queries are stalling the graphics pipeline.
//
// --- multi frame ---
// Renders the whole scene first (with state sorting) and keeps
// the depth buffer. For each object a bounding box is drawn
// with an occlusion query. The results are fetched in the next frame,
// if the box was visible the corresponding object is drawn.
// This is really fast but can lead to render errors on fast camera movements.
//
// --- hierarchical multi frame ---
// Similar to multi frame but it tries to reduce the number of
// occlusion tests by doing hierarchical occlusion tests.

#include <fstream>

// GLUT is used for window handling
#include <OpenSG/OSGGLUT.h>

// General OpenSG configuration, needed everywhere
#include <OpenSG/OSGConfig.h>

// The GLUT-OpenSG connection class
#include <OpenSG/OSGGLUTWindow.h>

// A little helper to simplify scene management and interaction
#include <OpenSG/OSGSimpleSceneManager.h>

// Methods to create simple geometry: boxes, spheres, tori etc.
#include <OpenSG/OSGSimpleGeometry.h>

#include <OpenSG/OSGGradientBackground.h>

#include <OpenSG/OSGImageFileHandler.h>
#include <OpenSG/OSGPathHandler.h>

#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGGraphOpSeq.h>
#include <OpenSG/OSGVerifyGeoGraphOp.h>
#include <OpenSG/OSGStripeGraphOp.h>
#include <OpenSG/OSGRenderAction.h>

// Activate the OpenSG namespace
OSG_USING_NAMESPACE

SimpleSceneManager *mgr;

NodePtr scene = NullFC;

// Standard GLUT callback functions
void display( void )
{
    mgr->redraw();
}

void reshape( int w, int h )
{
    mgr->resize( w, h );
    glutPostRedisplay();
}

void
motion(int x, int y)
{
    mgr->mouseMove( x, y );
    glutPostRedisplay();
}

void
mouse(int button, int state, int x, int y)
{
    if ( state )
        mgr->mouseButtonRelease( button, x, y );
    else
        mgr->mouseButtonPress( button, x, y );
    glutPostRedisplay();
}

void
key(unsigned char key, int , int )
{
    switch(key)
    {
    case 27:    exit(1);
    case 'a':   mgr->setHighlight(scene);
                break;
    case 's':   mgr->setHighlight(NullFC);
                break;
    case 'l':   mgr->useOpenSGLogo();
                break;
    case 'f':   mgr->setNavigationMode(Navigator::FLY);
                break;
    case 't':   mgr->setNavigationMode(Navigator::TRACKBALL);
                break;
    case 'q':   mgr->setStatistics(true);
                break;
    case 'w':   mgr->setStatistics(false);
                break;
    case 'o':   SceneFileHandler::the().write(scene, "out.osb");
                break;
    case 'c':
    {
        RenderAction *ract = (RenderAction *) mgr->getAction();
        ract->setOcclusionCulling(!ract->getOcclusionCulling());
        printf("Occlusion culling %s.\n", ract->getOcclusionCulling() ? "enabled" : "disabled");
    }
    break;
    case '1':
    {
        RenderAction *ract = (RenderAction *) mgr->getAction();
        ract->setOcclusionCullingMode(RenderAction::OcclusionStopAndWait);
        printf("Occlusion culling mode stop and wait.\n");
    }
    break;
    case '2':
    {
        RenderAction *ract = (RenderAction *) mgr->getAction();
        ract->setOcclusionCullingMode(RenderAction::OcclusionMultiFrame);
        printf("Occlusion culling mode multi frame.\n");
        mgr->redraw();
    }
    break;
    case '3':
    {
        RenderAction *ract = (RenderAction *) mgr->getAction();
        ract->setOcclusionCullingMode(RenderAction::OcclusionHierarchicalMultiFrame);
        printf("Occlusion culling mode hierarchical multi frame.\n");
        mgr->redraw();
    }
    break;
    }
    glutPostRedisplay();
}

// Initialize GLUT & OpenSG and set up the scene
int main (int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

    // GLUT init
    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    int winid = glutCreateWindow("OpenSG");
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(key);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    const char *fileName = (argc > 1) ? argv[1] : NULL;

    if(fileName != NULL)
        scene = SceneFileHandler::the().read(fileName, NULL);

    if(scene == NullFC)
    {
        printf("No filename given creating a default scene.\n");
        // ok we create some spheres and one big torus around them
        // so we can see the occlusion culling.
        
        scene = makeCoredNode<Group>();

        NodePtr spheres = makeCoredNode<Group>();

        // create spheres
        for(Real32 y=-0.5f;y<0.5f;y+=0.1f)
        {
            for(Real32 x=-0.5f;x<0.5f;x+=0.1f)
            {
                Matrix m;
                m.setTranslate(x, y, 0.0f);
                TransformPtr sphere_trans;
                NodePtr sphere_trans_node = makeCoredNode<Transform>(&sphere_trans);
                beginEditCP(sphere_trans);
                    sphere_trans->setMatrix(m);
                endEditCP(sphere_trans);
                NodePtr sphere = makeSphere(3.0f, 0.1f);
                beginEditCP(sphere_trans_node);
                    sphere_trans_node->addChild(sphere);
                endEditCP(sphere_trans_node);
        
                beginEditCP(spheres);
                    spheres->addChild(sphere_trans_node);
                endEditCP(spheres);
            }
        }

        // create torus.
        NodePtr torus = makeTorus(0.5f, 2.0f, 32, 32);

        beginEditCP(scene);
            scene->addChild(spheres);
            scene->addChild(torus);
        endEditCP(scene);
    }

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    mgr->setWindow( gwin );
    mgr->setRoot( scene );

    mgr->showAll();

    // create a gradient background.
    GradientBackgroundPtr gback = GradientBackground::create();
    beginEditCP(gback, GradientBackground::LineFieldMask);
        gback->clearLines();
        gback->addLine(Color3f(0.7, 0.7, 0.8), 0);
        gback->addLine(Color3f(0.0, 0.1, 0.3), 1);
    endEditCP(gback, GradientBackground::LineFieldMask);

    WindowPtr win = mgr->getWindow();
    beginEditCP(win);
        for(int i=0;i<win->getPort().size();++i)
        {
            ViewportPtr vp = win->getPort()[i];
            beginEditCP(vp);
                vp->setBackground(gback);
            endEditCP(vp);
        }
    endEditCP(win);

    // enable occlusion culling.
    RenderAction *ract = (RenderAction *) mgr->getAction();
    ract->setOcclusionCulling(true);
    ract->setOcclusionCullingMode(RenderAction::OcclusionHierarchicalMultiFrame);
    printf("Occlusion culling enabled in mode hierarchical multi frame.\n");
    printf("Press 'c' to toggle occlusion culling.\n");
    printf("Press '1' for occlusion culling mode stop and wait.\n");
    printf("Press '2' for occlusion culling mode multi frame.\n");
    printf("Press '3' for occlusion culling mode hierarchical multi frame.\n");
    printf("Press 'q' and 'w' to toogle render statistics.\n");

    // GLUT main loop
    glutMainLoop();

    return 0;
}
