#include "geometry.hpp"

#include <algorithm>
#include <cmath>
#include <type_traits>

Point::Point() {}

Point::Point(const Point& other)
    : x_coord_{other.x_coord_}, y_coord_{other.y_coord_} {}

Point& Point::operator=(const Point& other) {
  x_coord_ = other.x_coord_;
  y_coord_ = other.y_coord_;
  return *this;
}

Point::Point(const int64_t& x_coord, const int64_t& y_coord)
    : x_coord_{x_coord}, y_coord_{y_coord} {}

int64_t Point::GetX() const { return x_coord_; }

int64_t Point::GetY() const { return y_coord_; }

bool Point::operator==(const Point& other) const {
  return (x_coord_ == other.GetX()) && (y_coord_ == other.GetY());
}

int64_t Vector::GetX() const { return end_of_vector_.GetX(); }

int64_t Vector::GetY() const { return end_of_vector_.GetY(); }

Point Segment::GetA() const { return begin_point_; }

Point Segment::GetB() const { return end_point_; }

Point Point::operator-(const Point& second) const {
  return Point(x_coord_ - second.x_coord_, y_coord_ - second.y_coord_);
}

void Point::Move(const Vector& vector) {
  x_coord_ += vector.GetX();
  y_coord_ += vector.GetY();
}

bool Point::ContainsPoint(const Point& point) {
  return (point.x_coord_ == x_coord_ && point.y_coord_ == y_coord_);
}

bool Point::CrossSegment(const Segment& segment) {
  if (segment.GetA() == segment.GetB()) {
    return (segment.GetA() == *this);
  }
  if (((Vector(*this - segment.GetA()) ^
        Vector(segment.GetB() - segment.GetA())) *
       Vector(1, 1)) == 0) {
    if ((Vector(*this - segment.GetA()) *
             Vector(segment.GetB() - segment.GetA()) >=
         0) &&
        (Vector(*this - segment.GetB()) *
             Vector(segment.GetA() - segment.GetB()) >=
         0)) {
      return true;
    }
  }
  return false;
}

IShape* Point::Clone() {
  Point* copy = new Point(*this);
  return copy;
}

Vector::Vector() {}

Vector::Vector(const int64_t& x_coord, const int64_t& y_coord)
    : end_of_vector_{Point(x_coord, y_coord)} {}

Vector::Vector(const Point& point) : end_of_vector_{point} {}

int64_t Vector::operator*(const Vector& other) const {
  return this->end_of_vector_.GetX() * other.GetX() +
         this->end_of_vector_.GetY() * other.GetY();
}

Vector Vector::operator^(const Vector& other) const {
  return Vector(this->end_of_vector_.GetX() * other.GetY(),
                this->end_of_vector_.GetY() * other.GetX() * (-1));
}

Vector& Vector::operator+=(const Vector& other) {
  this->end_of_vector_ = Point(GetX() + other.GetX(), GetY() + other.GetY());
  return *this;
}

Vector& Vector::operator-=(const Vector& other) {
  this->end_of_vector_ = Point(GetX() - other.GetX(), GetY() - other.GetY());
  return *this;
}

Vector Vector::operator+(const Vector& other) const {
  return Vector(Point(GetX() + other.GetX(), GetY() + other.GetY()));
}

Vector Vector::operator-(const Vector& other) const {
  return Vector(end_of_vector_ - other.end_of_vector_);
}

Vector& Vector::operator*=(const int64_t& num) {
  end_of_vector_ = Point(GetX() * num, GetY() * num);
  return *this;
}

Vector& Vector::operator-() {
  *this *= (-1);
  return *this;
}

Vector operator*(const Vector& second, const int64_t& num) {
  Vector res = second;
  res *= num;
  return res;
}

Vector operator*(const int64_t& num, const Vector& second) {
  Vector res = second;
  res *= num;
  return res;
}

Segment::Segment() {}

