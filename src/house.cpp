#include "house/scene.hpp"
#include "house/render_utils.hpp"
#include <GL/freeglut.h>

namespace house {
void drawRoof() {
    const float eaveY=4.35F, ridgeY=6.65F, eaveX=3.55F, frontZ=-2.88F, backZ=2.88F;
    setMaterial(0.48F,0.12F,0.09F,12.0F);
    glBegin(GL_QUADS);
        glNormal3f(-0.55F,0.84F,0.0F);
        glVertex3f(-eaveX,eaveY,frontZ); glVertex3f(0.0F,ridgeY,frontZ);
        glVertex3f(0.0F,ridgeY,backZ); glVertex3f(-eaveX,eaveY,backZ);
        glNormal3f(0.55F,0.84F,0.0F);
        glVertex3f(0.0F,ridgeY,frontZ); glVertex3f(eaveX,eaveY,frontZ);
        glVertex3f(eaveX,eaveY,backZ); glVertex3f(0.0F,ridgeY,backZ);
    glEnd();
    setMaterial(0.86F,0.72F,0.55F);
    glBegin(GL_TRIANGLES);
        glNormal3f(0.0F,0.0F,-1.0F);
        glVertex3f(-3.0F,4.3F,-2.53F); glVertex3f(3.0F,4.3F,-2.53F); glVertex3f(0.0F,6.25F,-2.53F);
        glNormal3f(0.0F,0.0F,1.0F);
        glVertex3f(-3.0F,4.3F,2.53F); glVertex3f(0.0F,6.25F,2.53F); glVertex3f(3.0F,4.3F,2.53F);
    glEnd();
    drawBox(-1.78F,4.34F,0.0F,0.12F,0.16F,5.95F,0.28F,0.08F,0.06F);
    drawBox(1.78F,4.34F,0.0F,0.12F,0.16F,5.95F,0.28F,0.08F,0.06F);
    drawBox(0.0F,6.60F,0.0F,0.16F,0.14F,5.95F,0.30F,0.09F,0.07F);
}
void drawWindow(float x,float y,float z,bool frontFacing=true) {
    const float front=frontFacing?-1.0F:1.0F, frameZ=z+front*0.06F, glassZ=z+front*0.10F;
    drawBox(x,y,frameZ,1.18F,1.18F,0.16F,0.25F,0.12F,0.07F);
    drawBox(x,y,glassZ,0.94F,0.94F,0.055F,0.35F,0.70F,0.82F);
    drawBox(x,y,glassZ+front*0.04F,0.07F,0.98F,0.06F,0.92F,0.88F,0.72F);
    drawBox(x,y,glassZ+front*0.05F,0.98F,0.07F,0.06F,0.92F,0.88F,0.72F);
    drawBox(x,y-0.64F,frameZ,1.35F,0.10F,0.25F,0.34F,0.18F,0.10F);
}
void drawDoor() {
    drawBox(0.0F,1.25F,-2.60F,1.25F,2.5F,0.18F,0.30F,0.16F,0.08F);
    drawBox(0.0F,1.28F,-2.71F,1.02F,2.25F,0.06F,0.48F,0.25F,0.12F);
    drawBox(0.0F,2.13F,-2.75F,0.34F,0.36F,0.045F,0.30F,0.65F,0.78F);
    glPushMatrix(); glTranslatef(0.39F,1.22F,-2.78F);
    setMaterial(0.92F,0.72F,0.22F,72.0F); glutSolidSphere(0.07,16,12); glPopMatrix();
}
void drawHouse() {
    drawBox(0.0F,0.22F,0.0F,6.35F,0.44F,5.35F,0.48F,0.46F,0.40F);
    drawBox(0.0F,2.36F,0.0F,6.0F,3.85F,5.0F,0.88F,0.75F,0.56F);
    drawDoor(); drawWindow(-1.85F,2.85F,-2.53F); drawWindow(1.85F,2.85F,-2.53F);
    drawBox(3.04F,2.75F,0.65F,0.10F,1.05F,1.15F,0.25F,0.12F,0.07F);
    drawBox(3.10F,2.75F,0.65F,0.05F,0.85F,0.94F,0.35F,0.70F,0.82F);
    drawBox(-2.92F,2.35F,-2.56F,0.12F,3.75F,0.12F,0.95F,0.87F,0.72F);
    drawBox(2.92F,2.35F,-2.56F,0.12F,3.75F,0.12F,0.95F,0.87F,0.72F);
    drawBox(0.0F,4.22F,-2.57F,6.0F,0.14F,0.14F,0.95F,0.87F,0.72F);
    drawBox(0.0F,0.22F,-3.28F,3.5F,0.36F,1.55F,0.65F,0.58F,0.48F);
    drawBox(0.0F,0.12F,-4.12F,2.7F,0.22F,0.55F,0.60F,0.54F,0.46F);
    drawBox(0.0F,3.80F,-3.34F,3.65F,0.20F,1.75F,0.48F,0.12F,0.08F);
    drawBox(-1.48F,2.02F,-3.90F,0.18F,3.45F,0.18F,0.92F,0.84F,0.68F);
    drawBox(1.48F,2.02F,-3.90F,0.18F,3.45F,0.18F,0.92F,0.84F,0.68F);
    drawRoof();
    drawBox(1.70F,5.65F,1.35F,0.72F,2.35F,0.78F,0.55F,0.22F,0.16F);
    drawBox(1.70F,6.86F,1.35F,0.90F,0.18F,0.96F,0.34F,0.12F,0.10F);
}
}  // namespace house
