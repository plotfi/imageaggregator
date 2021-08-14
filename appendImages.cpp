// This may look like C code, but it is really -*- C++ -*-
//
// Copyright Bob Friesenhahn, 1999, 2000, 2003
//
// Test STL appendImages function
//

#include <Magick++.h>
#include <string>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

using namespace Magick;

int main( int /*argc*/, char ** argv)
{
    InitializeMagick(*argv);

    list<Image> imageList;
    readImages( &imageList, "one.jpg" );
    readImages( &imageList, "two.jpg" );

    Image appended;

    // Horizontal
    appendImages( &appended, imageList.begin(), imageList.end() );
    appended.write("wtf.jpg");

    return 0;
}





