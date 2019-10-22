//Vision
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
using namespace cv;

#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
using namespace std;

Mat im, imGray;

// Camera Callback
void cameraCallback(ConstImageStampedPtr &msg) {

  size_t width = msg->image().width();
  size_t height = msg->image().height();
  const char *data = msg->image().data().c_str();
  im(int(height), int(width), CV_8UC3, const_cast<char *>(data));

  im = im.clone();
  cvtColor(im, im, CV_RGB2BGR);
}

vector<Vec3f> detectMarble (Mat im) {

    // Converting image to grayscale
    cvtColor(im, imGray, Size(9,9), 2, 2);

    // Applying a GaussianBlur to reduce noise and avoid false circle detection
    GaussianBlur( imGray, imGray, Size(9, 9), 2, 2 );

    // Constructing a vector to store the information of the marbles positions
    vector<Vec3f> marbles;

    // Applying the Hough Circles Transform:
    HoughCircles( imGray, marbles, CV_HOUGH_GRADIENT, dp=1, min_dist=imGray.rows/8, param_1=200, param_2=100, min_radius=0, max_radius=0 );
        // dp = 1: The inverse ratio of resolution
        // min_dist = src_gray.rows/8: Minimum distance between detected centers
        // param_1 = 200: Upper threshold for the internal Canny edge detector
        // param_2 = 100*: Threshold for center detection.
        // min_radius = 0: Minimum radio to be detected. If unknown, put zero as default.
        // max_radius = 0: Maximum radius to be detected. If unknown, put zero as default

    return marbles;
}

void drawMarble(Mat im, vector<Vec3f> marbles){
    if (marbles.size() > 0){
        for (size_t i = 0; i < marbles.size(); i++) {
            Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            // circle center
            circle( im, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( im, center, radius, Scalar(0,0,255), 3, 8, 0 );
        }
        mutex.lock();
        cv::imshow("camera", im);
        mutex.unlock();
    }
}

int int main(int argc, char **_argv) {

    // load gazebo
    gazebo::client::setup(_argc, _argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();

    // Listen to the camera
    gazebo::transport::SubscriberPtr cameraSubscriber =
        node->Subscribe("~/pioneer2dx/camera/link/camera/image", cameraCallback);

    while (true) {
        gazebo::common::Time::MSleep(10);

        // Drawing Marbles
        drawMarble(im, detectMarble(im));
    }
    return 0;
}
