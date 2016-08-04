#include "Component.h"

Component::Component(Point p1, Size s, Scalar fg, Scalar bg, double o, bool l) :
    p1(p1), size(s), foreground(fg), background(bg), opacity(o), locked(l) {
  gui = imread("frame.PNG", IMREAD_UNCHANGED);
  p2 = Point(p1.x + size.width, p1.y + size.height);
}


Component::Component(Point p1, Point p2, Scalar fg, Scalar bg, double o, bool l) :
    p1(p1), p2(p2), foreground(fg), background(bg), opacity(o), locked(l) {
  gui = imread("frame.PNG", IMREAD_UNCHANGED);
  size = Size(p2.x - p1.x, p2.y - p1.y);
}

Component::Component() {
  p1 = Point(0,0);
  p2 = Point(MIN_WIDTH,MIN_HEIGHT);
  size = Size(MIN_WIDTH,MIN_HEIGHT);
  foreground = Scalar(0,0,0);
  background = Scalar(0,0,0);
  opacity = 0.0;
	locked = false;
}

Component::~Component() {}

void Component::draw(Mat& frame) {
  Mat gui_t; // temp gui mat for resize
  resize(gui, gui_t, this->size);

  overlay(frame, gui_t, p1);
}

bool Component::containsPoint(int x, int y) {
  return (x > p1.x && x < p1.x + size.width &&
          y > p1.y && y < p1.y + size.height);
}

bool Component::inResizeZone(int x, int y) {
  int buffer_x = (int)(RESIZE_PERCENT * size.width);
  int buffer_y = (int)(RESIZE_PERCENT * size.height);

  return (x >= p1.x + size.width - buffer_x &&
          y >= p1.y + size.height - buffer_y);
}

void Component::setFrame(std::string file) {
  Mat temp = imread(file, IMREAD_UNCHANGED); 
  if (temp.data) gui = temp;
}

Point Component::getPoint() {
  return p1;
}

void Component::setPoint(Point p) {
	if (!locked) p1 = p;
}

void Component::setPoint(double x, double y) {
	if (!locked) {
		p1.x = x;
		p1.y = y;
	}
}
void Component::movePoint(double dx, double dy) {
	if (!locked) {
		p1.x += dx;
		p1.y += dy;
	}
}

Point Component::getBottomRightPoint() {
  return Point(p1.x + size.width, p1.y + size.height);
}

Size Component::getSize() {
  return size;
}

void Component::setSize(Size s) {
	if (!locked) size = s;
}

void Component::setSize(double width, double height) {
	if (!locked && width >= MIN_WIDTH && height >= MIN_HEIGHT) {
		size.width = width;
		size.height = height;
	}
}

void Component::changeSize(double dw, double dh) {
	if (!locked) {
		if (size.width + dw >= MIN_WIDTH) size.width += dw;
		if (size.height + dh >= MIN_HEIGHT) size.height += dh;
	}
}

void Component::setOpacity(double o) {
	if (!locked) opacity = o;
}

void Component::changeOpacity(double c) {
	if (!locked) opacity += c;
}

void Component::lock() {
	locked = true;
}

void Component::unlock() {
	locked = false;
}

void Component::toggleLock() {
	locked = !locked;
}

void Component::setForeground(Scalar c) {
	if (!locked) foreground = c;
}

void Component::setBackground(Scalar c) {
	if (!locked) background = c;
}

void Component::overlay(Mat& bg, const Mat& fg, Point p) {
  for (int y = std::max(p.y, 0); y < bg.rows; ++y) {
    int fY = y - p.y;
    if (fY >= fg.rows) break;

    for (int x = std::max(p.x, 0); x < bg.cols; ++x) {
      int fX = x - p.x;
      if (fX >= fg.cols) break;

      double a = ((double)fg.data[fY*fg.step + fX*fg.channels() + 3]) / 255.0;
      for (int c = 0; a > 0 && c < bg.channels(); ++c) {
        unsigned char fgPx = fg.data[fY*fg.step + fX*fg.channels() + c];
        unsigned char bgPx = bg.data[y*bg.step + x*bg.channels() + c];
        bg.data[y*bg.step + bg.channels()*x + c] = bgPx * (1.0-opacity) + fgPx * opacity;
      }
    }
  }
}