Segment::Segment(const Point& first_point, const Point& second_point)
    : begin_point_{first_point}, end_point_{second_point} {}

double Segment::Length() const {
  return std::sqrt(std::pow(Vector(GetB() - GetA()).GetX(), 2) +
                   std::pow(Vector(GetB() - GetA()).GetY(), 2));
}

bool Segment::IsProjectionBelongsToSegment(const Point& point) const {
  return (Vector(Vector(point) - GetA()) * Vector(GetB() - GetA()) >= 0 &&
          Vector(Vector(point) - GetB()) * Vector(GetA() - GetB()) >= 0);
}

double Segment::Distance(const Point& point) const {
  double projection =
      ((Vector(point - GetA()) ^ Vector(GetB() - GetA())) * Vector(1, 1)) /
      Length();
  if (projection < 0) {
    projection *= -1;
  }
  if (IsProjectionBelongsToSegment(point)) {
    return projection;
  }
  return std::min(Segment(point, GetA()).Length(),
                  Segment(point, GetB()).Length());
}

void Segment::Move(const Vector& vector) {
  begin_point_.Move(vector);
  end_point_.Move(vector);
}

bool Segment::ContainsPoint(const Point& point) {
  Point copy = point;
  return copy.CrossSegment(*this);
}

bool Segment::ContainsPoint(const Point& point) const {
  Point copy = point;
  return copy.CrossSegment(*this);
}

bool Segment::CrossSegment(const Segment& segment) {
  if ((Vector(end_point_ - begin_point_) ^
       Vector(segment.GetA() - begin_point_)) *
          Vector(1, 1) *
          (Vector(end_point_ - begin_point_) ^
           Vector(segment.GetB() - begin_point_)) *
          Vector(1, 1) <=
      0) {
    if ((Vector(segment.GetB() - segment.GetA()) ^
         Vector(begin_point_ - segment.GetA())) *
            Vector(1, 1) *
            (Vector(segment.GetB() - segment.GetA()) ^
             Vector(end_point_ - segment.GetA())) *
            Vector(1, 1) <=
        0) {
      return !((Vector(segment.GetB() - segment.GetA()) ^
                Vector(begin_point_ - segment.GetA())) *
                       Vector(1, 1) *
                       (Vector(segment.GetB() - segment.GetA()) ^
                        Vector(end_point_ - segment.GetA())) *
                       Vector(1, 1) ==
                   0 &&
               ((!(ContainsPoint(segment.GetA())) &&
                 !(ContainsPoint(segment.GetB()))) &&
                !(segment.ContainsPoint(begin_point_)) &&
                !(segment.ContainsPoint(end_point_))));
    }
  }
  return false;
}

IShape* Segment::Clone() {
  Segment* copy = new Segment(*this);
  return copy;
}

Line::Line() {}

Line::Line(const Point& first_point, const Point& second_point)
    : first_point_{first_point}, second_point_{second_point} {
  a_coefficient_ = first_point.GetY() - second_point.GetY();
  b_coefficient_ = second_point.GetX() - first_point.GetX();
  c_coefficient_ = first_point.GetX() * a_coefficient_ * (-1) -
                   first_point.GetY() * b_coefficient_;
}

int64_t Line::GetA() const { return a_coefficient_; }

int64_t Line::GetB() const { return b_coefficient_; }

int64_t Line::GetC() const { return c_coefficient_; }

void Line::Move(const Vector& vector) {
  c_coefficient_ += a_coefficient_ * vector.GetX() * (-1) +
                    b_coefficient_ * vector.GetY() * (-1);
}

bool Line::ContainsPoint(const Point& point) {
  return (a_coefficient_ * point.GetX() + b_coefficient_ * point.GetY() +
              c_coefficient_ ==
          0);
}

bool Line::CrossSegment(const Segment& segment) {
  return (((Vector(second_point_ - first_point_) ^
            Vector(segment.GetA() - first_point_)) *
           (Vector(second_point_ - first_point_) ^
            Vector(segment.GetB() - first_point_))) <= 0);
}

