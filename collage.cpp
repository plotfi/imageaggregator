#include <fstream>
#include <sstream>
#include <Magick++.h>
//#include <string>
#include <iostream>
#include <list>
#include <vector>
using namespace std;

using namespace Magick;

int aggregate(char* filename0, char* filename1, char* filename2, char* filename3, char* aggregate) {

    InitializeMagick("");

    list<Image> imageList;
    readImages( &imageList, filename0 );
    readImages( &imageList, filename1 );
    readImages( &imageList, filename2 );
    readImages( &imageList, filename3 );

    Image appended;

    try {
    appendImages( &appended, imageList.begin(), imageList.end() );
    appended.write(aggregate);
    } catch (...) {
        return -1;
        
    }
    
    return 0;
}

int main(int argc, char **argv) {

    ifstream from(argv[1]);
    ostringstream ost;
    char ch;
/*
    while(from.get(ch)) {
        ost << ch;
    }
    cout << ost.str() << endl;

*/

aggregate(argv[1], argv[2],argv[3],argv[4], argv[5]);
  
    return 0;
}



