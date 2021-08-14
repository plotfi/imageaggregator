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

#include <sstream>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include "IAThreadPool.h"

#define enable_rules 0
#define enable_collage 0
// If these idiotic options are set
// Then you must run as a IA that
// Sends one images at a time

#if enable_collage
#include "IAimaging.h"
#endif


using std::cout;
using std::endl;
using std::ostringstream;
using std::string;
//using str::fstream;
//using namespace std;

void IAThreadPool::print_object() {
    cout                                     << endl 
            << "--------------------------"  << endl;
    cout    << "Incoming Connection Queue: " << endl;
    incoming_queue->print_object();
    
    cout                                     << endl 
            << "--------------------------"  << endl;
    cout    << "Pending Request Queue: "     << endl;
    pending_queue->print_object();
}


/*! \fn IAThreadPool::IAThreadPool(unsigned short port)
 *  \brief Contructor of the IAThreadPool class: this one takes in a port to listen on and defaults to DEFAULT_THREAD_POOL_SIZE threads. 
 *  \param port The Port number to listen on. 
 */
IAThreadPool::IAThreadPool(unsigned short port) {
  IAThreadPool::init_data(port, DEFAULT_THREAD_POOL_SIZE);
}

/*! \fn IAThreadPool::IAThreadPool(unsigned short port, size_t start_size)
 *  \brief Contructor of the IAThreadPool class: this one takes in a port to listen on and the thread pool size.
 *  \param port The Port number to listen on. 
 *  \param start_size The number of threads in this pool object.
 */
IAThreadPool::IAThreadPool(unsigned short port, size_t start_size) {
  IAThreadPool::init_data(port, start_size);
}

/*! \fn void IAThreadPool::init_data (unsigned short port, size_t start_size)
 *  \brief A member function of IAThreadPool that initializes various fields of the object, and is used in the contructor.
 *  \param port The Port number to listen on. 
 *  \param start_size The number of threads in this pool object.
 *  \return void
 */
void IAThreadPool::init_data(unsigned short port, size_t start_size) {
  pool = new pthread_t[num_threads = start_size];
  incoming_queue = new IARequestQueue();
  pending_queue  = new IARequestQueue();
  ig_pool = NULL;

  query_pool = NULL;
  port_number = port;
}

/*! \fn void IAThreadPool::init_threads()
 *  \brief A member function of IAThreadPool that spawns the producer and consumer threads.
 *  \return void
 */
void IAThreadPool::init_threads() {
  if(pthread_create(&the_producer, NULL, IAThreadPool::producer, this)) {
    cout << "Error creating producer thread." << endl;
  }

  for(int i=0; i<num_threads; i++) {
    if(pthread_create(&pool[i], NULL, IAThreadPool::consumer, this)) {
      cout << "Error creating consumer thread " << i << "." << endl;
    }
  }
}

/*! \fn void* IAThreadPool::producer(void* pass)
 *  \brief The producer opens a TCP server socket, and listens on it for new connections, it then puts requests on some queue for a consumer to get to.
 *  \param pass Whatever is passed in my pthread_create().
 *  \return void
 */
void* IAThreadPool::producer(void* pass) {
  IAThreadPool* oneself = (IAThreadPool*)pass;
  int clntSock;
   
  cout << "Producer on Port " << oneself->port_number << endl;

  oneself->servSock = IANetworking::ContructTCPSocket(oneself->port_number);

    for(;;) {
        if (-1 == (clntSock = IANetworking::AcceptConnection(oneself->servSock))) {
            continue;
        } 
        // can be query from client, or reply from ImageGenerator
        IA_Request *request = new IA_Request();
        request->socket     = clntSock;
        request->rules      = NULL;
        request->filename   = NULL;
        request->requestID  = -1;
        
        if(NULL==oneself->query_pool) {
            request->type   = QUERY;
            cout << "Adding to query queue." << endl;
        } else if(NULL == oneself->ig_pool) {
            if(oneself->incoming_queue->size() > 20 || oneself->pending_queue->size() > 20) {
                cout << "Detected dos... closing socket." << endl;
                close(clntSock);
                continue;
            }
            
            request->type   = REPLY;
            cout << "Adding to reply queue." << endl;
        }
        oneself->incoming_queue->qAdd(request);
    }
}

