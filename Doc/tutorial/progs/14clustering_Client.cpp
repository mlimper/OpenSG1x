// all needed include files
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>

#include <OpenSG/OSGMultiDisplayWindow.h>
#include <OpenSG/OSGSceneFileHandler.h>

OSG_USING_NAMESPACE
using namespace std;

SimpleSceneManager *mgr;
NodePtr scene;

int setupGLUT( int *argc, char *argv[] );

int main(int argc, char **argv)
{
    osgInit(argc,argv);

    int winid = setupGLUT(&argc, argv);
	
	//this time we'll have not a GLUTWindow here, but this one
    MultiDisplayWindowPtr multiWindow = MultiDisplayWindow::create();

	//set some necessary values
    beginEditCP(multiWindow);
		// we connect via multicast
		multiWindow->setConnectionType("Multicast");
		//multiWindow->setServiceAddress("192.168.2.142");
		// we want to rendering servers... 
		multiWindow->getServers().push_back("Server1");
		multiWindow->getServers().push_back("Server2");	
    endEditCP(multiWindow);

	//any scene here
    scene = makeTorus(.5, 2, 16, 16);
	
	// and the ssm as always
    mgr = new SimpleSceneManager;

    mgr->setWindow(multiWindow);
    mgr->setRoot  (scene);
    mgr->showAll();
    
    multiWindow->init();
    
    glutMainLoop();

    return 0;
}

void display(void)
{
    //redrawing as usual
	mgr->redraw();
	
	//the changelist should be cleared - else things
	//could be copied multiple times
	OSG::Thread::getCurrentChangeList()->clearAll();
	
	// to ensure a black navigation window
	glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (state)
        mgr->mouseButtonRelease(button, x, y);
    else
        mgr->mouseButtonPress(button, x, y);
    glutPostRedisplay();
}

void motion(int x, int y)
{
    mgr->mouseMove(x, y);
    glutPostRedisplay();
}

int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
	
    return winid;
}
