#include <OSGConfig.h>

#ifdef OSG_STREAM_IN_STD_NAMESPACE
#include <iostream>
#else
#include <iostream.h>
#endif

#include <OSGFieldContainerFactory.h>
#include <OSGSFSysTypes.h>
#include <OSGNode.h>
#include <OSGThread.h>
#include <OSGAttachment.h>
#include <OSGMFVecTypes.h>
#include <OSGBaseFunctions.h>
#include <OSGChangeList.h>
#include <OSGFieldContainer.h>
#include <OSGGroup.h>
#include <OSGFieldContainerPtr.h>


void test1(void)
{
    OSG::FieldContainerPtr pFC;

    fprintf(stderr, "Create\n");

    pFC = OSG::FieldContainerFactory::the()->createNode("Node");

    fprintf(stderr, "Add\n");
    OSG::addRefCP(pFC);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    fprintf(stderr, "Sub\n");
    OSG::subRefCP(pFC);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    OSG::NodePtr           p1  = OSG::Node::create();

    OSG::addRefCP(p1);

    OSG::NodePtr p2 = OSG::Node::create();
    OSG::NodePtr p3 = OSG::Node::create();

    OSG::NodePtr p4 = OSG::Node::create();
    OSG::NodePtr p5 = OSG::Node::create();
    OSG::NodePtr p6 = OSG::Node::create();
    OSG::NodePtr p7 = OSG::Node::create();

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->addChild(p2);
    p1->addChild(p3);

    p2->addChild(p4);
    p2->addChild(p5);

    p3->addChild(p6);
    p3->addChild(p7);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->dump(0);

    p1.dump(0, OSG::FCDumpFlags::RefCount);
    p2.dump(0, OSG::FCDumpFlags::RefCount);
    p3.dump(0, OSG::FCDumpFlags::RefCount);
    p4.dump(0, OSG::FCDumpFlags::RefCount);
    p5.dump(0, OSG::FCDumpFlags::RefCount);
    p6.dump(0, OSG::FCDumpFlags::RefCount);
    p7.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::subRefCP(p1);

    OSG::Thread::getCurrent()->getChangeList()->dump();
}

void test2(void)
{
    OSG::Thread::getCurrent()->getChangeList()->dump();

    OSG::NodePtr           p1  = OSG::Node::create();

    OSG::addRefCP(p1);

    OSG::GroupPtr pG  = OSG::Group::create();

    p1->setCore(pG);

    OSG::NodePtr  p2 = OSG::Node::create();
    
    pG = OSG::Group::create();
    p2->setCore(pG);

    OSG::NodePtr p3  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p3->setCore(pG);

    OSG::NodePtr p4  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p4->setCore(pG);

    OSG::NodePtr p5  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p5->setCore(pG);

    OSG::NodePtr p6  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p6->setCore(pG);

    OSG::NodePtr p7  = OSG::Node::create();

    pG = OSG::Group::create();
    p7->setCore(pG);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->addChild(p2);
    p1->addChild(p3);

    p2->addChild(p4);
    p2->addChild(p5);

    p3->addChild(p6);
    p3->addChild(p7);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->dump(0);

    p1.dump(0, OSG::FCDumpFlags::RefCount);
    p2.dump(0, OSG::FCDumpFlags::RefCount);
    p3.dump(0, OSG::FCDumpFlags::RefCount);
    p4.dump(0, OSG::FCDumpFlags::RefCount);
    p5.dump(0, OSG::FCDumpFlags::RefCount);
    p6.dump(0, OSG::FCDumpFlags::RefCount);
    p7.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::subRefCP(p1);

    OSG::Thread::getCurrent()->getChangeList()->dump();
}

void test3(void)
{
    OSG::Thread::getCurrent()->getChangeList()->dump();

    OSG::NodePtr           p1  = OSG::Node::create();

    OSG::addRefCP(p1);

    OSG::GroupPtr pG  = OSG::Group::create();

    p1->setCore(pG);

    OSG::NodePtr  p2 = OSG::Node::create();
    
    p2->setCore(pG);

    OSG::NodePtr p3  = OSG::Node::create();
    
    p3->setCore(pG);

    OSG::NodePtr p4  = OSG::Node::create();
    
    p4->setCore(pG);

    OSG::NodePtr p5  = OSG::Node::create();
    
    p5->setCore(pG);

    OSG::NodePtr p6  = OSG::Node::create();
    
    p6->setCore(pG);

    OSG::NodePtr p7  = OSG::Node::create();

    pG = OSG::Group::create();
    p7->setCore(pG);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->addChild(p2);
    p1->addChild(p3);

    p2->addChild(p4);
    p2->addChild(p5);

    p3->addChild(p6);
    p3->addChild(p7);

    OSG::addRefCP(p6);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->dump(0);

    p1.dump(0, OSG::FCDumpFlags::RefCount);
    p2.dump(0, OSG::FCDumpFlags::RefCount);
    p3.dump(0, OSG::FCDumpFlags::RefCount);
    p4.dump(0, OSG::FCDumpFlags::RefCount);
    p5.dump(0, OSG::FCDumpFlags::RefCount);
    p6.dump(0, OSG::FCDumpFlags::RefCount);
    p7.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::subRefCP(p1);

    p6->dump(0);

    OSG::subRefCP(p6);

    OSG::Thread::getCurrent()->getChangeList()->dump();
}

