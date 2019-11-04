//File: ObstacleAvoidance.cpp
#include "fl/Headers.h"
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <iostream>
#include <math.h>

using namespace std;

double currentAngle;
double currentDistance;

int vX;
int vY;
double w;
double z;

int goalX = 5;
int goalY = -5;
double robotToGoalX;
double robotToGoalY;

double robotUnitVX;
double robotUnitVY;

double angleToGoal;

// --------------- CALLBACKS --------------------------

void inputCallback(ConstVector2dPtr &msg)
{
  currentAngle = msg->x();
  currentDistance = msg->y();
}

void poseCallback(ConstPosesStampedPtr &msg)
{
  for (int i = 0; i < msg->pose_size(); i++)
  {
    if (msg->pose(i).name() == "pioneer2dx")
    {
        vX = (int)(msg->pose(i).position().x()*10);
        vY = (int)(msg->pose(i).position().y()*10);
				w = msg->pose(i).orientation().w();
				z = msg->pose(i).orientation().z();
    }
  }
}

// --------------- CALLBACKS --------------------------

float calc_angle(float z, float w)
{
	if (z < 0)
	{
		return (360 - (2 * acos(w)*180.0 / M_PI));
	}
	else
	{
		return (2 * acos(w)*180.0 / M_PI);
	}
}

double calcFuzzyAngle()
{
  double robotDir = calc_angle(z, w);

  robotToGoalX = goalX - vX;
  robotToGoalY = goalY - vY;

  robotUnitVX = cos(robotDir) + vX;
  robotUnitVY = sin(robotDir) + vY;

  double tmpRobot = atan2(robotUnitVY,robotUnitVX) * 180 /M_PI;
  double tmpGoal = atan2(robotToGoalY,robotToGoalX) * 180 / M_PI;

  angleToGoal = tmpRobot - tmpGoal;
  return angleToGoal / 180;
}

int main(int argc, char* argv[]){
    using namespace fl;
    Engine* engine = FllImporter().fromFile("ObstacleAvoidance.fll");

    std::string status;
    if (not engine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);

    InputVariable* obstacle = engine->getInputVariable("obstacle");
    InputVariable* returnPoint = engine->getInputVariable("returnPoint");
    InputVariable* distance = engine->getInputVariable("distance");
    OutputVariable* steer = engine->getOutputVariable("mSteer");
    OutputVariable* speed = engine->getOutputVariable("mSpeed");

    // Load gazebo
    gazebo::client::setup(argc, argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // Listen to Gazebo topics
    gazebo::transport::SubscriberPtr inputSubscriber = node->Subscribe("~/fuzzy_control/input", inputCallback);

    // Publish to the robot vel_cmd topic
    gazebo::transport::PublisherPtr movementPublisher = node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");

    // Subscribe to the pose topic
    gazebo::transport::SubscriberPtr poseSubscriber = node->Subscribe("~/pose/info", poseCallback);

    //float speed = 0.3f;
    //Ændre speed når vi skal dreje, jo skarpere jo lavere hastighed
    while(true){
      gazebo::common::Time::MSleep(10);

      cout << "fuzzy input angle to goal: " << calcFuzzyAngle() << endl;

      obstacle->setValue(currentAngle);
      distance->setValue(currentDistance);
      returnPoint->setValue(calcFuzzyAngle());
      engine->process();
      // Generate a pose
      ignition::math::Pose3d pose(double(speed->getValue()), 0, 0, 0, 0, double(steer->getValue()));

      // Convert to a pose message
      gazebo::msgs::Pose msg;
      gazebo::msgs::Set(&msg, pose);
      movementPublisher->Publish(msg);

      //FL_LOG("obstacle.input = " << Op::str(currentAngle) << " distance.input = " << Op::str(currentDistance) << " => " << "steer.output = " << Op::str(steer->getValue()));
    }
}
