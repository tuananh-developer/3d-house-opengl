#pragma once

namespace house {
void setMaterial(float r, float g, float b, float shininess = 24.0F);
void drawBox(float x, float y, float z, float width, float height, float depth,
             float r, float g, float b);
}  // namespace house
