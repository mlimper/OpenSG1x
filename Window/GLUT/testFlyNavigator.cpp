/*---------------------------------------------------------------------------*\
 *                                OpenSG                                     *
 *                                                                           *
 *                                                                           *
 *             Copyright (C) 2000,2001 by the OpenSG Forum                   *
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

#include <OSGConfig.h>

#include <iostream>

#include <OSGGLUT.h>

#include <OSGFieldContainerFactory.h>
#include <OSGSFSysTypes.h>
#include <OSGVector.h>
#include <OSGQuaternion.h>
#include <OSGMatrix.h>
#include <OSGMatrixUtility.h>
#include <OSGBoxVolume.h>
#include <OSGNode.h>
#include <OSGGroup.h>
#include <OSGThread.h>
#include <OSGTransform.h>
#include <OSGAttachment.h>
#include <OSGMFVecTypes.h>
#include <OSGAction.h>
#include <OSGDrawAction.h>
#include <OSGSimpleGeometry.h>
#include <OSGSceneFileHandler.h>

#include <OSGDirectionalLight.h>

#include "OSGViewport.h"
#include "OSGCamera.h"
#include "OSGWindow.h"
#include "OSGGLUTWindow.h"
#include "OSGCamera.h"
#include "OSGPerspectiveCamera.h"
#include "OSGSolidBackground.h"
//#include "OSGUniformBackground.h"

#include "OSGSceneFileHandler.h"

#include "OSGTrackball.h"
#include "OSGNavigator.h"


/**---added stuff------------------------------------------------------------------*/

/**---end of added stuff-----------------------------------------------------------*/

using namespace OSG;

DrawAction * ract;

NodePtr  root;

NodePtr  file;

PerspectiveCameraPtr cam;
ViewportPtr vp;
WindowPtr win;

TransformPtr cam_trans;

Navigator flyer;

int mouseb = 0;
int lastx=0, lasty=0;

bool buttonPressed;

void
display(void)
{
    Matrix m1, m2, m3;

    if ( buttonPressed ) flyer.moveTo(lastx,lasty);

    m1=flyer.getMatrix();
//    m1.transpose();

//    m2.setTranslate(0,0,8);
//    m1.mult(m2);

    cam_trans->getSFMatrix()->setValue( m1 );

    win->draw( ract );
}

void reshape( int w, int h )
{
    cerr << "Reshape: " << w << "," << h << endl;
    win->resize( w, h );
}


void
animate(void)
{
    glutPostRedisplay();
}

// tballall stuff


void
motion(int x, int y)
{
/*  Real32 w = win->getWidth(), h = win->getHeight();


    Real32  a = 2. * ( lastx / w - .5 ),
                b = 2. * ( .5 - lasty / h ),
                c = 2. * ( x / w - .5 ),
                d = 2. * ( .5 - y / h );

    if ( mouseb & ( 1 << GLUT_LEFT_BUTTON ) )
    {
        tball.updateRotation( a, b, c, d );
    }
    else if ( mouseb & ( 1 << GLUT_MIDDLE_BUTTON ) )
    {
        tball.updatePosition( a, b, c, d );
    }
    else if ( mouseb & ( 1 << GLUT_RIGHT_BUTTON ) )
    {
        tball.updatePositionNeg( a, b, c, d );
    }*/
    lastx = x;
    lasty = y;

    if ( buttonPressed ) flyer.moveTo(x,y);


}

void
mouse(int button, int state, int x, int y)
{
    if ( state == 0 )
    {
        switch ( button )
        {
        case GLUT_LEFT_BUTTON:  flyer.buttonPress(Navigator::LEFT_MOUSE,x,y);
                                break;
        case GLUT_MIDDLE_BUTTON:flyer.buttonPress(Navigator::MIDDLE_MOUSE,x,y);
                                break;
        case GLUT_RIGHT_BUTTON: flyer.buttonPress(Navigator::RIGHT_MOUSE,x,y);
                                break;
        }
        mouseb |= 1 << button;
        buttonPressed=true;
    }
    else if ( state == 1 )
    {
        switch ( button )
        {
        case GLUT_LEFT_BUTTON:  flyer.buttonRelease(Navigator::LEFT_MOUSE,x,y);
                                break;
        case GLUT_MIDDLE_BUTTON:flyer.buttonRelease(Navigator::MIDDLE_MOUSE,x,y);
                                break;
        case GLUT_RIGHT_BUTTON: flyer.buttonRelease(Navigator::RIGHT_MOUSE,x,y);
                                break;
        }
        mouseb &= ~(1 << button);
        buttonPressed=false;
    }
    lastx = x;
    lasty = y;
}

