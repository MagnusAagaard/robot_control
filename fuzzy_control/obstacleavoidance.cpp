//File: ObstacleAvoidance.cpp
#include "fl/Headers.h"
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <iostream>
using namespace std;

double currentAngle;
double currentDistance;

void inputCallback(ConstVector2dPtr &msg)
{
  currentAngle = msg->x();
  currentDistance = msg->y();
}

int main(int argc, char* argv[]){
    using namespace fl;
    Engine* engine = FllImporter().fromFile("ObstacleAvoidance.fll");

    std::string status;
    if (not engine->isReady(&status))
        throw Exception("[engine error] engine is not ready:n" + status, FL_AT);

    InputVariable* obstacle = engine->getInputVariable("obstacle");
    InputVariable* distance = engine->getInputVariable("distance");
    OutputVariable* steer = engine->getOutputVariable("mSteer");

    // Load gazebo
    gazebo::client::setup(argc, argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // Listen to Gazebo topics
    gazebo::transport::SubscriberPtr inputSubscriber =
        node->Subscribe("~/fuzzy_control/input", inputCallback);

    // Publish to the robot vel_cmd topic
    gazebo::transport::PublisherPtr movementPublisher =
            node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");


    float speed = 0.3f;
    //Ændre speed når vi skal dreje, jo skarpere jo lavere hastighed
    while(true){
      gazebo::common::Time::MSleep(10);
      obstacle->setValue(currentAngle);
      distance->setValue(currentDistance);
      engine->process();
      // Generate a pose
      ignition::math::Pose3d pose(double(speed), 0, 0, 0, 0, double(steer->getValue()));
      FL_LOG("obstacle.input = " << Op::str(currentAngle) << " distance.input = " << Op::str(currentDistance) << " => " << "steer.output = " << Op::str(steer->getValue()));

      // Convert to a pose message
      gazebo::msgs::Pose msg;
      gazebo::msgs::Set(&msg, pose);
      movementPublisher->Publish(msg);
      //FL_LOG("obstacle.input = " << Op::str(currentAngle) << " distance.input = " << Op::str(currentDistance) << " => " << "steer.output = " << Op::str(steer->getValue()));
    }
}
