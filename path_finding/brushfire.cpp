#include <iostream>
#include <opencv2/highgui.hpp>
using namespace std;
using namespace cv;

int main()
{
  Mat img = imread("maps/smallworld.png", CV_LOAD_IMAGE_GRAYSCALE);
  Mat img2;
  if(! img.data ) {
      cout <<  "Could not open or find the image" << endl ;
      return -1;
    }
  cout << img.size() << endl;
  resize(img, img2, Size(img.width() * 10, img.height() * 10), 0, 0, INTER_NEAREST);
  //resize(img, img2,  Size(img.width*10,img.height*10),0,0,INTER_NEAREST);
  imshow("Display window", img);
  imshow("New", img2);
  waitKey(0);
  return 0;
}
