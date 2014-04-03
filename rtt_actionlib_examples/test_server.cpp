
#include <iostream>
#include <rtt/deployment/ComponentLoader.hpp>
#include <rtt/plugin/PluginLoader.hpp>
#include <ocl/DeploymentComponent.hpp>

#include <ocl/TaskBrowser.hpp>
#include <rtt/os/main.h>

#include <rtt/RTT.hpp>
#include <rtt_actionlib/rtt_actionlib.h>
#include <rtt_actionlib/rtt_action_server.h>
#include <rtt/scripting/Scripting.hpp>

#include <actionlib/action_definition.h>
#include <rtt_actionlib_examples/SomeActionAction.h>
#include <rtt_rosclock/rtt_rosclock.h>

class SomeComponent : public RTT::TaskContext 
{
private:

  ACTION_DEFINITION(rtt_actionlib_examples::SomeActionAction);
  
  // Convenience typedefs
  typedef actionlib::ServerGoalHandle<rtt_actionlib_examples::SomeActionAction> GoalHandle;
  GoalHandle current_gh_;

  Feedback feedback_;
  Result result_;

  // RTT action server
  rtt_actionlib::RTTActionServer<rtt_actionlib_examples::SomeActionAction> rtt_action_server_;
public:

  // Component constructor
  SomeComponent(std::string name) :
    TaskContext(name, RTT::base::TaskCore::PreOperational)
  { 
    // Add action server ports to this task's root service
    rtt_action_server_.addPorts(this->provides());
    
    // Bind action server goal and cancel callbacks (see below)
    rtt_action_server_.registerGoalCallback(boost::bind(&SomeComponent::goalCallback, this, _1));
    rtt_action_server_.registerCancelCallback(boost::bind(&SomeComponent::cancelCallback, this, _1));
  }

  // RTT configure hook
  bool configureHook() {
    return true;
  }

  // RTT start hook
  bool startHook() {
    // Start action server
    rtt_action_server_.start();
    return true;
  }

  // RTT update hook
  void updateHook() {
    // Pursue goal...
    if(current_gh_.isValid() && current_gh_.getGoalStatus().status == actionlib_msgs::GoalStatus::ACTIVE) {
      double percent_complete = 
        100.0 *
        (rtt_rosclock::host_now() - current_gh_.getGoalID().stamp).toSec() /
        (current_gh_.getGoal()->delay_time.toSec());

      // Publish feedback
      feedback_.percent_complete = percent_complete;
      current_gh_.publishFeedback(feedback_);

      // Set succeded if complete
      if(percent_complete >= 100.0) {
        result_.actual_delay_time = rtt_rosclock::host_now() - current_gh_.getGoalID().stamp;
        current_gh_.setSucceeded(result_);
      }
    }
  }
  
  // Accept/reject goal requests here
  void goalCallback(GoalHandle gh) {
    // Always preempt the current goal and accept the new one
    if(current_gh_.isValid() && current_gh_.getGoalStatus().status == actionlib_msgs::GoalStatus::ACTIVE) {
      result_.actual_delay_time = rtt_rosclock::host_now() - current_gh_.getGoalID().stamp;
      current_gh_.setCanceled(result_);
    }
    gh.setAccepted();
    current_gh_ = gh;
  }

  // Handle preemption here
  void cancelCallback(GoalHandle gh) {
    if(current_gh_ == gh && current_gh_.getGoalStatus().status == actionlib_msgs::GoalStatus::ACTIVE) {
      result_.actual_delay_time = rtt_rosclock::host_now() - current_gh_.getGoalID().stamp;
      current_gh_.setCanceled(result_);
    }
  }
};


using namespace RTT::corba;
using namespace RTT;

int ORO_main(int argc, char** argv)
{
  // Get a pointer to the component above
  OCL::DeploymentComponent deployer("deployer");

  RTT::Logger::Instance()->setLogLevel(RTT::Logger::Debug);
  
  // Import the necessary plugins
  deployer.import("rtt_ros");
  deployer.getProvider<RTT::Scripting>("scripting")->eval("ros.import(\"rtt_actionlib_examples\");");
  
  // Create an instance of our component
  SomeComponent some_component("some_component");
  deployer.addPeer(&some_component);

  // Connect the component to some ros topics
  std::string script = "\
    loadService(\"some_component\",\"actionlib\");\
    some_component.actionlib.connect(\"/some/ros/namespace/my_action\");";
  deployer.getProvider<RTT::Scripting>("scripting")->eval(script);

  // Create an activity for processing the action
  Activity activity( 15, 0.05 ); // priority=15, period=20Hz
  some_component.setActivity(&activity);

  // Start the server
  some_component.configure();
  some_component.start();

  // Interactive task browser
  OCL::TaskBrowser browse( &deployer );
  browse.loop();

  return 0;
} 



