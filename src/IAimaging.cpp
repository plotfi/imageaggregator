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

#include "IAxml.h"
#include "IAimaging.h"
//#include <fstream>
//#include <sstream>
//#include <string>
#include <iostream>
#include <list>
//#include <vector>

using namespace Magick;
using namespace std;



int IAimaging::aggregate(char* xmlFilenameList, char* aggregate) {
    char* filename;
    char* position = xmlFilenameList;

    InitializeMagick("");

    list<Image> imageList;

    while(NULL != (filename = IAxml::xml_getTagContent_C_string(&position, XML_OPEN_IAIMAGE, XML_CLOSE_IAIMAGE))) { 
        cout << "Reading: " << filename << endl;
        readImages(&imageList, filename);
    }
    
    Image appended;

    try { //TODO: Better Error Checking  
    
    appendImages( &appended, imageList.begin(), imageList.end() );
    appended.write(aggregate);
    } catch(...) {
        return -1;
    }
    return 0;
}
