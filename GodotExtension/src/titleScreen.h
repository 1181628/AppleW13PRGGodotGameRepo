#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <godot_cpp/classes/node.hpp>

// Add other Godot classes or data types when needed.
// #include <godot_cpp/variant/vector2.hpp>
// #include <godot_cpp/variant/string.hpp>

namespace godot {

// Change TitleScreen and Node to the required class and parent class.
class TitleScreen : public Node {
    GDCLASS(TitleScreen, Node);

private:
    // Add variables that the class needs to store.
    // int health = 100;
    // double move_speed = 200.0;
    // bool is_active = true;

protected:
    static void _bind_methods();

public:
    TitleScreen();
    ~TitleScreen();

    // Keep only the Godot callbacks that the class needs.
    void _ready() override;
    void _process(double delta) override;
    // void _physics_process(double delta) override;

    // Declare custom functions here.
    // void take_damage(int damage);
    // void attack();
    // bool is_alive() const;
};

}

#endif