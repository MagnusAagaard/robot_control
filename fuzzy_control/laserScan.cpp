#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>

#include <iostream>
using namespace std;

double angle;
double minDistance;

void scanCallback(ConstLaserScanStampedPtr &msg)
{
  double angleMin = msg->scan().angle_min();
  double angleMax = msg->scan().angle_max();
  double angleStep = msg->scan().angle_step();
  double minVal = msg->scan().range_max();
  int index = 0;

  for(uint i = 0; i < msg->scan().count(); i++)
  {
    if(msg->scan().ranges(i) < minVal){
      minVal = msg->scan().ranges(i);
      index = i;
    }
  }
  angle = (angleMin + angleStep*index)/angleMax;
  minDistance = minVal;
}

int main(int _argc, char **_argv)
{
  // Load gazebo
  gazebo::client::setup(_argc, _argv);

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();

  // Listen to Gazebo topics
  gazebo::transport::SubscriberPtr scanSubscriber =
      node->Subscribe("~/pioneer2dx/hokuyo/link/laser/scan", scanCallback);

  gazebo::transport::PublisherPtr scanPublisher = node->Advertise<gazebo::msgs::Vector2d>("~/fuzzy_control/input");

  while(true){
    gazebo::common::Time::MSleep(10);
    ignition::math::Vector2d values(angle, minDistance);
    gazebo::msgs::Vector2d msg;
    gazebo::msgs::Set(&msg, values);
    scanPublisher->Publish(msg);
  }

  gazebo::client::shutdown();
  return 0;
}
