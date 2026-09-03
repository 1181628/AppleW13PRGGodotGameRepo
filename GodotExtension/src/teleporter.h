#ifndef TELEPORTER_H
#define TELEPORTER_H

#include "interactable.h"

namespace godot {

class Teleporter : public Interactable {
    GDCLASS(Teleporter, Interactable);

private:
    String path = "res://scenes/main_scene.tscn";

protected:
    static void _bind_methods();

public:
    Teleporter();
    ~Teleporter();

    void interact() override;
};

}

#endif