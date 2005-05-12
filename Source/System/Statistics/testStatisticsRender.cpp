
#include <OSGGLUT.h>
#include <OSGConfig.h>
#include <OSGSimpleGeometry.h>
#include <OSGPassiveWindow.h>
#include <OSGSimpleSceneManager.h>
#include <OSGSceneFileHandler.h>

#include <OSGDrawable.h>
#include <OSGSimpleStatisticsForeground.h>
#include <OSGStatElemTypes.h>
#include <OSGStatCollector.h>

OSG_USING_NAMESPACE

SimpleSceneManager *mgr;

StatCollector *collector;

// redraw the window
void display(void)
{   
    mgr->redraw();

    // all done, swap    
    glutSwapBuffers();
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
        case 27:    
        {
            osgExit();
            exit(0);
        }
        
        case 'v':
        {
            mgr->getAction()->setVolumeDrawing(
                                    !mgr->getAction()->getVolumeDrawing());
		    std::cerr << "Volume Drawing: " 
                      << (mgr->getAction()->getVolumeDrawing()?"on":"off") 
                      << std::endl;
        }
        break;
    }
}


int main(int argc, char **argv)
{
    osgInit(argc,argv);

    // GLUT init
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(500, 500);
    glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    PassiveWindowPtr pwin=PassiveWindow::create();
    pwin->init();

    // create the scene
    NodePtr scene;
    
    if(argc > 1)
    {
        scene = Node::create();
        GroupPtr g = Group::create();
        
        beginEditCP(scene);
        scene->setCore(g);
        
        for(UInt16 i = 1; i < argc; ++i)
            scene->addChild(SceneFileHandler::the().read(argv[i]));
        
        endEditCP(scene);
    }
    else
    {
        scene = makeTorus(.5, 3, 16, 16);
    }

    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // create the window and initial camera/viewport
    mgr->setWindow(pwin );
    // tell the manager what to manage
    mgr->setRoot  (scene);
    
    // show the whole scene
    mgr->showAll();

    // add the statistics forground
    
    SimpleStatisticsForegroundPtr statfg = SimpleStatisticsForeground::create();
    
    beginEditCP(statfg);
    statfg->setSize(25);
    statfg->setColor(Color4f(0,1,0,0.7));
    statfg->addElement(RenderAction::statDrawTime, "Draw FPS: %r.3f");
    statfg->addElement(DrawActionBase::statTravTime, "TravTime: %.3f s");
    statfg->addElement(RenderAction::statDrawTime, "DrawTime: %.3f s");
    statfg->addElement(DrawActionBase::statCullTestedNodes, 
                       "%d Nodes culltested");
    statfg->addElement(DrawActionBase::statCulledNodes, 
                       "%d Nodes culled");
    statfg->addElement(RenderAction::statNMaterials, 
                       "%d material changes");
    statfg->addElement(RenderAction::statNMatrices, 
                       "%d matrix changes");
    statfg->addElement(RenderAction::statNGeometries, 
                       "%d Nodes drawn");
    statfg->addElement(RenderAction::statNTransGeometries, 
                       "%d transparent Nodes drawn");
    statfg->addElement(Drawable::statNTriangles, 
                       "%d triangles drawn");
    statfg->addElement(Drawable::statNLines, 
                       "%d lines drawn");
    statfg->addElement(Drawable::statNPoints, 
                       "%d points drawn");
    statfg->addElement(Drawable::statNPrimitives,
                        "%d primitive groups drawn");
    statfg->addElement(Drawable::statNVertices, 
                       "%d vertices transformed");
    endEditCP(statfg);
    
    collector = &statfg->getCollector();
    
    // add optional elements
    collector->getElem(Drawable::statNTriangles);
    
    mgr->getAction()->setStatistics(collector);
    
    beginEditCP(pwin->getPort(0));
    pwin->getPort(0)->getForegrounds().push_back(statfg);
    endEditCP  (pwin->getPort(0));
    
    // GLUT main loop
    glutMainLoop();

    return 0;
}
