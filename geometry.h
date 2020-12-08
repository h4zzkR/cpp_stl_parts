//
// Created by h4zzkR on 07.12.2020.
//

#ifndef GEOZOO_GEOMETRY_H
#define GEOZOO_GEOMETRY_H

#include <iostream>
#include <vector>
#include <cmath>
#include <ostream>
#include <set>

#define EPS 1e-6

bool Equal(double x, double y) {
    return std::abs(x - y) <= EPS * std::abs(x);
    // see Knuth section 4.2.2 pages 217-218
}

bool notEqual(double x, double y) {
    return !Equal(x, y);
}

/* POINT AND LINE */
class Point {
public:
    double x = 0;
    double y = 0;

    Point() = default;
    Point(double x, double y) : x(x), y(y) {}
    bool operator==(const Point& b) const {
        if (Equal(x, b.x) && Equal(y, b.y)) return true;
        return false;
    }
    bool operator!=(const Point& b) const { return !(*this == b); }
    Point& operator+=(const Point& b) {
        x += b.x;
        y += b.y;
        return *this;
    }
    Point& operator-=(const Point& b) {
        x -= b.x;
        y -= b.y;
        return *this;
    }
    Point& operator-() {
        x *= -1;
        y *= -1;
        return *this;
    }
    Point operator-() const {
        Point copy = *this;
        copy.x *= -1;
        copy.y *= -1;
        return copy;
    }

    void rotate(Point point, double angle) {
        double sina = sin(angle), cosa = cos(angle);
        double x_ = cosa * (x - point.x) - sina * (y - point.y) + point.x;
        double y_ = sina * (x - point.x) + cosa * (y - point.y) + point.y;
        x = x_;
        y = y_;
    }

    void rotate(Point point, double& sina, double& cosa) {
        double x_ = cosa * (x - point.x) - sina * (y - point.y) + point.x;
        double y_ = sina * (x - point.x) + cosa * (y - point.y) + point.y;
        x = x_;
        y = y_;
    }

    static bool lowerequal(double a, double b) {
        double c = b - a;
        return (b - a >= EPS || Equal(c, 0));
    }

    static bool between(double a, double b, double c) {
        return lowerequal(std::min(a,b), c) && lowerequal(c, std::max(a,b));
    }

    Point& operator=(const Point& b) = default;
    ~Point() = default;
};

Point operator+(const Point& a, const Point& b) {
    Point copy = a;
    copy += b;
    return copy;
}
Point operator-(const Point& a, const Point& b) {
    Point copy = a;
    copy -= b;
    return copy;
}

class Vector : public Point {
    /*
     * Класс со всякими полезными штуками
     */
public:
    Point start{0,0}, end{0,0};
    Vector(const Point& start, const Point& end) {
        this->start = start;
        this->end = end;
        x = end.x - start.x;
        y = end.y - start.y;
    }

    static double abs(const Vector v) {
        return sqrt(pow(v.x, 2) + pow(v.y, 2));
    }

    static double det(const Point one, const Point two) {
        return one.x * two.y - one.y * two.x;
    }

    static double dot(const Vector a, const Vector b) {
        return a.x * b.x + a.y * b.y;
    }

    static double cross(const Vector& a, const Vector& b) {
        return a.x * b.y - a.y * b.x;
    }

    static double cos(const Vector& a, const Vector& b) {
        return dot(a, b) / (abs(a) * abs(b));
    }

    static double getAngle(const Vector& a, const Vector& b, bool radian = false) {
        return (radian) ? atan2(cross(a,b), dot(a,b)) : atan2(cross(a, b), dot(a, b)) * 180.0 / M_PI;
    }

    static bool sign(double a) {
        return (a > 0);
    }

    bool isZeroVector() {
        return (Equal(x, 0) && Equal(y, 0));
    }

    Point center() const {
        return Point((start.x + end.x) / 2, (start.y + end.y) / 2);
    }
};

class Line {
    double A = 0, B = 0, C = 0;

    void toGeneral(const Point dirVector, const Point baseVector) {
        A = dirVector.y;
        B = -dirVector.x;
        C = (dirVector.x * baseVector.y - dirVector.y * baseVector.x);
    }

public:

    friend std::ostream& operator<< (std::ostream &out, const Line& l);

    bool containsPoint(const Point& point) const {
        return (Equal(A * point.x + B * point.y + C, 0));
    }

