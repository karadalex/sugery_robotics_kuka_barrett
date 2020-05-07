#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/CollisionObject.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <tf2/LinearMath/Quaternion.h>
#include <std_msgs/Float64.h>

using namespace std;

int main(int argc, char** argv)
{
  ros::init(argc, argv, "robot_planner2");
  ros::NodeHandle node_handle;
  ros::AsyncSpinner spinner(1);
  spinner.start();

	// Setup Move group
  static const std::string PLANNING_GROUP = "iiwa_arm";
  moveit::planning_interface::MoveGroupInterface move_group(PLANNING_GROUP);
	move_group.setGoalPositionTolerance(0.00005);
	move_group.setGoalOrientationTolerance(0.00005);
	move_group.setPlanningTime(10);
	move_group.allowReplanning(true);

	// The package MoveItVisualTools provides many capabilties for visualizing objects, robots,
	// and trajectories in RViz as well as debugging tools such as step-by-step introspection of a script
	namespace rvt = rviz_visual_tools;
	moveit_visual_tools::MoveItVisualTools visual_tools("world");
	visual_tools.deleteAllMarkers();
	// Remote control is an introspection tool that allows users to step through a high level script
	// via buttons and keyboard shortcuts in RViz
	visual_tools.loadRemoteControl();
	// RViz provides many types of markers, in this demo we will use text, cylinders, and spheres
	Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
	text_pose.translation().z() = 1.75;
	visual_tools.publishText(text_pose, "MoveGroupInterface Demo", rvt::WHITE, rvt::XLARGE);
	// Batch publishing is used to reduce the number of messages being sent to RViz for large visualizations
	visual_tools.trigger();

	// Raw pointers are frequently used to refer to the planning group for improved performance.
	const robot_state::JointModelGroup* joint_model_group = move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);

	vector<vector<float>> path;

	// X Y Z Roll Pitch Yaw
	// path.push_back({0, 0, 2.262, 0, 0, 0}); // For z >= 2.261 the robot reaches end of workspace, which is a signularity and cant be calculated from the numerical IK
	path.push_back({0, 0, 2.26, 0, 0, 0}); // Home position

	// Pick position 1
	path.push_back({0, -0.7236, 1.5, M_PI, 0, -M_PI_2});
	path.push_back({0, -0.7236, 1.21, M_PI, 0, -M_PI_2});
	path.push_back({0, -0.7236, 1.5, M_PI, 0, -M_PI_2});

	// Place position 1
	path.push_back({-0.33, 0.6739, 1.58, 0.019, 0.711, 0.072});

	geometry_msgs::Pose target_pose;
	tf2::Quaternion quaternion;
	moveit::planning_interface::MoveGroupInterface::Plan my_plan;

	for (int i = 0; i < path.size(); ++i) {
		// Arm Kinematics (KUKA iiwa)
		vector<float> pose = path.at(i);

		target_pose.position.x = pose[0];
		target_pose.position.y = pose[1];
		target_pose.position.z = pose[2];

		quaternion.setRPY(pose[3], pose[4], pose[5]);
		target_pose.orientation.w = quaternion.getW();
		target_pose.orientation.x = quaternion.getX();
		target_pose.orientation.y = quaternion.getY();
		target_pose.orientation.z = quaternion.getZ();

		move_group.setPoseTarget(target_pose);

		bool success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);
		ROS_INFO_NAMED("robot_planner2", "Visualizing plan %d (pose goal) %s", i, success ? "SUCCESS" : "FAILED");
		// Visualize plan
		ROS_INFO_NAMED("robot_planner2", "Visualizing plan %d as trajectory line", i);
		visual_tools.publishAxisLabeled(target_pose, "pose");
		visual_tools.publishText(text_pose, "Pose Goal", rvt::WHITE, rvt::XLARGE);
		visual_tools.publishTrajectoryLine(my_plan.trajectory_, joint_model_group);
		visual_tools.trigger();
		
		std::string nextButtonMsg = "Press 'next' in the RvizVisualToolsGui window to execute plan";
		visual_tools.prompt(nextButtonMsg);

		move_group.execute(my_plan);
	}

  ros::shutdown();
  return 0;
}