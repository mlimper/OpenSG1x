// all needed include files
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>

#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGSimpleAttachments.h>

OSG_USING_NAMESPACE
using namespace std;


SimpleSceneManager *mgr;
NodePtr scene;

int setupGLUT( int *argc, char *argv[] );

//This is the function that will be called when a node
//is entered during traversal.
Action::ResultE enter(NodePtr& node){   
    if (getName(node)){
		cout << getName(node) << endl;
	}else
		cout << "No name was set!" << endl;

    return Action::Continue; 
}

//This function will test if the core is of type
//geometry and if it is, it will print the node's
//name
Action::ResultE isGeometry(NodePtr& node){
	// this tests if the core is derived from geometry
	if (node->getCore()->getType().isDerivedFrom(Geometry::getClassType()))
		if (getName(node))
			cout << "Found a geometry core stored in " << getName(node) << endl;
		else
			cout << "Found a geometry core but node has no name" << endl;
	
	return Action::Continue;
}


NodePtr createScenegraph(){
    // the scene must be created here
	NodePtr n = SceneFileHandler::the().read("data/torus_sphere_cone.wrl");
    
    //we check the result
    if (n != NullFC)
		return n;
    else{
        cout << "Loading the specified file was not possible!" << endl;
        return NullFC;
    }
	
	return n;
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

void keyboard(unsigned char k, int x, int y){
    switch(k){
        case 27:    {
            osgExit();
            exit(1);
        }
        break;
		
		// this will print the names of all nodes
		// in the whole graph
		case 'p':
			cout << endl << endl;
			cout << "Printing all node names";
			cout << "---------------------------------------";
			cout << endl << endl;
			
			// now we invoke the traversal
			traverse(scene, 
					osgTypedFunctionFunctor1CPtrRef
					<Action::ResultE, NodePtr>(enter));
			
		break;
		
		// this will only print the names of nodes
		// which have a geometry core
		case 'g':
			cout << endl << endl;
			cout << "Printing all geometry nodes";
			cout << "---------------------------------------";
			cout << endl << endl;
		
			// traverse the graph
			traverse(scene,
					osgTypedFunctionFunctor1CPtrRef
					<Action::ResultE, NodePtr>(isGeometry));
		break;
	}
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
	glutKeyboardFunc(keyboard);
    return winid;
}
