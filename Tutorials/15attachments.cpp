// OpenSG Tutorial Example: Attachments
//
// In many cases there is a need to attach some data to existing structures
// instead of deriving new classes. Many systems have a void* userdata 
// to do that. OpenSG uses the more flexible concept of Attachments. 
//

// Headers
#include <OpenSG/OSGGLUT.h>
#include <string>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGAction.h>
#include <OpenSG/OSGSceneFileHandler.h>

// New Headers
#include <OpenSG/OSGAttachment.h>
#include <OpenSG/OSGSimpleAttachments.h>

// Activate the OpenSG namespace
OSG_USING_NAMESPACE

// The SimpleSceneManager to manage simple applications
SimpleSceneManager *mgr;

// forward declaration so we can have the interesting stuff upfront
int setupGLUT(int *argc, char *argv[]);

// Helper class to find a named node
// This class uses the NameAttachment directly, see below for more on name
// handling.
class NamedNodeFinder
{
  public:
  
    NamedNodeFinder(void) : _name(), _found() {}

    NodePtr operator() (NodePtr root, std::string name)
    {
        _name=&name;
        _found=NullFC;
        
        traverse(root, osgTypedMethodFunctor1ObjPtrCPtrRef(this, &NamedNodeFinder::check));
        
        return _found;
    }
    
  private:
     
    Action::ResultE check(NodePtr& node)
    {
        AttachmentPtr a = node->findAttachment(Name::getClassType());
        
        if(a != NullFC)
        {
            NamePtr n = NamePtr::dcast(a);
            
            if(n->getField().getValue() == *_name)
            {
                _found = node;
                return Action::Quit;
            }
        }
        
        return Action::Continue;        
    }
 
    NodePtr  _found;
    std::string  *_name;
};

/*
   Define a simple Attachment using only a single Field
   
   You need to define the type of the field and a couple names and put it 
   into the AttachmentDesc. This is a trait used to define a SimpleAttachment
   and its Ptr.
   
   Access to the value is done via the getField() method (see below).
   
   This is just a convenience class for simple attachments. If you need
   more complex attachments you have to define them just like any other
   FieldContainer and derive them from Attachment. See Examples/NewTypes
   for info on how to do that.
   
   Note that if you want to put this into a library and use it from the outside
   you have to make sure that the static _type and _desc members are only
   instantiated in the library, not in the user's code. The procedure is the 
   same for any FieldContainer, see Examples/NewTypes for details.
*/
 
struct MyAttachmentDesc
{
    static  FieldDescription *_desc[];

    typedef SFInt32           FieldTypeT;

    static const Char8       *getTypeName  (void)     { return "My";          }
    static const Char8       *getFieldName (void)     { return "my";          }
    static const Char8       *getGroupName (void)     { return "my";          }
    static const Char8       *getClassName (void)     { return "MyAttachment";}
    static const Char8       *getParentTypeName(void) { return "Attachment";  }

    static FieldDescription **getDesc       (void)    { return _desc;         }

    static InitContainerF     getInitMethod(void)     { return NULL;          }
};

// The FieldDescription necessary to register the new class with the 
// Type system.
FieldDescription *MyAttachmentDesc::_desc[] =
{
    new FieldDescription(
        FieldTypeT::getClassType(), 
        getFieldName(), 
        OSG_FC_FIELD_IDM_DESC(SimpleAttachment<
                                  MyAttachmentDesc>::SimpleField),
        false,
        (FieldAccessMethod) &SimpleAttachment<
                                  MyAttachmentDesc>::getFieldPtr)

};

#ifdef WIN32
OSG_FC_TYPE_FUNCTIONS_INL_TMPL_DEF(MyAttachmentDesc,
                                   SimpleAttachment);
#endif

// Instantiate some necessary helper functions/classes.
OSG_FC_DLLEXPORT_DEF(SimpleAttachment, MyAttachmentDesc, );

// define the classes to simplify access of the new attachment
typedef SimpleAttachment<MyAttachmentDesc            > MyAttachment;
// the first parameter is the Ptr to the parent class
typedef FCPtr           <AttachmentPtr,  MyAttachment> MyAttachmentPtr;


