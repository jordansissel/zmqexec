//
//  Hello World server
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (void) {
  void *context = zmq_init (1);
  void *socket = zmq_socket(context, ZMQ_REP);
  zmq_bind(socket, "tcp://*:5555");

  for (;;) {
    zmq_msg_t request;
    zmq_msg_init (&request);
    zmq_recv (socket, &request, 0);
    printf ("Received Hello\n");
    zmq_msg_close (&request);

    //  Send reply back to client
    zmq_msg_t reply;
    zmq_msg_init_size (&reply, 5);
    memcpy (zmq_msg_data (&reply), "World", 5);
    zmq_send (socket, &reply, 0);
    zmq_msg_close (&reply);
  }
  
  zmq_close (socket);
  zmq_term (context);
  return 0;
}
