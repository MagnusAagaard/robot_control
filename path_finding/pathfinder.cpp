#include <opencv2/opencv.hpp>

#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>

#include<iostream>
#include <math.h>
#include <vector>
using namespace std;
using namespace cv;

#define PI 3.14159265

int vX;
int vY;
double w;
double z;

void poseCallback(ConstPosesStampedPtr &msg)
{
  for (int i = 0; i < msg->pose_size(); i++)
  {
    if (msg->pose(i).name() == "pioneer2dx")
    {
        vX = (int)msg->pose(i).position().x()*10;
        vY = (int)msg->pose(i).position().y()*10;
				w = msg->pose(i).orientation().w();
				z = msg->pose(i).orientation().z();
    }
  }
}

void brushFire(Mat& workspace, Mat& output, Point& start, Point& goal)
{
	for (int i = 0; i < workspace.rows; i++) {
		for (int j = 0; j < workspace.cols; j++) {
			if (workspace.at<ushort>(i, j) == 0 || i == 0 || j == 0 || i == workspace.rows - 1 || j == workspace.cols - 1)
				output.at<ushort>(i, j) = 1;
		}
	}
	output.at<ushort>(goal) = 2;

	double minimum = 0;
	double maximum = 2;
	while (minimum == 0) {
		for (int i = 0; i < output.rows; i++) {
			for (int j = 0; j < output.cols; j++) {
				if (output.at<ushort>(i, j) == maximum) {
					for (int k = -1; k < 2; k++) {
						for (int l = -1; l < 2; l++) {
							if (i + k >= 0 && i + k < output.rows && j + l >= 0 && j + l < output.cols) {
								if (output.at<ushort>(i + k, j + l) == 0) {
									if (maximum <= 65535)
										output.at<ushort>(i + k, j + l) = maximum + 1;
									else
										output.at<ushort>(i + k, j + l) = maximum;
								}
							}
						}
					}
				}
			}
		}
		minMaxLoc(output, &minimum, &maximum);
	}
	output.at<ushort>(start) = 0;
	cout << "Brushfire done" << endl;
}

void fixBorder(Mat &workspace)
{
	for (int j = 0; j < workspace.cols; j++) {
		bool seenBlack = false;
		int tmpRow = 0;
		while (!seenBlack && tmpRow < workspace.rows)
		{
			if (workspace.at<ushort>(tmpRow, j) == 0) {
				seenBlack = true;
			}
			else {
				workspace.at<ushort>(tmpRow, j) = 0;
				tmpRow++;
			}
		}
	}
	for (int j = workspace.cols-1; j >= 0; j--) {
		bool seenBlack = false;
		int tmpRow = workspace.rows-1;
		while (!seenBlack && tmpRow >= 0)
		{
			if (workspace.at<ushort>(tmpRow, j) == 0) {
				seenBlack = true;
			}
			else {
				workspace.at<ushort>(tmpRow, j) = 0;
				tmpRow--;
			}
		}
	}
	for (int j = 0; j < workspace.rows; j++) {
		bool seenBlack = false;
		int tmpCol = 0;
		while (!seenBlack && tmpCol < workspace.cols)
		{
			if (workspace.at<ushort>(j, tmpCol) == 0) {
				seenBlack = true;
			}
			else {
				workspace.at<ushort>(j, tmpCol) = 0;
				tmpCol++;
			}
		}
	}
	for (int j = workspace.rows - 1; j >= 0; j--) {
		bool seenBlack = false;
		int tmpCol = workspace.cols - 1;
		while (!seenBlack && tmpCol >= 0)
		{
			if (workspace.at<ushort>(j,tmpCol) == 0) {
				seenBlack = true;
			}
			else {
				workspace.at<ushort>(j,tmpCol) = 0;
				tmpCol--;
			}
		}
	}

}

float distanceToGoal(Point &p, Point &goal)
{
	return ((float)sqrt((pow((p.x - goal.x), 2) + pow((p.y - goal.y), 2))));
}

