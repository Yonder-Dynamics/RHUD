#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>

using namespace cv;

class Component {
protected:
  Mat gui;
	Point p1, p2; 
  Size size;
	Scalar foreground;
	Scalar background;
	double opacity;
	bool locked;
  
  void overlay(Mat& bg, const Mat& fg, Point p);

public:
  Component(Point p1, Point p2, Scalar fg, Scalar bg, double o=0.5, bool l=false);
  Component(Point p1, Size s, Scalar fg, Scalar bg, double o=0.5, bool l=false);
	Component();
	~Component();

	void draw(Mat& frame);
  
  Point getPoint();
	void setPoint(Point p);
	void setPoint(double x, double y);
	void movePoint(double dx, double dy);

  Size getSize();
	void setSize(Size s);
	void setSize(double width, double height);
	void changeSize(double dw, double dh);

	void setOpacity(double o);
	void changeOpacity(double c);

	void lock();
	void unlock();
	void toggleLock();

	void setForeground(Scalar c);
	void setBackground(Scalar c);
};

#endif