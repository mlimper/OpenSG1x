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

#include <string.h>
#include <OSGGraphOp.h>
#include <OSGSimpleAttachments.h>

OSG_USING_NAMESPACE

/***************************************************************************\
 *                            Description                                  *
\***************************************************************************/

/*! \class osg::GraphOp
    \ingroup GrpSystemNodeCoresDrawablesGeometry
    
A base class used to traverse geometries.

Parameters are generally passed using the setParams() method (or indirectly
from constructing a GraphOpSeq via string) and follow the following 
conventions:

- Parameters are a key=value pair (e.g. max_polygons=1000). Note that
  there can be no spaces around the =, and that parameter values cannot
  contain spaces or ')' either right now.
- Special case: boolean parameters. If just the name is given, this is
  considered equivalent to True.
- Multiple parameters are separated by spaces.

Calling usage() gives a short textual desription of the purpose and
parameters of a GraphOp.
*/

/***************************************************************************\
 *                           Instance methods                              *
\***************************************************************************/

/*-------------------------------------------------------------------------*\
 -  public                                                                 -
\*-------------------------------------------------------------------------*/


/*------------- constructors & destructors --------------------------------*/

GraphOp::GraphOp(const char* name): 
    _name(name), _excludeListNodes(), _excludeListNames()
{
}

GraphOp::~GraphOp(void)
{
}

bool GraphOp::traverse(NodePtr& node)
{
    Action::ResultE res;
    res = ::traverse(node,
          osgTypedMethodFunctor1ObjPtrCPtrRef<Action::ResultE,
          GraphOp,
          NodePtr>(this,&GraphOp::traverseEnter),
          osgTypedMethodFunctor2ObjPtrCPtrRef<Action::ResultE,
          GraphOp,
          NodePtr,
          Action::ResultE>(this,&GraphOp::traverseLeave));

    if (res == Action::Continue) 
        return true;
    else 
        return false;
}

/*--------------------------- Exclude List --------------------------------*/

void GraphOp::addToExcludeList(NodePtr& node)
{
    if (!isInExcludeListNodes(node))
        _excludeListNodes.push_back(node);
}

void GraphOp::addToExcludeList(const std::string &name)
{
    if (!isInExcludeListNames(name))
        _excludeListNames.push_back(name);
}

void GraphOp::removeFromExcludeList(NodePtr& node)
{
    _excludeListNodes.remove(node);
}

void GraphOp::removeFromExcludeList(const std::string &name)
{
    _excludeListNames.remove(name);
}

void GraphOp::clearExcludeList(void)
{
    _excludeListNames.clear();
    _excludeListNodes.clear();
}

bool GraphOp::isInExcludeListNodes(NodePtr& node)
{
    std::list<NodePtr>::iterator list_iter;
    list_iter = std::find(_excludeListNodes.begin(),_excludeListNodes.end(),node);

    if (list_iter==_excludeListNodes.end()) 
        return false;
    else 
        return true;
}

bool GraphOp::isInExcludeListNames(const std::string &name)
{
    std::list<std::string>::iterator namelist_iter;
    namelist_iter = std::find(_excludeListNames.begin(),_excludeListNames.end(),name);

    if (namelist_iter==_excludeListNames.end()) 
        return false;
    else 
        return true;
}

bool GraphOp::isInExcludeList(NodePtr& node)
{
    if (isInExcludeListNodes(node) || (OSG::getName(node)!=NULL && isInExcludeListNames(OSG::getName(node))))
        return true;
    else
        return false;
}

// Parameter Helpers

GraphOp::ParamSet::ParamSet(const std::string &params)
{
    _values.clear();
    _used.clear();
    
    std::string::const_iterator it = params.begin(), end = params.end();
    
    std::string key, value;
    
    while(it != end)
    {
        char c = 0;
        
        key.clear();
        value.clear();
        
        // Read key
        while(it != end)
        {
            c = *it++;
            
            if(c == ' ' || c == '=')
                break;
            
            key.push_back(tolower(c));
        }
        
        // Do we have a value? Read it
        if (it != end && c == '=')
        {
            while(it != end)
            {
                c = *it++;

                if(c == ' ')
                    break;

                value.push_back(c);
            }           
        }

        // Add key, value pair
        
        FDEBUG(("GraphOp::ParamSet: key='%s', value='%s'\n", key.c_str(),
                                                             value.c_str()));
        _values[key] = value;
        
        // Skip to next param

        while(it != end && (*it == ' '));
   }
}

bool GraphOp::ParamSet::operator()(const char *name, std::string &val)
{
    valuesT::iterator it = _values.find(name);
    
    if(it != _values.end())
    {
        val = (*it).second;
        
        _used[name] = true;
        
        return true;
    }
    return false;
}
 
bool GraphOp::ParamSet::operator()(const char *name, Real32 &val)
{
    valuesT::iterator it = _values.find(name);
    
    if(it != _values.end())
    {
        const Char8* c = (*it).second.c_str();       
        FieldDataTraits<Real32>::getFromString(val, c);
        
        _used[name] = true;
        return true;
    }
    return false;
}

bool GraphOp::ParamSet::operator()(const char *name, UInt16 &val)
{
    valuesT::iterator it = _values.find(name);
    
    if(it != _values.end())
    {
        const Char8* c = (*it).second.c_str();       
        FieldDataTraits<UInt16>::getFromString(val, c);
        
        _used[name] = true;
        return true;
    }
    return false;
}

bool GraphOp::ParamSet::operator()(const char *name, UInt32 &val)
{
    valuesT::iterator it = _values.find(name);
    
    if(it != _values.end())
    {
        const Char8* c = (*it).second.c_str();       
        FieldDataTraits<UInt32>::getFromString(val, c);
        
        _used[name] = true;
        return true;
    }
    return false;
}

bool GraphOp::ParamSet::operator()(const char *name, bool &val)
{
    valuesT::iterator it = _values.find(name);
    
    if(it != _values.end())
    {
        if((*it).second.length() == 0)
        {
            val = true;
        }
        else
        {
            const Char8* c = (*it).second.c_str();       
            FieldDataTraits2<bool>::getFromString(val, c);
        }
        
        _used[name] = true;
        return true;
    }
    return false;
}

void GraphOp::ParamSet::markUsed(const char *name)
{
    _used[name] = true;
}

std::string GraphOp::ParamSet::getUnusedParams(void)
{
    std::string out;
    
    for (valuesT::iterator it = _values.begin(); it != _values.end(); ++it)
    {
        usedT::iterator uit = _used.find((*it).first);
        
        if(uit == _used.end())
        {
            if(out.length())
                out += " ";
                
            out += (*it).first;
        }
    }
 
    return out;   
}


/*-------------------------------------------------------------------------*\
 -  protected                                                              -
\*-------------------------------------------------------------------------*/