void test4(void)
{
    OSG::Thread::getCurrent()->getChangeList()->dump();

    OSG::NodePtr           p1  = OSG::Node::create();

    OSG::addRefCP(p1);

    OSG::GroupPtr pG  = OSG::Group::create();
    OSG::NamePtr n1 = OSG::Name::create();

    p1->setCore(pG);
    p1->addAttachment(n1);

    OSG::NodePtr p2 = OSG::Node::create();
    OSG::NamePtr n2 = OSG::Name::create();
    
    p2->setCore(pG);
    p2->addAttachment(n2);

    OSG::NodePtr p3  = OSG::Node::create();
    OSG::NamePtr n3 = OSG::Name::create();
    
    p3->setCore(pG);
    p3->addAttachment(n3);

    OSG::NodePtr p4  = OSG::Node::create();
    OSG::NamePtr n4 = OSG::Name::create();
    
    p4->setCore(pG);
    p4->addAttachment(n4);

    OSG::NodePtr p5  = OSG::Node::create();
    
    p5->setCore(pG);
    p5->addAttachment(n4);

    OSG::NodePtr p6  = OSG::Node::create();
    
    p6->setCore(pG);
    p6->addAttachment(n4);

    OSG::NodePtr p7  = OSG::Node::create();

    pG = OSG::Group::create();

    p7->setCore(pG);
    p7->addAttachment(n4);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->addChild(p2);
    p1->addChild(p3);

    p2->addChild(p4);
    p2->addChild(p5);

    p3->addChild(p6);
    p3->addChild(p7);

    OSG::addRefCP(p6);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->dump(0);

    p1.dump(0, OSG::FCDumpFlags::RefCount);
    p2.dump(0, OSG::FCDumpFlags::RefCount);
    p3.dump(0, OSG::FCDumpFlags::RefCount);
    p4.dump(0, OSG::FCDumpFlags::RefCount);
    p5.dump(0, OSG::FCDumpFlags::RefCount);
    p6.dump(0, OSG::FCDumpFlags::RefCount);
    p7.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::subRefCP(p1);

    p6->dump(0);

    OSG::subRefCP(p6);

    OSG::Thread::getCurrent()->getChangeList()->dump();
}


void test5(void)
{
    OSG::Thread::getCurrent()->getChangeList()->dump();

    OSG::NodePtr           p1  = OSG::Node::create();

    OSG::addRefCP(p1);

    OSG::GroupPtr pG  = OSG::Group::create();

    p1->setCore(pG);

    OSG::NodePtr  p2 = OSG::Node::create();
    
    pG = OSG::Group::create();
    p2->setCore(pG);

    OSG::NodePtr p3  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p3->setCore(pG);

    OSG::NodePtr p4  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p4->setCore(pG);

    OSG::NodePtr p5  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p5->setCore(pG);

    OSG::NodePtr p6  = OSG::Node::create();
    
    pG = OSG::Group::create();
    p6->setCore(pG);

    OSG::NodePtr p7  = OSG::Node::create();

    pG = OSG::Group::create();
    p7->setCore(pG);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->addChild(p2);
    p1->addChild(p3);

    p2->addChild(p4);
    p2->addChild(p5);

    p3->addChild(p6);
    p3->addChild(p7);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p1->dump(0);

    OSG::NodePtr  p8 = OSG::Node::create();
    
    pG = OSG::Group::create();
    p8->setCore(pG);

    OSG::NodePtr  p9 = OSG::Node::create();
    
    pG = OSG::Group::create();
    p9->setCore(pG);

    OSG::NodePtr  p9 = OSG::Node::create();
    
    pG = OSG::Group::create();
    p9->setCore(pG);

    p9->dump(0);

    p1->replaceChildBy(p2, p9);

    p1->dump(0);
    p9->dump(0);

    OSG::subRefCP(p1);

    OSG::Thread::getCurrent()->getChangeList()->dump();
}

