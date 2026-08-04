#ifndef EXAMPLE_NODE_H
#define EXAMPLE_NODE_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class ExampleNode : public Sprite2D {
    GDCLASS(ExampleNode, Sprite2D)

private:
    double speed;

protected:
    static void _bind_methods();

public:
    ExampleNode();
    ~ExampleNode();

    void _process(double delta) override;
    void set_speed(const double p_speed);
    double get_speed() const;
};

}

#endif // EXAMPLE_NODE_H