// all needed include files
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGGeometry.h>

OSG_USING_NAMESPACE

// this will specify the resolution of the mesh
#define N   100

//the two dimensional array that will store all height values
Real32 wMesh[N][N];

//the origin of the water mesh
Pnt3f wOrigin = Pnt3f(0,0,0);

//width and length of the mesh
UInt16 width = 100;
UInt16 length = 100;

SimpleSceneManager *mgr;
NodePtr scene;

int setupGLUT( int *argc, char *argv[] );

NodePtr createScenegraph(){
    // the scene must be created here
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            wMesh[i][j] = 0;
            
    // GeoPTypes will define the types of primitives to be used
    GeoPTypesPtr type = GeoPTypesUI8::create();
    beginEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
	// we want to use quads ONLY 
	type->addValue(GL_QUADS);
    endEditCP(type, GeoPTypesUI8::GeoPropDataFieldMask);
    
    // GeoPLength will define the number of vertices of
    // the used primitives
    GeoPLengthsPtr length = GeoPLengthsUI32::create();
    beginEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);
	// the length of our quads is four ;-)
	length->addValue(4);
    endEditCP(length, GeoPLengthsUI32::GeoPropDataFieldMask);

    // GeoPositions3f stores the positions of all vertices used in 
    // this specific geometry core
    GeoPositions3fPtr pos = GeoPositions3f::create();
    beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
	// here they all come
	for (int x = 0; x < N; x++)
            for (int z = 0; z < N; z++)
		pos->addValue(Pnt3f(x, wMesh[x][z], z));
    endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);

    //GeoColors3f stores all color values that will be used
    GeoColors3fPtr colors = GeoColors3f::create();
    beginEditCP(colors, GeoColors3f::GeoPropDataFieldMask);
        for (int x = 0; x < N; x++)
            for (int z = 0; z < N; z++)
		colors->addValue(Color3f(0,0,1));
    endEditCP(colors, GeoColors3f::GeoPropDataFieldMask);
    
    GeoNormals3fPtr norms = GeoNormals3f::create();
    beginEditCP(norms, GeoNormals3f::GeoPropDataFieldMask);
        for (int x = 0; x < N; x++)
            for (int z = 0; z < N; z++)
		// As initially all heights are set to zero thus yielding a plane,
		// we set all normals to (0,1,0) parallel to the y-axis
		norms->addValue(Vec3f(0,1,0));
    endEditCP(norms, GeoNormals3f::GeoPropDataFieldMask);
    
    SimpleMaterialPtr mat = SimpleMaterial::create();
    
    // GeoIndicesUI32 points to all relevant data used by the 
    // provided primitives
    GeoIndicesUI32Ptr indices = GeoIndicesUI32::create();
    beginEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);
        for (int x = 0; x < N-1; x++)
            for (int z = 0; z < N-1; z++){
		// points to four vertices that will
		// define a single quad
		indices->addValue(z*N+x);	
		indices->addValue((z+1)*N+x);
		indices->addValue((z+1)*N+x+1);
		indices->addValue(z*N+x+1);
            }
    endEditCP(indices, GeoIndicesUI32::GeoPropDataFieldMask);

        GeometryPtr geo = Geometry::create();
    beginEditCP(geo,
        Geometry::TypesFieldMask	|
	Geometry::LengthsFieldMask	|
	Geometry::IndicesFieldMask	|
	Geometry::PositionsFieldMask	|
	Geometry::NormalsFieldMask      |
        Geometry::MaterialFieldMask     |
	Geometry::ColorsFieldMask	
	);
	
	geo->setTypes(type);
	geo->setLengths(length);
	geo->setIndices(indices);
	geo->setPositions(pos);
	geo->setNormals(norms);
        geo->setMaterial(mat);
	geo->setColors(colors);

    endEditCP(geo,
        Geometry::TypesFieldMask	|
	Geometry::LengthsFieldMask	|
	Geometry::IndicesFieldMask	|
	Geometry::PositionsFieldMask	|
	Geometry::NormalsFieldMask	|
        Geometry::MaterialFieldMask     |
	Geometry::ColorsFieldMask	
	);
        
    NodePtr root = Node::create();
    beginEditCP(root);
        root->setCore(geo);
    endEditCP(root);
	
    return root;
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
    
    Navigator * nav = mgr->getNavigator();
    nav->setFrom(nav->getFrom()+Vec3f(0,2,0));
    
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
