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
    printf("Receiving...\n");
    zmq_recv (socket, &request, 0);
    printf("Received %.*s\n", zmq_msg_size(&request), zmq_msg_data(&request));

    process(argc, argv, zmq_msg_size(&request), zmq_msg_data(&data));

    zmq_msg_t reply;
    zmq_msg_init_size(&reply, zmq_msg_size(&request));
    memcpy (zmq_msg_data(&reply), zmq_msg_data(&request), zmq_msg_size(&request));
    zmq_send (socket, &reply, 0);
    zmq_msg_close (&request);
    zmq_msg_close (&reply);
  }
  
  zmq_close (socket);
  zmq_term (context);
  return 0;
}

void process(int argc, char *argv[], int size, char *data) {
  /* TODO(sissel): fork+execve the program
   * emit the message data to stdin
   * read the reply from stdout. */
}
