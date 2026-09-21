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

// I reused Interactable so each object would not need its own detection code
// The Player could use the same interaction call for different object types
// However, I wrote this early on, and the final game differed from my original plan
// A simpler approach would have been enough for teleporting, so this structure became less useful than expected
// I still used it for the final teleporter because I don't want to wast it