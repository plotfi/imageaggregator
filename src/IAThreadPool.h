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
#include "IAxml.h"
#define DEFAULT_THREAD_POOL_SIZE 8

/* Image Aggregator thread pool class */
/* Puyan Lotfi
 * puyan@cc.gatech.edu
 */

/*! \class IAThreadPool IARequest.h  pthread.h
 *  \brief This class basically provides a thread pool for 
 *  the Image Aggregator. I can dish out multiple IAThreadPool objects for listening to various ports. Right now I have a couple pointers inside the IAThreadPool class that point to other IAThreadPools. This can be useful for processing requests from queue to queue. 
 *  \param  port_number The port that this IAThreadPool object listens on.
 *  \param  incoming_queue The queue for incoming requests.
 *  \param  pending_queue  The queue for requests pending: queries waiting to be satisfied by a reply from the ImageGenerator. This isn't always used. 
 *  \param  servSock The socket descriptor for the port the thread pool object is listening on.
 *  \param  ig_pool  If the object is used to accept requests from the ImageGenerator, this is NULL. Otherwise, this points to thjat object.
 *  \param  query_pool If the object is used to accept requests from the client/query engine, this is NULL. Otherwise it points to that object.
 *  \param  num_threads Number of threads in the pool. This is set when the object is instantiated, right now it doesn't change after that. 
 *  \param  pool        Just an array of integers that holds pthread_t descriptors, You could think of this as the thread pool.
 *  \param  the_producer This is the pthread_t of the producer thread. 
 */
class IAThreadPool {
    public:                     
        IAThreadPool(unsigned short port_number); // I may add methods to grow the number of threads later
        IAThreadPool(unsigned short port_number, size_t start_size); // Contructor spawns all the threads.
        ~IAThreadPool() {delete pool;}
        void print_object();
        void init_data(unsigned short port, size_t start_size);
        inline unsigned int size() { return num_threads; }
        void init_threads();
        static void*    producer(void* pass);
        static void*    consumer(void* pass);
        static void     qu_pool_handle(IAThreadPool* oneself, IA_Request* current_query);
        static void     ig_pool_handle(IAThreadPool* oneself, IA_Request* current_reply);
        IAThreadPool*   query_pool;
        IAThreadPool*   ig_pool;
        int             servSock;
        int             images_to_wait_for;
        unsigned short  port_number;
        IARequestQueue* incoming_queue; 
        IARequestQueue* pending_queue;
        char*           actuatorIP;
        unsigned int    actuatorPort;
        pthread_mutex_t pool_mutex;
        pthread_cond_t  pool_cond;
    private:
        unsigned int num_threads;  // Number of threads in pool
        pthread_t* pool;           // ie, the consumer list
        pthread_t  the_producer;   // I want to make a joke here.  But people might not get it. 
};

