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

#include <sys/socket.h>         /* Socket Functions */
#include <sys/types.h>          /* Socket Datatypes */
#include <netinet/in.h>         /* IP Datatypes     */
#include <arpa/inet.h>          /* Address Structs  */
// #include <sstream>
#define MAXPENDING 5            /* Max Requests     */
#define REUSE_PORT 1
#define DEFAULT_RECV_BYTE_SIZE   10000

#define DieWithError(msg) { \
  cout << msg << " PwnT! Error on line " <<  __LINE__ << endl; \
  perror(msg); \
  exit(EXIT_FAILURE);}

// using std::ostringstream;
// using std::string;

/*! \class IANetworking 
 *  \brief this class contains a lot of helpful wrappers for networking stuff. 
 * 
 */
class IANetworking {
    public:
        static int ClientConnect(char* servIP, unsigned short ServPort);
        static int ContructTCPSocket(unsigned short port_number);
        static int AcceptConnection(int server_socket);
        static int uber_recv(int sock, char* delimit, char** data, size_t max_to_receive, bool & done);
        static int uber_recv(int sock, char* delimit, char** data);
        static int uber_send(int sock, char* data, bool & connected);
        static int uber_send(int sock, char* data, int size, bool & connected);
};