    bool operator==(const Line &other) const {
        if (notEqual(other.A, 0) && notEqual(other.B, 0) && notEqual(other.C, 0)) {
            if ((Equal(A / other.A , B / other.B)) && (Equal(B / other.B, C / other.C)))
                return true;
        } else if (Equal(other.A,0) && notEqual(other.B, 0) && notEqual(other.C, 0)) {
            return (Equal(A, 0) && Equal(B / other.B, C / other.C));
        } else if (Equal(other.A, 0) && notEqual(other.B, 0) && Equal(other.C, 0)) {
            return (Equal(A, 0) && Equal(C, other.C));
        } else if (notEqual(other.A, 0) && Equal(other.B, 0) && notEqual(other.C, 0)) {
            return (Equal(A / other.A, C / other.C) && Equal(B, 0));
        } else if (notEqual(other.A, 0) && Equal(other.B, 0) && Equal(other.C, 0)) {
            return (Equal(B, 0) && Equal(C, 0));
        }
        return false;
    }

    bool operator!=(const Line &b) const { return !(*this == b); }

    void rotate(const Point& point, double angle) { // angle in radians
        double cosa = cos(angle), sina = sin(angle);
        double A_ = (A * cosa + B * sina);
        double B_ = (B * cosa - A * sina);
        C = (C + point.x * (A - A*cosa - B*sina) + point.y * (B + A * sina - B * cosa));
        A = A_;
        B = B_;
    }

    void rotate(const Point& point, double& sina, double& cosa) { // angle in radians
//        double cosa = cos(angle), sina = sin(angle);
        double A_ = (A * cosa + B * sina);
        double B_ = (B * cosa - A * sina);
        C = (C + point.x * (A - A*cosa - B*sina) + point.y * (B + A * sina - B * cosa));
        A = A_;
        B = B_;
    }

    void shift(double& x, double& y) {
        C = C - A * x - B * y;
    }

    static Point cross(const Line one, const Line two) { // TODO
        double d = Vector::det(Point(one.A, one.B), Point(two.A, two.B));
        double x = -Vector::det(Point(one.C, one.B), Point(two.C, two.B)) / d;
        double y = -Vector::det(Point(one.A, one.C), Point(two.A, two.C)) / d;
        return Point(x, y);
    }

    static Line getOrth(const Line one, const Point point) {
        return Line(-one.getB(), one.getA(), one.getB() * point.x - one.getA() * point.y);
    }

    static Point projection(const Line one, const Point point) {
        return cross(one, getOrth(one, point));
    }

    static bool isParallel(const Line one, const Line two) {
        return Equal(Vector::det(Point(one.getA(), one.getB()), Point(two.getA(), two.getB())), 0);
    }

    Vector getBaseVector() const {
        if (Equal(A, 0))
            return Vector(Point(0,0), Point(1,0));
        else if (Equal(B, 0))
            return Vector(Point(0, 0), Point(0, 1));
        else {
            Point one(1, (-C - A*1) / B);
            Point two(2, (-C - A*2) / B);
            return Vector(one, two);
        }
    }

    Point getPoint() const {
        if (Equal(A, 0))
            return Point(0, -C / B);
        else if (Equal(B, 0))
            return Point(-C / A, 0);
        else {
            Point one(1, (-C - A*1) / B);
            return one;
        }
    }

    double getA() const { return A; }
    double getB() const { return B; }
    double getC() const { return C; }

    Line() {
        A = 0;
        B = 0;
        C = 0;
    }

    Line(Point a, Point b) { toGeneral(b - a, a); }
    Line(double k, double b) : Line(Point(0, k * 0 + b), Point(1, k * 1 + b)) {}
    Line(Point one, double k) : Line(k, one.y - k * one.x) {}
    Line(double A, double B, double C) : A(A), B(B), C(C) { }
    ~Line() = default;
};

std::ostream& operator<< (std::ostream &out, const Line& l) {
    out << std::to_string(l.A) + "x" + " + " + std::to_string(l.B) + "y" + " + " + std::to_string(l.C) + " = 0";
    return out;
}

/* END OF POINT AND LINE */
/* START OF SHAPES */

class Shape {
    /*
     * Abstract base class
     */
public:
    bool isCurve = false;
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(Point point) const = 0;
    virtual void rotate(Point center, double angle) = 0;
    virtual void reflex(Point center) = 0;
    virtual void reflex(Line axis) = 0;
    virtual void scale(Point center, double coefficient) = 0;

