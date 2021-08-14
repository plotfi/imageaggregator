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


#include "IARequest.h"

/* Image Aggregator request queue class */
/* Puyan Lotfi
 * puyan@cc.gatech.edu
 */

/*! \class IARequestQueue IARequest.h 
 *  \brief With this class, I can create objects that
 *  store lists of rules, sockets, requestIDs, etc.
 *  The threads in the IAThreadPool class shall 
 *  access these lists atomically.
 * I have decided to use this class for both request
 * Queues and thread queues, I am still thinking 
 * about sharing it with the same object as that
 * the thread pool uses.One thing to note is that IARequest elements are
 * added to the queue at the tail, and removes at the head (by default).
 * It is a true queue.
 *  \param length The number of IARequest elements in the queue.
 *  \param count THis is an important field, which is used to keep count of the number of times elements have been added to the queue. This will be used for the ImageAggregator to assign requestIDs.
 *  \param mutex This is a pthread mutex that is used by IARequestQueue::qAdd() and IARequestQueue::qRemove() to add and remove IARequest elements to the queue.
 *  \param isReady This is a pthread condition variable used to signal threads when a mutex is unlocked.
 *  \param qHead This is the head of the queue.
 *  \param qTail This is the tail of the queue.
 */
class IARequestQueue {
    public:
      /** Nothing gets passed to this contructor: all it does is init the mutex and condition variable.
       */
        IARequestQueue();
      /** This destructor is somewhat schetchy: should I really go through the queue and delete everything?
        */
        ~IARequestQueue();
        void print_object();
        void qAdd(IA_Request* new_request);
        IA_Request* qRemove(int Request_ID);
        /** This inline method returns the queue length.
         */
        inline unsigned int size() { return length; }
        /** This inline method returns a pointer to the pthread mutex.
         */
        inline pthread_mutex_t* get_mutex() { return &mutex; }
        /** This inline method returns a pointer to the pthread condition variable. 
         */
        inline pthread_cond_t* get_is_ready() { return &isReady; }
        /** This inline method returns a pointer to the head of the linked list.
         *  Note that the list is a bidirectional circular linked list.
         */
        inline IA_Request* getHead() {return qHead;} // Note! Do not use these yet! Not untile a reader lock is implemented. 
        /** This inline method returns a pointer to the tail of the linked list.
         *  Note that the list is a bidirectional circular linked list.
         */
        inline IA_Request* getTail() {return qTail;} // Note! Do not use these yet! Not untile a reader lock is implemented. 
    private:
        unsigned int length;
        unsigned int count; //The difference between count and length is that I never decrement count.
        pthread_mutex_t mutex;
        pthread_cond_t isReady;
        IA_Request* qHead;
        IA_Request* qTail;
};

