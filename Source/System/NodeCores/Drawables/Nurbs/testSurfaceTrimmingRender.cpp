// OpenSG NURBS example: testSurfaceTrimmingRender.cpp
//
// Shows how to render a trimmed NURBS surface.

#include <OSGGLUT.h>
#include <OSGConfig.h>
#include <OSGSimpleSceneManager.h>
#include <OSGSimpleMaterial.h>
#include <OSGGLUTWindow.h>
#include <OSGSolidBackground.h>
#include <OSGSurface.h>

OSG_USING_NAMESPACE

SimpleSceneManager *mgr;
SimpleMaterialPtr  gpcl_defaultmat;
Real32             g_error;
SurfacePtr         gpcl_surface;

// redraw the window
void display(void)
{      
    // render
    
    mgr->redraw();

    // all done, swap  (better not GV)
    //glutSwapBuffers();
}

// react to size changes
void reshape(int w, int h)
{
    mgr->resize(w,h);
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
void keyboard(unsigned char k, int, int)
{
    switch(k)
    {
    case 27:    exit(1);
    case 'y':   
    case 'z':
                glPolygonMode( GL_FRONT_AND_BACK, GL_POINT);
                std::cerr << "PolygonMode: Point." << std::endl;
                break;
    case 'x':   glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
                std::cerr << "PolygonMode: Line." << std::endl;
                break;
    case 'c':   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
                std::cerr << "PolygonMode: Fill." << std::endl;
                break;
    case 'f':   g_error *= 2;
                std::cerr << "Error: " << g_error << std::endl;
                beginEditCP( gpcl_surface, Surface::ErrorFieldMask );
                {
                    gpcl_surface->setError( g_error );
                }
                endEditCP(   gpcl_surface, Surface::ErrorFieldMask );
                break;
    case 'g':   g_error /= 2;
                std::cerr << "Error: " << g_error << std::endl;
                beginEditCP( gpcl_surface, Surface::ErrorFieldMask );
                {
                    gpcl_surface->setError( g_error );
                }
                endEditCP(   gpcl_surface, Surface::ErrorFieldMask );
                break;
    }
}


void setupDefaultMaterial( void )
{
    gpcl_defaultmat = SimpleMaterial::create();
	addRefCP( gpcl_defaultmat );
    beginEditCP(gpcl_defaultmat);
	{
		gpcl_defaultmat->setDiffuse( Color3f(1.0,0.0,0.0) ); // RED
		gpcl_defaultmat->setAmbient( Color3f(0.2,0.2,0.2) );
		gpcl_defaultmat->setEmission( Color3f(0.02,0.02,0.02) );
		gpcl_defaultmat->setSpecular( Color3f(0.78,0.78,0.78) );
		gpcl_defaultmat->setShininess( 128 );
	}
    endEditCP(gpcl_defaultmat);
}






NodePtr makeScene( void )
{
    setupDefaultMaterial();

    NodePtr root = Node::create();
	addRefCP( root );
    SurfacePtr surface = Surface::create();
    GeoPositions3fPtr cps = GeoPositions3f::create();
    beginEditCP( cps, GeoPositions3f::GeoPropDataFieldMask );
    {
        // control points should always be 3D for the time being,
        // rational support will be added later
        cps->clear();
        cps->push_back( Pnt3f(  1,  1,  0 ));
        cps->push_back( Pnt3f(  1,  0,  0 ));
        cps->push_back( Pnt3f(  1,  0,  1 ));
        cps->push_back( Pnt3f(  1, -1,  1 ));

        cps->push_back( Pnt3f(  0,  1,  0 ));
        cps->push_back( Pnt3f(  0,  0,  0 ));
        cps->push_back( Pnt3f(  0,  0,  1 ));
        cps->push_back( Pnt3f(  0, -1,  1 ));

        cps->push_back( Pnt3f(  0,  1,  1 ));
        cps->push_back( Pnt3f(  0,  0,  1 ));
        cps->push_back( Pnt3f(  0,  0,  0 ));
        cps->push_back( Pnt3f(  0, -1,  0 ));

        cps->push_back( Pnt3f( -1,  1,  1 ));
        cps->push_back( Pnt3f( -1,  0,  1 ));
        cps->push_back( Pnt3f( -1,  0,  0 ));
        cps->push_back( Pnt3f( -1, -1,  0 ));
    }
    endEditCP( cps, GeoPositions3f::GeoPropDataFieldMask );
    
    std::vector<Real64> knots1;
    std::vector<Real64> knots2;
    std::vector<Pnt2f> points;
    knots1.clear();
    knots1.push_back(0);
    knots1.push_back(0);
    knots1.push_back(1);
    knots1.push_back(1);
    knots2.clear();
    knots2.push_back(0);
    knots2.push_back(0);
    knots2.push_back(0);
    knots2.push_back(1);
    knots2.push_back(1);
    knots2.push_back(1);
        
    beginEditCP( surface );
    {
        // first, let's clear the trimming
        surface->removeCurves();

        // add the outer trimming around the domain
        points.clear();
        points.push_back( Pnt2f(0,0) );
        points.push_back( Pnt2f(1,0) );
        surface->addCurve( 1, knots1, points, true );
        points.clear();
        points.push_back( Pnt2f(1,0) );
        points.push_back( Pnt2f(1,1) );
        surface->addCurve( 1, knots1, points, false );
        points.clear();
        points.push_back( Pnt2f(1,1) );
        points.push_back( Pnt2f(0,1) );
        surface->addCurve( 1, knots1, points, false );
        points.clear();
        points.push_back( Pnt2f(0,1) );
        points.push_back( Pnt2f(0,0) );
        surface->addCurve( 1, knots1, points, false );

        // add inner trimming loop 1
        points.clear();
        points.push_back( Pnt2f(0.3,0.5) );
        points.push_back( Pnt2f(0.1,0.9) );
        points.push_back( Pnt2f(0.5,0.7) );
        surface->addCurve( 2, knots2, points, true );
        points.clear();
        points.push_back( Pnt2f(0.5,0.7) );
        points.push_back( Pnt2f(0.9,0.9) );
        points.push_back( Pnt2f(0.7,0.5) );
        surface->addCurve( 2, knots2, points, false );
        points.clear();
        points.push_back( Pnt2f(0.7,0.5) );
        points.push_back( Pnt2f(0.9,0.1) );
        points.push_back( Pnt2f(0.5,0.3) );
        surface->addCurve( 2, knots2, points, false );
        points.clear();
        points.push_back( Pnt2f(0.5,0.3) );
        points.push_back( Pnt2f(0.1,0.1) );
        points.push_back( Pnt2f(0.3,0.5) );
        surface->addCurve( 2, knots2, points, false );
        
        // add inner trimming loop 2
        points.clear();
        points.push_back( Pnt2f(0.4,0.5) );
        points.push_back( Pnt2f(0.4,0.4) );
        points.push_back( Pnt2f(0.5,0.4) );
        surface->addCurve( 2, knots2, points, true );
        points.clear();
        points.push_back( Pnt2f(0.5,0.4) );
        points.push_back( Pnt2f(0.6,0.4) );
        points.push_back( Pnt2f(0.6,0.5) );
        surface->addCurve( 2, knots2, points, false );
        points.clear();
        points.push_back( Pnt2f(0.6,0.5) );
        points.push_back( Pnt2f(0.6,0.6) );
        points.push_back( Pnt2f(0.5,0.6) );
        surface->addCurve( 2, knots2, points, false );
        points.clear();
        points.push_back( Pnt2f(0.5,0.6) );
        points.push_back( Pnt2f(0.4,0.6) );
        points.push_back( Pnt2f(0.4,0.5) );
        surface->addCurve( 2, knots2, points, false );
        
        // set up dimensions and knot vectors:
        surface->setDimU( 3 );
        surface->setDimV( 3 );
        surface->getKnotsU().clear();
        surface->getKnotsU().push_back( 0 );
        surface->getKnotsU().push_back( 0 );
        surface->getKnotsU().push_back( 0 );
        surface->getKnotsU().push_back( 0 );
        surface->getKnotsU().push_back( 1 );
        surface->getKnotsU().push_back( 1 );
        surface->getKnotsU().push_back( 1 );
        surface->getKnotsU().push_back( 1 );
        surface->getKnotsV().clear();
        surface->getKnotsV().push_back( 0 );
        surface->getKnotsV().push_back( 0 );
        surface->getKnotsV().push_back( 0 );
        surface->getKnotsV().push_back( 0 );
        surface->getKnotsV().push_back( 1 );
        surface->getKnotsV().push_back( 1 );
        surface->getKnotsV().push_back( 1 );
        surface->getKnotsV().push_back( 1 );
        
        // set control points and texture control points
        surface->setControlPoints( cps );
        
        // set error
        surface->setError( g_error );
        
        // and finally set the material
        surface->setMaterial( gpcl_defaultmat );
    }
    endEditCP( surface );
    beginEditCP( root );
    {
        root->setCore( surface );
    }
    endEditCP( root );
    gpcl_surface = surface;

    return root;
}

int main(int argc, char **argv)
{
    g_error = 0.01;

    if ( argc == 2 )
    {
        g_error = atof( argv[1] );
    }
    if ( g_error < 0.001 )
    {
        g_error = 0.001;
    }
    
    osgInit(argc,argv);
    // GLUT init
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    GLUTWindowPtr gwin = GLUTWindow::create();
	beginEditCP(gwin);
	{
	    gwin->setId(winid);
	}
    endEditCP(gwin);
    gwin->init();

    // create the scene
    NodePtr scene;
    scene = makeScene( );
    
    if ( scene == NullFC )
    {
        std::cerr<<"makeScene returned NullFC, exiting..."<<std::endl;
        return -1;
    }

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // create the window and initial camera/viewport
    mgr->setWindow( gwin );
    // tell the manager what to manage
    mgr->setRoot  ( scene );
    
    // show the whole scene
    mgr->showAll();
    mgr->redraw();
	SolidBackgroundPtr bgr = SolidBackground::create();
    beginEditCP( bgr );
    bgr->setColor( Color3f( 1.0, 1.0, 1.0 ));
    endEditCP( bgr );
    mgr->getWindow()->getPort(0)->setBackground( bgr );
    
    // GLUT main loop
    glutMainLoop();

    return 0;
}

