#include "fl/Headers.h"
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <iostream>
using namespace std;

double vX;
double vY;
double aW;
double aZ;

void poseCallback(ConstPosesStampedPtr &msg)
{
  for (int i = 0; i < msg->pose_size(); i++)
  {
    if (msg->pose(i).name() == "pioneer2dx")
    {
        vX = msg->pose(i).position().x();
        vY = msg->pose(i).position().y();
        aW = msg->pose(i).orientation().w();
        aZ = msg->pose(i).orientation().z();
    }
  }
}

int main(int argc, char* argv[])
{
  using namespace fl;

  // Load gazebo
  gazebo::client::setup(argc, argv);

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();

  gazebo::transport::SubscriberPtr poseSubscriber =
      node->Subscribe("~/pose/info", poseCallback);

  while (true)
  {
    FL_LOG("x = " << Op::str(vX) << " y = " << Op::str(vY));
  }

}
