// all needed include files
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>

#include <OpenSG/OSGThreadManager.h>

OSG_USING_NAMESPACE

SimpleSceneManager *mgr;
NodePtr scene;

int setupGLUT( int *argc, char *argv[] );

NodePtr createScenegraph(){
    // the scene must be created here
	NodePtr n = makeSphere(2,2);
	return n;
}

void printA(void *args){
		while (true)
			SLOG << "Taschenrechner" << endLog;
			//std::cout << "Taschenrechner" << std::endl;
}

void printB(void *args){
		while (true)
			SLOG << "Power Mac G5" << endLog;
			//std::cout << "Power Mac G5" << std::endl;
}

int main(int argc, char **argv)
{
    osgInit(argc,argv);
        
    int winid = setupGLUT(&argc, argv);
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    scene =createScenegraph();

    mgr = new SimpleSceneManager;
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);
    mgr->showAll();
	
	Thread* threadOne = dynamic_cast<Thread *>(ThreadManager::the()->getThread("One"));
	Thread* threadTwo = dynamic_cast<Thread *>(ThreadManager::the()->getThread("Two"));
    
	threadOne->runFunction(printA, 1, NULL);
	threadTwo->runFunction(printB, 1, NULL);
	
    glutMainLoop();

    return 0;
}

void reshape(int w, int h)
{
    mgr->resize(w, h);
    glutPostRedisplay();
}

void display(void)
{
        mgr->redraw();
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
    
    int winid = glutCreateWindow("OpenSG First Application");
    
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutIdleFunc(display);
    return winid;
}
