#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <iostream>
#include <fstream>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv) {
    int                sock;
    int                file_size_in_bytes;                       
    struct sockaddr_in ServAddr; 
    unsigned short     ServPort = 31338;     
    char*              servIP   = "127.0.0.1";                
    char*              image_filename;
    char*              xml_filename;
    char               file_byte;
                
   
    image_filename  = argv[1];
    xml_filename    = argv[2];

    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        cout << "Socket failed" << endl;
    memset(&ServAddr, 0, sizeof( ServAddr));    
    ServAddr.sin_family      = AF_INET;             
    ServAddr.sin_addr.s_addr = inet_addr(servIP);   
    ServAddr.sin_port        = htons(ServPort); 
    if(connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0)
         cout << "Connect failed" << endl;


    /*ifstream image_read_file(image_filename);
    for(file_size_in_bytes=0;
        image_read_file.get(file_byte);
        file_size_in_bytes++);
        image_read_file.close(); */
   
    ifstream xml_read_file(xml_filename); 
    while(xml_read_file.get(file_byte)) { 
        if(1 != send(sock, &file_byte, 1, 0))
            cout << "Send Failed" << endl;
            //cout << file_byte;
    } send(sock, "\0", 1, 0);
    
    ifstream image_read_file(image_filename);
    while(image_read_file.get(file_byte)) { 
        if(1 != send(sock, &file_byte, 1, 0))
            cout << "Send Failed" << endl;
            //cout << file_byte;
    } close(sock);
    return 0; 
}

