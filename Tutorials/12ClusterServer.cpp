// OpenSG Tutorial Example: Cluster Server
//
// This is a full functional OpenSG cluster server. In OpenSG
// the terms server and client are used similar to X11. The
// application is the client. Instances that are used for 
// rendering are called server.
//

#include <iostream>

// GLUT is used for window handling
#include <OpenSG/OSGGLUT.h>
// General OpenSG configuration, needed everywhere
#include <OpenSG/OSGConfig.h>
// The Cluster server definition
#include <OpenSG/OSGClusterServer.h>
// The GLUT-OpenSG connection class
#include <OpenSG/OSGGLUTWindow.h>
// Render action definition. 
#include <OpenSG/OSGRenderAction.h>

OSG_USING_NAMESPACE

// local glut window
GLUTWindowPtr   window;
// render action
RenderAction   *ract;
// pointer the the cluster server instance
ClusterServer  *server;

// forward declaration so we can have the interesting stuff upfront
void display();
void reshape( int width, int height );

// Initialize GLUT & OpenSG and start the cluster server
int main(int argc,char **argv)
{
    int             winid;
    char           *name          ="ClusterServer";
    char           *connectionType="StreamSock";
    bool            fullscreen     =true;
    std::string     address        ="";
    char           *opt;
    
    // evaluate params
    for(int a=1 ; a<argc ; ++a)
    {
        if(argv[a][0] == '-')
        {
            switch(argv[a][1])
            {
                case 'm': connectionType="Multicast";
                          break;
                case 'w': fullscreen=false;
                          break;
                case 'a': address = argv[a][2] ? argv[a]+2 : argv[++a];
                          if(address == argv[argc])
                          { 
                              std::cerr << "address missing" << std::endl;
                              return 0;
                          }
                          std::cout << address << std::endl;
                          break;
                default:  std::cout << argv[0] 
                                    << "-m "
                                    << "-w "
                                    << "-a address "
                                    << std::endl;
                          return 0;
            }
        }
        else
        {
            name=argv[a];
        }
    }
    try
    {
        // init OpenSG
        osgInit(argc, argv);

        // initialize Glut
        glutInit(&argc, argv);
        glutInitDisplayMode( GLUT_RGB | 
                             GLUT_DEPTH | 
                             GLUT_DOUBLE);
        winid = glutCreateWindow(name);
        if(fullscreen)
            glutFullScreen();
        glutDisplayFunc(display);       
        glutIdleFunc(display);
        glutReshapeFunc(reshape);       
        glutSetCursor(GLUT_CURSOR_NONE);

        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_NORMALIZE );

        // create the render action
        ract=RenderAction::create();

        // setup the OpenSG Glut window
        window     = GLUTWindow::create();
        window->setId(winid);
        window->init();

        // create the cluster server
        server     = new ClusterServer(window,name,connectionType,address);
        // start the server
        server->start();

        // enter glut main loop
        glutMainLoop();
    }
    catch(OSG_STDEXCEPTION_NAMESPACE::exception &e)
    {
        std::cerr << e.what() << std::endl;
        delete server;
        osgExit(); 
    }
    return 0;
}

/* render loop */
void display()
{
    try
    {
        // receive scenegraph and do rendering
        server->render(ract);
        // clear changelist 
        OSG::Thread::getCurrentChangeList()->clearAll();
    } 
    catch(OSG_STDEXCEPTION_NAMESPACE::exception &e)
    {
        std::cerr << e.what() << std::endl;
        // try to restart server
        server->stop();
        // start server, wait for client to connect
        server->start();
    }
}

/* window reshape */
void reshape( int width, int height )
{
    // set new window size
	window->resize( width, height );
}
