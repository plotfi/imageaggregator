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

#include <unistd.h>
#include <pthread.h>
#include "ImageAggregator.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <signal.h>


using namespace std;


int main(int argc, char **argv) {
    IA_run_info run_info;
    
    
    if(argc < 6) {
        cout << argv[0] << " <query port> <reply port> <actuator port> <actuator ip> <images>" << endl;
        exit(1);
    }
    
    
    run_info.clnt_query_port=atoi(argv[1]); 
    run_info.igen_reply_port=atoi(argv[2]);
    run_info.actuator_port=atoi(argv[3]);
    run_info.actuator_ip=argv[4];
    run_info.qu_threads=8;
    run_info.ig_threads=1;
    run_info.images = atoi(argv[5]);
    
  ImageAggregator *ia = new ImageAggregator(&run_info);
//ia->control_console();

//   struct sigaction SIGTERM_action; 
//   SIGTERM_action.sa_handler = exit_handler;
//   if (sigfillset(&SIGTERM_action.sa_mask) < 0) {
//       perror("sigfillset() failed\n");
//       exit(EXIT_FAILURE);
//   }
//   SIGTERM_action.sa_flags = 0;
//   if (sigaction(SIGINT, &SIGTERM_action, 0) < 0) {
//       perror("Failed to register signal handler\n");
//       exit(EXIT_FAILURE);
//   }
//   
//   if (sigaction(SIGTERM, &SIGTERM_action, 0) < 0) {
//       perror("Failed to register signal handler\n");
//       exit(EXIT_FAILURE);
//   }
  
  int c;
  while(c!='q') {
      c = getchar();
      if('p' == c) ia->print_object();
  }
  
  return 0;
}
