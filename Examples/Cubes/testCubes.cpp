// OpenSG Example: Cubes
//

// GLUT is used for window handling
#include <OpenSG/OSGGLUT.h>

// General OpenSG configuration, needed everywhere
#include <OpenSG/OSGConfig.h>

// Methods to create simple geometry: boxes, spheres, tori etc.
#include <OpenSG/OSGSimpleGeometry.h>

// The GLUT-OpenSG connection class
#include <OpenSG/OSGGLUTWindow.h>

// A little helper to simplify scene management and interaction
#include <OpenSG/OSGSimpleSceneManager.h>

#include <OpenSG/OSGSimpleGeometry.h> // for the DefaultMaterial
#include <OpenSG/OSGSceneFileHandler.h>

#include "OSGCubes.h"

// little helper to create a random number between a and b
#define random( a, b ) \
( ( rand() / float(RAND_MAX) ) * ( (b) - (a) ) + (a) )

// Activate the OpenSG namespace
OSG_USING_NAMESPACE

// The SimpleSceneManager to manage simple applications
SimpleSceneManager *mgr;

NodePtr scene;
CubesPtr cubes;

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
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();
   
    // Create the Cubes node

    scene = Node::create();
    cubes = Cubes::create();

    beginEditCP(cubes);
    cubes->setMaterial( getDefaultMaterial() );  
    
    // create a bunch of random cubes
    
    for ( int i=0; i < 10; i++ )
    {
        cubes->getMFPosition()->push_back( Pnt3f( random(-2, 2), 
                                                 random(-2, 2), 
                                                 random(-2, 2)) );
        cubes->getMFLength()->push_back( random(0.5, 2) );
        cubes->getMFColor()->push_back( Color3f( random(0.5, 1), 
                                                random(0.5, 1), 
                                                random(0.5, 1)) );
    }
    
    endEditCP(cubes);

    beginEditCP(scene);
    scene->setCore( cubes );
    endEditCP(scene);


    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();
    
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
    mgr->redraw();
}

// react to size changes
void reshape(int w, int h)
{
    mgr->resize(w, h);
    glutPostRedisplay();
}

// react to mouse button presses
void mouse(int button, int state, int x, int y)
{
    if (state)
        mgr->mouseButtonRelease(button, x, y);
    else
        mgr->mouseButtonPress(button, x, y);
        
    glutPostRedisplay();
}

// react to mouse motions with pressed buttons
void motion(int x, int y)
{
    mgr->mouseMove(x, y);
    glutPostRedisplay();
}

// react to keys
void keyboard(unsigned char k, int x, int y)
{
    switch(k)
    {
    case 27:    exit(1);
    case ' ':   // Add a new cube
            beginEditCP(cubes, Cubes::PositionFieldMask | 
                               Cubes::LengthFieldMask | 
                               Cubes::ColorFieldMask);

            cubes->getMFPosition()->push_back( Pnt3f( random(-4, 4), 
                                                      random(-4, 4), 
                                                      random(-4, 4)) );
            cubes->getMFLength()->push_back( random(0.5, 2) );
            
            cubes->getMFColor()->push_back( Color3f( random(0.5, 1), 
                                                     random(0.5, 1), 
                                                     random(0.5, 1)) );
                                                     
            endEditCP(cubes, Cubes::PositionFieldMask | 
                             Cubes::LengthFieldMask | 
                             Cubes::ColorFieldMask);
            glutPostRedisplay();
            break;
    
    case 'v':   
            {
            bool vd = mgr->getAction()->getVolumeDrawing();
            mgr->getAction()->setVolumeDrawing(!vd);
            SLOG << "Volume Drawing set to " << !vd << endLog;
            }
            break;
            
    case 'w':
            SceneFileHandler::the().write(scene, "cubestest.osg");
            SLOG << "Wrote cubestest.osg" << endLog;
            break;   
    }
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    return winid;
}
