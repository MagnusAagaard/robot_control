#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include "ParticleFilter.h"

#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>


using namespace std;
using namespace cv;

int robotX;
int robotY;
double robotAngle;
double lin_vel = 0.0;
double ang_vel = 0.0;
double ranges[100];

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

void poseCallback(ConstPosesStampedPtr &_msg) {
  // Dump the message contents to stdout.
  //  std::cout << _msg->DebugString();

  for (int i = 0; i < _msg->pose_size(); i++) {
    if (_msg->pose(i).name() == "pioneer2dx") {
        robotX = (int)(_msg->pose(i).position().x()*10);
        robotY = (int)(_msg->pose(i).position().y()*10);
        robotAngle = calc_angle(_msg->pose(i).orientation().z(), _msg->pose(i).orientation().w());
    }
  }
}

void cmdCallback(ConstPosePtr &msg)
{
    ang_vel = msg->orientation().z() * 2;
    lin_vel = msg->position().x()*10;
    //std::cout << "Ang: " << ang_vel << ", Lin: " << lin_vel << std::endl;
}

void lidarCallback(ConstLaserScanStampedPtr &msg) {

  float angle_min = float(msg->scan().angle_min());
  //  double angle_max = msg->scan().angle_max();
  float angle_increment = float(msg->scan().angle_step());

  float range_min = float(msg->scan().range_min());
  float range_max = float(msg->scan().range_max());

  int nranges = msg->scan().ranges_size();
  int nintensities = msg->scan().intensities_size();

  assert(nranges == nintensities);
  for (int i = 0; i < nranges; i+=2) {
    //float angle = angle_min + i * angle_increment;
    float range = std::min(float(msg->scan().ranges(i)), range_max);
    ranges[i/2] = range;
        //std::cout << angle << " " << range << " " << intensity << std::endl;
  }
}

int main(int _argc, char **_argv)
{
    // Load gazebo
    gazebo::client::setup(_argc, _argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();
     gazebo::transport::SubscriberPtr poseSubscriber = node->Subscribe("~/pose/info", poseCallback);
     gazebo::transport::SubscriberPtr lidarSubscriber =
           node->Subscribe("~/pioneer2dx/hokuyo/link/laser/scan", lidarCallback);
     gazebo::transport::SubscriberPtr velSubscriber = node->Subscribe("~/pioneer2dx/vel_cmd", cmdCallback);

     const int scale = 1;
     Mat workspaceTmp = cv::imread("maps/smallworld.png", cv::IMREAD_GRAYSCALE);
     Mat workspace;
     resize(workspaceTmp, workspace, Size(workspaceTmp.cols*scale*10/1.41735, workspaceTmp.rows*scale*10/1.41735), 0, 0, cv::INTER_NEAREST);

     Mat workspaceTmpBGR = cv::imread("maps/smallworld.png", cv::IMREAD_COLOR);
     Mat workspaceBGR;
     resize(workspaceTmpBGR, workspaceBGR, Size(workspaceTmpBGR.cols * scale * 10 / 1.41735, workspaceTmpBGR.rows * scale * 10 / 1.41735), 0, 0, cv::INTER_NEAREST);

     double sigma_pos[3] = { 0.03, 0.03, 0.01 };
     double dt = 0.05;
     double initTheta = 0.0;
     gazebo::common::Time::MSleep(1000);
     ParticleFilter filter(workspace);
     Point initPoint(robotX + workspace.cols/2, robotY + workspace.rows/2);
     filter.initParticles(initPoint, initTheta, sigma_pos, 10);

    while(true){
        gazebo::common::Time::MSleep(dt*1000);
        filter.prediction(dt, sigma_pos, lin_vel, ang_vel);
        //filter.updateWeights(ranges);
        filter.showParticles(workspace);
        //cout << "Pos: " << robotX + workspace.cols/2 << ", " << robotY+workspace.rows/2 << ". Angle: " << robotAngle << endl;
    }

    gazebo::client::shutdown();

    imshow("image", workspace);
    cv::waitKey(0);

    return 0;
}
