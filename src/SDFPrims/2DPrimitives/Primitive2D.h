//
// Created by lowej2 on 2/15/2025.
//

#ifndef PRIMITIVE2D_H
#define PRIMITIVE2D_H
#include "Ray.h"
#include "PrettyLogger.h"

class Primitive2D {
public:
  // virtual ~Primitive2D() = default;
  virtual Vector2 getBBMin() const {
    LOG_WARN("The native getBBMin() function should never be called");
    return Vector2();
  };
  virtual Vector2 getBBMax() const {
    LOG_WARN("The native getBBMax() function should never be called");
    return Vector2();
  };
  virtual float getSignedDistance(Vector2 rayOrigin) const {
    LOG_WARN("The native getSignedDistance() function should never be called");
    return 0.0f;
  }

};
#endif //PRIMITIVE2D_H