IShape* Line::Clone() {
  Line* copy = new Line(*this);
  return copy;
}

Ray::Ray() {}

Ray::Ray(const Point& first_point, const Point& second_point)
    : begin_{first_point},
      directional_vector_{Vector(second_point - first_point)},
      intermediate_{second_point} {}

Point Ray::GetA() const { return begin_; }

Vector Ray::GetVector() const { return directional_vector_; }

void Ray::Move(const Vector& vector) {
  begin_ = Point(begin_.GetX() + vector.GetX(), begin_.GetY() + vector.GetY());
}

bool Ray::ContainsPoint(const Point& point) {
  if ((point.GetX() - begin_.GetX()) * directional_vector_.GetY() ==
      (point.GetY() - begin_.GetY()) * directional_vector_.GetX()) {
    Vector intermediate(begin_ - point);
    if (((intermediate.GetX() * (-1) <= 0 && directional_vector_.GetX() <= 0) ||
         (intermediate.GetX() * (-1) >= 0 &&
          directional_vector_.GetX() >= 0)) &&
        ((intermediate.GetY() * (-1) <= 0 && directional_vector_.GetY() <= 0) ||
         (intermediate.GetY() * (-1) >= 0 &&
          directional_vector_.GetY() >= 0))) {
      return true;
    }
  }
  return false;
}

Point Kramer(const Line& first, const Line& second) {
  int64_t main_det =
      first.GetA() * second.GetB() - first.GetB() * second.GetA();
  int64_t first_det =
      first.GetC() * (-1) * second.GetB() + first.GetB() * second.GetC();
  int64_t second_det =
      first.GetA() * (-1) * second.GetC() + first.GetC() * second.GetA();
  return Point(first_det / main_det, second_det / main_det);
}

bool Ray::CrossSegment(const Segment& segment) {
  if (ContainsPoint(segment.GetA()) || ContainsPoint(segment.GetB())) {
    return true;
  }
  Point intersec =
      Kramer(Line(segment.GetA(), segment.GetB()), Line(begin_, intermediate_));
  return ContainsPoint(intersec);
}

IShape* Ray::Clone() {
  Ray* copy = new Ray(*this);
  return copy;
}

Circle::Circle() {}

Circle::Circle(const Point& centre, const int64_t& radius)
    : centre_{centre}, radius_{radius} {}

Point Circle::GetCentre() const { return centre_; }

int64_t Circle::GetRadius() const { return radius_; }

void Circle::Move(const Vector& vector) {
  centre_ =
      Point(centre_.GetX() + vector.GetX(), centre_.GetY() + vector.GetY());
}

bool Circle::ContainsPoint(const Point& point) {
  return ((centre_.GetX() - point.GetX()) * (centre_.GetX() - point.GetX()) +
              (centre_.GetY() - point.GetY()) *
                  (centre_.GetY() - point.GetY()) <=
          radius_ * radius_);
}

bool Circle::CrossSegment(const Segment& segment) {
  if (segment.Distance(centre_) <= radius_) {
    if ((ContainsPoint(segment.GetA())) && (ContainsPoint(segment.GetB()))) {
      return (((centre_.GetX() - segment.GetA().GetX()) *
                       (centre_.GetX() - segment.GetA().GetX()) +
                   (centre_.GetY() - segment.GetA().GetY()) *
                       (centre_.GetY() - segment.GetA().GetY()) ==
               radius_ * radius_) ||
              ((centre_.GetX() - segment.GetB().GetX()) *
                       (centre_.GetX() - segment.GetB().GetX()) +
                   (centre_.GetY() - segment.GetB().GetY()) *
                       (centre_.GetY() - segment.GetB().GetY()) ==
               radius_ * radius_));
    }
    return true;
  }
  return false;
}

IShape* Circle::Clone() {
  Circle* copy = new Circle(*this);
  return copy;
}