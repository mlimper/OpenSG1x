#include <OSGGLUT.h>
#include <OSGConfig.h>
#include <iostream>
#include <OSGLog.h>
#include <OSGClusterServer.h>
#include <OSGGLUTWindow.h>
#include <OSGRenderAction.h>
#include <OSGSceneFileHandler.h>
#include <OSGViewport.h>

OSG_USING_NAMESPACE

int             winid;
ClusterServer  *server;
GLUTWindowPtr   window;
RenderAction   *ract;
bool            running=false;
bool            exitOnError=false;
UInt32          servicePort=8437;
std::string     serviceGroup="224.245.211.234";

void display()
{
    if(!running)
    {
        server->start();
        running=true;
        glutShowWindow();
    }
    try
    {
        server->render(ract);
        // clear changelist from prototypes
        OSG::Thread::getCurrentChangeList()->clearAll();
    } 
    catch(OSG_STDEXCEPTION_NAMESPACE::exception &e)
    {
        if(exitOnError)
        {
            SLOG << e.what() << std::endl;
            try
            {
                delete server;
            }
            catch(...)
            {
            }
            printf("Exit on error %s",e.what());
            osgExit();
            exit(0);
        }
        else
        {
            // try to restart server
            try
            {
                server->stop();
            }
            catch(...)
            {
            }
            running=false;
            glutHideWindow();
        }
    }
}

void reshape( int width, int height )
{
    std::cout << "reshape " << width << " " << height << std::endl;
	window->resize( width, height );
}

void key(unsigned char key, int /*x*/, int /*y*/)
{
	switch ( key )
	{
        case 'd':
            window->getPort()[0]->getRoot()->dump();
            break;
        case 's':
            SceneFileHandler::the().write(window->getPort()[0]->getRoot(),"server.osb");
            exit(0);
            break;
	}
}

int main(int argc,char **argv)
{
    char           *name          ="ClusterServer";
    char           *connectionType="StreamSock";
    bool           fullscreen     =true;
    std::string    address        ="";
    int            width=-1,height=300,x=0,y=0;
    bool           doStereo=false;
    char           *str;

    for(int i = 1 ; i < argc ; ++i)
    {
        if(argv[i][0] == '-')
        {
            switch(argv[i][1])
            {
                case 'm':
                    connectionType="Multicast";
                    break;
                case 's':
                    doStereo=true;
                    break;
                case 'w':
                    fullscreen=false;
                    break;
                case 'e':
                    exitOnError=true;
                    break;
                case 'a':
                    address=&(argv[i][2]);
                    break;
                case 'g':
                    if(argv[i][2] != '\0')
                        str=argv[i]+2;
                    else
                        str=argv[++i];
                    if(sscanf(str,"%d,%d,%d,%d",
                              &width,&height,&x,&y)!=4)
                    {
                        SWARNING << "Wrong args in -g. Use -gw,h,x,y" 
                                 << std::endl;
                        exit(0);
                    }
                    break;
                case 'p':
                    if(argv[i][2] != '\0')
                        servicePort=atoi(argv[i]+2);
                    else
                        servicePort=atoi(argv[++i]);
                    break;
                case 'j':
                    if(argv[i][2] != '\0')
                        serviceGroup=argv[i]+2;
                    else
                        serviceGroup=argv[++i];
                    break;
                case 'h':
                    std::cout << argv[0] 
                              << "-m "
                              << "-s "
                              << "-w "
                              << "-e "
                              << "-g w,h,x,y "
                              << "-a Address "
                              << "-j group "
                              << "-p servicePort "
                              << std::endl;
                    std::cout << "-m         use multicast" << std::endl;
                    std::cout << "-s         enable stereo" << std::endl;
                    std::cout << "-w         no fullscreen" << std::endl;
                    std::cout << "-e         exit after closed connection" 
                              << std::endl;
                    std::cout << "-g         geometry" << std::endl;
                    std::cout << "-a Address Server network address"
                              << std::endl;
                    std::cout << "-m Address wait for requests on "
                              << "multicast group" << std::endl;
                    std::cout << "-p port    wait for requests on port"
                              << std::endl;
                    return 0;
            }
        }
        else
        {
            name=argv[i];
        }
    }

    try
    {
    	ChangeList::setReadWriteDefault();
        osgInit(argc, argv);
        glutInit(&argc, argv);
        if(doStereo)
            glutInitDisplayMode( GLUT_STEREO | 
                                 GLUT_RGB | 
                                 GLUT_DEPTH | 
                                 GLUT_DOUBLE);
        else
            glutInitDisplayMode( GLUT_RGB | 
                                 GLUT_DEPTH | 
                                 GLUT_DOUBLE);
        glutInitWindowPosition(x,y);
        if(width>0)
            glutInitWindowSize(width,height);
        winid = glutCreateWindow("OpenSG Cluster Client");
        if(fullscreen)
            glutFullScreen();
        /*
          else
          glutReshapeWindow(300,300);
        */
        if(width>0)
            glutReshapeWindow(width,height);
        glutPositionWindow(x, y);

        glutPopWindow();
        glutDisplayFunc(display);       
        glutIdleFunc(display);       
        glutKeyboardFunc(key);
        glutReshapeFunc(reshape);       
        glutSetWindowTitle(name);
        // switch off cursor
        glutSetCursor(GLUT_CURSOR_NONE);
        glEnable( GL_LIGHTING );
        glEnable( GL_LIGHT0 );
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_NORMALIZE );
        ract=RenderAction::create();
        window     = GLUTWindow::create();
        window->setId(winid);
        window->init();
        server     = new ClusterServer(window,
                                       name,
                                       connectionType,
                                       address,
                                       servicePort,
                                       serviceGroup);
        server->start();
        running=true;
        glutMainLoop();
    } 
    catch(OSG_STDEXCEPTION_NAMESPACE::exception &e)
    {
        SLOG << e.what() << std::endl;
        delete server;
        osgExit(); 
    }
    return 0;
}