// Initialize GLUT & OpenSG and set up the scene
int main(int argc, char **argv)
{
    // OSG init
    osgInit(argc,argv);

    // GLUT init
    int winid = setupGLUT(&argc, argv);

    // the connection between GLUT and OpenSG
    GLUTWindowPtr gwin = GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    // load the scene

    NodePtr scene;
    
    if(argc < 2)
    {
        FWARNING(("No file given!\n"));
 
        scene = makeTorus(.5, 2, 16, 16);
    }
    else
    {
        scene = SceneFileHandler::the().read(argv[1]);
    }

    /*   
       An Attachment is a special field container that can be attached to
       many of the internal classes like Nodes, NodeCores and many others.
       There can be multiple Attachments attached to an object.

       Attachments can be attached to all FieldContainers that are derived from
       AttachmentContainer. This includes most higher-level classes in the
       system, like Nodes, NodeCores, Windows, Viewports etc. See the
       inheritance graph for details.

       One predefined kind of Attachment is the Name, which can
       keep the name of an object. Some of loaders (e.g. the WRL loader)
       create these kinds of Attachments for named nodes.
    */
    
    /*
        An Attachment is a FieldContainer and as such needs to be created using
        ::create().
    */
    NamePtr name = Name::create();
    
    /* 
        The NameAttachment only has a single field, there's no need to use the
        mask here.
    */
    beginEditCP(name);
    {
        name->getField().setValue("Scene");
    }
    endEditCP  (name);
    
    /*
        Attach the name to the scene node.
    */
    beginEditCP(scene, Node::AttachmentsFieldMask);
    {
        scene->addAttachment(name);
    }
    endEditCP  (scene, Node::AttachmentsFieldMask);
    
    /*
        Check if the scene has a Name attachment
        
        Attachments are categorized by the GroupID of their class. Every
        AttachmentContainer generally keeps only one attachment of a specific
        kind. 
    */
    AttachmentPtr a;
    
    a = scene->findAttachment(Name::getClassType());
    
    if(a!=NullFC)
    {
        NamePtr n = NamePtr::dcast(a);
        
        SLOG << "Node name: " << n->getField().getValue() << endLog;
    }
    else
    {
        SLOG << "Node has no name!" << endLog;
    }
    
    /*
        As names are used quite often there are two convenience functions 
        that wrap the code given above: setName and getName. They are declared
        in OSGSimpleAttachments.h.
    */
    if(getName(scene))
    {
        SLOG << "Node is named: " << getName(scene) << endLog;
    }
    else
    {
        SLOG << "Node is unnamed." << endLog;
    }
    
    setName(scene, "Scene");
    
    
    // use the NamedNodeFinder helper to find a named object

    NamedNodeFinder f;
    NodePtr found;
    
    found = f(scene, "Scene");  
    SLOG << "Found object " << found << " named Scene." << endLog;
    
    found = f(scene, "TF_DETAIL"); 
    if(found == NullFC)
    {
        SLOG << "Found no object named TF_DETAIL (did you load the tie?)." 
             << endLog;
    }
    else
    {
        SLOG << "Found object " << found << " named TF_DETAIL." << endLog;
    }
    
    
    
    // Use the simple attachment defined above
    
    MyAttachmentPtr mya = MyAttachment::create();
    
    beginEditCP(mya);
    {
        mya->getField().setValue(42);
    }
    endEditCP  (mya);
    
    // attach it to the scene
    beginEditCP(scene, Node::AttachmentsFieldMask);
    {
        scene->addAttachment(mya);
    }
    endEditCP  (scene, Node::AttachmentsFieldMask);

    // and check if it's still there       
    a = scene->findAttachment(MyAttachment::getClassType());
    
    if(a!=NullFC)
    {
        MyAttachmentPtr m = MyAttachmentPtr::dcast(a);
        
        SLOG << "Node my value: " << m->getField().getValue() << endLog;
    }
    else
    {
        SLOG << "Node has no myAttachment!" << endLog;
    }
    
    
    /*
       In case you don't want to create a new Attachment or cannot do that
       because it needs to reference external data, you just attach a void*
       using the VoidPAttachment. This is somewhat equivalent to the standard
       userdata pointer. 
       
       Note that the referenced data will not be threadsafe. Every thread has
       its own copy of the reference, but if if multiple threads reference the
       same data they can interfere. 
       
       Future versions will have some provisions to allow making this
       threadsafe. Stay tuned.
    */
    
    
    VoidPAttachmentPtr myvoid = VoidPAttachment::create();
    UInt32 dummy = 1234;
    
    beginEditCP(myvoid);
    {
        myvoid->getField().setValue(&dummy);
    }
    endEditCP  (myvoid);
    
    // attach it to the scene
    beginEditCP(scene, Node::AttachmentsFieldMask);
    {
        scene->addAttachment(myvoid);
    }
    endEditCP  (scene, Node::AttachmentsFieldMask);

    // and check if it's still there       
    a = scene->findAttachment(VoidPAttachment::getClassType());
    
    if(a!=NullFC)
    {
        VoidPAttachmentPtr m = VoidPAttachmentPtr::dcast(a);
        
        SLOG << "Node voidp value: " 
             << *(reinterpret_cast<UInt32 *>(m->getField().getValue()))
             << endLog;
    }
    else
    {
        SLOG << "Node has no voidp attachment!" << endLog;
    }
    
    
    // create the SimpleSceneManager helper
    mgr = new SimpleSceneManager;

    // tell the manager what to manage
    mgr->setWindow(gwin );
    mgr->setRoot  (scene);

    // show the whole scene
    mgr->showAll();

    // GLUT main loop
    glutMainLoop();

    return 0;
}

//
// GLUT callback functions
//

// redraw the window
void display(void)
{
    mgr->redraw();
}

// react to size changes
void reshape(int w, int h)
{
    mgr->resize(w, h);
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
void keyboard(unsigned char k, int x, int y)
{
    switch(k)
    {
        case 27:
        {
            osgExit();
            exit(1);
        }
    }
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    int winid = glutCreateWindow("OpenSG");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);

    return winid;
}
