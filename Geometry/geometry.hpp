#include <iostream>
#include <vector>

class Point;

class Vector;

class Segment;

class IShape {
 public:
  virtual void Move(const Vector& vector) = 0;
  virtual bool ContainsPoint(const Point& point) = 0;
  virtual bool CrossSegment(const Segment& segment) = 0;
  virtual IShape* Clone() = 0;
  virtual ~IShape() = default;
};

class Point : public IShape {
 public:
  Point();
  Point(const Point& other);
  Point& operator=(const Point& other);
  Point(const int64_t& x_coord, const int64_t& y_coord);
  int64_t GetX() const;
  int64_t GetY() const;
  Point operator-(const Point& second) const;
  bool operator==(const Point& other) const;

  void Move(const Vector& vector);
  bool ContainsPoint(const Point& point);
  bool CrossSegment(const Segment& segment);

  IShape* Clone();

 private:
  int64_t x_coord_ = 0;
  int64_t y_coord_ = 0;
};

class Vector {
 public:
  Vector();
  Vector(const int64_t& x_coord, const int64_t& y_coord);
  Vector(const Point& point);
  int64_t operator*(const Vector& other) const;
  Vector operator^(const Vector& other) const;
  Vector& operator+=(const Vector& other);
  Vector& operator-=(const Vector& other);
  Vector operator+(const Vector& other) const;
  Vector operator-(const Vector& other) const;
  Vector& operator*=(const int64_t& num);
  friend Vector operator*(const int64_t& num, const Vector& second);
  friend Vector operator*(const Vector& second, const int64_t& num);
  Vector& operator-();
  int64_t GetX() const;
  int64_t GetY() const;

 private:
  Point end_of_vector_;
};

Vector operator*(const int64_t& num, const Vector& second);
Vector operator*(const Vector& second, const int64_t& num);

class Segment : public IShape {
 public:
  Segment();
  Segment(const Point& first_point, const Point& second_point);
  Point GetA() const;
  Point GetB() const;
  bool IsProjectionBelongsToSegment(const Point& point) const;
  double Distance(const Point& point) const;
  double Length() const;
  void Move(const Vector& vector);
  bool ContainsPoint(const Point& point);
  bool ContainsPoint(const Point& point) const;
  bool CrossSegment(const Segment& segment);
  IShape* Clone();

 private:
  Point begin_point_;
  Point end_point_;
};

class Line : public IShape {
 public:
  Line();
  Line(const Point& first_point, const Point& second_point);
  int64_t GetA() const;
  int64_t GetB() const;
  int64_t GetC() const;
  void Move(const Vector& vector);
  bool ContainsPoint(const Point& point);
  bool CrossSegment(const Segment& segment);
  IShape* Clone();

 private:
  int64_t a_coefficient_ = 0;
  int64_t b_coefficient_ = 0;
  int64_t c_coefficient_ = 0;
  Point first_point_;
  Point second_point_;
};

class Ray : public IShape {
 public:
  Ray();
  Ray(const Point& first_point, const Point& second_point);
  Point GetA() const;
  Vector GetVector() const;
  void Move(const Vector& vector);
  bool ContainsPoint(const Point& point);
  bool CrossSegment(const Segment& segment);
  IShape* Clone();

 private:
  Point begin_;
  Point intermediate_;
  Vector directional_vector_;
};

class Circle : public IShape {
 public:
  Circle();
  Circle(const Point& centre, const int64_t& radius);
  Point GetCentre() const;
  int64_t GetRadius() const;
  void Move(const Vector& vector);
  bool ContainsPoint(const Point& point);
  bool CrossSegment(const Segment& segment);
  IShape* Clone();

 private:
  Point centre_;
  int64_t radius_;
};