#include <OSGConfig.h>
#include <OSGFieldContainerFields.h>
#include <OSGNodeCore.h>
#include <OSGLog.h>

#include "OSGWebInterface.h"

/*! \class osg::WebInterface

The WebInterface class provides a simple access to all
FieldContainers in a running OpenSG application. If
handleRequests() is called in each render loop, it is
possible to connect the interface with a standard web
browser http://hostname:8888. In glut applications the
idle functions could be used. It is poosible to add 
new features to the interfce by subclassing and adding
new handlers.

*/

/*-------------------------------------------------------------------------*/
/*                            Constructors                                 */

/*! Cunstruct a WebInterface for the given port. If the port
  is used, try to bind to the following port number.
*/
WebInterface::WebInterface(UInt32 port)
{
    bool bound=false;
    _socket.open();
    _socket.setReusePort(true);
    do
    {
        try
        {
            _socket.bind(SocketAddress(SocketAddress::ANY,port));
            _socket.listen();
            bound = true;
        }
        catch(SocketException &) 
        {
            port++;
        }
    }
    while(!bound);
    FLOG(("WebInterface bound to port:%d\n",port));
    // register bas handlers
    addHandler("/"          ,&WebInterface::rootHandler);
    addHandler("/changelist",&WebInterface::changelistHandler);
    addHandler("/fcview"    ,&WebInterface::fcViewHandler);
    addHandler("/fcedit"    ,&WebInterface::fcEditHandler);
    addHandler("/treeview"  ,&WebInterface::treeViewHandler);
}

/*-------------------------------------------------------------------------*/
/*                             Destructor                                  */

/*! Destructor 
 */
WebInterface::~WebInterface()
{
    _socket.close();
}

/*-------------------------------------------------------------------------*/
/*                            request handling                             */

/*! Handle all incomming http requests. Return, if no request is
  pending
*/
void WebInterface::handleRequests()
{
    std::string         path,url;
    ParameterT          param;
    HandlerT::iterator  hI;
    std::stringbuf     *pbuf;
    int                 size;
    char               *buffer;
    int                 pos;

    while(checkRequest(url))
    {
        decodeUrl(url,path,param);
        // write http header
        _body.str("");
        _body <<
            "HTTP/1.1 200 OK\r\n"
            "Connection: close\r\n"
            "Server: OpenSGMicroWebInterface\r\n"
            "Expires: 0\r\n"
            "Content-Type: text/html\r\n"
            "\r\n";
        // find handler
        hI = _handler.find(path);
        if(hI != _handler.end())
            (this->*hI->second)(_body,path,param);
        else
            _body << "<html>Invalid path</html>";
        // finish request
        pbuf = _body.rdbuf();
        size = pbuf->in_avail();
        buffer = new char[ 32000 ];
        pos = 0;
        while(size > 32000)
        {
            memcpy(buffer,pbuf->str().c_str()+pos,32000);
            _accepted.send(buffer,32000);
            size -= 32000;
            pos  += 32000;
        }
        if(size)
        {
            memcpy(buffer,pbuf->str().c_str()+pos,size);
            _accepted.send(buffer,size);
        }
        delete [] buffer;
        // close connection
        _accepted.close();
    }
}

/*! Suspend processing until a http request is pending for the
  given duration in seconds. If duration is < 0. waitRequest
  will wait vor ever
*/
bool WebInterface::waitRequest(double duration)
{
    if(_socket.waitReadable(duration))
        return true;
    else
        return true;
}

/*-------------------------------------------------------------------------*/
/*                             set                                         */

/*! Set the scenegraph root node.
 */
void WebInterface::setRoot(NodePtr root)
{
    _root = root;
}

/*-------------------------------------------------------------------------*/
/*                            handler management                           */

/*! Add a new page handler 
 */
void WebInterface::addHandler(char *path,MethodT method)
{
    _handler[path] = method;
}

/*-------------------------------------------------------------------------*/
/*                         url encoding/decoding                           */

/*! Encode the given path and params into a valid http url.
 */
std::string WebInterface::encodeUrl(const std::string &path,
                                    const ParameterT &param)
{
    std::string result=path;
    ParameterT::const_iterator pI;
    int c;
    char ch;
    char *hex="0123456789abcdef";

    if(param.size())
        result += "?";
    for(pI = param.begin() ; pI != param.end() ; ++pI)
    {
        if(pI != param.begin())
            result += '&';
        result += pI->first;
        if(!pI->second.empty())
            result += '=';
        for(c = 0 ; c < pI->second.size() ; ++c)
        {
            ch = pI->second[c];
            switch(ch)
            {
                case ' ':
                    result += '+';
                    break;
                case '+':
                case ';':
                case '/':
                case '?':
                case ':':
                case '@':
                case '&':
                case '=':
                    result += '%';
                    result += hex[ch/16];
                    result += hex[ch&15];
                    break;
                default:
                    result += ch;
            }
        }
    }
    return result;
}

