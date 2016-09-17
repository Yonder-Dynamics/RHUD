#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "Component.h"

std::string window_name = "Yonder Dynamics RHUD";
int width = 1920;
int height = 1080;
int source = 1;

int prevX, prevY;
bool mouse_down = false;
int selected = -1;
bool resize_mode = false;

using namespace cv;

std::vector<Component*> comps;

int getSelection(int x, int y) {
  if (comps.size() == 0) return -1;

  if (selected >= 0 && comps[selected]->containsPoint(x,y)) {
    if (comps[selected]->inResizeZone(x,y)) resize_mode = true;
    return selected;
  } 

  for (int i = 0; i < comps.size(); ++i) {
    if (comps[i]->containsPoint(x,y)) {
      if (comps[i]->inResizeZone(x,y)) resize_mode = true;
      return i;
    }
  }

  return -1;
}

void mouseCallback(int event, int x, int y, int flags, void* userdata) {
  if (event == EVENT_LBUTTONDOWN) {
    selected = getSelection(x,y);
    if (flags == EVENT_FLAG_CTRLKEY) {
      if (selected >= 0) comps.erase(comps.begin() + selected);
      selected = -1;
    } else if (flags == EVENT_FLAG_SHIFTKEY) {
      if (selected >= 0) comps[selected]->setFrame("hud.PNG");
    } else {
      mouse_down = true;
      prevX = x;
      prevY = y;
    }
  } else if (event == EVENT_LBUTTONUP) {
    mouse_down = false;
    resize_mode = false;
  } else if (event == EVENT_MOUSEMOVE && mouse_down) {
    int dx = x - prevX;
    int dy = y - prevY;

    if (selected >= 0) {
      if (resize_mode) {
        comps[selected]->changeSize(dx, dy);
      } else {
        comps[selected]->movePoint(dx, dy);
      }
    }

    prevX = x;
    prevY = y;
  }
}

int main(int argc, char** argv) {
  if (argc == 2) {
    source = std::atoi(argv[1]);
  }

  VideoCapture feed(source);
  feed.set(CV_CAP_PROP_FRAME_WIDTH, width);
  feed.set(CV_CAP_PROP_FRAME_HEIGHT, height);
  if (!feed.isOpened()) {
    std::cerr << "Could not open video feed.\nDefaulting to black screen.\n";
  }

  namedWindow(window_name, CV_WINDOW_NORMAL); 
  resizeWindow(window_name, width, height);

  // set up callbacks
  setMouseCallback(window_name, mouseCallback, NULL);

  comps.push_back(new Component(Point(10, 10), Point(310, 250), 
        "Temperature: 90 C", Scalar(255, 255, 255), Scalar(0, 255, 255)));
  comps.push_back(new Component(Point(400, 50), Point(555, 175), 
        "Wind Speed: 30 mph", Scalar(255, 255, 255), Scalar(0, 255, 255), 0.67));
  comps.push_back(new Component(Point(700, 100), Size(600, 450), 
        "Humidity: 13 units", Scalar(255, 255, 255), Scalar(0, 255, 255), 1.0));

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

