#include <merely3d/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef void(*GlfwWindowDestroyFunc)(GLFWwindow *);
typedef std::unique_ptr<GLFWwindow, GlfwWindowDestroyFunc> GlfwWindowPtr;

namespace merely3d
{
    class Window::WindowData
    {
    public:
        WindowData(GlfwWindowPtr ptr) : glfw_window(std::move(ptr)), viewport_size(0, 0) {}

        GlfwWindowPtr glfw_window;
        std::pair<int, int> viewport_size;
    };

    static void check_and_update_viewport_size(GLFWwindow * window, int & viewport_width, int & viewport_height)
    {
        int fb_width;
        int fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        if (fb_width != viewport_width || fb_height != viewport_height)
        {
            viewport_width = fb_width;
            viewport_height = fb_height;
            glViewport(0, 0, viewport_width, viewport_height);
        }
    }

    Window::Window(Window && other)
        : _d(nullptr)
    {
        if (other._d)
        {
            _d = other._d;
            other._d = nullptr;
        }
    }

    Window::~Window()
    {
        if (_d)
        {
            delete _d;
        }
    }

    Window::Window(WindowData * data)
    {
        assert(data);
        _d = data;
    }

    void Window::render_frame_impl(Frame & frame)
    {
        assert(_d);
        glfwMakeContextCurrent(_d->glfw_window.get());

        check_and_update_viewport_size(_d->glfw_window.get(), _d->viewport_size.first, _d->viewport_size.second);

        // TODO: Make clear color configurable
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Process resulting command buffer from Frame

        /* Swap front and back buffers */
        glfwSwapBuffers(_d->glfw_window.get());

        /* Poll for and process events */
        glfwPollEvents();
    }

    bool Window::should_close() const
    {
        return glfwWindowShouldClose(_d->glfw_window.get());
    }

    Window WindowBuilder::build() const
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // This is apparently needed on Mac OS X. Can we simply set it for all platforms...?
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow * glfw_window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);

        if (!glfw_window)
        {
            // TODO: Better error, more information
            throw std::runtime_error("Failed to initialize GLFWwindow");
        }

        // TODO: Should glad initialization happen in a different location...?
        // Is it safe to re-initialize GLAD?
        glfwMakeContextCurrent(glfw_window);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            // TODO: Better error message
            throw std::runtime_error("Failed to initialize GLAD");
        }

        auto window_ptr = GlfwWindowPtr(glfw_window, glfwDestroyWindow);

        auto window_data = new Window::WindowData(std::move(window_ptr));

        auto window = Window(window_data);
        return std::move(window);
    }

}