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

#include <pthread.h>
#include "IARequestQueue.h"

#include <iostream>

using std::cout;
using std::endl;

IARequestQueue::IARequestQueue(): length(0), count(0), qHead(NULL), qTail(NULL) {
    pthread_mutex_init(get_mutex(), NULL);
    pthread_cond_init(get_is_ready(), NULL);
}

IARequestQueue::~IARequestQueue() {
    while (length > 0 ) {
        delete IARequestQueue::qRemove(-1);
    } // This may not necesarily be the best idea.
} 

void IARequestQueue::print_object() {
    IA_Request* current = getHead();
    pthread_mutex_lock(IARequestQueue::get_mutex());
    if(NULL != getHead()) {
        do {
            cout << "Request Type: " << current->type       << endl;
            cout << "Socket: "       << current->socket     << endl;
            cout << "ID: "           << current->requestID  << endl;
            cout << "Rules: "        << current->rules      << endl;
            current = current->next;
        } while(getHead()!=current);
    } else {
        cout << "Empty>" << endl;
    }
    pthread_mutex_unlock(IARequestQueue::get_mutex());
    pthread_cond_signal(IARequestQueue::get_is_ready());
}

/*! \fn void IARequestQueue::qAdd(IA_Request* new_request) 
 *  \brief This member method of IARequestQueue adds an IA_Request to a circular linked list atomonically.
 *  \param new_request This is a pointer to the IA_Request that is to be added to the queue.
 *  \return void
 */
void IARequestQueue::qAdd(IA_Request* new_request) {
    if (NULL == new_request) { 
        //printf("Invalid Request (NULL).\n"); 
      return; 
    } else {
        //printf("Adding request %d.\n", new_request->requestID);
        new_request->next = new_request;
        new_request->prev = new_request;
        if(-1 == new_request->requestID) new_request->requestID = count;
    }

    //printf("Adding %s\n", new_request->rules);

    pthread_mutex_lock(IARequestQueue::get_mutex());

    if (NULL == qHead) {
        //new_request->next = new_request->prev = new_request;
        qTail = new_request;
        qHead = new_request;

        //printf("1) %s Head: %d Tail: %d New: %d\n", qHead->rules, qHead, qTail, new_request);
        length++;
        count++;
    } else {
        //printf("2) %s Head: %d Tail: %d New: %d\n", qHead->rules, qHead, qTail, new_request);

        qTail->next = new_request;
        qHead->prev = new_request;
        new_request->prev = qTail;
        new_request->next = qHead;
        qTail = qTail->next; //the new_request is the new head
        //printf("3) %s Head: %d Tail: %d New: %d\n", qHead->rules, qHead, qTail, new_request);
        length++;
        count++;
    }
    pthread_mutex_unlock(IARequestQueue::get_mutex());
    pthread_cond_signal(IARequestQueue::get_is_ready());
}

/*! \fn IA_Request* IARequestQueue::qRemove(int Request_ID)
 *  \brief This member method of IARequestQueue removes from an element from a  circular linked list of IA_Requests atomonically. 
 *  \param Request_ID This will be used to say which element we want to remove from the queue. If -1, then dequeue the head element. 
 *  \return void
 */
IA_Request* IARequestQueue::qRemove(int Request_ID) {
    IA_Request* temp;

    pthread_mutex_lock(IARequestQueue::get_mutex());            //Lock mutex

    while (!qHead) {
        pthread_cond_wait( IARequestQueue::get_is_ready(), IARequestQueue::get_mutex() );  //No requests waiting
    }

    
    temp = qHead;
    
    //Fix the list
    if(length > 1) {
      qHead = qHead->next;
      if(2 == length) qTail = qHead;
      qHead->prev = qTail;
      qTail->next = qHead;
    } else {
      qHead = qTail = NULL;
    }

    length--;

    //Fine grain lock, release lock before freeing old node
    pthread_mutex_unlock(IARequestQueue::get_mutex());

    // check if I need to signal a condition.
    
    return temp;
    // Make sure to free this    
}
