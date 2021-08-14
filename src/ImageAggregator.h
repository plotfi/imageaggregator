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

#include "IAThreadPool.h"

typedef struct _IA_run_info {
    unsigned short clnt_query_port;
    unsigned short igen_reply_port;
    unsigned short actuator_port;
    char*          actuator_ip;
    int            qu_threads;
    int            ig_threads;
    int            images;
}IA_run_info;

/*! \class ImageAggregator IAThreadPool.h
 * \brief This class mainly servers to swap thread pools and other things.
 * That means it makes my main() look like it does almost nothing.
 * This also serves as a good place for me to put extra documentation.
 */
class ImageAggregator {
    public:
        ImageAggregator();
        ImageAggregator(IA_run_info* run_info);
        int init(IA_run_info* run_info);
        void signal_mask_hand();

        void print_object();
        //void* control_console();
    
        IAThreadPool* _query_pool;
        IAThreadPool* _ig_pool;
};

/*! \mainpage ImageAggregator Documentation Main Page
 *
 * \section intro_sec Introduction
 *  The ImageAggregator is a component that receives image requests from a query engine. These queries 
 *  essentially state a set of rules which represent which camera or which room is of interest. Next a 
 *  query is sent from the ImageAggregator to the Actuator which asks for a set of images. At some point
 *  the reply to this request is sent to the ImageAggregator by the ImageGenerator. The images from the 
 *  ImageGenerator are then filtered through the ruleset from the original query from the client/query 
 *  engine. Once the proper images are selected, an aggregate function will be applied that will put 
 *  the images into a collage. Then the collage will be sent back to the client.
 * 
 * \section install_sec Installation
 *
 * Using the GNU Autotools.
 * 
 * \subsection step1 Step n:
 *
 * 
 */
