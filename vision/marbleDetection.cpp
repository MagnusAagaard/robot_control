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

Mat img, im;
vector<Vec3f> marbles;

// Camera Callback
void cameraCallback(ConstImageStampedPtr &msg) {

  size_t width = msg->image().width();
  size_t height = msg->image().height();
  const char *data = msg->image().data().c_str();
  img(int(height), int(width), CV_8UC3, const_cast<char *>(data));

  img = img.clone();
  im = img.clone();
  cvtColor(im, im, CV_RGB2BGR);
}

// Egde detection
void cannyThreshold(){
    int ratio = 3;
    int lowThreshold = 0;
    int kernel_size = 3;
    string window_name = "Egde";

    namedWindow( window_name , WINDOW_AUTOSIZE);
    Canny( img, img, lowThreshold, lowThreshold*ratio, kernel_size);
}

void imageProcessing(){
    // Converting image to grayscale
    cv::cvtColor(img, img, COLOR_BGR2GRAY);

    // Applying a GaussianBlur to reduce noise and avoid false circle detection
    //GaussianBlur( img, img, Size(9, 9), 1, 1);

    // Applying a medianBlur and egde detection to reduce noise and avoid false circle detection
    medianBlur(img, img, 7);

    //showHistogram(img);

    int thresholdValueBin = 87;

    // Binary Vision to only detect the marble
    threshold(img, img, thresholdValueBin, 255, THRESH_BINARY);

    // Morphology
    erode(img, img, Mat());

    // Egde detection
    cannyThreshold();
}

// Constructing a vector to store the information of the marbles positions
vector<Vec3f> marbles;
mutex mtx;

void drawMarble(){
    cout << "marbles.size(): " << marbles.size() << endl;
    if (marbles.size() > 0){
        for (size_t i = 0; i < marbles.size(); i++) {
            Point center(cvRound(marbles[i][0]), cvRound(marbles[i][1]));
            int radius = cvRound(marbles[i][2]);
            // circle center
            //circle( im, center, 3, Scalar(0,255,0), -1, 8, 0 );
            // circle outline
            circle( im, center, radius, Scalar(255,0,0), 1, 8, 0 );
        }
    }
    mtx.lock();
    namedWindow("camera with marbles", WINDOW_AUTOSIZE);
    imshow("camera with marbles", im);
    mtx.unlock();
}

void detectMarble() {
    // Clear vector
    marbles.clear();

    // image Processing
    imageProcessing();

    int houghCirclesThreshold = 18;

    // Applying the Hough Circles Transform:
    HoughCircles( img, marbles, HOUGH_GRADIENT, 1, img.rows, houghCirclesThreshold, houghCirclesThreshold, 0, 0 );
    // dp = 1: The inverse ratio of resolution
    // min_dist = src_gray.rows/8: Minimum distance between detected centers
    // param_1 = 200: Upper threshold for the internal Canny edge detector
    // param_2 = 100*: Threshold for center detection.
    // min_radius = 0: Minimum radio to be detected. If unknown, put zero as default.
    // max_radius = 0: Maximum radius to be detected. If unknown, put zero as default

    // Draw the marbles
    drawMarble();
    waitKey();
}

int main(int argc, char **_argv) {

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

        //Detect Marbles
        detectMarble();
    }
    return 0;
}
