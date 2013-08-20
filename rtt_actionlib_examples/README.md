RTT Actionlib Examples
======================

This package contains examples which demonstrate the [Orocos
RTT](http://orocos.org) / [actionlib](http://ros.org/wiki/actionlib)
interfaces.

## Action Server

This example serves an actionlib action which counts a number of seconds, and
then reports back exactly how long it counted. As it's counting, it publishes
the percentage complete. This example can be run by running the sever in one
shell:

```bash
rosrun rtt_actionlib_examples test_server-gnulinux
```

Then you can easily call the server from another shell:
```bash
rosrun rtt_actionlib_examples test_client.py
```

You can see the implementation of the server logic as well as the main function
in [test_server.cpp](test_server.cpp).
