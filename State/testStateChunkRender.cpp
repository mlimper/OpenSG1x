
#include <GL/glut.h>
#include <OSGLog.h>

#include <OSGWindow.h>
#include <OSGGLUTWindow.h>
#include <OSGDrawAction.h>

#include "OSGStateChunk.h"
#include "OSGTransformChunk.h"
#include "OSGMaterialChunk.h"
#include "OSGTextureChunk.h"
#include "OSGTextureTransformChunk.h"
#include "OSGBlendChunk.h"
#include "OSGPolygonChunk.h"
#include "OSGLineChunk.h"

OSG_USING_NAMESPACE

TransformChunkPtr tchunk1, tchunk2;
MaterialChunkPtr mchunk1, mchunk2;
TextureChunkPtr xchunk1;
BlendChunkPtr blchunk;
PolygonChunkPtr pchunk1,pchunk2;
TextureTransformChunkPtr txchunk;
LineChunkPtr lichunk1,lichunk2;

Image image;

GLint dlid, dlid2;

WindowPtr win;

DrawAction *dact;

void
display(void)
{
    float t = glutGet( GLUT_ELAPSED_TIME );

    win->frameInit();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    switch ( (int) ( t / 4000 ) % 3 )   
    {
    case 0:     glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); break;
    case 1:     glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); break;
    case 2:     glPolygonMode( GL_FRONT_AND_BACK, GL_POINT ); break;
    }

    Matrix m;
    Quaternion q;
    q.setValueAsAxisDeg( 0,1,0, -t/20 );
    m.setRotate( q );
    tchunk1->setMatrix( m );

    tchunk1->activate( dact );
    mchunk1->activate( dact );
    pchunk1->activate( dact );
    lichunk1->activate( dact );

    glCallList( dlid );

    m.setIdentity();
    m.setTranslate( cos(t/1000), 0, sin(t/1000) );
    tchunk2->setMatrix( m );


    pchunk2->changeFrom( dact, pchunk1.getCPtr() );
    tchunk2->changeFrom( dact, tchunk1.getCPtr() );
    mchunk2->changeFrom( dact, mchunk1.getCPtr() );
    lichunk2->changeFrom( dact, lichunk1.getCPtr() );
    blchunk->activate( dact );

    glCallList( dlid );

    tchunk2->deactivate( dact );
    mchunk2->deactivate( dact );
    pchunk2->deactivate( dact );
    lichunk2->deactivate( dact );
    blchunk->deactivate( dact );

    xchunk1->activate( dact );
    txchunk->activate( dact );

    glCallList( dlid2 );

    xchunk1->deactivate( dact );
    txchunk->deactivate( dact );

    win->frameExit();

    glutSwapBuffers();
}

void key(unsigned char key, int , int)
{
    switch ( key )
    {
    case 27:    exit(0);
    case 'a':   UChar8 imgdata[16];
                for ( int i = 0; i < 16; i++ )
                    imgdata[i] = (UChar8) rand();
                image.set( Image::OSG_RGBA_PF, 2, 2, 1, 1, 1, 0, imgdata );
                xchunk1->imageContentChanged();
                break;
    case 'b':   {
                UChar8 imgdata[16];
                for ( int i = 0; i < 16; i++ )
                    imgdata[i] = (UChar8) rand();
                image.set( Image::OSG_RGBA_PF, 2, 2, 1, 1, 1, 0, imgdata );
                beginEditCP( xchunk1 );
                xchunk1->setImage( &image );
                endEditCP( xchunk1 );
                }
                break;
    }
}