    virtual bool operator==(const Shape& other) const = 0;
    virtual bool operator!=(const Shape& other) const = 0;
    virtual ~Shape() = 0;

    virtual int verticesCount() const {
        return 0;
    }
};

Shape::~Shape() {}

/* KINGDOM OF CURVES */
class Ellipse : public Shape {
protected:
    Point focus1{};
    Point focus2{};

    Line directrice1;
    Line directrice2;

    double majorAxis; // a
    double minorAxis; // b
    double focusHalfDistance; // c
    double eccentricity_;

    struct Equation {
        // https://math.stackexchange.com/questions/426150/what-is-the-general-equation-of-the-ellipse-that-is-not-in-the-origin-and-rotate
        /*
         * Структура для хранения параметров уравнения эллипса
         */
        double cosa = 1, sina = 0;
        double a;
        double b;
        double angle;

        Equation(double sina, double cosa, double a, double b) {
            this->a = a;
            this->b = b;
            this->cosa = cosa;
            this->sina = sina;
        }

        Equation() {}
    };
    Equation eq;

    void rotateDirectrices(const Point point, double& sina, double& cosa) {
        directrice1.rotate(point, sina, cosa);
        directrice2 = directrice1;
        sina = 0;
        cosa = -1;
        directrice2.rotate(center(), sina, cosa); // PI
    }

    void shiftDirectrises(double x0, double y0) {
        directrice1.shift(x0, y0);
        directrice2.shift(x0, y0);
    }

    void rotateFocuses(const Point point, double& sina, double& cosa) {
        focus1.rotate(point, sina, cosa);
        focus2.rotate(point, sina, cosa);
    }

    void reflexDirectrices(const Line axis) {
        double sina = 0, cosa = -1;
        if (!Line::isParallel(axis, directrice1)) {
            Point cross = Line::cross(axis, directrice1);
            double angle = Vector::getAngle(axis.getBaseVector(), directrice1.getBaseVector(), true);
            directrice1.rotate(cross, 2*angle);
            directrice2 = directrice1;
            directrice2.rotate(center(), sina, cosa);
        } else {
            Point center = axis.getPoint();
            directrice1.rotate(center, sina, cosa);
            directrice2.rotate(center, sina, cosa);
        }
    }

    void calculateParams() {
        /*
         * Расчет параметров эллипса
         * Фокусы могут находится не на оси абсцисс,
         * поэтому каноническое уравнение эллипса не подходит
         * Сначала я считаю угол поворота фокальной оси относительно начала координат,
         * Потом поворочаиваю на этот угол директрисы
         */
        Vector axis(focus1, focus2);
        Vector base_vector(Point(0, 0), Point(1,0)); // угол считается относительно начала координат

        double angle = Vector::getAngle(axis, base_vector, true);
        double sina = sin(angle), cosa = cos(angle);

        focusHalfDistance = Vector::abs(axis) / 2;
        eccentricity_ = focusHalfDistance / majorAxis;

        if (Equal(focusHalfDistance, 0))
            minorAxis = majorAxis;
        else
            minorAxis = sqrt(pow(majorAxis, 2) - pow(focusHalfDistance, 2));
        eq = Equation(sina, cosa, majorAxis, minorAxis);
        eq.angle = angle;
    }

public:
    virtual double perimeter() const {
        return M_PI * (3 * (eq.a + eq.b)  - sqrt((3 * eq.a + eq.b) * (eq.a + 3*eq.b)));
    }

    double area() const {
        return M_PI * majorAxis * minorAxis;
    }

    double eccentricity() const {
        return eccentricity_;
    }

    std::pair<Point,Point> focuses() const {
        return std::make_pair(focus1, focus2);
    }

    std::pair<Line, Line> directrices() {
        /*
         * поворот директрис
         */
        double dcts_x = majorAxis / eccentricity_;
        Line axis(focus1, focus2);
        Point dcts(dcts_x, 0), cent = center();
        dcts.rotate(Point(0,0), eq.sina, eq.cosa);
        dcts.x += cent.x; dcts.y += cent.y;
        directrice1 = Line::getOrth(axis, dcts);
        Point dcts2(-dcts_x, 0);
        dcts2.rotate(Point(0,0), eq.sina, eq.cosa);
        dcts2.x += cent.x; dcts.y += cent.y;
        directrice2 = Line::getOrth(axis, dcts2);
        return std::make_pair(directrice1, directrice2);
    }