Point findMin(Mat &brushfire, Point& p, Point &goal)
{
	Point currentPoint = p;
	int tmpMin = 5000;
	Point minPoint;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			int tmpX = currentPoint.x + i;
			int tmpY = currentPoint.y + j;
			Point tmpPoint(tmpX, tmpY);
			if (tmpPoint != p && brushfire.at<ushort>(tmpPoint) < tmpMin && brushfire.at<ushort>(tmpPoint) != 1) {
				tmpMin = brushfire.at<ushort>(tmpPoint);
				minPoint = tmpPoint;
			}
			else if (brushfire.at<ushort>(tmpPoint) != 1 && distanceToGoal(tmpPoint, goal) < distanceToGoal(minPoint, goal)) {
				tmpMin = brushfire.at<ushort>(tmpPoint);
				minPoint = tmpPoint;
			}
		}
	}
	return minPoint;
}

void waveFrontPlanner(Mat &workspace, Mat &brushfire, Point &start, Point &goal, vector<Point> &path)
{
	Point minPoint = findMin(brushfire, start, goal);
	Point currentPoint = minPoint;
	brushfire.at<ushort>(start) = brushfire.at<ushort>(minPoint);
	currentPoint = minPoint;

	while (brushfire.at<ushort>(currentPoint) != 2)
	{
		Point newPoint = findMin(brushfire, currentPoint, goal);
		workspace.at<ushort>(newPoint) = 0;
		path.push_back(newPoint);
		currentPoint = newPoint;
	}
	cout << "Planning done!" << endl;
}

float calc_angle(float z, float w)
{
	if (z < 0)
	{
		return (360 - (2 * acos(w)*180.0 / PI));
	}
	else
	{
		return (2 * acos(w)*180.0 / PI);
	}
}

float calc_angle_to_point(float &cra, Point &p)
{
	float angle_to_point = atan2((p.x - vX), (p.y - vY)) * 180 / PI;
	return (angle_to_point - cra);
}

void generateCmd(ignition::math::Pose3d &pose, vector<Point> &path)
{
	if(path.size() < 1){
		cout << "Goal reached!" << endl;
		return;
	}
	Point robotPos(vX, vY);
	float speed = 0.0f;
	float dir = 0.0f;
	float currentRobotAngle = calc_angle(z, w);
	float angleBetweenPoints = calc_angle_to_point(currentRobotAngle, path[0]);
	if(angleBetweenPoints > 10)
	{
		dir = 0.3f;
	}
	else if(angleBetweenPoints < -10)
	{
		dir = -0.3f;
	}
	else if(distanceToGoal(path[0], robotPos) > 3)
	{
		speed = 0.3f;
	}
	else
	{
		path.erase(path.begin());
	}
	pose.Set(double(speed), 0, 0, 0, 0, double(dir));
}

int main()
{
	//Gazebo setup
	gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();
	gazebo::transport::SubscriberPtr poseSubscriber = node->Subscribe("~/pose/info", poseCallback);
	// Publish to the robot vel_cmd topic
	gazebo::transport::PublisherPtr movementPublisher = node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");

	// Publish a reset of the world
	gazebo::transport::PublisherPtr worldPublisher = node->Advertise<gazebo::msgs::WorldControl>("~/world_control");
	gazebo::msgs::WorldControl controlMessage;
	controlMessage.mutable_reset()->set_all(true);
	worldPublisher->WaitForConnection();
	worldPublisher->Publish(controlMessage);

	//Plan
	Mat workspaceTmp = imread("maps/smallworld.png", IMREAD_GRAYSCALE);
	Mat workspace;
	resize(workspaceTmp, workspace, Size(workspaceTmp.cols*3.33, workspaceTmp.rows*3.33), 0, 0, INTER_NEAREST);
	Mat workspace16bit;
	workspace.convertTo(workspace16bit, CV_16U, 255);
	Mat brushfire(workspace16bit.size(), workspace16bit.type(), Scalar(0));
	fixBorder(workspace16bit);
	int gx = 50;
	int gy = workspace16bit.rows / 2;
	int sx = workspace16bit.cols/2;
	int sy = workspace16bit.rows/2;
	Point goal(gx, gy);
	Point start(sx, sy);
	brushFire(workspace16bit, brushfire, start, goal);
	vector<Point> path;
	waveFrontPlanner(workspace16bit, brushfire, start, goal, path);
	//imshow("image", workspace16bit);
	//waitKey(0);

	while(true){
		gazebo::common::Time::MSleep(10);
		// Generate a pose
		ignition::math::Pose3d pose;
		generateCmd(pose, path);

    // Convert to a pose message
    gazebo::msgs::Pose msg;
    gazebo::msgs::Set(&msg, pose);
    movementPublisher->Publish(msg);
	}

	gazebo::client::shutdown();
	return 0;
}
