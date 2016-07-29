#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Component.h"

std::string window_name = "Yonder Dynamics RHUD";
int width = 1080;
int height = 720;
int source = 1;

int prevX, prevY;
bool mouse_down = false;
int selected = -1;

using namespace cv;

std::vector<Component*> comps;

int getSelection(int x, int y) {
  if (comps.size() == 0) return -1;

  if (selected >= 0) {
    Point p = comps[selected]->getPoint();
    Size s = comps[selected]->getSize();
    if (x > p.x && x < p.x + s.width && y > p.y && y < p.y + s.height) 
      return selected;
  }

  for (int i = 0; i < comps.size(); ++i) {
    Point p = comps[i]->getPoint();
    Size s = comps[i]->getSize();

    if (x > p.x && x < p.x + s.width && y > p.y && y < p.y + s.height) {
      return i;
    }
  }

  return -1;
}

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    mouse_down = true;
    prevX = x;
    prevY = y;
    selected = getSelection(x,y);
  } else if (event == EVENT_LBUTTONUP) {
    mouse_down = false;
  } else if (event == EVENT_MOUSEMOVE && mouse_down) {
    int dx = x - prevX;
    int dy = y - prevY;

    if (selected >= 0) comps[selected]->movePoint(dx, dy);

    prevX = x;
    prevY = y;
  }
}

int main(int argc, char** argv) {
  if (argc == 2) {
    source = std::atoi(argv[1]);
  }

  VideoCapture feed(source);
  if (!feed.isOpened()) {
    std::cerr << "Could not open video feed.\nDefaulting to black screen.\n";
  }

  namedWindow(window_name, CV_WINDOW_NORMAL); 
  resizeWindow(window_name, width, height);

  // set up callbacks
  setMouseCallback(window_name, mouseCallback, NULL);

  comps.push_back(new Component(Point(10, 10), Point(310, 250), 
    Scalar(255, 255, 255), Scalar(0, 255, 255)));
  comps.push_back(new Component(Point(400, 50), Point(555, 175), 
    Scalar(255, 255, 255), Scalar(0, 255, 255), 0.67));
  comps.push_back(new Component(Point(700, 100), Size(600, 450), 
    Scalar(255, 255, 255), Scalar(0, 255, 255), 1.0));

  Mat frame;
  while(true) {
    if (feed.isOpened()) {
      feed >> frame;
    } else {
      frame = Mat(height, width, CV_8UC4, Scalar(0,0,0));
    }

    if (!frame.data) {
      std::cerr << "Error reading video feed.\n";
      return -1;
    }

    for (Component* c : comps) {
      c->draw(frame);
    }
    if (selected >= 0) comps[selected]->draw(frame);
    
    imshow(window_name, frame);

    if (waitKey(10) == 27) {
      break;
    }
  }

  return 0;
}

