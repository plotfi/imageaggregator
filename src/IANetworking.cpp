/***************************************************************************
 *   Copyright (C) 2006 by Puyan Lotfi                                     *
 *   puyan@cc.gatech.edu                                                   *
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
#include "IANetworking.h"

using std::cout;
using std::endl;

/*! \fn int IANetworking::ClientConnect(char* servIP, unsigned short ServPort)
 *  \brief Connect to a remote machine on a port.
 *  \param ServPort The port to connect to.
 *  \return int  socket.
 */
int IANetworking::ClientConnect(char* servIP, unsigned short ServPort) {
    int                sock;
    struct sockaddr_in ServAddr;

    if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        cout << "Socket failed" << endl;
        return -1;
    }
    memset(&ServAddr, 0, sizeof( ServAddr));    
    ServAddr.sin_family      = AF_INET;             
    ServAddr.sin_addr.s_addr = inet_addr(servIP);   
    ServAddr.sin_port        = htons(ServPort); 
    if(connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) {
        cout << "Connect failed" << endl;
        return -1;
    }
    return sock;
}

/*! \fn int IANetworking::ContructTCPSocket(unsigned short port_number) 
 *  \brief contructs a TCP server socket on a specified port.
 *  \param port_number The port to Listen on.
 *  \return int server socket.
 */
int IANetworking::ContructTCPSocket(unsigned short port_number) {
  int        servSock;                          /* Server sock descriptor.      */
  int        opt;                               /* Reuse ocupied port           */
  struct     sockaddr_in  ServAddr;             /* Server Socket addr           */ 
  struct     sockaddr_in  ClntAddr;             /* Client Socket addr           */ 
 
  opt      = REUSE_PORT;                        /* reuse port sockopt           */
                                                /* Construct local              */
                                                /* address structure            */
  memset( &ServAddr, 0, sizeof(ServAddr) );     /* Zero serve  struct           */
  memset( &ClntAddr, 0, sizeof(ClntAddr) );     /* Zero client struct           */
  ServAddr.sin_family = AF_INET;                /* Inet addr family             */
  ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming iface           */
  ServAddr.sin_port = htons(port_number);       /* Local (server) port          */

  if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
    DieWithError("socket() failed");            /* create socket                */

  if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) 
    DieWithError("setsockopt() failed");        /* sockopt: reuse ports         */

  if (bind(servSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0) 
    DieWithError("bind() failed");              /* bind to the port             */

  if (listen(servSock, MAXPENDING) < 0) 
    DieWithError("listen() failed");

  return servSock; 
}

/*! \fn int IANetworking::AcceptConnection(int server_socket) 
 *  \brief Accepts incoming connections on the server_socket.
 *  \param server_socket socket created by ContructTCPSocket()
 *  \return int client socket
 */
int IANetworking::AcceptConnection(int server_socket) {
  int        clntSock;                          /* client sock descriptor.      */
  struct     sockaddr_in  ClntAddr;             /* Client Socket addr           */ 
  unsigned   int          ClntLen;              /* client address               * 
                                                 * struct length                */
  ClntLen  = sizeof(ClntAddr);                  /* Client addr length           */

  //cout << "Wating to accept connections" << endl; 

  if ((clntSock = accept(server_socket, (struct sockaddr *) &ClntAddr, &ClntLen)) < 0) {
      //DieWithError("accept() failed");          /* Wait for a client to connect */
      clntSock = -1; //negative socket, so it knows to ignore this connection
  }

  //cout << "Handleing client" << endl;

  return clntSock;
}

/*! \fn int IANetworking::uber_recv(int sock, char* delimit, char** data,  size_t max_to_receive, bool & done)
 *  \brief Remember that curToRecv should have at least max_to_receive bytes allocated.
 *  \param sock, The socket to receive on.
 *  \param delimit The character delimiter to receive on. 
 *  \param data Where the reveived data will be copied to. Passed by referenced. Must be freed.
 *  \param max_to_receive Max ammount of data (bytes) to receive.
 *  \param done specifies if the delmiter was found in the received char*
 *  \return int number of bytes received
 */