    Point center() const {
        Vector cntr(focus1, focus2);
        return (cntr.isZeroVector()) ? focus1 : cntr.center();
    }

    bool containsPoint(Point point) const {
        Point cent = center();
        double a = (point.x - cent.x);
        double b = (point.y - cent.y);
        double val1 = pow((a * eq.cosa +  b * eq.sina), 2);
        double val2 = pow((a * eq.sina - b * eq.cosa) ,2);
        double val = val1 / pow(majorAxis, 2) + val2 / pow(minorAxis, 2);
        return (Point::lowerequal(val, 1));
    }

    void rotate(Point center, double angle) {
        angle = angle * M_PI / 180.0;
        double sina = sin(angle), cosa = cos(angle);
        if (focus1 != focus2) {
            rotateFocuses(center, sina, cosa);
            double sina_ = eq.sina, cosa_ = eq.cosa;
            eq.sina = sina_ * cosa - cosa_ * sina;
            eq.cosa = cosa * cosa_ + sina * sina_;
        } else {
            focus1.rotate(center, sina, cosa);
            focus2 = focus1;
        }
    }

    void reflex(Point center) {
        rotate(center, 180);
    }

    void reflex(Line axis) {
        Point projection = Line::projection(axis, focus1);
        focus1.rotate(projection, M_PI);
        projection = Line::projection(axis, focus2);
        focus2.rotate(projection, M_PI);
        reflexDirectrices(axis);
    }

    void scale(Point center, double coefficient) {
        Point point(focus1.x - center.x, focus1.y - center.y);
        majorAxis *= coefficient;
        if (focus1 != focus2) {
            focus1 = Point(center.x + coefficient * point.x, center.y + coefficient * point.y);
            point = Point(focus2.x - center.x, focus2.y - center.y);
            focus2 = Point(center.x + coefficient * point.x, center.y + coefficient * point.y);
            calculateParams();
        } else {
            focus1 = Point(center.x + coefficient * point.x, center.y + coefficient * point.y);
            minorAxis = majorAxis;
            focus2 = focus1;
        }
    }

    explicit Ellipse(Point focus1, Point focus2, double focal_rads) {
        this->focus1 = focus1;
        this->focus2 = focus2;
        majorAxis = focal_rads / 2;
        calculateParams();
        isCurve = true;
    }

    explicit Ellipse() { isCurve = true; }

    bool operator==(const Shape& other) const {
        if (Equal(area(), other.area())) {
            if (other.isCurve) {
                const Ellipse other_ = dynamic_cast<const Ellipse &>(other);
                const Equation &oeq = other_.eq;
                Point cent = center(), cent_ = other_.center();
                if (Equal(cent.x, cent_.x) && Equal(cent.y, cent_.y) &&
                    Equal(eq.sina, oeq.sina) && Equal(eq.cosa, oeq.cosa) && Equal(eq.a, oeq.a) &&
                    Equal(eq.b, oeq.b)) return true;
            }
        }
        return false;
    }

    bool operator!=(const Shape& other) const {
        return !(*this == other);
    }

    bool isCongruentTo(const Shape& other) const {
        if (Equal(area(), other.area())) {
            if (other.isCurve) {
                const Ellipse other_ = dynamic_cast<const Ellipse &>(other);
                const Equation &oeq = other_.eq;
                if (Equal(eq.a, oeq.a) && Equal(eq.b, oeq.b)) return true;
            }
        }
        return false;
    }

    bool isSimilarTo(const Shape& other) const {
        if (other.isCurve) {
            const Ellipse other_ = dynamic_cast<const Ellipse &>(other);
//            double k = sqrt(area() / other.area());
            if (Equal(eq.b / other_.eq.b, eq.a / other_.eq.a)) return true;
        }
        return false;
    }

    ~Ellipse() = default;
};

class Circle : public Ellipse {

    std::pair<Line, Line> directrices() { return std::pair<Line, Line>(); }


public:
    explicit Circle() = default;

    explicit Circle(Point center, double radius) {
//        : Ellipse(center, center, 2*radius) { }
        majorAxis = radius;
        minorAxis = radius;
        focus1 = center;
        focus2 = center;
        eq.a = 1;
        eq.b = 1;
        eq.sina = 0;
        eq.cosa = 1;
    }

