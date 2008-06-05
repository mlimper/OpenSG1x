/*---------------------------------------------------------------------------*\
*                                OpenSG                                     *
*                                                                           *
*                                                                           *
*             Copyright (C) 2000-2002 by the OpenSG Forum                   *
*                                                                           *
*                            www.opensg.org                                 *
*                                                                           *
*   contact: dirk@opensg.org, gerrit.voss@vossg.org, jbehr@zgdv.de          *
*                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
*                                License                                    *
*                                                                           *
* This library is free software; you can redistribute it and/or modify it   *
* under the terms of the GNU Library General Public License as published    *
* by the Free Software Foundation, version 2.                               *
*                                                                           *
* This library is distributed in the hope that it will be useful, but       *
* WITHOUT ANY WARRANTY; without even the implied warranty of                *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
* Library General Public License for more details.                          *
*                                                                           *
* You should have received a copy of the GNU Library General Public         *
* License along with this library; if not, write to the Free Software       *
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
*                                                                           *
\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
*                                Changes                                    *
*                                                                           *
*                                                                           *
*                                                                           *
*                                                                           *
*                                                                           *
*                                                                           *
\*---------------------------------------------------------------------------*/

/***************************************************************************\
*                             Includes                                    *
\***************************************************************************/

#include "OSGSharePtrGraphOp.h"

#include <functional>
#include <algorithm>

#include <OSGLog.h>
#include <OSGGeometry.h>
#include <OSGGeoFunctions.h>
#include <OSGVector.h>

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::UnifyVerticesGraphOp
    \ingroup GrpSystemNodeCoresDrawablesGeometry
    
A class used to create indexed geometries.

*/

/***************************************************************************\
 *                           Class variables                               *
\***************************************************************************/

std::set<FieldContainerPtr> SharePtrGraphOp::_added_cores;

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*------------- constructors & destructors --------------------------------*/

SharePtrGraphOp::SharePtrGraphOp(const char* name): 
    GraphOp(name),
    _fctypes(),
    _includes(),
    _excludes(),
    _share_counter(0)
{
}

SharePtrGraphOp::~SharePtrGraphOp(void)
{
}

GraphOp *SharePtrGraphOp::create()
{
    return new SharePtrGraphOp();
}

bool SharePtrGraphOp::traverse(NodePtr& root)
{
    if(!_includes.empty() && !_excludes.empty())
    {
        FFATAL(("SharePtrGraphOp : Setting both the includes and excludes"
                "is not allowed!\n"));
        return false;
    }
    
    _fctypes.clear();
    _share_counter = 0;

    bool result = true;
    compareFCs(root);
    // HACK but doing it directly in compareFCs() leads to crash while destroying the
    // scenegraph and this only with one special geometry file ...
    fillAttachmentParents(root);

    FINFO(("Shared %u ptrs with types:\n", _share_counter));

    for(fcsMap::iterator i = _fctypes.begin();i != _fctypes.end();++i)
        FINFO(("\t'%s'\n", (*i).first.c_str()));

    return result;
}

void SharePtrGraphOp::setParams(const std::string params)
{
    ParamSet ps(params);   
    std::string incl, excl;
    
    ps("includes",  incl);
    ps("excludes",  excl);
    
    if(incl.length() && excl.length())
    {
        FWARNING(("SharePtrGraphOp: can't set includes and excludes\n"));
    }
    else
    {
        setIncludes(incl);
        setExcludes(excl);
    }
    
    std::string out = ps.getUnusedParams();
    if(out.length())
    {
        FWARNING(("SharePtrGraphOp doesn't have parameters '%s'.\n",
                out.c_str()));
    }
}

std::string SharePtrGraphOp::usage(void)
{
    return 
    "SharePtr: try to share FieldContainers in a subtree\n"
    "  Compares the values of all referenced FieldContainers in the\n"
    "  subtree and shares them if possible. Primarily useful for\n"
    "  Geometries and Materials, but works for everything.\n"
    "Params: name (type, default)\n"
    "  includes (string, ""): comma-separated list of types to share\n"
    "  excludes (string, ""): comma-separated list of types not to share\n"
    "  (Only one of the two can be used, not both at the same time)\n"
    ;
}