/*! \fn void* IAThreadPool::consumer(void* pass)
 *  \brief The producer processes requests on the various requests queues, and forwards them where they should go.
 *  \param pass Whatever is passed in my pthread_create().
 *  \return void
 */
void* IAThreadPool::consumer(void* pass) {
    IAThreadPool* oneself = (IAThreadPool*)pass;
    IA_Request* moved_request; // request that is to be moved from the connection queue to the pending queue.
    
    cout << "Consumer Thread Spawned" << endl;
    
    while(true) {
        moved_request = oneself->incoming_queue->qRemove(-1); // Remove from the incoming connection queue. 

        if(NULL==oneself->query_pool) {
            cout << "Handling Client Query" << endl;
            IAThreadPool::qu_pool_handle(oneself, moved_request);
        } else if (NULL == oneself->ig_pool) {
            cout << "Handling Image Generator Reply" <<endl;
            IAThreadPool::ig_pool_handle(oneself, moved_request);
        } else {
            cout << "This pool is unhooked, Why? " << endl;
        }
    }
}


/*! \fn void IAThreadPool::qu_pool_handle(IAThreadPool* oneself)
 *  \brief This handles the consumer thread stuff for the query threads.
 *  \param oneself This is the IAThreadPool object of interest.
 *  \param current_query I need to send forward a query to the actuator 
 *         before I can add it to the pending queue, so this call needs
 *         an extra parameter.
 *  \return void
 */
void IAThreadPool::qu_pool_handle(IAThreadPool* oneself, IA_Request* current_query) {
    
    cout << "Query Consumer Thread" << endl;
    
    int ig, qu;
    int actuator_sock;
    bool connected = true;
    XMLByte* receive_buffer;      // This is just a to what recv() gets off the network. 
    ostringstream ost;
        
    if (0 >=  IAxml::xml_recv_query(current_query->socket, (XMLByte*)XML_DELIMIT_QUERY, &receive_buffer, current_query->requestID)) {
        cout << "Broken Connection." << endl;
        close(current_query->socket);
        delete current_query;
        return;                                         // receive some data
    } else {
        current_query->rules = (char*)receive_buffer;
    }
    
    cout << "Actuator IP: " << oneself->actuatorIP << " Actuator Port: " << oneself->actuatorPort << endl;
    
    if (0 < (actuator_sock = IANetworking::ClientConnect(oneself->actuatorIP, oneself->actuatorPort))) {
    
        // TODO Get the host info, for the actuator
        // TODO forward request to the actuator.
        IAxml::xml_send(actuator_sock,
                        (XMLByte*)current_query->rules,
                        connected);
    
        char* useless; 
        if(0 < IAxml::xml_recv_reply(actuator_sock, "\\CLOSE", &useless)) {
            delete[] useless; //Speaking of pointless crap...
            close(actuator_sock);
        }
    }
    
    
//     ost << "The request ID for this connection is: " 
//         << current_query->requestID << endl;
    
//     IAxml::xml_send(current_query->socket,
//                     (XMLByte*)ost.str().c_str(),
//                     connected);
    
    oneself->pending_queue->qAdd(current_query);
    
//     qu = oneself->pending_queue->size();
//     ig = oneself->ig_pool->pending_queue->size();
//     cout<< "qu_pool> " << "ig: " << ig << " " << "qu: " << qu << endl;
}

/*! \fn void IAThreadPool::ig_pool_handle(IAThreadPool* oneself)
 *  \brief This handles the consumer thread stuff for the query threads.
 *  \param oneself This is the IAThreadPool object of interest.
 *  \param current_reply I am just doing this for consistency.
 *  \return void
 */
