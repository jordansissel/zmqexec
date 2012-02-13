#define _GNU_SOURCE
#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void process(int argc, char *argv[], int size, char *data, void *socket);
int write_all(int fd, const void *buf, size_t count);

int main (int argc, char *argv[]) {
  void *context = zmq_init (1);
  void *socket = zmq_socket(context, ZMQ_REP);
  char *port;
  char *address;
  int rc;

  // check argc instead?
  // need to make sure port is an int really
  if(argc != 2) {
    port = "5555";
    fprintf(stderr, "using default port: %s\n", port);
  }
  else
  {
    port = argv[1];
    fprintf(stderr, "using explicit port: %s\n", port);
  }
  asprintf(&address, "tcp://*:%s", port);
  zmq_bind(socket, address);

  for (;;) {
    zmq_msg_t request;
    zmq_msg_init (&request);
    printf("Reading from zmq...\n");
    rc = zmq_recv(socket, &request, 0);
    if (rc != 0) {
      fprintf(stderr, "zmq_recv error: %s\n", zmq_strerror(rc));
      fprintf(stderr, "restarting...\n");
      execvp(argv[0], argv);
      break;
    }
    printf("Received %.*s\n", (int)zmq_msg_size(&request), (char *)zmq_msg_data(&request));

    /* skip the process file name (argv[0]) and pass the rest as the intended command 
     * the process method will reply on the socket */
    process(argc - 1, argv + 1, zmq_msg_size(&request), zmq_msg_data(&request), socket);
    zmq_msg_close(&request);
    sleep(1);
  }
  
  zmq_close (socket);
  zmq_term (context);
  free(address);
  return 0;
}

void process(int argc, char *argv[], int size, char *data, void *socket) {
  int childin[2], childout[2]; /*, childerr[2];*/
  pid_t pid;

  /* fork+execve the program
   * emit the message data to stdin
   * read the reply from stdout. */
  
  pipe(childin);
  pipe(childout);
  //pipe(childerr);

  fprintf(stderr, "Running command: %s\n", argv[0]);
  pid = fork();
  if (pid == 0) {
    close(childin[1]);
    close(childout[0]);
    //close(childerr[0]);
    dup2(childin[0], 0);
    dup2(childout[1], 1);
    //dup2(childerr[1], 2);

    /* child stderr passes through to normal stderr */
    execvp(argv[0], argv);

    /* if we get here, execvp failed */
    fprintf(stderr, "execvp('%s', ...) failed: [%d] %s\n", argv[0],
            errno, strerror(errno));
    exit(1); /* in case the execvp fails */
  } else {
    /* parent */
    zmq_msg_t reply;
    close(childin[0]);
    close(childout[1]);
    //close(childerr[1]);

    /* TODO(sissel): Check return value of write_all */
    write_all(childin[1], data, size);
    close(childin[1]); /* close stdin to the child now */

    /* TODO(sissel): Read result from stdout */
    char buf[4096];
    ssize_t bytes;
    int rc;
    for (int eof = 0;!eof;) {
      /* read until we run out of things to read */
      //fprintf(stderr, "Reading from child...\n");
      bytes = read(childout[0], buf, 4096);
      if (bytes == 0) { /* eof */
        eof = 1;
      } else {
        printf("subprocess: '%.*s'\n", (int)bytes, buf);
        zmq_msg_init_size(&reply, bytes);
        memcpy(zmq_msg_data(&reply), buf, bytes);
        rc = zmq_send(socket, &reply, ZMQ_SNDMORE);
        if (rc != 0) {
          fprintf(stderr, "zmq_send error: %s\n", zmq_strerror(rc));
        }
        zmq_msg_close(&reply);
      }
    }

    /* Done reading stdout from our child. 
     * At this point, child's stdout has given EOF,
     * Reap the children! */
    int status;
    close(childout[0]);
    //fprintf(stderr, "Waiting for child(%d) to exit\n", pid);
    waitpid(pid, &status, 0);

    /* Do something with status code? */

    /* Send an empty message without ZMQ_SNDMORE to indicate
     * that we are done with this reply */
    zmq_msg_init_size(&reply, 0);
    zmq_send(socket, &reply, 0);
    zmq_msg_close(&reply);
  }
} /* process */

/* Writes all bytes from a buffer to an fd.
 * Returns 0 on success, errno on error */
int write_all(int fd, const void *buf, size_t size) {
  /* Write the message to the child's stdin */
  size_t remaining = size;
  while (remaining > 0) {
    int bytes;
    bytes = write(fd, buf, size);
    if (bytes < 0) {
      /* TODO(sissel): Handle the error, probably should abort. */
      return errno;
    } else {
      remaining -= bytes;
    }
  }
  return 0;
} /* write_all */