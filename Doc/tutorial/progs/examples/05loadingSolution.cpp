// all needed include files
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>

//additional include files
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGBINWriter.h>

OSG_USING_NAMESPACE
using namespace std;

SimpleSceneManager *mgr;
NodePtr scene;

int setupGLUT( int *argc, char *argv[] );

NodePtr createScenegraph(int argc, char **argv){
    // the scene must be created here
    
    //first we create a group root node which all loaded
    //files will be attached to
    NodePtr root = Node::create();
    beginEditCP(root);
        root->setCore(Group::create());
    endEditCP(root);
    
    //we assume that all given parameters are filename
    for (int i = 1;i < argc; i++){
        NodePtr n = SceneFileHandler::the().read(argv[i]);
    
        //we check the result
        if (n != NullFC){
            // add the loaded node to the root
            beginEditCP(root);
                root->addChild(n);
            endEditCP(root);
        }else
            cout << "Loading the specified file (" << argv[i] << ") was not possible!" << endl;
    }
    
    return root;
}

int main(int argc, char **argv)
{
    osgInit(argc,argv);
        
    int winid = setupGLUT(&argc, argv);
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    scene =createScenegraph(argc, argv);

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

void keyboard(unsigned char k, int , int ){
    switch(k){
        case 's':
        
            // there were some changes in the interface since version 1.2.0
#if OSG_MINOR_VERSION > 2
            // this is the cvs version or version 1.3+
        
            // create an output stream (this is STL code, and
            // is not OpenSG specific!)
            ofstream out("data/output.bin");
            if(!out){
                cout << "Output stream could not be created!" << endl;
                return;
            }
            //create the writer object
            BINWriter writer(out);
#else
            // this code applies to version 1.2
            FILE* outFile = fopen("data/output.bin", "wb");
            if(outFile == NULL){
                cout << "File could not be created!" << endl;
                return; 
            }
            //create the writer object
            BINWriter writer(outFile);
#endif
            
            //write the file now
            writer.write(scene);
            
            cout << "File written!" << endl;
            break;
    }
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
    glutKeyboardFunc(keyboard);
    glutIdleFunc(display);
    
    return winid;
}