void
vis(int visible)
{
    if (visible == GLUT_VISIBLE)
    {
        glutIdleFunc(animate);
    }
    else
    {
        glutIdleFunc(NULL);
    }
}

void key(unsigned char key, int x, int y)
{
    switch ( key )
    {
        case 27:    osgExit(); exit(0);
        
        case 'p': cout << "p pressed" << endl; break;            
        
        case 'a':   glDisable( GL_LIGHTING );
            cerr << "Lighting disabled." << endl;
            break;
        case 's':   glEnable( GL_LIGHTING );
            cerr << "Lighting enabled." << endl;
            break;
        case 'z':   glPolygonMode( GL_FRONT_AND_BACK, GL_POINT);
            cerr << "PolygonMode: Point." << endl;
            break;
        case 'x':   glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
            cerr << "PolygonMode: Line." << endl;
            break;
        case 'c':   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
            cerr << "PolygonMode: Fill." << endl;
            break;
        case 'r':   cerr << "Sending ray through " << x << "," << y << endl;
            Line l;
            cam->calcViewRay( l, x, y, *vp );
            cerr << "From " << l.getPosition() << ", dir " << l.getDirection() << endl;
            break;
    }
}


int main (int argc, char **argv)
{
    osgInit(argc,argv);

    // GLUT init

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    int winid = glutCreateWindow("OpenSG");
    glutKeyboardFunc(key);
    glutVisibilityFunc(vis);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutIdleFunc(display);

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    // OSG

    SceneFileHandler::the().print();

    // create the graph

    // beacon for camera and light
    NodePtr b1n = Node::create();
    GroupPtr b1 = Group::create();
    beginEditCP(b1n);
    b1n->setCore( b1 );
    endEditCP(b1n);

    // transformation
    NodePtr t1n = Node::create();
    TransformPtr t1 = Transform::create();
    beginEditCP(t1n);
    t1n->setCore( t1 );
    t1n->addChild( b1n );
    endEditCP(t1n);

    cam_trans = t1;

    // light

    NodePtr dlight = Node::create();
    DirectionalLightPtr dl = DirectionalLight::create();

    beginEditCP(dlight);
    dlight->setCore( dl );
    endEditCP(dlight);

    beginEditCP(dl);
    dl->setAmbient( .3, .3, .3, 1 );
    dl->setDiffuse( 1, 1, 1, 1 );
    dl->setDirection(0,0,1);
    dl->setBeacon( b1n);
    endEditCP(dl);

    // root
    root = Node::create();
    GroupPtr gr1 = Group::create();
    beginEditCP(root);
    root->setCore( gr1 );
    root->addChild( t1n );
    root->addChild( dlight );
    endEditCP(root);

    // Load the file

    NodePtr file = NullFC;

    if ( argc > 1 )
        file = SceneFileHandler::the().read(argv[1]);

    if ( file == NullFC )
    {
        cerr << "Couldn't load file, ignoring" << endl;
        //file = makeTorus( .5, 2, 16, 16 );
        file = makeBox( 2,4,6, 1,1,1 );
    }

    file->updateVolume();

    Vec3f min,max;
    file->getVolume().getBounds( min, max );

    cout << "Volume: from " << min << " to " << max << endl;

    NodePtr file2=makeBox(2,4,6,1,1,1);

    beginEditCP(dlight);
    dlight->addChild( file );
    dlight->addChild( file2 );
    endEditCP(dlight);

    cerr << "Tree: " << endl;
//  root->dump();

    // Camera

    cam = PerspectiveCamera::create();
    cam->setBeacon( b1n );
    cam->setFov( deg2rad( 90 ) );
    cam->setNear( 0.1 );
    cam->setFar( 10000 );

    // Background
    SolidBackgroundPtr bkgnd = SolidBackground::create();

    // Viewport

    vp = Viewport::create();
    vp->setCamera( cam );
    vp->setBackground( bkgnd );
    vp->setRoot( root );
    vp->setSize( 0,0, 1,1 );

    // Window
    cout << "GLUT winid: " << winid << endl;

    GLUTWindowPtr gwin;

    GLint glvp[4];
    glGetIntegerv( GL_VIEWPORT, glvp );

    gwin = GLUTWindow::create();
    gwin->setId(winid);
    gwin->setSize( glvp[2], glvp[3] );

    win = gwin;

    win->addPort( vp );

    win->init();

    // Action

    ract = DrawAction::create();

    // tball

    flyer.setMode(Navigator::FLY);

    flyer.setViewport(vp);

    // run...

    glutMainLoop();

    return 0;
}