void SharePtrGraphOp::setIncludes(const std::string &includes)
{
    _includes.clear();
    // parse comma separated names.
    std::string::const_iterator nextComma;
    std::string::const_iterator curPos = includes.begin();
    while(curPos < includes.end())
    {
        nextComma = std::find(curPos, includes.end(), ',');
        // strip leading spaces
        curPos = std::find_if(curPos, nextComma,
                              std::not1(std::ptr_fun(isspace)));
        _includes.push_back(std::string(curPos, nextComma));
        if(nextComma != includes.end())
            ++nextComma;
        curPos = nextComma;
    }
}

void SharePtrGraphOp::setExcludes(const std::string &excludes)
{
    _excludes.clear();
    // parse comma separated names.
    std::string::const_iterator nextComma;
    std::string::const_iterator curPos = excludes.begin();
    while(curPos < excludes.end())
    {
        nextComma = std::find(curPos, excludes.end(), ',');
        // strip leading spaces
        curPos = std::find_if(curPos, nextComma,
                              std::not1(std::ptr_fun(isspace)));
        _excludes.push_back(std::string(curPos, nextComma));
        if(nextComma != excludes.end())
            ++nextComma;
        curPos = nextComma;
    }
}

Action::ResultE SharePtrGraphOp::traverseEnter(NodePtr &/*node*/)
{
    return Action::Continue;
}

Action::ResultE SharePtrGraphOp::traverseLeave(NodePtr &/*node*/,
                                               Action::ResultE /*res*/)
{
    return Action::Continue;
}

/*-------------------------------------------------------------------------*\
 -  private                                                                -
\*-------------------------------------------------------------------------*/

bool SharePtrGraphOp::isInList(const std::vector<std::string> &tlist,
                               const FieldContainerPtr &fc)
{
    for(UInt32 k=0;k<tlist.size();++k)
    {
        FieldContainerType *fct = FieldContainerFactory::the()
                                        ->findType(tlist[k].c_str());
        if(fct == NULL)
        {
            FWARNING(("SharePtrGraphOp: Unknown type '%s'!\n", tlist[k].c_str()));
            break;
        }

        if(fc->getType().isDerivedFrom(*fct))
            return true;
    }

    return false;
}

