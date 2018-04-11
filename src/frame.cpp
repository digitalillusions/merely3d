#include <merely3d/frame.hpp>

#include "command_buffer.hpp"

using Eigen::Transform;
using Eigen::Translation3f;
using Eigen::DiagonalMatrix;

namespace merely3d
{
    template <>
    void Frame::draw(const merely3d::Renderable<Box> &renderable)
    {
        _buffer->push_renderable(renderable);
    }

    template <>
    void Frame::draw(const merely3d::Renderable<Rectangle> &renderable)
    {
        _buffer->push_renderable(renderable);
    }

    template <>
    void Frame::draw(const merely3d::Renderable<Sphere> & sphere)
    {
        _buffer->push_renderable(sphere);
    }
}