// Some needed include files
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGSimpleTexturedMaterial.h>
#include <OpenSG/OSGSolidBackground.h>
#include <OpenSG/OSGTextPixmapFace.h>
#include <OpenSG/OSGTextLayoutParam.h>
#include <OpenSG/OSGTextLayoutResult.h>
#include <iostream>

// In most cases it is useful to add this line, else every OpenSG command
// must be preceeded by an extra OSG::
OSG_USING_NAMESPACE
using namespace std;

// The SimpleSceneManager is a little usefull class which helps us to
// manage little scenes. It will be discussed in detail later on
SimpleSceneManager *mgr;

// We have a forward declaration here, just to sort the code
int setupGLUT(int *argc, char *argv[]);

int main(int argc, char *argv[])
{
    // Init the OpenSG subsystem
    osgInit(argc, argv);

    // We create a GLUT Window (that is almost the same for most applications)
    int winid = setupGLUT(&argc, argv);
    GLUTWindowPtr gwin = GLUTWindow::create();
    gwin->setId(winid);
    gwin->init();

    // Create the face
    string family = "SANS";
    TextFace::Style style = TextFace::STYLE_PLAIN;
    UInt32 size = 32;
    TextPixmapFace *face = TextPixmapFace::create(family, style, size);
    if (face == 0)
    {
        cerr << "ERROR: Cannot create face object!" << endl;
        return -1;
    }

    // Do not forget to increment the reference counter!
    addRefP(face);

    // Lay out one single line of text
    string text = "Hello World!"; // Use UTF-8 encoding!
    TextLayoutParam layoutParam;
    layoutParam.horizontal = true;
    layoutParam.leftToRight = true;
    layoutParam.topToBottom = true;
    layoutParam.majorAlignment = TextLayoutParam::ALIGN_FIRST;
    layoutParam.minorAlignment = TextLayoutParam::ALIGN_FIRST;
    layoutParam.spacing = 1.f;
    layoutParam.length.push_back(0.f);
    layoutParam.maxExtend = 0.f;
    TextLayoutResult layoutResult;
    face->layout(text, layoutParam, layoutResult);

    // Render the text into an OpenSG image
    Vec2f offset;
    UInt32 border = 1;
    ImagePtr imagePtr = face->makeImage(layoutResult, offset, border);

    // We do not need the face any more, so we decrement the
    // reference counter to release it. Do not use the face
    // object beyond this point!
    subRefP(face);

    // Create the geometry which we will assign the texture to
    Real32 width = imagePtr->getWidth();
    Real32 height = imagePtr->getHeight();
    NodePtr plane = makePlane(width, height, 1, 1);

    // Create the texture that will hold the image
    SimpleTexturedMaterialPtr tex = SimpleTexturedMaterial::create();
    beginEditCP(tex);
        tex->setImage(imagePtr);
        tex->setEnvMode(GL_MODULATE);
        tex->setDiffuse(Color3f(1, 0, 0));
    endEditCP(tex);

    // Assign the texture to the geometry
    GeometryPtr geo = GeometryPtr::dcast(plane->getCore());
    beginEditCP(geo, Geometry::MaterialFieldMask);
        geo->setMaterial(tex);
    endEditCP(geo, Geometry::MaterialFieldMask);

    // Transform the geometry so that the origin of the text is at
    // the origin of the world coordinate system
    NodePtr scene = Node::create();
    TransformPtr transformPtr = Transform::create();
    Matrix m;
    m.setTranslate(offset.x() + width / 2, offset.y() - height / 2, 0);
    beginEditCP(transformPtr);
        transformPtr->setMatrix(m);
    endEditCP(transformPtr);
    beginEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);
        scene->setCore(transformPtr);
        scene->addChild(plane);
    endEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);

    // Create and setup the SSM
    mgr = new SimpleSceneManager;
    mgr->setWindow(gwin);
    mgr->setRoot(scene);

    // Create a blue background
    SolidBackgroundPtr bg = SolidBackground::create();
    beginEditCP(bg);
        bg->setColor(Color3f(0.1, 0.1, 0.5));
    endEditCP(bg);
    beginEditCP(gwin->getPort(0));
        gwin->getPort(0)->setBackground(bg);
    endEditCP(gwin->getPort(0));

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
void display()
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

// The GLUT subsystem is set up here. This is very similar to other GLUT
// applications If you have worked with GLUT before, you may have the feeling
// of meeting old friends again, if you have not used GLUT before that is no
// problem. GLUT will be introduced briefly in the next section.

int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    int winid = glutCreateWindow("OpenSG Pixmap Text Example");

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);

    return winid;
}