FieldContainerPtr SharePtrGraphOp::compareFCs(const FieldContainerPtr &fc)
{
    if(fc == NullFC)
        return fc;

    const FieldContainerType &type = fc->getType();
    UInt32 fcount = type.getNumFieldDescs();

    for(UInt32 i=1;i <= fcount;++i)
    {
        const FieldDescription* fdesc = type.getFieldDescription(i);
    
        if(fdesc->isInternal())
            continue;

        // ignore parents field
        if(strcmp(fdesc->getCName(), "parents") == 0)
            continue;

        // ignore attachments
        if(strcmp(fdesc->getCName(), "attachments") == 0)
            continue;

        BitVector mask = fdesc->getFieldMask();

        Field *fc_field = fc->getField(i);
        const FieldType &ftype = fc_field->getType();
        std::string fieldType = ftype.getName().str();
        
        // field
        if(strstr(ftype.getCName(), "Ptr") != NULL)
        {
            if(fc_field->getCardinality() == FieldType::SINGLE_FIELD)
            {
                FieldContainerPtr ffc = 
                    static_cast<SFFieldContainerPtr *>(fc_field)->getValue();
                
                FieldContainerPtr nffc = compareFCs(ffc);
                
                if(nffc != ffc)
                {
                    addRefCP(nffc);
                    beginEditCP(fc, mask);
                    static_cast<SFFieldContainerPtr *>(fc_field)->setValue(nffc);
                    endEditCP(fc, mask);
#if 0
                    // for attachments we need to update the parents field!
                    AttachmentPtr attachment = AttachmentPtr::dcast(nffc);
                    if(attachment != NullFC)
                    {
                        AttachmentPtr attorg = AttachmentPtr::dcast(ffc);
                        beginEditCP(attorg, Attachment::ParentsFieldMask);
                            attorg->subParent(fc);
                        endEditCP(attorg, Attachment::ParentsFieldMask);

                        FieldContainerPtr fcb = fc;
                        fcb.setParentFieldPos(fdesc->getFieldId());
                        beginEditCP(attachment, Attachment::ParentsFieldMask);
                            attachment->addParent(fc);
                            //attachment->getParents().clear();
                        endEditCP(attachment, Attachment::ParentsFieldMask);
                    }
#endif
                    subRefCP(ffc);
                }
            }
            else if(fc_field->getCardinality() == FieldType::MULTI_FIELD)
            {
                beginEditCP(fc, mask);
                for(UInt32 j=0;j < static_cast<MFFieldContainerPtr*>(fc_field)->size();++j)
                {
                    FieldContainerPtr ffc = (*(static_cast<MFFieldContainerPtr *>(fc_field)))[j];
                    
                    FieldContainerPtr nffc = compareFCs(ffc);
                    
                    if(nffc != ffc)
                    {
                        addRefCP(nffc);
                        (*(static_cast<MFFieldContainerPtr *>(fc_field)))[j] = nffc;
#if 0
                        // for attachments we need to update the parents field!
                        AttachmentPtr attachment = AttachmentPtr::dcast(nffc);
                        if(attachment != NullFC)
                        {
                            AttachmentPtr attorg = AttachmentPtr::dcast(ffc);
                            beginEditCP(attorg, Attachment::ParentsFieldMask);
                                attorg->subParent(fc);
                            endEditCP(attorg, Attachment::ParentsFieldMask);

                            FieldContainerPtr fcb = fc;
                            fcb.setParentFieldPos(fdesc->getFieldId());
                            beginEditCP(attachment, Attachment::ParentsFieldMask);
                                attachment->addParent(fc);
                            endEditCP(attachment, Attachment::ParentsFieldMask);
                        }
#endif
                        subRefCP(ffc);
                    }
                }
                endEditCP(fc, mask);
            }
        }
    }

    std::string typestr = fc->getType().getCName();
    
    if(!_includes.empty())
    {
        if(!isInList(_includes, fc))
            return fc;
    }
    
    if(!_excludes.empty())
    {
        if(isInList(_excludes, fc))
            return fc;
    }

    fcsMap::iterator fi = _fctypes.find(typestr);
    
    if(fi == _fctypes.end())
    {
        fcsSet fcs;
        _fctypes[typestr] = fcs;
        fi = _fctypes.find(typestr);
    }

    fcsSet &fcs = (*fi).second;
    
    // already shared?
    if(fcs.count(fc))
        return fc;
    
    for(fcsSet::iterator i=fcs.begin();i!=fcs.end();++i)
    {
        if(isEqual(*i, fc))
        {
            if(*i != fc)
            {
                ++_share_counter;
                FDEBUG(("Shared %u ptrs.\r", _share_counter));
            }
            return *i;
        }
    }
    
    fcs.insert(fc);

    return fc;
}


template <typename T>
static bool compareMField(Field *a, Field *b)
{
    MField<T> *mfa = dynamic_cast<MField<T> *>(a);
    MField<T> *mfb = dynamic_cast<MField<T> *>(b);

    if(mfa == NULL && mfb == NULL)
        return true;

    if(mfa == NULL || mfb == NULL)
        return false;
    
    if(mfa->size() != mfb->size())
        return false;

    for(UInt32 j=0;j<mfa->size();++j)
    {
        if((*mfa)[j] != (*mfb)[j])
        {
            return false;
        }
    }

    return true;
}


/*!
 * \brief Compares to field containers
 * \param field container a
 * \param field container b
 * \return true if equal.
 */
