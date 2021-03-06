//
// Created by karadalex on 9/8/20.
//

#ifndef SRC_TRAJECTORYEXECUTION_H
#define SRC_TRAJECTORYEXECUTION_H

#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit_msgs/CollisionObject.h>
#include <moveit_visual_tools/moveit_visual_tools.h>
#include <tf2/LinearMath/Quaternion.h>
#include <std_msgs/Float64.h>
#include <vector>

using namespace std;

geometry_msgs::Pose getPoseFromPathPoint(vector<float> pathPoint);

class TrajectoryExecution {
public:
	moveit::planning_interface::MoveGroupInterface move_group = moveit::planning_interface::MoveGroupInterface("iiwa_arm");
	moveit_visual_tools::MoveItVisualTools visual_tools = moveit_visual_tools::MoveItVisualTools("world");
	Eigen::Isometry3d text_pose;
	const robot_state::JointModelGroup* joint_model_group;

	/**
	 * Prepare to execute new trajection. Initialize move_group and visual_tools provided by moveit
	 * @param PLANNING_GROUP: string, name of robot to plan
	 * @param pos_tolerance: double, position tolerance
	 * @param orient_tolerance: double, orientation tolerance
	 * @param plan_time_sec: int, planning time in seconds
	 * @param replanning: bool, allow for replanning
	 * @param plan_attempts: int, planning attempts
	 * @param base_frame: string, base reference frame
	 */
	TrajectoryExecution(const string PLANNING_GROUP, double pos_tolerance, double orient_tolerance,
											int plan_time_sec, bool replanning, int plan_attempts,
											const string base_frame);

 	/**
 	 * Simple execution of path. Path input is given in the format: X Y Z Roll Pitch Yaw
 	 * @param path
 	 */
	void executePath(vector<vector<float>> path);

	void executePath(vector<geometry_msgs::Pose> path);

	/**
	 * Calculate and execute a cartesian trajectory between the given waypoints
	 * @param waypoints
	 * @param traj_name
	 */
	void executeCartesianPath(vector<geometry_msgs::Pose> waypoints, const char* traj_name);
};

#endif //SRC_TRAJECTORYEXECUTION_H
