#include "house/scene.hpp"
#include "house/render_utils.hpp"
#include <GL/freeglut.h>

namespace house {
void drawGround() {
    setMaterial(0.28F,0.48F,0.22F,8.0F);
    glBegin(GL_QUADS);
        glNormal3f(0.0F,1.0F,0.0F);
        glVertex3f(-18.0F,-0.08F,-16.0F); glVertex3f(18.0F,-0.08F,-16.0F);
        glVertex3f(18.0F,-0.08F,16.0F); glVertex3f(-18.0F,-0.08F,16.0F);
    glEnd();
    for(int i=0;i<5;++i) {
        const float z=-4.0F-static_cast<float>(i)*1.15F;
        drawBox(0.0F,0.015F,z,2.0F,0.08F,0.86F,0.62F,0.60F,0.54F);
    }
    drawBox(-5.0F,0.08F,-3.8F,2.0F,0.16F,1.1F,0.25F,0.20F,0.13F);
    drawBox(5.0F,0.08F,-3.8F,2.0F,0.16F,1.1F,0.25F,0.20F,0.13F);
    for(int i=0;i<5;++i) {
        const float x=-5.7F+static_cast<float>(i)*0.35F;
        glPushMatrix(); glTranslatef(x,0.48F,-3.8F);
        setMaterial((i%2==0)?0.88F:0.95F,(i%2==0)?0.25F:0.68F,0.30F);
        glutSolidSphere(0.22,16,12); glPopMatrix();
        const float x2=4.3F+static_cast<float>(i)*0.35F;
        glPushMatrix(); glTranslatef(x2,0.48F,-3.8F);
        setMaterial(0.95F,0.72F,0.22F); glutSolidSphere(0.22,16,12); glPopMatrix();
    }
}
void drawTree(float x,float z,float scale) {
    glPushMatrix(); glTranslatef(x,0.0F,z); glScalef(scale,scale,scale);
    drawBox(0.0F,1.2F,0.0F,0.42F,2.4F,0.42F,0.34F,0.18F,0.08F);
    const float foliage[][3]={{0.0F,3.0F,0.0F},{-0.62F,2.65F,0.08F},{0.58F,2.68F,-0.06F},{0.0F,3.65F,0.02F}};
    for(const auto& p:foliage) {
        glPushMatrix(); glTranslatef(p[0],p[1],p[2]);
        setMaterial(0.16F,0.42F,0.16F,8.0F); glutSolidSphere(0.88,20,16); glPopMatrix();
    }
    glPopMatrix();
}
void drawFence() {
    const float z=-7.2F;
    drawBox(0.0F,0.64F,z,15.0F,0.12F,0.14F,0.88F,0.82F,0.70F);
    drawBox(0.0F,1.20F,z,15.0F,0.12F,0.14F,0.88F,0.82F,0.70F);
    for(int i=0;i<=20;++i) {
        const float x=-7.5F+static_cast<float>(i)*0.75F;
        drawBox(x,0.91F,z,0.12F,0.78F,0.16F,0.94F,0.88F,0.76F);
    }
    for(int i:{-10,0,10}) {
        const float x=static_cast<float>(i)*0.75F;
        drawBox(x,0.95F,z,0.30F,1.45F,0.30F,0.62F,0.38F,0.20F);
        glPushMatrix(); glTranslatef(x,1.72F,z);
        setMaterial(0.62F,0.38F,0.20F); glutSolidCone(0.22,0.30,12,2); glPopMatrix();
    }
}
}  // namespace house