/*! strip and decode parameter values from a given URL. Parameter,
  value paires are stored in a string map. 
*/
void WebInterface::decodeUrl(const std::string &url,
                             std::string &path,
                             ParameterT &param)
{
    std::string::const_iterator sI=url.begin();
    std::string                 name,value;
    char                        bu[2];
    int                         c;

    path.clear();
    param.clear();
   
    while(sI !=url.end() && *sI != ' ' && *sI != '?') 
        path.push_back(*(sI++));
    if(*sI == '?')
    {
        do
        {
            name = value = "";
            // read name
            while(++sI != url.end() && 
                  *sI != ' ' && *sI != '=' && *sI != '&') 
                name.push_back(*sI);
            if(*sI == '=')
            {
                // read value
                while(++sI != url.end() && *sI != ' ' && *sI != '&') 
                {
                    switch(*sI)
                    {
                        case '+': value.push_back(' ');
                            break;
                        case '%': 
                            for(c = 0 ; c < 2 ; ++c)
                            {
                                bu[c] = *(++sI);
                                if(bu[c] >= 'A')
                                    bu[c] = bu[c] - 'A' + 10; 
                                else
                                    if(bu[c] >= 'a')
                                        bu[c] = bu[c] - 'a' + 10; 
                                    else
                                        bu[c] -= '0';
                            }
                            value.push_back((char)(bu[0]*16+bu[1]));
                            break;
                        default:
                            value.push_back(*sI);
                    }
                }
            }
            param[name]=value;
        } while(*sI == '&');
    }
}

/*-------------------------------------------------------------------------*/
/*                         helper                                          */

/*! Helper function. Create a link to a node view html page.
 */
std::string WebInterface::createFCViewReference(FieldContainerPtr fcPtr,
                                                UInt32 fcId)
{
    std::stringstream                   result;

    if(fcPtr == NullFC)
    {
        if(fcId)
            result << "Unknown(" << fcId << ")";
        else
            result << "NullFC";
    }
    else
        result << "<a href =\"fcview?id="
               << fcPtr.getFieldContainerId()
               << "\">"
               << fcPtr->getType().getName().str()
               << "("
               << fcPtr.getFieldContainerId()
               << ")"
               << "</a>";
    return result.str();
}


/*! Handle pending http requests. The requested url is return in
  url. If no request is pending, false is returned.
*/
bool WebInterface::checkRequest(std::string &url)
{
    std::string name,value;
    char bu[4];
    char ch;

    url.clear();

    if(!_socket.waitReadable(0))
        return false;
    _accepted = _socket.accept();
    if(_accepted.recv(bu,4) && strncmp(bu,"GET ",4) == 0)
        while(_accepted.recv(&ch,1) && ch != ' ') 
            url.push_back(ch);
    do
        while(_accepted.recv(&ch,1) && ch != '\n');
    while(_accepted.recv(&ch,1) && ch != '\r');
    _accepted.recv(&ch,1);
    if(url.empty())
        return false;
    else
        return true;
}

/*! Traversal function for the treViewHandler
 */
void WebInterface::treeViewNode(std::ostream &os,NodePtr node,
                                ParameterT &param)
{
    ParameterT::const_iterator          pI;
    char idstr[32];
    std::string                         folder;

    if(node == NullFC)
    {
        os << "<li>NullFC</li>\n";
        return;
    }
    sprintf(idstr,"%d",node.getFieldContainerId());
    if(param.count(idstr))
    {
        param["close"] = idstr;
        folder = encodeUrl("treeview",param);
        param.erase("close");
        os << "<li><a href=\"" << folder << "\">&nbsp - &nbsp</a>";
    }
    else
    {
        param["open"] = idstr;
        folder = encodeUrl("treeview",param);
        param.erase("open");
        os << "<li><a href=\"" << folder << "\">&nbsp + &nbsp</a>";
    }
    os << "&nbsp &nbsp &nbsp "
       << createFCViewReference(node);
    if(node->getCore() != NullFC)
    {
        os <<  "&nbsp &nbsp &nbsp Core: "
           << createFCViewReference(node->getCore());
    }
    os << "</li>";

    if(param.count(idstr))
    {
        os << "<ul>\n";
        for(MFNodePtr::iterator nI=node->getMFChildren()->begin();
            nI != node->getMFChildren()->end();
            ++nI)
        {
            treeViewNode(os,*nI,param);
        }
        os << "</ul>\n";
    }
}

/*-------------------------------------------------------------------------*/
/*                      web page handler                                   */

/*! main page
 */
