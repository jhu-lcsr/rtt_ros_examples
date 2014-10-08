
--[ Set log-level ]--
rtt.setLogLevel("Warning")

--[ get convenience objects ]--
gs = gs or rtt.provides()
tc = tc or rtt.getTC()
depl = depl or tc:getPeer("Deployer")

--[ required imports ]--
depl:import("rtt_ros")
depl:import("rtt_ros")
ros = gs:provides("ros")
ros:import("rtt_ros_integration_example")

--[ Load a HelloRobot component ]--
depl:loadComponent("hello_robot","HelloRobot")
hr = depl:getPeer("hello_robot")

--[ Give it a periodic activity ]--
depl:setActivity(
  hr:getName(),
  0.0,
  depl:getAttribute("LowestPriority"):get(),
  rtt.globals.ORO_SCHED_OTHER)

--[ Create connections to some ros topics ]--
depl:stream(hr:getName()..".float_out", ros:topic("float_out"))
depl:stream(hr:getName()..".float_in", ros:topic("float_in"))
depl:stream(hr:getName()..".string_out", ros:topic("string_out"))
depl:stream(hr:getName()..".string_in", ros:topic("string_in"))

--[ Connect the component to some ros topics ]--
hr:loadService("rosservice")
hr:provides("rosservice"):connect("increment", "/increment", "std_srvs/Empty")
hr:provides("rosservice"):connect( "updated", "/updated", "std_srvs/Empty")

--[ Configure and start the component ]--
hr:configure()
hr:start()
