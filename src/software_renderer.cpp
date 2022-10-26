#include "software_renderer.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

#include "triangulation.h"

using namespace std;

namespace CMU462 {


// Implements SoftwareRenderer //

void SoftwareRendererImp::draw_svg( SVG& svg ) {

  // set top level transformation
  transformation = svg_2_screen;

  // draw all elements
  for ( size_t i = 0; i < svg.elements.size(); ++i ) {
    draw_element(svg.elements[i]);
  }

  // draw canvas outline
  Vector2D a = transform(Vector2D(    0    ,     0    )); a.x--; a.y--;
  Vector2D b = transform(Vector2D(svg.width,     0    )); b.x++; b.y--;
  Vector2D c = transform(Vector2D(    0    ,svg.height)); c.x--; c.y++;
  Vector2D d = transform(Vector2D(svg.width,svg.height)); d.x++; d.y++;

  rasterize_line(a.x, a.y, b.x, b.y, Color::Black);
  rasterize_line(a.x, a.y, c.x, c.y, Color::Black);
  rasterize_line(d.x, d.y, b.x, b.y, Color::Black);
  rasterize_line(d.x, d.y, c.x, c.y, Color::Black);

  // resolve and send to render target
  resolve();

}

void SoftwareRendererImp::set_sample_rate(size_t sample_rate) {

    // Task 4: 
    // You may want to modify this for supersampling support
    this->sample_rate = sample_rate;
    sample_h = target_h * sample_rate;
    sample_w = target_w * sample_rate;
    sample_buffer.resize(4 * sample_h * sample_w);

}

void SoftwareRendererImp::set_render_target(unsigned char* render_target, size_t width, size_t height) {

    // Task 4: 
    // You may want to modify this for supersampling support
    this->render_target = render_target;
    this->target_w = width;
    this->target_h = height;
    this->sample_rate = 1;
    sample_h = target_h * sample_rate;
    sample_w = target_w * sample_rate;
    sample_buffer.resize(4 * sample_h * sample_w);
}

void SoftwareRendererImp::draw_element(SVGElement* element) {

    // Task 5 (part 1):
    // Modify this to implement the transformation stack
    auto temp = transformation;
    transformation = transformation * element->transform;

    switch (element->type) {
    case POINT:
      draw_point(static_cast<Point&>(*element));
      break;
    case LINE:
      draw_line(static_cast<Line&>(*element));
      break;
    case POLYLINE:
      draw_polyline(static_cast<Polyline&>(*element));
      break;
    case RECT:
      draw_rect(static_cast<Rect&>(*element));
      break;
    case POLYGON:
      draw_polygon(static_cast<Polygon&>(*element));
      break;
    case ELLIPSE:
      draw_ellipse(static_cast<Ellipse&>(*element));
      break;
    case IMAGE:
      draw_image(static_cast<Image&>(*element));
      break;
    case GROUP:
      draw_group(static_cast<Group&>(*element));
      break;
    default:
      break;
    }
      transformation = temp;
}


// Primitive Drawing //

void SoftwareRendererImp::draw_point( Point& point ) {

  Vector2D p = transform(point.position);
  rasterize_point( p.x, p.y, point.style.fillColor );

}

void SoftwareRendererImp::draw_line( Line& line ) { 

  Vector2D p0 = transform(line.from);
  Vector2D p1 = transform(line.to);
  rasterize_line( p0.x, p0.y, p1.x, p1.y, line.style.strokeColor );

}

void SoftwareRendererImp::draw_polyline( Polyline& polyline ) {

  Color c = polyline.style.strokeColor;

  if( c.a != 0 ) {
    int nPoints = polyline.points.size();
    for( int i = 0; i < nPoints - 1; i++ ) {
      Vector2D p0 = transform(polyline.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polyline.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_rect( Rect& rect ) {

  Color c;
  
  // draw as two triangles
  float x = rect.position.x;
  float y = rect.position.y;
  float w = rect.dimension.x;
  float h = rect.dimension.y;

  Vector2D p0 = transform(Vector2D(x, y));
  Vector2D p1 = transform(Vector2D(x + w, y));
  Vector2D p2 = transform(Vector2D(x, y + h));
  Vector2D p3 = transform(Vector2D(x + w, y + h));
  
  // draw fill
  c = rect.style.fillColor;
  if (c.a != 0 ) {
    rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    rasterize_triangle( p2.x, p2.y, p1.x, p1.y, p3.x, p3.y, c );
  }

  // draw outline
  c = rect.style.strokeColor;
  if( c.a != 0 ) {
    rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    rasterize_line( p1.x, p1.y, p3.x, p3.y, c );
    rasterize_line( p3.x, p3.y, p2.x, p2.y, c );
    rasterize_line( p2.x, p2.y, p0.x, p0.y, c );
  }

}

void SoftwareRendererImp::draw_polygon( Polygon& polygon ) {

  Color c;

  // draw fill
  c = polygon.style.fillColor;
  if( c.a != 0 ) {

    // triangulate
    vector<Vector2D> triangles;
    triangulate( polygon, triangles );

    // draw as triangles
    for (size_t i = 0; i < triangles.size(); i += 3) {
      Vector2D p0 = transform(triangles[i + 0]);
      Vector2D p1 = transform(triangles[i + 1]);
      Vector2D p2 = transform(triangles[i + 2]);
      rasterize_triangle( p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, c );
    }
  }

  // draw outline
  c = polygon.style.strokeColor;
  if( c.a != 0 ) {
    int nPoints = polygon.points.size();
    for( int i = 0; i < nPoints; i++ ) {
      Vector2D p0 = transform(polygon.points[(i+0) % nPoints]);
      Vector2D p1 = transform(polygon.points[(i+1) % nPoints]);
      rasterize_line( p0.x, p0.y, p1.x, p1.y, c );
    }
  }
}

void SoftwareRendererImp::draw_ellipse( Ellipse& ellipse ) {

  // Extra credit 

}

void SoftwareRendererImp::draw_image( Image& image ) {

  Vector2D p0 = transform(image.position);
  Vector2D p1 = transform(image.position + image.dimension);

  rasterize_image( p0.x, p0.y, p1.x, p1.y, image.tex );
}

void SoftwareRendererImp::draw_group( Group& group ) {

  for ( size_t i = 0; i < group.elements.size(); ++i ) {
    draw_element(group.elements[i]);
  }

}

// Rasterization //

// The input arguments in the rasterization functions 
// below are all defined in screen space coordinates

void SoftwareRendererImp::rasterize_point( float x, float y, Color color) {

    // fill in the nearest pixel
    int sx = (int)std::floor(x * sample_rate);
    int sy = (int)std::floor(y * sample_rate);
	if (sample_rate > 1) {
		for (size_t i = 0; i < sample_rate; ++i) {
			for (size_t j = 0; j < sample_rate; ++j) {
				fill_sample(sx + j, sy + i, color);
			}
		}
	} else {
		int sx = (int)floor(x);
		int sy = (int)floor(y);
        fill_pixel(sx, sy, color);
    }
}

void SoftwareRendererImp::rasterize_line( float x0, float y0,
                                          float x1, float y1,
                                          Color color) {

    // Task 2: 
    // Implement line rasterization
    int sx1 = (int)std::floor(x1);
    int sx0 = (int)std::floor(x0);
    int sy1 = (int)std::floor(y1);
    int sy0 = (int)std::floor(y0);
    if (sx0 > sx1) {
        std::swap(sx0, sx1);
        std::swap(sy0, sy1);
    }
    int dx = sx1 - sx0;
	int dy = sy1 - sy0;
    if (dy >= 0) {
        if (dy <= dx) {
            int eps = 0;
            int y = sy0;
            for (int x = sx0; x <= sx1; ++x) {
                rasterize_point(x, y, color);
                eps += dy;
                if ((eps << 1) >= dx) {
                    ++y;
                    eps -= dx;
                }
            }
        }
        else {
            int eps = 0;
            int x = sx0;
            for (int y = sy0; y <= sy1; ++y) {
                rasterize_point(x, y, color);
                eps += dx;
                if ((eps << 1) >= dy) {
                    ++x;
                    eps -= dy;
                }
            }
        }
    } else {
        if (dy >= -dx) {
            int eps = 0;
            int y = sy0;
            for (int x = sx0; x <= sx1; ++x) {
                rasterize_point(x, y, color);
                eps += dy;
                if ((eps << 1) <= -dx) {
                    --y;
                    eps += dx;
                }
            }
        } else {
			std::swap(sx0, sx1);
			std::swap(sy0, sy1);
			dy = sy1 - sy0;
			dx = sx1 - sx0;
			int eps = 0;
			int x = sx0;
			for (int y = sy0; y <= sy1; ++y) {
				rasterize_point(x, y, color);
				eps += dx;
				if ((eps << 1) <= -dy) {
					--x;
					eps += dy;
				}
			}
        }
    }
}

void SoftwareRendererImp::rasterize_triangle( float x0, float y0,
                                              float x1, float y1,
                                              float x2, float y2,
                                              Color color ) {
    // Task 3: 
    // Implement triangle rasterization
    int sx2 = (int)std::floor(x2);
    int sx1 = (int)std::floor(x1);
    int sx0 = (int)std::floor(x0);
    int sy2 = (int)std::floor(y2);
    int sy1 = (int)std::floor(y1);
    int sy0 = (int)std::floor(y0);

    // Bounding box
    int x_min = std::min({ sx0, sx1, sx2 }) - 1;
    int x_max = std::max({ sx0, sx1, sx2 }) + 1;
    int y_min = std::min({ sy0, sy1, sy2 }) - 1;
    int y_max = std::max({ sy0, sy1, sy2 }) + 1;

    // Check pixels in bounding box
    for (float y = y_max - (0.5f / sample_rate); y >= y_min; y -= (1.0f / sample_rate)) {
        for (float x = x_min + (0.5f / sample_rate); x <= x_max; x += (1.0f / sample_rate)) {
            float apx = x0 - x;
            float apy = y0 - y;
            float bpx = x1 - x;
            float bpy = y1 - y;
            float cpx = x2 - x;
            float cpy = y2 - y;

            float abx = x1 - x0;
            float aby = y1 - y0;
            float cax = x0 - x2;
            float cay = y0 - y2;
            float bcx = x2 - x1;
            float bcy = y2 - y1;

            bool ab_ap = abx * apy - aby * apx >= 0.0f;
            bool bc_bp = bcx * bpy - bcy * bpx >= 0.0f;
            bool ca_cp = cax * cpy - cay * cpx >= 0.0f;

            bool all_same_sign = (ab_ap == bc_bp) && (ab_ap == ca_cp);
            if (all_same_sign) {
                if (sample_rate > 1) {
                    fill_sample((int)std::floor(x * sample_rate), (int)std::floor(y * sample_rate), color);
                } else {
                    fill_pixel((int)std::floor(x), (int)std::floor(y), color);
                }
            }
        }
    }
}

void SoftwareRendererImp::rasterize_image( float x0, float y0,
                                           float x1, float y1,
                                           Texture& tex ) {
    // Task 6: 
    // Implement image rasterization

}

void SoftwareRendererImp::sample_pixel(size_t start, size_t num_workers) {
    for (size_t i = start; i < target_h * target_w; i += num_workers) {
        size_t sampled_size = sample_rate * sample_rate;
        size_t x = i % target_w;
        size_t y = i / target_w;
        size_t sx = sample_rate * x;
        size_t sy = sample_rate * y;
        Color fill_color;
        fill_color.r = 0.0f;
        fill_color.g = 0.0f;
        fill_color.b = 0.0f;
        fill_color.a = 0.0f;
        for (size_t by = 0; by < sample_rate; ++by) {
            for (size_t bx = 0; bx < sample_rate; ++bx) {
                fill_color.r += (static_cast<float>(sample_buffer[4 * (sx + bx + (sy + by) * sample_w)]) / 255.0f);
                fill_color.g += (static_cast<float>(sample_buffer[4 * (sx + bx + (sy + by) * sample_w) + 1]) / 255.0f);
                fill_color.b += (static_cast<float>(sample_buffer[4 * (sx + bx + (sy + by) * sample_w) + 2]) / 255.0f);
                fill_color.a += (static_cast<float>(sample_buffer[4 * (sx + bx + (sy + by) * sample_w) + 3]) / 255.0f);
            }
        }
        fill_color.r /= static_cast<float>(sampled_size);
        fill_color.g /= static_cast<float>(sampled_size);
        fill_color.b /= static_cast<float>(sampled_size);
        fill_color.a /= static_cast<float>(sampled_size);
        fill_pixel(x, y, fill_color);
    }
}

// resolve samples to render target
void SoftwareRendererImp::resolve( void ) {

    // Task 4: 
    // Implement supersampling
    // You may also need to modify other functions marked with "Task 4".
    if (sample_rate == 1) {
        memset(sample_buffer.data(), 255, 4 * sample_w * sample_h);
        return;
    }
    int num_threads = std::thread::hardware_concurrency();
    size_t sampled_size = sample_rate * sample_rate;
    // std::chrono::time_point<std::chrono::system_clock> start, end;
    // start = std::chrono::system_clock::now();
    std::vector<std::thread> workers;
    for (size_t id = 0; id < num_threads; ++id) {
        workers.emplace_back(&CMU462::SoftwareRendererImp::sample_pixel, this, id, num_threads);
    }
    for (auto& thread : workers) {
        thread.join();
    }
    // end = std::chrono::system_clock::now();
    // std::chrono::duration<double, std::milli> elapsed_time = end - start;
    // std::cout << elapsed_time.count() << std::endl;
    memset(sample_buffer.data(), 255, 4 * sample_w * sample_h);
}

void SoftwareRendererImp::fill_pixel(int x, int y, const Color& color) {
	if (x < 0 || x >= target_w) return;
	if (y < 0 || y >= target_h) return;

	// fill sample - NOT doing alpha blending!
	render_target[4 * (x + y * target_w)] = (uint8_t)(color.r * 255);
	render_target[4 * (x + y * target_w) + 1] = (uint8_t)(color.g * 255);
	render_target[4 * (x + y * target_w) + 2] = (uint8_t)(color.b * 255);
	render_target[4 * (x + y * target_w) + 3] = (uint8_t)(color.a * 255);
}

void SoftwareRendererImp::fill_sample(int sx, int sy, const Color& color) {
	if (sx < 0 || sx >= sample_w) return;
	if (sy < 0 || sy >= sample_h) return;

	// fill sample - NOT doing alpha blending!
	sample_buffer[4 * (sx + sy * sample_w)] = (uint8_t)(color.r * 255);
	sample_buffer[4 * (sx + sy * sample_w) + 1] = (uint8_t)(color.g * 255);
	sample_buffer[4 * (sx + sy * sample_w) + 2] = (uint8_t)(color.b * 255);
	sample_buffer[4 * (sx + sy * sample_w) + 3] = (uint8_t)(color.a * 255);
}

} // namespace CMU462
