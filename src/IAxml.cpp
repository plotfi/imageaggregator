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

#include <iostream>
#include <sstream>
#include <string.h>
#include "IAxml.h"

using std::cout;
using std::endl;
using std::ostringstream;
using std::string;

int IAxml::xml_recv_image(int sock, XMLByte* xmlData, char** imageData) {
    bool done   = false;
    int  length = IAxml::xml_getTagContentInt((char**)&xmlData, XML_OPEN_LENGTH, XML_CLOSE_LENGTH);
    return IANetworking::uber_recv(sock, NULL, imageData, length, done);
}

int IAxml::xml_recv_query(int sock, XMLByte* delimit, XMLByte** xmlData, int requestID) {
    ostringstream processed_request;
    int           totalBytesRecv = 0;
    int           bytesTotal = 0;
    char*         tag_position; // this is a generic pointer ot whereever the hell I want. 
    char*         buffer;
    totalBytesRecv += IANetworking::uber_recv(sock, (char*)delimit, &buffer);
    tag_position = strstr(buffer, XML_DELIMIT_QUERY);
    tag_position[0] = '\0';
    processed_request << buffer << "\n" << XML_OPEN_REQUESTID << requestID << XML_CLOSE_REQUESTID << "\n" << XML_DELIMIT_QUERY << "\n" ;
    char* copy = new char[strlen(processed_request.str().c_str()) + 1];
    strcpy(copy, processed_request.str().c_str());
    tag_position[0] = '<';
    delete[] buffer;
    *xmlData = (XMLByte*)copy;
    //cout << *xmlData << endl;
    return totalBytesRecv;
}

int IAxml::xml_recv_reply(int sock, XMLByte* delimit, XMLByte** xmlData) {
    int totalBytesRecv = 0;
    char* buffer;
    totalBytesRecv += IANetworking::uber_recv(sock, (char*)delimit, &buffer);
    *xmlData = (XMLByte*)buffer;
    //cout << *xmlData << endl;
    return totalBytesRecv; 
}

int IAxml::xml_send(int sock, XMLByte* xmlData, bool & connected) {
    int totalBytesSent;
    totalBytesSent = IANetworking::uber_send(sock, (char*)xmlData, connected);
    return totalBytesSent;
}

// void IAxml::clientToActuator(XMLByte** out, XMLByte* in, int requestID) {
// }
// 
// void IAxml::ImageGeneratorToRequestID(int* out, XMLByte* in) {
// }

// void IAxml::ImageGeneratorToImage(FILE* image, XMLByte* in) {
// }

int IAxml::xml_getTagContentInt(char** position, char* open_tag, char* close_tag) {
    int   retVal=-1;
    char  temp;
    char* terminator;
    char* request = *position;
    if(NULL!=position && NULL!=request) {
        if(NULL != (terminator  = strstr(request, close_tag))) {
            temp = terminator[0];
            terminator[0] = '\0';
            if(NULL != (*position = strstr(request, open_tag) + strlen(open_tag))) {
                retVal = atoi(*position);
                //cout << "IAxml::xml_getTagContentInt> Return Value: " << retVal << endl;
                terminator[0] = temp;
                *position = strstr(request, close_tag) + strlen(close_tag);
            }
        }
    }
    //cout << "IAxml::xml_getTagContentInt> Return Value: " << retVal << endl;
    return retVal;
}


char* IAxml::xml_getTagContent_C_string(char** position, char* open_tag, char* close_tag) {
    ostringstream   retVal_ost;
    char* retVal = NULL;
    char  temp;
    char* terminator;
    char* request = *position;
    if(NULL!=position && NULL!=request) {
        if(NULL != (terminator  = strstr(request, close_tag))) {
            temp = terminator[0];
            terminator[0] = '\0';
            if(NULL != (*position = strstr(request, open_tag) + strlen(open_tag))) {
                retVal_ost << *position;
                retVal = (char*) retVal_ost.str().c_str();
                cout << retVal << endl;;
                terminator[0] = temp;
                *position = strstr(request, close_tag) + strlen(close_tag);
            }
        }
    }
    //cout << "IAxml::xml_getTagContent_C_string> Return Value: " << retVal << endl;
    return retVal;
}