    double perimeter() const override {
        return 2 * M_PI * majorAxis;
    }

    double radius() {
        return majorAxis;
    }

    ~Circle() = default;
};

/* KINGDOM OF POLYGONS */
class Polygon : public Shape {
protected:
    std::vector <Point> points;
    Vector getSegment(int start) const {
        return Vector(points[start], points[(start+1) % verticesCount()]);
    }

public:

    virtual double perimeter() const {
        double sum = 0;
        for (int i = 0; i < verticesCount(); ++i)
            sum += Vector::abs(getSegment(i));
        return sum;
    }

    virtual double area() const { // Gauss
        double sum = 0;
        Point point;
        for (int i = 0; i < verticesCount(); ++i) {
            int ii = (i + 1) % verticesCount();
            sum += Vector::det(Point(points[i].x, points[i].y), Point(points[ii].x, points[ii].y));
        }
        return (sum < 0) ? sum / (-2) : sum / 2;
    }

    int verticesCount() const {
        return static_cast<int>(points.size());
    }

    std::vector<Point> getVertices () const {
        return points;
    }

    virtual bool isConvex() { // not here
        int size = verticesCount();
        if (verticesCount() > 3) {
            bool psign = false;
            for (int i = 0; i < size; ++i) {
                Point& A = points[i % size];
                Point& B = points[(i+1) % size];
                Point& C = points[(i+2) % size];
                double cross = Vector::cross(Vector(B, A), Vector(B, C));
                if (i == 0)
                    psign = Vector::sign(cross);
                else if (Vector::sign(cross) != psign)
                    return false;
            }
        }
        return true;
    }

    bool containsPoint(Point point) const { // not here
        bool inside = false;
        int size = verticesCount();
        for (int i = 1; i < size + 1; ++i) {
            double ray = 0;
            Point point1 = points[prev(i)];
            Point point2 = points[i % size];
            if (point.y > std::min(point1.y, point2.y) && point.y <= std::max(point1.y, point2.y)) {
                if (point.x <= std::max(point1.x, point2.x)) {
                    if (notEqual(point2.y, point1.y))
                        ray = (point.y - point1.y) * (point2.x - point1.x) / (point2.y - point1.y) + point1.x;
                    if (point.x <= ray)
                        inside = !inside;
                }
            }
        }
        return inside;
    }

    void rotate(Point center, double angle) { // not here
        angle *= M_PI / 180;
        for (int i = 0; i < verticesCount(); ++i)
            points[i].rotate(center, angle);
    }

    void reflex(Point center) {
        rotate(center, 180);
    }

    void reflex(Line axis) {
        for (int i = 0; i < verticesCount(); ++i) {
            Point projection = Line::projection(axis, points[i]);
            points[i].rotate(projection, M_PI);
        }
    }

    virtual void scale(Point center, double coefficient) {
        for(int i = 0; i < verticesCount(); ++i) {
            Point point(points[i].x - center.x, points[i].y - center.y);
            points[i] = Point(center.x + coefficient * point.x, center.y + coefficient * point.y);
        }
    }

    int prev (int i) const {
        int size = verticesCount();
        return (i - 1 + size) % size;
    }

    int next (int i) const {
        int size = verticesCount();
        return (i + 1 + size) % size;
    }

    bool operator==(const Shape& other) const override {
        if (other.isCurve)
            return false;
        if (!Equal(area(), other.area()))
            return false;
        const Polygon other_ = dynamic_cast<const Polygon &>(other);
        if (other_.verticesCount() != verticesCount())
            return false;


        int pos = -1;
        for (int i = 0; i < verticesCount(); ++i) {
            if (points[0] == other_.points[i]) {
                pos = i;
                break;
            }
        }
        if (pos < 0)
            return false;
        bool mode;
        if (points[next(0)] == other_.points[next(pos)])
            mode = false;
        else if (points[next(0)] == other_.points[prev(pos)])
            mode = true;
        else
            return false;

        for (int i = 1; i < verticesCount(); ++i) {
            pos = (mode) ? prev(pos) : next(pos);
            if (points[i] != other_.points[pos])
                return false;
        }

        return true;
    }

    bool operator!=(const Shape& other) const {
        return !(*this == other);
    }

