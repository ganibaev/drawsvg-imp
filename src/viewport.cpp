#include "viewport.h"

#include "CMU462.h"

namespace CMU462 {

void ViewportImp::set_viewbox( float centerX, float centerY, float vspan ) {

    // Task 5 (part 2): 
    // Set svg coordinate to normalized device coordinate transformation. Your input
    // arguments are defined as normalized SVG canvas coordinates.
    this->centerX = centerX;
    this->centerY = centerY;
    this->vspan = vspan;
    CMU462::Matrix3x3 translate = Matrix3x3::identity();
    CMU462::Matrix3x3 scale = Matrix3x3::identity();
    CMU462::Matrix3x3 move = Matrix3x3::identity();
    CMU462::Matrix3x3 rescale = Matrix3x3::identity();
    move[2] = CMU462::Vector3D(1, 1, 1);
    scale[0][0] = 1.0f / (vspan);
    scale[1][1] = 1.0f / (vspan);
    rescale[0][0] = 0.5f;
    rescale[1][1] = 0.5f;
    translate[2][0] = -centerX;
    translate[2][1] = -centerY;
    set_svg_2_norm (rescale * move * scale * translate);
}

void ViewportImp::update_viewbox( float dx, float dy, float scale ) { 
  
    this->centerX -= dx;
    this->centerY -= dy;
    this->vspan *= scale;
    set_viewbox( centerX, centerY, vspan );
}

} // namespace CMU462
