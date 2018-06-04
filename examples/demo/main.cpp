#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>
#include <algorithm>

#include <merely3d/app.hpp>
#include <merely3d/window.hpp>
#include <merely3d/camera_controller.hpp>

#include <Eigen/Geometry>

#include "example_model.hpp"

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Key;
using merely3d::Action;
using merely3d::EventHandler;
using merely3d::Material;
using merely3d::Color;
using merely3d::CameraController;
using merely3d::Line;

using merely3d::red;
using merely3d::green;

using merely3d::renderable;
using merely3d::Rectangle;
using merely3d::Box;
using merely3d::Sphere;

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Quaternionf;
using Eigen::AngleAxisf;

/// A simple click event handler, showcasing how to handle click events,
/// as well as how to map screen coordinates to a ray moving
/// out from the camera, which can be used in e.g. mouse picking applications.
///
/// Here we just render the corresponding ray and let it stay in the simulation for
/// a few seconds before removing it (e.g. the lifetime variable below).
class ClickEventHandler : public merely3d::EventHandler
{
public:
    ClickEventHandler()
        : line(Vector3f::Zero(), Vector3f::Zero()),
          lifetime(0.0)
    {
        line.color = merely3d::red();
    }

    bool mouse_button_press(merely3d::Window &window,
                            merely3d::MouseButton button,
                            merely3d::Action action,
                            int) override
    {
        using merely3d::MouseButton;
        using merely3d::Action;

        if (button == MouseButton::Right && action == Action::Press)
        {
            const auto cursor_pos = window.get_current_cursor_position();
            const auto view_pos = window.unproject_screen_coordinates(cursor_pos);

            const Vector3f world_pos = window.camera().transform() * view_pos;
            const Vector3f direction = (world_pos - window.camera().position()).normalized();

            const float ray_length = 100.0f;

            line.from = window.camera().position() + 0.3 * direction;
            line.to = window.camera().position() + ray_length * direction;
            lifetime = 10.0f;

            return true;
        }

        return false;
    }

    void before_frame(Window &, Frame & frame) override
    {
        if (lifetime > 0.0f)
        {
            frame.draw_line(line);
            frame.draw(renderable(Sphere(0.001))
                        .with_position(line.from)
                        .with_material(Material().with_color(merely3d::red())
                                                 .with_pattern_grid_size(0.0f)));
        }

        lifetime = std::max(0.0, lifetime - frame.time_since_prev_frame());
    }

private:
    merely3d::Line line;
    double lifetime;
};

int main()
{
    // Constructing the app first is essential: it makes sure that
    // GLFW is set up properly. Note that as an alternative, you can call
    // glfw init/terminate yourself directly, but you must be careful that
    // any windows are destroyed before calling terminate(). App automatically
    // takes care of this as long as it outlives any windows.
    merely3d::App app;

    auto window = WindowBuilder()
                   .dimensions(1024, 768)
                   .title("Hello merely3d!")
                   .multisampling(8)
                   .build();

    window.camera().look_in(Vector3f(1.0, 0.0, -1), Vector3f(0.0, 0.0, 1.0));
    window.camera().set_position(Vector3f(-1.0, 0.0, 3.0));

    window.add_event_handler(std::shared_ptr<EventHandler>(new CameraController));
    window.add_event_handler(std::shared_ptr<EventHandler>(new ClickEventHandler));

    // Create a static mesh which we can use when rendering
    const auto & vn = merely3d::example_model::vertices_and_normals;
    const auto & idx = merely3d::example_model::indices;
    const auto model_vertices_normals = std::vector<float>(vn.begin(), vn.end());
    const auto model_indices = std::vector<unsigned int>(idx.begin(), idx.end());
    const auto model = merely3d::StaticMesh(model_vertices_normals, model_indices);

    std::cout << model_vertices_normals.size() << std::endl;

    while (!window.should_close())
    {
        window.render_frame([&model] (Frame & frame)
        {
            frame.draw(renderable(Rectangle(0.5, 0.5))
                        .with_position(1.0, 0.0, 0.5)
                        .with_orientation(AngleAxisf(0.78, Vector3f(1.0f, 0.0f, 0.0f)))
                        .with_material(Material().with_color(Color(0.5, 0.3, 0.3))));

            frame.draw(renderable(Box(1.0, 1.0, 1.0))
                        .with_position(4.0, 0.0, 1.1));

            frame.draw(renderable(Box(0.2, 5.0, 1.0))
                        .with_position(0.0, 0.0, 1.0));

            frame.draw(renderable(Box(0.2, 1.0, 1.0))
                        .with_position(0.0, 0.0, 5.0)
                        .with_orientation(AngleAxisf(0.5, Vector3f(1.0, 1.0, 1.0)))
                        .with_material(Material().with_color(red())));

            frame.draw(renderable(Sphere(1.0))
                        .with_position(3.0, 3.0, 3.0));

            frame.draw(renderable(Sphere(1.0))
                        .with_position(3.0, 3.0, 6.0)
                        .with_material(Material().with_wireframe(true)));

            frame.draw(renderable(Box(1.0, 2.0, 1.0))
                        .with_position(-3.0, 3.0, 9.0)
                        .with_material(Material().with_wireframe(true)));

            frame.draw(renderable(Rectangle(10.0, 2.0))
                        .with_position(0.0, 0.0, 10.0)
                        .with_material(Material().with_pattern_grid_size(0.0f)));

            frame.draw(renderable(Rectangle(10.0, 2.0))
                        .with_position(0.0, 8.0, 10.0)
                        .with_material(Material().with_wireframe(true)));

            frame.draw(renderable(model)
                        .with_position(8.0, 8.0, 5.0)
                        .with_material(Material().with_pattern_grid_size(0.0f)
                                                 .with_color(green())));

            frame.draw_line(Line(Vector3f(0.0, 0.0, 0.0), Vector3f(10.0, -5.0, 10.0)));

            const auto floor_color = Color(0.5f, 0.35f, 0.35f);
            frame.draw(renderable(Rectangle(20.0f, 20.0f))
                               .with_position(0.0f, 0.0f, 0.0f)
                               .with_material(Material().with_color(floor_color)));
        });
    }

    return 0;
}
