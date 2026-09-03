#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include <godot_cpp/classes/area2d.hpp>

namespace godot {

class Interactable : public Area2D {
    GDCLASS(Interactable, Area2D);

private:
    void _on_body_entered(Node2D *body);
    void _on_body_exited(Node2D *body);

protected:
    static void _bind_methods();

public:
    Interactable();
    ~Interactable();

    void _ready() override;

    // Runs when the Player interacts with this object
    virtual void interact();
};

}

#endif