void WebInterface::rootHandler(std::ostream &os,
                               std::string &,
                               ParameterT &)
{
    os <<
        "<html>"
        "<h1>OpenSG Web Interface</h1>"
        "<ui>"
        "<li><a href=\"changelist\">ChangeList</a>"
        "<li><a href=\"treeview\">SceneGraph</a>"
        "</ui>"
        "</html>";
}

/*! View the current changelist
 */
void WebInterface::changelistHandler(std::ostream &os,
                                     std::string &,
                                     ParameterT &)
{
    ChangeList::idrefd_const_iterator   createdI;
    ChangeList::changed_const_iterator  changedI;
    ChangeList::idrefd_const_iterator   destroyedI;
    FieldContainerPtr                   fcPtr;
    ChangeList                          *changeList;
    std::string                         type,mask;
    int                                 col=0;
    const int                           createdCols=6;
    const int                           changedCols=3;
    const int                           destroyedCols=6;

    changeList=OSG::Thread::getCurrentChangeList();

    os << "<html>"
       << "<h1>ChangeList</h1>";
    
    // created
    os << "<h2>Created</h2>"
       << "<table><tr>";
    for(col = 0 ; col < createdCols ; ++col)
        os << "<th>FieldContainer</th>";
    os << "</tr>\n";
    for(col = 0,createdI = changeList->beginCreated();
        createdI != changeList->endCreated(); createdI++)
    {
        fcPtr = FieldContainerFactory::the()->getContainer(*createdI);
        if(!col)
            os << "<tr>";
        os << "<td>" << createFCViewReference(fcPtr) << "</td>";
        col = (col+1) % createdCols;
        if(!col)
            os << "</tr>\n";
    }
    while(col && col++ < createdCols)
        os << "<td>&nbsp;</td>";
    os << "</tr>\n</table>\n";

    // Changed
    os << "<h2>Changed</h2>"
       << "<table><tr>";
    for(col = 0 ; col < changedCols ; ++col)
        os << "<th>FieldContainer</th><th>Change Mask</th>";
    os << "</tr>\n";
    for(col = 0,changedI = changeList->beginChanged();
        changedI != changeList->endChanged(); 
        changedI++)
    {
        fcPtr = FieldContainerFactory::the()->getContainer(changedI->first);
        mask = "Unknown";
        if(fcPtr != NullFC)
        {
            if(changedI->second == FieldBits::AllFields)
                mask = "AllFields";
            else
            {
                mask = "";
                for(int i=0;i<fcPtr->getType().getNumFieldDescs();i++)
                {
                    FieldDescription *desc=fcPtr->getType().getFieldDescription(i+1);
                    if(desc->getFieldMask() & changedI->second)
                    {
                        if(!mask.empty())
                            mask += "<br>";
                        mask += desc->getName().str();
                    }
                }
            }
        }
        if(!col)
            os << "<tr>";
        os << "<td>"
           << createFCViewReference(fcPtr)
           << "</td><td>"
           << mask 
           << "</td>";
        col = (col+1) % changedCols;
        if(!col)
            os << "</tr>\n";
    }
    while(col && col++ < changedCols)
        os << "<td>&nbsp;</td>";
    os << "</tr>\n</table>\n";

    // destroyed
    os << "<h2>Destroyed</h2>"
       << "<table><tr>";
    for(col = 0 ; col < destroyedCols ; ++col)
        os << "<th>FieldContainer</th>";
    os << "</tr>\n";
    for(col = 0,destroyedI = changeList->beginDestroyed();
        destroyedI != changeList->endDestroyed(); destroyedI++)
    {
        fcPtr = FieldContainerFactory::the()->getContainer(*destroyedI);
        if(!col)
            os << "<tr>";
        os << "<td>" << createFCViewReference(fcPtr,*destroyedI) << "</td>";
        col = (col+1) % destroyedCols;
        if(!col)
            os << "</tr>\n";
    }
    while(col && col++ < destroyedCols)
        os << "<td>&nbsp;</td>";
    os << "</tr>\n</table>\n";
    os << "</html>";
}

/*! FieldContainer view handler
 */
