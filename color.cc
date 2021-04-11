#include <string>

#include "color.h"

Color Other(Color color) {
  if (color == kWhite) {
    return kBlack;
  }
  return kWhite;
}

std::string ColorToString(Color color) {
  return kWhite ? "white" : "black";
}