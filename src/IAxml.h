/***************************************************************************
 *   Copyright (C) 2006 by Puyan Lotfi
 *   puyan@cc.gatech.edu
 *                                                                         *
 *   Permission is hereby granted, free of charge, to any person obtaining *
 *   a copy of this software and associated documentation files (the       *
 *   "Software"), to deal in the Software without restriction, including   *
 *   without limitation the rights to use, copy, modify, merge, publish,   *
 *   distribute, sublicense, and/or sell copies of the Software, and to    *
 *   permit persons to whom the Software is furnished to do so, subject to *
 *   the following conditions:                                             *
 *                                                                         *
 *   The above copyright notice and this permission notice shall be        *
 *   included in all copies or substantial portions of the Software.       *
 *                                                                         *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       *
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    *
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR     *
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR *
 *   OTHER DEALINGS IN THE SOFTWARE.                                       *
 ***************************************************************************/

// #include <xercesc/util/PlatformUtils.hpp>
// #include <xercesc/util/XMLString.hpp>
// #include <xercesc/dom/DOM.hpp>
// #include <xercesc/dom/DOMBuilder.hpp>
// #include <xercesc/dom/DOMImplementation.hpp>
// #include <xercesc/dom/DOMImplementationLS.hpp>
// #include <xercesc/framework/MemBufInputSource.hpp>
// #include <xercesc/framework/Wrapper4InputSource.hpp>
// #include <xercesc/util/OutOfMemoryException.hpp>
// #if defined(XERCES_NEW_IOSTREAMS)
// #include <iostream>
// #else
// #include <iostream>
// #endif

#include "IANetworking.h"

/*XERCES_CPP_NAMESPACE_USE*/;

#define XML_DELIMIT_REPLY "</ImageResponse>"
#define XML_DELIMIT_QUERY "</UserQuery>"
#define XML_OPEN_REQUESTID "<ClientID>"
#define XML_CLOSE_REQUESTID "</ClientID>"
#define XML_OPEN_QUERY_RESPONSE "<QueryResponse>"
#define XML_CLOSE_QUERY_RESPONSE "</QueryResponse>"
#define XML_OPEN_LENGTH "<length>"
#define XML_CLOSE_LENGTH "</length>"
#define XML_OPEN_IAIMAGE   "<INTERNAL_IMAGE>"
#define XML_CLOSE_IAIMAGE   "</INTERNAL_IMAGE>"

#define XMLByte char



/*! \class IANetworking 
 *  \brief This class will contain stuff that is very much xml centric. Including some networking stuff. 
 * 
 */
class IAxml {
    public:
        static int xml_recv_query(int sock, XMLByte* delimit, XMLByte** xmlData, int requestID);
        static int xml_recv_reply(int sock, XMLByte* delimit, XMLByte** xmlData);
        static int xml_send(int sock, XMLByte* data, bool & connected);
        static int xml_getTagContentInt(char** position, char* open_tag, char* close_tag);
        static char* xml_getTagContent_C_string(char** position, char* open_tag, char* close_tag);
        static int xml_recv_image(int sock, XMLByte* xmlData, char** imageData);

//         static void IAxml::clientToActuator(XMLByte* out, XMLByte* in, int requestID);
//         static void IAxml::ImageGeneratorToRequestID(int* out, XMLByte* in);
//         static void IAxml::ImageGeneratorToImage(FILE* image, XMLByte* in);
};
