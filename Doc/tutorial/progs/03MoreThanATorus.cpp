//what follows here is the smallest OpenSG programm possible
//most things used here are explained now or on the next few pages, so don't 
//worry if not everythings clear right at the beginning...

//Some needed inlcude files - these will become more, believe me ;)
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>

//In most cases it is useful to add this line, else every OpenSG command
//must be proceeded by an extra OSG::
OSG_USING_NAMESPACE

//The SimpleSceneManager is a little usefull class which helps us to 
//manage little scenes. It will be discussed in detail later on
SimpleSceneManager *mgr;
NodePtr scene;

//we have a forward declarion here, just to sort the code 
int setupGLUT( int *argc, char *argv[] );

//This function will create our scenegraph
NodePtr createScenegraph(){
    // First we will create all needed geometry
    // the body of the house
    NodePtr houseMain = makeBox(20,20,20,1,1,1);
    
    // now the roof
    NodePtr roof = makeBox(14.14, 14.14, 20, 1, 1, 1);
    
    // and the chimney - we have the top and sides generated
    // but we have no need for the bottom (it is inside the house)
    NodePtr chimney = makeCylinder(10,1,8,true,true,false);

    // Now we create the root node and attach the geometry nodes to it
    NodePtr n = Node::create();
    beginEditCP(n, Node::CoreFieldMask | Node::ChildrenFieldMask);
        n->setCore(Group::create());
        n->addChild(houseMain);
        n->addChild(roof);
        n->addChild(chimney);
    endEditCP(n, Node::CoreFieldMask | Node::ChildrenFieldMask);
    return n;
}

int main(int argc, char **argv)
{
        // Init the OpenSG subsystem
        osgInit(argc,argv);
        
        // We create a GLUT Window (that is almost the same for most applications)
        int winid = setupGLUT(&argc, argv);
        GLUTWindowPtr gwin= GLUTWindow::create();
        gwin->setId(winid);
        gwin->init();

        // That will be our whole scene for now : an incredible Torus
        scene = createScenegraph();

        // Create and setup our little friend - the SSM
        mgr = new SimpleSceneManager;
        mgr->setWindow(gwin );
        mgr->setRoot  (scene);
        mgr->showAll();
    
        // Give Control to the GLUT Main Loop
        glutMainLoop();

        return 0;
}

// react to size changes
void reshape(int w, int h)
{
    mgr->resize(w, h);
    glutPostRedisplay();
}

// just redraw our scene if this GLUT callback is invoked
void display(void)
{
        mgr->redraw();
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

//The GLUT subsystem is set up here. This is very similar to other GLUT applications
//If you have worked with GLUT before, you may have the feeling of meeting old friends again,
//if you have not used GLUT before that is no problem. GLUT will be introduced shortly on the
//next section

int setupGLUT(int *argc, char *argv[])
{
        glutInit(argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
        int winid = glutCreateWindow("OpenSG First Application");
    
        glutDisplayFunc(display);
        glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
        glutIdleFunc(display);
        return winid;
}