int main (int argc, char **argv)
{
    OSG::osgInit(argc, argv);

//    test1();

//    test2();

//    test3();

    test4();

/*
    OSG::Thread::getCurrent()->getChangeList()->dump();
    OSG::Thread::getCurrent()->getChangeList()->clearAll();
    OSG::Thread::getCurrent()->getChangeList()->dump();

    OSG::PLOG << "Num types : "
              << OSG::FieldContainerFactory::the()->getNumTypes()
              << endl;

    OSG::FieldContainerPtr pFC;

    pFC.dump(0, OSG::FCDumpFlags::RefCount);

    pFC = OSG::FieldContainerFactory::the()->createNode("Node");

    pFC.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::addRefCP(pFC);

    pFC.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    pFC->dump();

    OSG::subRefCP(pFC);

    pFC.dump(0, OSG::FCDumpFlags::RefCount);

    OSG::Thread::getCurrent()->getChangeList()->dump();
    OSG::Thread::getCurrent()->getChangeList()->clearAll();

    OSG::NodePtr           p1  = OSG::Node::create();
    OSG::FieldContainerPtr pb1 = p1;

    OSG::addRefCP(p1);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    pb1.dump(0, OSG::FCDumpFlags::RefCount);

    cerr << " N : " << p1->getType().getId()              << endl;
    cerr << " N : " << OSG::Node::getClassType().getId() << endl;

    OSG::NodePtr p2 = OSG::Node::create();
    OSG::NodePtr p3 = OSG::Node::create();
    OSG::NodePtr p4 = OSG::Node::create();
    OSG::NodePtr p5 = OSG::Node::create();
    OSG::NodePtr p6 = OSG::Node::create();

    p1->dump (0, OSG::FCDumpFlags::RefCount);
    p2->dump (0, OSG::FCDumpFlags::RefCount);
    p3->dump (0, OSG::FCDumpFlags::RefCount);
    p4->dump (0, OSG::FCDumpFlags::RefCount);
    p5->dump (0, OSG::FCDumpFlags::RefCount);
    p6->dump (0, OSG::FCDumpFlags::RefCount);

    OSG::addRefCP(p6);

    OSG::beginEditCP(p1);

    cerr << "add p2" << endl;

    p1->addChild(p2);
    p1->dump (0, OSG::FCDumpFlags::RefCount);
    p1.dump(0, OSG::FCDumpFlags::RefCount);

    cerr << "add p3" << endl;

    OSG::addRefCP(p3);
    p1->addChild(p3);
    p1->dump (0, OSG::FCDumpFlags::RefCount);
    p1.dump  (0, OSG::FCDumpFlags::RefCount);
    p3.dump  (0, OSG::FCDumpFlags::RefCount);

    cerr << "insert p4" << endl;

    p1->insertChild(1, p4);
    p1->dump (0, OSG::FCDumpFlags::RefCount);
    p1.dump  (0, OSG::FCDumpFlags::RefCount);

    cerr << "replace p3 -> p5" << endl;

    p1->replaceChild(2, p5);
    p1->dump (0, OSG::FCDumpFlags::RefCount);
    p1.dump  (0, OSG::FCDumpFlags::RefCount);

    OSG::Thread::getCurrent()->getChangeList()->dump();

    p3.dump (0, OSG::FCDumpFlags::RefCount);
    p3->dump(0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);

    cerr << "replace p5 -> p3" << endl;

    p1->replaceChildBy(p5, p3);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);

    cerr << "sub p2" << endl;

    p2.dump (0, OSG::FCDumpFlags::RefCount);
    p1->subChild(p2);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);

    cerr << "sub p6" << endl;

    p1->subChild(p6);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);

    cerr << "sub child 1" << endl;

    p1->subChild(1);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);

    cerr << "sub child 0" << endl;

    p1->subChild(0);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);

    OSG::endEditCP(p1);



    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p2.dump (0, OSG::FCDumpFlags::RefCount);
    p3.dump (0, OSG::FCDumpFlags::RefCount);
    p4.dump (0, OSG::FCDumpFlags::RefCount);
    p5.dump (0, OSG::FCDumpFlags::RefCount);
    p6.dump (0, OSG::FCDumpFlags::RefCount);

    cerr << endl << endl << "Core test" << endl;

    p1.dump (0, OSG::FCDumpFlags::RefCount);

    OSG::GroupPtr g1 = OSG::Group::create();

    p1->setCore(g1);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    g1.dump (0, OSG::FCDumpFlags::RefCount);

    p1->setCore(OSG::NullFC);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    g1.dump (0, OSG::FCDumpFlags::RefCount);

    g1 = OSG::Group::create();

    p1->setCore(g1);
    g1.dump (0, OSG::FCDumpFlags::RefCount);

    p2 = OSG::NodePtr::dcast(p1->shallowCopy());
//    OSG::addRefCP(p2);

    g1.dump (0, OSG::FCDumpFlags::RefCount);

    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p2->dump(0, OSG::FCDumpFlags::RefCount);

    p1->setCore(g1);
    g1.dump (0, OSG::FCDumpFlags::RefCount);
    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p2->dump(0, OSG::FCDumpFlags::RefCount);

    cerr << endl << endl << "Indirect Del" << endl;

    p2->setCore(OSG::NullFC);

    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p2.dump(0, OSG::FCDumpFlags::RefCount);


    cerr << endl << endl << "Attachment test" << endl;

    OSG::NamePtr n1 = OSG::Name::create();
    OSG::NamePtr n2 = OSG::Name::create();

    p1->addAttachment(n1);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    n1.dump (0, OSG::FCDumpFlags::RefCount);

    p1->subAttachment(n1);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    n1.dump (0, OSG::FCDumpFlags::RefCount);

    n1 = OSG::Name::create();

    p1->addAttachment(n1);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    n1.dump (0, OSG::FCDumpFlags::RefCount);

    p1->addAttachment(n2);

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p1->dump(0, OSG::FCDumpFlags::RefCount);
    n1.dump (0, OSG::FCDumpFlags::RefCount);
    n2.dump (0, OSG::FCDumpFlags::RefCount);

    p2 = OSG::NodePtr::dcast(p1->shallowCopy());

    p1.dump (0, OSG::FCDumpFlags::RefCount);
    p2.dump (0, OSG::FCDumpFlags::RefCount);

    p1->dump(0, OSG::FCDumpFlags::RefCount);
    p2->dump(0, OSG::FCDumpFlags::RefCount);




    OSG::subRefCP(p3);
    OSG::subRefCP(p1);
*/

    OSG::osgExit();
}

