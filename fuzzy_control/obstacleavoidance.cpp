//File: ObstacleAvoidance.cpp
#include "fl/Headers.h"
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <iostream>
#include <math.h>
#include <fstream>

using namespace std;

const char* path_to_csv = "/home/magnus/robot_control/fuzzy_control/tests/testworld/path.csv";

double currentAngle;
double currentDistance;
int vX;
int vY;
double w;
double z;

// --------------- CALLBACKS --------------------------

void inputCallback(ConstVector2dPtr &msg)
{
  currentAngle = msg->x();
  currentDistance = msg->y();
}

void poseCallback(ConstPosesStampedPtr &msg)
{
  std::ofstream myfile;
  myfile.open(path_to_csv, std::ios::out | std::ios::app);
  for (int i = 0; i < msg->pose_size(); i++)
  {
    if (msg->pose(i).name() == "pioneer2dx")
    {
        vX = (int)(msg->pose(i).position().x()*10);
        vY = (int)(msg->pose(i).position().y()*10);
				w = msg->pose(i).orientation().w();
				z = msg->pose(i).orientation().z();
        myfile << msg->pose(i).position().x() << ",";
        myfile << msg->pose(i).position().y() << std::endl;
        myfile.close();
    }
  }
}

// --------------- CALLBACKS --------------------------

float calc_angle(float z, float w)
{
	if (z < 0)
	{
		return (2*M_PI - (2 * acos(w)));
	}
	else
	{
		return (2 * acos(w));
	}
}

float calc_angle_to_point(float &cra, int goalx, int goaly)
{ //Fucker helt op her, AP stikker af
  //cout << "pX: " << p.x << endl;
  //cout << "pX: " << vX << endl;
  //cout << "pY: " << p.y << endl;
  //cout << "pX: " << vY << endl;

	float angle_to_point = atan2((goaly-vY), (goalx-vX));
  //cout << "AP: " << angle_to_point << endl;
	return (std::fmod((angle_to_point-cra + 3*M_PI), 2*M_PI)-M_PI);
}

int main(int argc, char* argv[]){
    using namespace fl;
    Engine* engine = FllImporter().fromFile("ObstacleAvoidance.fll");

    std::string status;
    if (not engine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);

    InputVariable* obstacle = engine->getInputVariable("obstacle");
    InputVariable* distance = engine->getInputVariable("distance");
    InputVariable* goal = engine->getInputVariable("goal");
    OutputVariable* steer = engine->getOutputVariable("mSteer");
    OutputVariable* speed = engine->getOutputVariable("mSpeed");

    // Load gazebo
    gazebo::client::setup(argc, argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();
    gazebo::transport::SubscriberPtr poseSubscriber = node->Subscribe("~/pose/info", poseCallback);

    // Listen to Gazebo topics
    gazebo::transport::SubscriberPtr inputSubscriber = node->Subscribe("~/fuzzy_control/input", inputCallback);

    // Publish to the robot vel_cmd topic
    gazebo::transport::PublisherPtr movementPublisher = node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");
    //testworld: (20,30),(20,20),(20,10),(20,0),(20,-10),(20,-20), (40,20)
    //angleworld: 49,1
    int gx = 40;
  	int gy = -20;
    std::ofstream myfile;
    myfile.open(path_to_csv, std::ios::out | std::ios::app);
    myfile << gx << "," << gy << std::endl;

    //float speed = 0.3f;
    //Ændre speed når vi skal dreje, jo skarpere jo lavere hastighed
    while(true){
      gazebo::common::Time::MSleep(10);
      float cra = calc_angle(z, w);
      float atp = calc_angle_to_point(cra, gx, gy)/M_PI;

      obstacle->setValue(currentAngle);
      distance->setValue(currentDistance);
      goal->setValue(atp);
      engine->process();

      FL_LOG("obstacle.input = " << Op::str(currentAngle) << " distance.input = " << Op::str(currentDistance) << " goal.input = " << Op::str(atp) << " => " << "steer.output = " << Op::str(steer->getValue()) << " => " << "speed.output = " << Op::str(speed->getValue()));

      // Generate a pose
      ignition::math::Pose3d pose(double(speed->getValue()), 0, 0, 0, 0, double(steer->getValue()));

      // Convert to a pose message
      gazebo::msgs::Pose msg;
      gazebo::msgs::Set(&msg, pose);
      movementPublisher->Publish(msg);


    }
}