    bool isCongruentTo(const Shape& other) const override {
        if (other.isCurve)
            return false;
        const Polygon other_ = dynamic_cast<const Polygon &>(other);
        if (other_.verticesCount() != verticesCount())
            return false;

        int size = verticesCount();
        std::multiset<std::pair<double,double>> origin_l;
        std::multiset<std::pair<double,double>> other_l;

        Vector current(points[0], points[1]), current_other(other_.points[0], other_.points[1]);
        for (int i = 1; i < size + 1; ++i) {
            int pos = i % size, next_pos = next(i);
            Vector orgn_next = Vector(points[pos], points[next_pos]);
            Vector other_next = Vector(other_.points[pos], other_.points[next_pos]);

            origin_l.insert(std::make_pair(Vector::abs(current), Vector::cross(current, orgn_next)));
            other_l.insert(std::make_pair(Vector::abs(current_other), Vector::cross(current_other, other_next)));

            current_other = other_next;
            current = orgn_next;
        }

        auto it1 = origin_l.begin();
        auto it2 = other_l.begin();

        for(; it1 != origin_l.end(); ++it1, ++it2)
            if (notEqual(it1->first, it2->first) && notEqual(it1->second, it2->second))
                return false;
        return true;
    }

    bool isSimilarTo(const Shape& other) const {
        if (other.isCurve)
            return false;
        const Polygon other_ = dynamic_cast<const Polygon &>(other);
        if (other_.verticesCount() != verticesCount())
            return false;

        int size = verticesCount();
        std::multiset<std::pair<double,double>> origin_l;
        std::multiset<std::pair<double,double>> other_l;

        Vector current(points[0], points[1]), current_other(other_.points[0], other_.points[1]);
        for (int i = 1; i < size + 1; ++i) {
            int pos = i % size, next_pos = next(i);
            Vector orgn_next = Vector(points[pos], points[next_pos]);
            Vector other_next = Vector(other_.points[pos], other_.points[next_pos]);

            double abs_or = Vector::abs(current), abs_ot = Vector::abs(current_other);
            double d1 = abs_or * Vector::abs(orgn_next), d2 = abs_ot * Vector::abs(other_next);

            origin_l.insert(std::make_pair(abs_or, Vector::cross(current, orgn_next) / d1));
            other_l.insert(std::make_pair(abs_ot, Vector::cross(current_other, other_next) / d2));

            current_other = other_next;
            current = orgn_next;
        }

        auto it1 = origin_l.begin();
        auto it2 = other_l.begin();

        double k = 1;
        int cnt = 0;
        for(; it1 != origin_l.end(); ++it1, ++it2) {
            if (cnt == 0) {
                k = it1->first / it2->first;
            } else {
                if (notEqual(it1->first / it2->first, k) && notEqual(it1->second, it2->second))
                    return false;
            }
            ++cnt;
        }
        return true;
    }

    Polygon() {
        points.reserve(110);
    }

    explicit Polygon(std::vector <Point>& points) {
        this->points.reserve(points.size());
        this->points.insert(this->points.end(), points.begin(), points.end());
    }

    explicit Polygon(std::initializer_list<Point> l) : Polygon() {
        for (auto point : l) {
            points.push_back(point);
        }
    }

    ~Polygon() = default;
};

class Rectangle : public Polygon {
    bool canon = false;

public:
    bool isConvex() { return true; }

    Point center() {
        return Vector(points[0], points[2]).center();
    }

    std::pair<Line, Line> diagonals() {
        return std::make_pair(Line(points[0], points[2]), Line(points[1], points[3]));
    }

    double perimeter() const override {
        double a = Vector::abs(Vector(points[0], points[1]));
        double b = Vector::abs(Vector(points[1], points[2]));
        return (a + b) * 2;
    }

    double area() const override {
        double a = Vector::abs(Vector(points[0], points[1]));
        double b = Vector::abs(Vector(points[1], points[2]));
        return (a * b);
    }

    Rectangle(Point a, Point d, double k) {
        if (k < 1) k = 1 / k;
        Point b = a;
        b.rotate(Vector(a, d).center(), 2*atan(k));
        Point c = b;
        c.rotate(Vector(a, d).center(), M_PI);
        std::vector <Point> ps = {a, b, d, c};
        points.insert(points.end(), ps.begin(), ps.end());
        canon = true;
    }