#if 0
using namespace OSG;

void x(Node *xa)
{
    fprintf(stderr, "x : \n");
    xa->print();
}

void y(const Node *xa)
{
    fprintf(stderr, "y : \n");
    xa->print();
}

void initGeos(void)
{
    fprintf(stderr, "Initialize Geo\n");
}

struct SimpleAttDesc
{
    typedef MFVec3f FieldTypeT;

    static const char *getTypeName (void) { return "GeoPoints"; }
    static const char *getFieldName(void) { return "points"; }
    static const char *getGroupName(void) { return "points"; }
    static const char *getClassName(void) { return "GeoPointsAttachment"; }
    static InitContainerF getInitMethod(void) { return initGeos;}
};

typedef SimpleAttachment<SimpleAttDesc> GeoPoints;

template class SimpleAttachment<SimpleAttDesc>;

typedef FCPtr<AttachmentPtr,  GeoPoints> GeoPointsPtr;


#if 0
class NewNode;
typedef FCPtr<NodePtr,  NewNode> NewNodePtr;

class NewNode : public Node
{
  public :

    static FieldContainerType _type;

    static NewNodePtr create(void)
        {
            NewNodePtr fc;

#ifdef OSG_HAS_TEMPLATE_RETURNVALUES
            if(_type.getPrototype() != NullFC)
                fc = _type.getPrototype()->clone().dcast<NewNodePtr>();
#else
            if(_type.getPrototype() != NULL)
                _type.getPrototype()->clone().dcast(fc);
#endif

            return fc;
        }

    static NewNodePtr createEmpty(void)
        {
            NewNodePtr returnValue;

            newPtr(returnValue);

            return returnValue;
        }

    NewNode(void) {} ;
    ~NewNode(void) {};

    const FieldContainerType &getType(void) const { return _type; }
          FieldContainerType &getType(void)       { return _type; }

    virtual FieldContainerPtr clone(void) const
    {
            NewNodePtr returnValue;

            newPtr(returnValue, this);

            return returnValue;
    };

};

FieldContainerType NewNode::_type("NewNode",
                                        "Node",
                                        0,
                                        (PrototypeCreateF) &NewNode::createEmpty,
                                        0,
                                        NULL,
                                        0);

#endif

struct DynDesc
{
    typedef Attachment    Parent;
    typedef AttachmentPtr ParentPtr;

    static const char *getTypeName      (void) { return "DynDesc";    }
    static const char *getParentTypeName(void) { return "Attachment"; }
    static const char *getGroupName     (void) { return "Test";       }
};

