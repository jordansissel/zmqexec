# zmqexec

http : cgi :: zeromq : zmqexec

Listens with a REP socket for incoming requests. The message is presented to
the subprocess via stdin and the reply is expected to come via stdout.

Builds statically against libzmq and libuuid (required by libzmq). Requires,
libstdc++ though.

Original goal was to be able to use zmq's reqrep from logstash as a filter
which calls out to an external source. This 'zmqexec' program allows you
to make any script or tool act as a filter in logstash, now.