bool SharePtrGraphOp::isEqual(const osg::FieldContainerPtr &a,
                              const osg::FieldContainerPtr &b)
{
    // Compare the pointers.
    if(a == b)
        return true;

    if(a == NullFC || b == NullFC)
        return false;

    if(a->getType() != b->getType())
        return false;
    
    //printf("comparing: %s\n", a->getType().getName().str());
    
    const FieldContainerType &type = a->getType();
    //UInt32 fcount = type.getNumFieldDescs();
    // ignore dynamic fields.
    UInt32 fcount = osgMin(type.getNumFieldDescs(), b->getType().getNumFieldDescs());
    
    for(UInt32 i=1;i <= fcount;++i)
    {
        const FieldDescription* fdesc = type.getFieldDescription(i);
    
        if(fdesc->isInternal())
            continue;

        // ignore parents field
        if(strcmp(fdesc->getCName(), "parents") == 0)
            continue;

        // ignore attachments
        if(strcmp(fdesc->getCName(), "attachments") == 0)
            continue;
    
        Field *a_field = a->getField(i);
        Field *b_field = b->getField(i);
    
        const FieldType &a_ftype = a_field->getType();
        const FieldType &b_ftype = b_field->getType();

        //printf("comparing field %s\n", a_ftype.getCName());

        if(a_ftype != b_ftype)
            return false;
    
        if(strstr(a_ftype.getCName(), "Ptr") == NULL)
        {
            // some speedup.
            // This is a HACK this will be changed in near future.
            if(a_field->getCardinality() == FieldType::MULTI_FIELD)
            {
                if(dynamic_cast<MField<UInt8> *>(a_field) != NULL)
                {
                    if(!compareMField<UInt8>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Int32> *>(a_field) != NULL)
                {
                    if(!compareMField<Int32>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<UInt32> *>(a_field) != NULL)
                {
                    if(!compareMField<UInt32>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Real32> *>(a_field) != NULL)
                {
                    if(!compareMField<Real32>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Vec2f> *>(a_field) != NULL)
                {
                    if(!compareMField<Vec2f>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Vec3f> *>(a_field) != NULL)
                {
                    if(!compareMField<Vec3f>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Pnt3f> *>(a_field) != NULL)
                {
                    if(!compareMField<Pnt3f>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Color3f> *>(a_field) != NULL)
                {
                    if(!compareMField<Color3f>(a_field, b_field))
                        return false;
                }
                else if(dynamic_cast<MField<Color4f> *>(a_field) != NULL)
                {
                    if(!compareMField<Color4f>(a_field, b_field))
                        return false;
                }
                else
                {
                    FINFO(("Slow multi field string compare for '%s'!\n",
                          a_ftype.getCName()));
                    std::string av, bv;
                    a_field->getValueByStr(av);
                    b_field->getValueByStr(bv);
                    if(av != bv)
                        return false;
                }
            }
            else
            {
                // This is very slow with multi fields!!!!
                std::string av, bv;
                a_field->getValueByStr(av);
                b_field->getValueByStr(bv);
                if(av != bv)
                    return false;
            }
        }
        else
        {
            if(a_field->getCardinality() == FieldType::SINGLE_FIELD)
            {
                if(!isEqual(static_cast<SFFieldContainerPtr *>(a_field)->getValue(),
                            static_cast<SFFieldContainerPtr *>(b_field)->getValue()))
                    return false;
            }
            else if(a_field->getCardinality() == FieldType::MULTI_FIELD)
            {
                if(static_cast<MFFieldContainerPtr*>(a_field)->size() !=
                   static_cast<MFFieldContainerPtr*>(b_field)->size())
                   return false;
    
                for(UInt32 j=0;j < static_cast<MFFieldContainerPtr*>(a_field)->size();++j)
                {
                    if(!isEqual((*(static_cast<MFFieldContainerPtr *>(a_field)))[j],
                                (*(static_cast<MFFieldContainerPtr *>(b_field)))[j]))
                        return false;
                }
            }
        }
    }
    return true;
}

Action::ResultE SharePtrGraphOp::clearAttachmentParent(NodePtr &node)
{
    if(node == NullFC)
        return Action::Continue;

    FieldContainerPtr fc = node->getCore();

    if(fc == NullFC)
        return Action::Continue;

    // the core could be shared this would lead to duplicated parent entries.
    if(_added_cores.count(fc) == 1)
        return Action::Continue;

    _added_cores.insert(fc);

    FieldContainerType  &fcType = fc->getType();

    //go through all fields
    for(UInt32 i = 1; i <= fcType.getNumFieldDescs(); ++i)
    {
        FieldDescription    *fDesc = fcType.getFieldDescription(i);
        Field               *fieldPtr = fc->getField(i);
        const FieldType     &fType = fieldPtr->getType();
        std::string         fieldType = fType.getCName();
        BitVector           mask = fDesc->getFieldMask();

        if(fDesc->isInternal())
        {
            continue;
        }

        if(strstr(fieldType.c_str(), "Ptr") != NULL)
        {
            if(fieldType[0] == 'S' && fieldType[1] == 'F') // single field
            {
                AttachmentPtr attachment =
                    AttachmentPtr::dcast((static_cast<SFFieldContainerPtr *>(fieldPtr))
                    ->getValue());
                if(attachment != NullFC)
                {
                    fc.setParentFieldPos(fDesc->getFieldId());
                    beginEditCP(attachment, Attachment::ParentsFieldMask);
                        attachment->getParents().clear();
                    endEditCP(attachment, Attachment::ParentsFieldMask);
                }
            }
            else if(fieldType[0] == 'M' && fieldType[1] == 'F') // multi field
            {
                MFFieldContainerPtr *mfield = 
                    static_cast<MFFieldContainerPtr *>(fieldPtr);

                UInt32 noe = mfield->size();
                for(UInt32 j = 0; j < noe; ++j)
                {
                    AttachmentPtr attachment =
                        AttachmentPtr::dcast((*(mfield))[j]);
                    if(attachment != NullFC)
                    {
                        fc.setParentFieldPos(fDesc->getFieldId());
                        beginEditCP(attachment, Attachment::ParentsFieldMask);
                            attachment->getParents().clear();
                        endEditCP(attachment, Attachment::ParentsFieldMask);
                    }
                }
            }
        }
    }

    return Action::Continue;
}

Action::ResultE SharePtrGraphOp::addAttachmentParent(NodePtr &node)
{
    if(node == NullFC)
        return Action::Continue;

    FieldContainerPtr fc = node->getCore();

    if(fc == NullFC)
        return Action::Continue;

    // the core could be shared this would lead to duplicated parent entries.
    if(_added_cores.count(fc) == 1)
        return Action::Continue;

    _added_cores.insert(fc);

    FieldContainerType  &fcType = fc->getType();

    //go through all fields
    for(UInt32 i = 1; i <= fcType.getNumFieldDescs(); ++i)
    {
        FieldDescription    *fDesc = fcType.getFieldDescription(i);
        Field               *fieldPtr = fc->getField(i);
        const FieldType     &fType = fieldPtr->getType();
        std::string         fieldType = fType.getCName();
        BitVector           mask = fDesc->getFieldMask();

        if(fDesc->isInternal())
        {
            continue;
        }

        if(strstr(fieldType.c_str(), "Ptr") != NULL)
        {
            if(fieldType[0] == 'S' && fieldType[1] == 'F') // single field
            {
                AttachmentPtr attachment =
                    AttachmentPtr::dcast(static_cast<SFFieldContainerPtr *>(fieldPtr)
                    ->getValue());
                if(attachment != NullFC)
                {
                    fc.setParentFieldPos(fDesc->getFieldId());
                    beginEditCP(attachment, Attachment::ParentsFieldMask);
                        attachment->addParent(fc);
                    endEditCP(attachment, Attachment::ParentsFieldMask);
                }
            }
            else if(fieldType[0] == 'M' && fieldType[1] == 'F') // multi field
            {
                MFFieldContainerPtr *mfield = static_cast<MFFieldContainerPtr *>(fieldPtr);
                UInt32 noe = mfield->size();
                for(UInt32 j = 0; j < noe; ++j)
                {
                    AttachmentPtr attachment =
                        AttachmentPtr::dcast((*(mfield))[j]);
                    if(attachment != NullFC)
                    {
                        fc.setParentFieldPos(fDesc->getFieldId());
                        beginEditCP(attachment, Attachment::ParentsFieldMask);
                            attachment->addParent(fc);
                        endEditCP(attachment, Attachment::ParentsFieldMask);
                    }
                }
            }
        }
    }

    return Action::Continue;
}

void SharePtrGraphOp::fillAttachmentParents(const NodePtr &node)
{
    if(node == NullFC)
        return;

    _added_cores.clear();
    OSG::traverse(node, osgTypedFunctionFunctor1CPtrRef<Action::ResultE,
                     NodePtr>(clearAttachmentParent));
    _added_cores.clear();
    OSG::traverse(node, osgTypedFunctionFunctor1CPtrRef<Action::ResultE,
                     NodePtr>(addAttachmentParent));
    _added_cores.clear();
}
