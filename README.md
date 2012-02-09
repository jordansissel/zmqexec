# zmqexec

http : cgi :: zeromq : zmqexec

Listens with a REP socket for incoming requests. The message is presented to
the subprocess via stdin and the reply is expected to come via stdout.

Builds statically against libzmq and libuuid (required by libzmq). Requires,
libstdc++ though.