void IAThreadPool::ig_pool_handle(IAThreadPool* oneself, IA_Request* current_reply) {
    
    cout << "Reply Consumer Thread: Going to try to receive Image from Image generator." << endl;

    
    XMLByte*        receive_buffer;
    
    pthread_mutex_lock(&oneself->pool_mutex);
    
      //TODO: Read out the filesize
        bool            null_term = false;
        char            get_byte;
        ostringstream   tag;
        ostringstream   filename;
                        filename << "ig_image" << "_id_" << current_reply->requestID << ".jpg";
        std::ofstream   write_file(filename.str().c_str());
        while(0 < recv(current_reply->socket, &get_byte, 1, 0)) {
            if (null_term == false && get_byte!='\0') {
                cout << get_byte;
                tag << get_byte; 
            } else if (null_term == false && get_byte=='\0') {
                tag << '\0';
                null_term = true;
            } else if (null_term == true) {
                cout << get_byte;
                write_file << get_byte;
            } else {
                cout <<" ELSE?\n"; 
            }
        }
        
        cout << "Finished Receiving Image: GOT IMAGE" << endl;
        
        write_file.close();
        char* rule_cp     = new char[strlen((char*)tag.str().c_str()) + 1];
        char* filename_cp = new char[strlen((char*)filename.str().c_str()) + 1];
        strcpy(rule_cp, (char*)tag.str().c_str());
        strcpy(filename_cp, (char*)filename.str().c_str());
        current_reply->rules    = rule_cp;
        current_reply->filename = filename_cp;
        oneself->pending_queue->qAdd(current_reply);
    
    
    bool debug_ig = false;
    if(debug_ig || 
        ((0!=oneself->pending_queue->size() 
        || 0!=oneself->query_pool->pending_queue->size()) 
            && (oneself->pending_queue->size() >= oneself->images_to_wait_for  ))) {
        int query_count = 0;
        pthread_mutex_t* qu_pending_queue_mutex = oneself->query_pool->pending_queue->get_mutex();
        pthread_cond_t*  qu_pending_queue_condition = oneself->query_pool->pending_queue->get_is_ready();        
        pthread_mutex_t* ig_pending_queue_mutex = oneself->pending_queue->get_mutex();
        pthread_cond_t*  ig_pending_queue_condition = oneself->pending_queue->get_is_ready();
        do {// Grab query off of the pending query queue
            IA_Request*   head                  = oneself->pending_queue->getHead();
            IA_Request*   current_pending_reply = head;
            IA_Request*   current_pending_query = oneself->query_pool->pending_queue->qRemove(-1);
            bool          send_now              = false;
            int           total_image_size      = 0;
            int           total_images          = 0;
            ostringstream image_ost;
            ostringstream aggregate_filename;
                          aggregate_filename << "aggregate_image_id_" 
                                             << current_pending_query->requestID 
                                             << ".jpg";
            ostringstream tag_ost;
                          tag_ost << XML_OPEN_QUERY_RESPONSE 
                                  << endl
                                  << XML_OPEN_REQUESTID
                                  << current_pending_query->requestID
                                  << XML_CLOSE_REQUESTID
                                  << endl;

                          
            pthread_mutex_lock(qu_pending_queue_mutex);
            pthread_mutex_lock(ig_pending_queue_mutex);
                          
            query_count = oneself->query_pool->pending_queue->size();
            do { // Go through the replies for checking of the id tags, and aggregate as you go.
                int        id;
                bool       flag     = false;
                char*      position = current_pending_reply->rules; 
                 #if enable_rules
                  while(-1 != (id = IAxml::xml_getTagContentInt(&position, XML_OPEN_REQUESTID, XML_CLOSE_REQUESTID))) {
                      if(id == current_pending_query->requestID) {
                          char* temp_pos = current_pending_reply->rules;
                          flag = true;
                          total_image_size += IAxml::xml_getTagContentInt(&temp_pos, XML_OPEN_LENGTH, XML_CLOSE_LENGTH);
                          cout << "Length: " << total_image_size << endl;
                          break;
                      } else {
                          flag = false;
                          continue;
                      }
                  }
                  #endif
                
                    char* temp_pos = current_pending_reply->rules;
                    flag = true;
                    total_image_size += IAxml::xml_getTagContentInt(&temp_pos, XML_OPEN_LENGTH, XML_CLOSE_LENGTH);
                    cout << "Length: " << total_image_size << endl;

                if(true == flag) {
                    total_images++;
                    image_ost << XML_OPEN_IAIMAGE 
                              << current_pending_reply->filename
                              << XML_CLOSE_IAIMAGE;
                    send_now = true;
                } 
                current_pending_reply = current_pending_reply->next;
             } while(head!=current_pending_reply);
             pthread_mutex_unlock(qu_pending_queue_mutex);
             pthread_cond_signal(qu_pending_queue_condition);
             pthread_mutex_unlock(ig_pending_queue_mutex);
             pthread_cond_signal(ig_pending_queue_condition);
            
             if(send_now) { 
                 // The send flag is used to trigger sending, 
                 // You shouldn't bother sending if the connection got 
                 // terminated on the other end of if the replies are not for your query. 
                 //TODO: send image data aggregate
                 //TODO: Check to see if the query got anything.
                 //TODO: Make sure to be sending the right length tag.

                 ostringstream string_image_aggregate;
                 char          getMe;
                 int           aggregate_image_total_bytes = 0;

                 cout << "aggregate" << endl;
                 cout << image_ost.str() << endl;
                 cout << aggregate_filename.str() << endl;
                 
#if enable_collage

                 cout << "Collaging Images" << endl;


                 if(-1 != IAimaging::aggregate((char*)image_ost.str().c_str(), (char*)aggregate_filename.str().c_str())) {
                        
                        
                    std::ifstream read_aggregate(aggregate_filename.str().c_str());
                    
                    while(read_aggregate.get(getMe)) {
                        string_image_aggregate << getMe;
                        aggregate_image_total_bytes++;
                    }
    
                    tag_ost << XML_OPEN_LENGTH
                            << aggregate_image_total_bytes
                            << XML_CLOSE_LENGTH
                            << endl
                            << XML_CLOSE_QUERY_RESPONSE;
                            //<< '\0'; 

                    
                    bool          connected                   = true;
                    bool          terminate_client            = false;
    
                    IAxml::xml_send(current_pending_query->socket, 
                                    (XMLByte*)tag_ost.str().c_str(), 
                                    connected);
                    
                    if(!connected) terminate_client = true;
                    
                    IANetworking::uber_send(current_pending_query->socket, 
                                            (char*)string_image_aggregate.str().c_str(),
                                            aggregate_image_total_bytes,
                                            connected);
                    
                    remove((char*)aggregate_filename.str().c_str());
                    
                    
                    
                    if(!connected) terminate_client = true;
    
                    if(terminate_client) {
                        close(current_pending_query->socket);
                        delete current_pending_query;
                    } else {
                        oneself->query_pool->pending_queue->qAdd(current_pending_query);
                    }
                 }
#else

                    std::ifstream read_aggregate(filename.str().c_str());
                    while(read_aggregate.get(getMe)) {
                        string_image_aggregate << getMe;
                        aggregate_image_total_bytes++;
                    }
    
                    tag_ost << XML_OPEN_LENGTH
                            << aggregate_image_total_bytes
                            << XML_CLOSE_LENGTH
                            << endl
                            << XML_CLOSE_QUERY_RESPONSE;
                            //<< '\0'; 

                    
                    bool          connected                   = true;
                    bool          terminate_client            = false;
    
                    IAxml::xml_send(current_pending_query->socket, 
                                    (XMLByte*)tag_ost.str().c_str(), 
                                    connected);
                    
                    if(!connected) terminate_client = true;
                    
                    IANetworking::uber_send(current_pending_query->socket, 
                                            (char*)string_image_aggregate.str().c_str(),
                                            aggregate_image_total_bytes,
                                            connected);
                    
                    remove((char*)filename.str().c_str());
                    
                    
                    
                    if(!connected) terminate_client = true;
    
                    if(terminate_client) {
                        close(current_pending_query->socket);
                        delete current_pending_query;
                    } else {
                        oneself->query_pool->pending_queue->qAdd(current_pending_query);
                    }


#endif
                
             }
        } while(query_count > 0);
        //clean out the pending queue. It is ok to do it here because even if someone added stuff to the queue, 
        //I am still taking the same entries off anyways (since it removes the head)
        for(int remove_times=oneself->pending_queue->size();remove_times > 0; remove_times--) {
            IA_Request* current_pending_reply_to_remove = oneself->pending_queue->qRemove(-1);
            remove(current_pending_reply_to_remove->filename);
            close(current_pending_reply_to_remove->socket);
            delete[] current_pending_reply_to_remove->rules;
            delete[] current_pending_reply_to_remove->filename;
            delete current_pending_reply_to_remove;
        }
    }
    
    pthread_mutex_unlock(&oneself->pool_mutex);
    pthread_cond_signal(&oneself->pool_cond);

}