void WebInterface::fcViewHandler(std::ostream &os,
                                 std::string &,
                                 ParameterT &param)
{
    FieldContainerPtr                   fcPtr,childFcPtr;
    std::string                         type,value;
    UInt32                              id;
    ParameterT::const_iterator          pI=param.find("id");
    FieldDescription                   *desc;
    Field                              *field;
    MFFieldContainerPtr                *mfFCPtr;

    if(pI == param.end())
    {
        os << "<html>id missing</html>";
        return;
    }
    id=atoi(pI->second.c_str());

    os << "<html>";
    fcPtr = FieldContainerFactory::the()->getContainer(id);
    if(fcPtr == NullFC)
    {
        os << "NullFC";
    }
    else
    {
        os << "<h1>"
           << fcPtr->getTypeName()
           << "</h1>"
           << "<table><tr><th>Field</th><th>Field Type</th><th>&nbsp;</th>"
           << "<th>Value</th></tr>\n";
        for(int i=0;i<fcPtr->getType().getNumFieldDescs();++i)
        {
            FieldDescription *desc=fcPtr->getType().getFieldDescription(i+1);
            field = fcPtr->getField(desc->getFieldId());
            type = desc->getFieldType().getName().str();
            os << "<tr><td>"
               << desc->getName().str()
               << "</td><td>"
               << desc->getFieldType().getName().str()
               << "</td><td>";
            if(type == "SFAttachmentMap")
            {         
                os << "</td><td>\n";
                AttachmentMap &am = ((SFAttachmentMap *)field)->getValue();
                AttachmentMap::const_iterator mI;
                for(mI = am.begin() ; mI != am.end() ; ++mI)
                {
                    if(mI != am.begin())
                        os << "\n<br>";
                    os << createFCViewReference(mI->second);
                }
            }
            else
                if((type.size() > 3) && 
                   (type.find("Ptr",type.size()-3) != -1))
                {
                    os << "</td><td>\n";
                    if(type.find("MF") == 0)
                    {
                        mfFCPtr = ((MFFieldContainerPtr*)field);
                        for(int j = 0 ; j < mfFCPtr->size() ; ++j)
                        {
                            if(j>0)
                                os << "\n<br>";
                            os << createFCViewReference((*mfFCPtr)[j]);
                        }
                    }
                    else
                    {
                        childFcPtr = ((SFFieldContainerPtr*)field)->getValue();
                        os << createFCViewReference(childFcPtr);
                    }
                }
                else
                    if(field)
                    {
                        field->getValueByStr(value);
                        os << "<form action=\"fcedit\""
                           << "<input type=\"submit\" value=\"Edit\">"
                           << "<input type=\"hidden\" name=\"id\" value=\""
                           << fcPtr.getFieldContainerId()
                           << "\">"
                           << "<input type=\"hidden\" name=\"field\" value=\""
                           << desc->getFieldId()
                           << "\">"
                           << "</form>"
                           << "</td><td>\n";
                        os << value;
                    }

            os << "</td></tr>\n";
        }
        os << "</table>";
    }
    os << "</html>";
}

/*! Edit field value
 */
void WebInterface::fcEditHandler(std::ostream &os,
                                 std::string &,
                                 ParameterT &param)
{
    FieldContainerPtr fcPtr=NullFC;
    std::string       value="";
    UInt32            cid=0;
    UInt32            fid=0;
    Field            *field;
    FieldDescription *desc=NULL;

    if(param.count("id"))
    {
        cid = atoi(param["id"].c_str());
        fcPtr = FieldContainerFactory::the()->getContainer(cid);
    }
    if(param.count("field"))
    {
        fid = atoi(param["field"].c_str());
    }
    if(fcPtr == NullFC)
    {
        os << "<html>Unknown field container</html>";
        return;
    }
    field = fcPtr->getField(fid);
    if(field == NULL)
    {
        os << "<html>Unknown field in container</html>";
        return;
    }
    desc=fcPtr->getType().getFieldDescription(fid+1);
    if(param.count("value"))
    {
        beginEditCP(fcPtr,desc->getFieldMask());
        field->pushValueByStr(param["value"].c_str());
        endEditCP(fcPtr,desc->getFieldMask());
    }
    field->getValueByStr(value);
    os << "<html><h1>Edit "
       << fcPtr->getTypeName()
       << "."
       << desc->getName().str()
       << "</h1>\n"
       << "<form action=\"fcedit\""
       << "<textarea name=\"value\" cols=\"50\" rows=\"10\">"
       << value
       << "</textarea><p>"
       << "<input type=\"submit\" value=\" Change \">"
       << "<input type=\"hidden\" name=\"id\" value=\""
       << cid
       << "\">"
       << "<input type=\"hidden\" name=\"field\" value=\""
       << fid
       << "</form>"
       << "</html>";
}

/*! Show scenegraph tree. For each leave to open, a parameter with
  the container id must be set. The parameters open and closed
  are used to open or close folders.
*/
void WebInterface::treeViewHandler(std::ostream &os,
                                   std::string &,
                                   ParameterT &param)
{
    ParameterT::iterator pI;
    UInt32               id;

    // open folder
    pI = param.find("open");
    if(pI != param.end() && !pI->second.empty())
    {
        param[pI->second]="";
        param.erase(pI);
    }        
    // close folder
    pI = param.find("close");
    if(pI != param.end())
    {
        pI = param.find(pI->second);
        if(pI != param.end())
            param.erase(pI);
        param.erase("close");
    }        
  
    // Changed 
    os << "<html>"
       << "<h1>Scenegraph</h1>\n"
       << "<ul>\n";
    treeViewNode(os,_root,param);
    os << "</ul>"
       << "</html>";
}