int main (int argc, char **argv)
{

    osgInit(argc, argv);

//  Field *pI;

    cout << "TypeCounter: "
         << " | " << sizeof(FieldContainerPtr) << endl;


    FieldContainerPtr qFC = NullFC;

//  fprintf( stderr,"stderr output\n");
//    pFC.dump();

//    NullFC.dump();

//  cout << "Stream output" << endl;

//  cout << "pFC: " ;
//  cout << pFC << endl;

//  cout << "NullFC: " << qFC << endl;


    if(pFC != NullFC)
    {
//        pFC->getType().print();

//        cerr << pFC.dcast<NodePtr>()->getMFChildren() << endl;

//        pI = pFC->getField("children");

//        cerr << pI << endl;

//        cerr << pFC.dcast<NodePtr>()->getSFName() << endl;

//        pI = pFC->getField("name");

//        cerr << pI << endl;
    }

//    cerr << endl << endl;


//    pb1->getType().print();




    Thread::getCurrentChangeList()->dump();


//    p1.dump();
//    p2.dump();

    cerr << endl << "Attachment test" << endl;

    GeoPointsPtr pPoint;

    cerr << "GeoPoints::getClassname()=" << GeoPoints::getClassname() << endl;
    cerr << "GeoPointsPtr::getClassname()=" << GeoPointsPtr::getClassname() << endl;
    cerr << "pPoint.getClassname()=" << pPoint.getClassname() << endl;
    cerr << "(*pPoint).getClassname()=" << (*pPoint).getClassname() << endl;

    pPoint = GeoPoints::create();

    pPoint->getType().print();

//    p1.dump();
    p1->addAttachment(pPoint);
    p1->print(0);

    p1->subAttachment(pPoint);
    p1->print(0);
    pPoint->print(0);

//    NewNode *pNewProto = NewNode::getStaticType().createPrototype();

    cerr << endl << "Proto test\n" << endl;

//    NewNodePtr newNodeP = NewNode::create();
    NodePtr    nodeP    = Node::create();

    nodeP->getType().print();
//    newNodeP->getType().print();

//    NewNode *np = new NewNode();
//    Node *np = &(*nodeP);

//    Node::getStaticType().setPrototype(newNodeP);

    NodePtr    nodeP1    = Node::create();
    nodeP1->getType().print();

    if(! NullFC)
    {
        fprintf(stderr, "NN\n");
    }
    {
        fprintf(stderr, "IN\n");
    }

    if(! nodeP1)
    {
        fprintf(stderr, "PNN\n");
    }
    {
        fprintf(stderr, "PIN\n");
    }

    nodeP1 = NullNode;

    nodeP1.dump();

    typedef DynFieldAttachment<DynDesc> DynAtt;
    typedef FCPtr<AttachmentPtr, DynAtt> DynAttPtr;

    DynAttPtr da;

    da = DynAtt::create();

    da->getType().print();

    fprintf(stderr, "%x\n", da->getType().getFieldDescription(1));

    FieldDescription *pDesc;

    pDesc = new FieldDescription(
        SFString::getClassType(),
        "name",
        0, 0,
        false,
        (FieldIndexAccessMethod) &DynAtt::getDynamicField);

    UInt32 uiFI1 = da->addField(*pDesc);

    fprintf(stderr, "%d\n", uiFI1);



    pDesc = new FieldDescription(
        SFString::getClassType(),
        "name1",
        0, 0,
        false,
        (FieldIndexAccessMethod) &DynAtt::getDynamicField);

    UInt32 uiFI2 = da->addField(*pDesc);

    fprintf(stderr, "%d\n", uiFI2);

    da->getType().print();
    da->dump();

    da->subField(uiFI1);

    da->getType().print();

    da->dump();

    fprintf(stderr, "Retrieve Field 1 %d\n", da->getField(1));
    fprintf(stderr, "Retrieve Field 2 %d\n", da->getField(2));
    fprintf(stderr, "Retrieve Field 3 %d\n", da->getField(3));

    pDesc = new FieldDescription(
        SFString::getClassType(),
        "name2",
        0, 0,
        false,
        (FieldIndexAccessMethod) &DynAtt::getDynamicField);

    UInt32 uiFI3 = da->addField(*pDesc);

    fprintf(stderr, "%d\n", uiFI3);

    da->dump();

    da->getType().print();


    fprintf(stderr, "Retrieve Field 1 %d\n", da->getField(1));
    fprintf(stderr, "Retrieve Field 2 %d\n", da->getField(2));
    fprintf(stderr, "Retrieve Field 3 %d\n", da->getField(3));

    delete pDesc;

    return 0;
}

#endif
