#include "example_node.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void ExampleNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_speed"), &ExampleNode::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &ExampleNode::set_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
}

ExampleNode::ExampleNode() {
    speed = 400.0;
}

ExampleNode::~ExampleNode() {}

void ExampleNode::_process(double delta) {
    Vector2 pos = get_position();
    pos.x += speed * delta;
    set_position(pos);
}

void ExampleNode::set_speed(const double p_speed) {
    speed = p_speed;
}

double ExampleNode::get_speed() const {
    return speed;
}