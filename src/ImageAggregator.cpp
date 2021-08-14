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
#include <signal.h>
#include "ImageAggregator.h"

using std::cout;
using std::endl;

#define DEFAULT_QU_PORT 31337
#define DEFAULT_IG_PORT 31338
#define DEFAULT_AC_PORT 9012
#define DEFAULT_AC_ADDR "192.168.1.44"
#define DEFAULT_QU_THREADS 8
#define DEFAULT_IG_THREADS 1

    
ImageAggregator::ImageAggregator() {
    IA_run_info* run_info        = new IA_run_info();
    run_info->clnt_query_port = DEFAULT_QU_PORT;
    run_info->igen_reply_port = DEFAULT_IG_PORT;
    run_info->qu_threads      = DEFAULT_QU_THREADS;
    run_info->ig_threads      = DEFAULT_IG_THREADS;
    run_info->actuator_port   = DEFAULT_AC_PORT;
    run_info->actuator_ip     = DEFAULT_AC_ADDR;
    ImageAggregator::init(run_info);
}

ImageAggregator::ImageAggregator(IA_run_info* run_info) {
    ImageAggregator::init(run_info);
}

int ImageAggregator::init(IA_run_info* run_info) {
    ImageAggregator::signal_mask_hand();
    //TODO: Check NULLS
    _query_pool = new IAThreadPool(run_info->clnt_query_port, run_info->qu_threads);
    _ig_pool    = new IAThreadPool(run_info->igen_reply_port, run_info->ig_threads);
    _query_pool->ig_pool = _ig_pool;
    _ig_pool->query_pool = _query_pool;
    _ig_pool->actuatorIP   = _query_pool->actuatorIP   = run_info->actuator_ip; 
    _ig_pool->actuatorPort = _query_pool->actuatorPort = run_info->actuator_port;
    _ig_pool->images_to_wait_for = _query_pool->images_to_wait_for = run_info->images;
    _query_pool->init_threads();
    _ig_pool->init_threads();
    return 1;
}

void ImageAggregator::signal_mask_hand() {
    sigset_t   newmask; 
    sigset_t   oldmask; 
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGPIPE);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) 
    {
        fprintf(stderr, "can't set signal mask\n");
        exit(1);
    }
}


/*! \fn void* ImageAggregator::control_console()
 *  \brief This is pretty much going to serve as our control console.
 *
void* ImageAggregator::control_console() {
    int c;
    while(true) {
        c = getchar();    
        switch (c) {
            case 'q': break;
            break;
        }
    } 
}
 */
 
void ImageAggregator::print_object() {
    cout                                     << endl 
            << "--------------------------"  << endl;
    cout    << "Query Pool: " << endl;
    _query_pool->print_object();
    cout                                     << endl 
            << "--------------------------"  << endl;
    cout    << "IG Pool: " << endl;
    _ig_pool->print_object();
}
