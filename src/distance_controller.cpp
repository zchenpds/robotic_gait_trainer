#include "gait_training_robot/distance_controller.h"
#include "tf2_ros/transform_broadcaster.h"
#include "geometry_msgs/TransformStamped.h"
#include "angles/angles.h"

DistanceController::DistanceController():
  tf_base_to_pelvis_({}),
  estimated_state_({}),
  desired_state_({})
{
  nh_.subscribe("/distance_controller/cmd_vel_in", 1, &DistanceController::cmdVelCB, this);
  pub_cmd_vel_out_ = nh_.advertise<geometry_msgs::Twist>("/distance_controller/cmd_vel_out", 1);

  desired_state_.distance = 1.5;
  desired_state_.bearing = M_PI;
}

void DistanceController::cmdVelCB(const geometry_msgs::Twist & cmd_vel_in)
{
  tf::TransformListener listener;
  geometry_msgs::Twist cmd_vel_out = {};
  try
  {
    listener.lookupTransform("/base_link", "/skeleton_pelvis_link", ros::Time(0), tf_base_to_pelvis_);
    tf::Vector3 disp_vec =  tf_base_to_pelvis_.getOrigin();
    estimated_state_.distance = disp_vec.length();
    estimated_state_.bearing = angles::normalize_angle_positive(atan2(disp_vec.getY(), disp_vec.getX()));
    ROS_INFO_STREAM("Estimated state: " << estimated_state_);
    pub_cmd_vel_out_.publish(cmd_vel_out);
  }
  catch (tf::TransformException ex)
  {
    ROS_ERROR("%s", ex.what());
    ros::Duration(1.0).sleep();
    pub_cmd_vel_out_.publish(cmd_vel_out);
  }
  
  
}

int main(int argc, char ** argv)
{
  ros::init(argc, argv, "distance_controller");
  DistanceController dc;
  ros::spin();
  return 0;
}