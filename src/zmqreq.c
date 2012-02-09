#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char *argv[0]) {
  void *context = zmq_init (1);
  void *socket = zmq_socket(context, ZMQ_REQ);

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <zmq endpoint>\n", argv[0]);
    return 1;
  }
  zmq_connect(socket, argv[1]);

  for (;;) {
    char buf[4096];
    size_t len;
    int rc;
    fgets(buf, 4096, stdin);
    if (feof(stdin)) {
      break;
    }
    len = strlen(buf);
    zmq_msg_t request;
    zmq_msg_init_size(&request, len);
    memcpy(zmq_msg_data(&request), buf, len);
    printf("Sending: %.*s\n", (int)len, buf);
    rc = zmq_send(socket, &request, 0);
    printf("rc: %d: %s\n", rc, zmq_strerror(rc));
    zmq_msg_close (&request);

    //  Send reply back to client
    zmq_msg_t reply;
    zmq_msg_init(&reply);
    zmq_recv(socket, &reply, 0);
    printf("Read: %.*s\n", (int)zmq_msg_size(&reply), (char *)zmq_msg_data(&reply));
    zmq_msg_close (&reply);
  }
  
  zmq_close (socket);
  zmq_term (context);
  return 0;
}