int main( int argc, char *argv[] )
{
    osgInit(argc, argv);

    glutInit(&argc, argv);
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow("OpenSG");
    glutKeyboardFunc(key);
    // glutReshapeFunc(resize);
    glutDisplayFunc(display);
    // glutMouseFunc(mouse);
    // glutMotionFunc(motion);

    glutIdleFunc(display);

    // create the dummy structures

    // the window is needed for the chunks that access GLObjects

    win = GLUTWindow::create();

    dact = DrawAction::create();
    dact->setWindow( win.getCPtr() );

    win->init();

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60, 1, 0.1, 10 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( 3, 3, 3,  0, 0, 0,   0, 1, 0 );

    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    dlid = glGenLists( 1 );
    glNewList( dlid, GL_COMPILE );
    glutSolidSphere( .8, 8, 8 );
    glEndList();

    dlid2 = glGenLists( 1 );
    glNewList( dlid2, GL_COMPILE );
    glBegin( GL_POLYGON );
    glNormal3f( 0, 1, 0 );
    glColor3f( 1, 1, 1 );
    glTexCoord2f( 0, 0 );
    glVertex3f( -1.5, -1, -1.5 );
    glTexCoord2f( 2, 0 );
    glVertex3f(  1.5, -1, -1.5 );
    glTexCoord2f( 2, 2 );
    glVertex3f(  1.5, -1,  1.5 );
    glTexCoord2f( 0, 2 );
    glVertex3f( -1.5, -1,  1.5 );
    glEnd();
    glEndList();


    Matrix m;

    tchunk1 = TransformChunk::create();
    m.setTranslate( 0, 1, 0 );
    tchunk1->setMatrix( m );

    tchunk2 = TransformChunk::create();
    tchunk2->setMatrix( Matrix::identity() );


    mchunk1 = MaterialChunk::create();
    mchunk1->setDiffuse( Color4f( 1,0,0,0 ) );
    mchunk1->setAmbient( Color4f( 1,0,0,0 ) );
    mchunk1->setShininess( 20 );

    mchunk2 = MaterialChunk::create();
    mchunk2->setDiffuse( Color4f( 0,1,0,0 ) );
    mchunk2->setAmbient( Color4f( 0,1,0,0 ) );
    mchunk2->setShininess( 50 );

    // Texture chunk

//  UChar8 imgdata[] =
//      {  255,0,0,0,  0,255,0,0,  0,0,255,255,  255,255,255,255 };
    UChar8 imgdata[] =
        {  255,0,0,  255,0,0,  255,0,255,
           255,0,0,  255,0,0,  255,0,255,
           255,255,0,  255,255,0,  255,255,255,
           255,255,0,  255,255,0,  255,255,255, };
//  UChar8 limgdata[] =
//      {  0, 128, 64, 255 };
    image.set( Image::OSG_RGB_PF, 2, 2, 2, 1, 1, 0, imgdata );

    if ( argc > 1 )
        image.read( argv[1] );

    xchunk1 = TextureChunk::create();
    beginEditCP(xchunk1);
    xchunk1->setImage( &image );
    xchunk1->setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
    xchunk1->setMagFilter( GL_NEAREST );
    xchunk1->setWrapS( GL_REPEAT );
    xchunk1->setWrapT( GL_REPEAT );
    xchunk1->setEnvMode( GL_REPLACE );
    xchunk1->setScale( false );
    endEditCP(xchunk1);

    xchunk1->imageContentChanged();

    beginEditCP(xchunk1);
    xchunk1->setImage( &image );
    endEditCP(xchunk1);

    // blend chunk

    blchunk = BlendChunk::create();
#ifndef WIN32
    blchunk->setSrcFactor( GL_CONSTANT_ALPHA );
    blchunk->setDestFactor( GL_ONE_MINUS_CONSTANT_ALPHA );
#endif
    blchunk->setColor( Color4f( 1,1,1,0.1 ) );

    // texture transform chunk

    txchunk = TextureTransformChunk::create();
    beginEditCP(txchunk);
    txchunk->setMatrix( Matrix(4,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1) );
    endEditCP(txchunk);

    // polygon chunk

    pchunk1 = PolygonChunk::create();
    {
    UInt32 stipple[32] = {
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff,
        0xffff0000, 0x0000ffff, 0xffff0000, 0x0000ffff
        };

    pchunk1->getMFStipple()->clear();
    for ( int i = 0; i < 32; i++ )
        pchunk1->getMFStipple()->push_back( stipple[i] );
    }

    pchunk2 = PolygonChunk::create();
    {
    UInt32 stipple[32] = {
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        };

    pchunk2->getMFStipple()->clear();
    for ( int i = 0; i < 32; i++ )
        pchunk2->getMFStipple()->push_back( stipple[i] );
    }


    lichunk1 = LineChunk::create();
    lichunk1->setSmooth(true);
    lichunk1->setStipplePattern(0xf0f0);

    lichunk2 = LineChunk::create();
    lichunk2->setStippleRepeat(5);
    lichunk2->setStipplePattern(0xaaaa);

    glutMainLoop();

    return 0;
}
