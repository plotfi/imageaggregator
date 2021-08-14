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

extern int errno;


/* Header file, describes important info about
 * Requests to the Imahge Aggregator 
 */

/* Puyan Lotfi
 * puyan@cc.gatech.edu
 */

/*! \var request_type
 *  This specifies the request types: Query and Reply.
 */

typedef enum _request_type {REPLY, QUERY} request_type;

/*! \struct IA_Request 
 *  \brief An IA_Request essentially holds information
 *  about the type of request, the socket it 
 *  was received on, the ID, and the rules associated. 
 * \param IA_Request.type the request_type 
 * \param socket Client request socket
 * \param requestID not sure what this will be in the future
 * \param rules This could be xml.
 */
typedef struct _IA_Request {
    request_type type;     
    int          socket;   
    int          requestID; 
    char*        rules;
    char*        filename;
    _IA_Request* next;
    _IA_Request* prev;
} IA_Request;

