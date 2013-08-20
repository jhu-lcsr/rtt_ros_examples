#!/usr/bin/env python

import rospy
import rtt_actionlib_examples.msg
import actionlib

if __name__ == '__main__':
    rospy.init_node('some_action_client')

    client = actionlib.SimpleActionClient('/some/ros/namespace/my_action', 
            rtt_actionlib_examples.msg.SomeActionAction)

    client.wait_for_server()

    goal = rtt_actionlib_examples.msg.SomeActionGoal(rospy.Duration(3.0))

    print("Sending a goal to wait for 3.0 seconds...")
    client.send_goal(goal)

    client.wait_for_result()

    # Prints out the result of executing the action
    print(str(client.get_result()))