    explicit Rectangle(std::initializer_list<Point> l) : Polygon(l) {}

    ~Rectangle() = default;
};

class Square : public Rectangle {
public:
    Square(Point a, Point b, double k = 1) : Rectangle(a, b, k) {}

    explicit Square(std::initializer_list<Point> l) : Rectangle(l) {}

    Circle circumscribedCircle() {
        double radius = Vector::abs(Vector(points[0], points[2])) / 2;
        return Circle(center(), radius);
    }

    Circle inscribedCircle() {
        double radius = Vector::abs(Vector(points[0], points[2])) / (2 * 1.41421356);
        return Circle(center(), radius);
    }

    double area() const override {
//        std::cout << "SQU";
        double a = Vector::abs(Vector(points[0], points[1]));
        return pow(a, 2);
    }

    double perimeter() const override {
        double a = Vector::abs(Vector(points[0], points[1]));
        return 4 * a;
    }

    ~Square() = default;
};

class Triangle : public Polygon {

    double a = NAN, b = NAN, c = NAN;

    Line getMdts(Vector segment) {
        Point mid = segment.center();
        return Line::getOrth(Line(segment.start, segment.end), mid);
    }

    Line getHght(Point point, Vector segment) {
        Line one(segment.end, segment.start);
        return Line(one.getB(), -one.getA(), (one.getA() * point.y - one.getB() * point.x));
    }

    std::pair<Point, double> calculateCircum() {
        Point &A = points[0];
        Point &B = points[1];
        Point &C = points[2];
        if (std::isnan(a)) {
            a = Vector::abs(Vector(B,C));
            b = Vector::abs(Vector(A,C));
            c = Vector::abs(Vector(A,B));
        }
        double radius = (a * b * c) / (4 * area());
        Line mdts1 = getMdts(Vector(A,B));
        Line mdts2 = getMdts(Vector(B,C));
        return std::make_pair(Line::cross(mdts1, mdts2), radius);
    }

    std::pair<Point, double> calculateInscr() {
        double radius = (2 * area()) / perimeter();
        Point& A = points[0], B = points[1], C = points[2];
        if (std::isnan(a)) {
            a = Vector::abs(Vector(B,C));
            b = Vector::abs(Vector(A,C));
            c = Vector::abs(Vector(A,B));
        }
        double e = a + b + c;
        double x = (a * A.x + b * B.x + c * C.x) / e;
        double y = (a * A.y + b * B.y + c * C.y) / e;
        return std::make_pair(Point(x,y), radius);
    }

    Line getMdn(Point point, Vector segment) {
        Point mid = segment.center();
        return Line(point, mid);
    }

public:

    bool isConvex() { return true; }

    explicit Triangle(std::vector <Point>& points) : Polygon(points) {}

    void scale(Point center, double coefficient) { // not here
        for(int i = 0; i < verticesCount(); ++i) {
            Point point(points[i].x - center.x, points[i].y - center.y);
            points[i] = Point(center.x + coefficient * point.x, center.y + coefficient * point.y);
        }
        a = NAN;
        b = NAN;
        c = NAN;
    }

    explicit Triangle(Point& a, Point& b, Point& c) : Polygon{ a, b, c } {}

    Circle circumscribedCircle() {
        std::pair<Point, double> circum = calculateCircum();
        return Circle(circum.first, circum.second);
    }

    Circle inscribedCircle() {
        std::pair<Point, double> inscr = calculateInscr();
        return Circle(inscr.first, inscr.second);
    }

    Circle ninePointsCircle() {
        Circle circumscribed = circumscribedCircle();
        Point ecenter = Vector(orthocenter(), circumscribed.center()).center();
        return Circle(ecenter, circumscribed.radius() / 2);
    }

    Point centroid() {
        double x = (points[0].x + points[1].x + points[2].x) / 3;
        double y = (points[0].y + points[1].y + points[2].y) / 3;
        return Point(x,y);
    }

    Point orthocenter() {
        Line h1 = getHght(points[0], Vector(points[1], points[2]));
        Line h2 = getHght(points[1], Vector(points[0], points[2]));
        return Line::cross(h1, h2);
    }

    Line EulerLine() {
        return Line(centroid(), orthocenter());
    }

    explicit Triangle(std::initializer_list<Point> l) : Polygon(l) {}

    ~Triangle() = default;
};

#endif //GEOZOO_GEOMETRY_H