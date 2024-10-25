#include <vine/ge/Rect2i.h>

VI_GE_NS_BEGIN
Rect2i::Rect2i()
  : Rect2i(0, 0, 0, 0) {
}
Rect2i::Rect2i(const Point2i& top_left, const Vector2i& size)
  : x(top_left.x)
  , y(top_left.y)
  , w(size.x)
  , h(size.y) {
}

Rect2i::Rect2i(int x, int y, int w, int h)
  : x(x)
  , y(y)
  , w(w)
  , h(h) {
}

Point2i Rect2i::topLeft() const {
    return Point2i(x, y);
}
Point2i Rect2i::topRight() const {
    return Point2i(x + w, y);
}
Point2i Rect2i::bottomLeft() const {
    return Point2i(x, y + h);
}
Point2i Rect2i::bottomRight() const {
    return Point2i(x + w, y + h);
}

Vector2i Rect2i::size() const {
    return Vector2i(w, h);
}

bool Rect2i::operator==(const Rect2i& right) const {
    return x == right.x && y == right.y && w == right.w && h == right.h;
}
bool Rect2i::operator!=(const Rect2i& right) const {
    return !(*this == right);
}
VI_GE_NS_END