int IANetworking::uber_recv(int sock, char* delimit, char** data, size_t max_to_receive, bool & done) {
    char*     curToRecv = new char[max_to_receive];
    char*     dgramPtr;
    int       totalBytesRecv;
    int       newBytesRecv;

    // Setup Stuff
    newBytesRecv                = 0;
    totalBytesRecv              = 0;
    dgramPtr                    = curToRecv;
    *data                       = curToRecv;
    done                        = false;
    curToRecv[max_to_receive-1] = '\0';
    memset(curToRecv, 0, sizeof(char)*max_to_receive);

    while(totalBytesRecv < max_to_receive-1) {
        cout << "recv: am I blocking?" << endl;
        if(((newBytesRecv = recv(sock, dgramPtr, max_to_receive - totalBytesRecv, 0)) < 0)) {
            cout << "recv() failed" << endl;
            delete[] curToRecv;
            return -1; 
        } else if (newBytesRecv == 0) {
            cout << "I have to recv " << max_to_receive << " Bytes." << endl;
            cout << "I got " << totalBytesRecv << " Bytes." << endl;
            break;
        } else {
            dgramPtr        += newBytesRecv;
            totalBytesRecv  += newBytesRecv;
            if (NULL==delimit) {
                continue;
            } else if(NULL != strstr(curToRecv, delimit)) {
                char* terminator;
                terminator  = strstr(curToRecv, delimit) + 
                              strlen(delimit);
                *terminator = '\0';
                done        = true;
                break;
            }
        }
    }
    if(!done && NULL != delimit) cout << "Error: Your receive buffer (Size: " << max_to_receive << ") is too damn small." << endl;
    //else      cout << "DONE" << endl;
    return totalBytesRecv;
}

/*! \fn int IANetworking::uber_recv(int sock, char* delimit, char** data)
 *  \brief This uber_recv() doesn't require 
 *  \param sock, The socket to receive on.
 *  \param delimit The character delimiter to receive on. 
 *  \param data Where the reveived data will be copied to. Passed by referenced. Must be freed.
 *  \return int number of bytes received
 */
int IANetworking::uber_recv(int sock, char* delimit, char** data) {
    int             totalBytesRecv;
    bool            done;
    totalBytesRecv  = 0;
    done            = false;
    totalBytesRecv = IANetworking::uber_recv(sock, delimit, data, DEFAULT_RECV_BYTE_SIZE, done);
    //     do {
    //         totalBytesRecv += IANetworking::uber_recv(sock, delimit, DEFAULT_RECV_BYTE_SIZE, done, receive_stream);
    //     } while(!done);
    //     data = (char*)receive_stream.str().c_str();
//     cout                                              << endl 
//          << "---------------"                         << endl 
//          << "Reply: " << *data                        << endl 
//          << "Size: "  << totalBytesRecv               << endl 
//          << "---------------"                         << endl;
    return totalBytesRecv;
}

/*! \fn int IANetworking::uber_send(int sock, char* data, bool & connected) 
 *  \brief This is a little function I use to make sending cleaner and easier.
 *  \param sock, The socket to send on.
 *  \param data Where the sent data will be copied from. 
 *  \param connected This tells the caller if the socket is closed on the 
 *   other end. I need to include some signal masks to stop the thing from dying.
 *  \return int number of bytes send.
 */
int IANetworking::uber_send(int sock, char* data, bool & connected) {
    int totalBytesSent = 0;
    //cout << "Trying to send on query socket> " << endl;
    if (0 >= (totalBytesSent += send(sock, data, strlen(data), 0))) {
        connected = false;
    }
    if ( 0 >= (totalBytesSent += send(sock, "\0", 1, 0))) {
        connected = false; //YES! I do send a null terminator.
    }
    if(!connected) cout << "Sending Failed> " << endl;
    return totalBytesSent;
}

/*! \fn int IANetworking::uber_send(int sock, char* data, bool & connected) 
 *  \brief This is a little function I use to make sending cleaner and easier.
 *  \param sock, The socket to send on.
 *  \param data Where the sent data will be copied from. 
 *  \param connected This tells the caller if the socket is closed on the 
 *   other end. I need to include some signal masks to stop the thing from dying.
 *  \return int number of bytes send.
 */
int IANetworking::uber_send(int sock, char* data, int size, bool & connected) {
    int totalBytesSent = 0;
    //cout << "Trying to send on query socket> " << endl;
    if (size != (totalBytesSent += send(sock, data, size, 0))) {
        connected = false;
    }
    if(!connected) cout << "Sending Failed> " << endl;
    return totalBytesSent;
}
