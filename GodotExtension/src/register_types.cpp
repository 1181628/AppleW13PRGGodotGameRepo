#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

// Allows Godot to recognise and use the classes we created in C++ by including their header files
#include "player.h"
#include "enemy1.h"
#include "playerStatus.h"
#include "interactable.h"
#include "teleporter.h"
#include "titleScreen.h"

using namespace godot;

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Registers the C++ class with Godot's ClassDB so it can appear under its assigned parent class and be used as a node in Godot
    ClassDB::register_class<Player>();
    ClassDB::register_class<Enemy1>();
    ClassDB::register_class<PlayerStatus>();
    ClassDB::register_class<Interactable>();
    ClassDB::register_class<Teleporter>();
    ClassDB::register_class<TitleScreen>();
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
GDExtensionBool GDE_EXPORT my_extension_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address, 
    const GDExtensionClassLibraryPtr p_library, 
    GDExtensionInitialization *r_initialization
) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_gdextension_types);
    init_obj.register_terminator(uninitialize_gdextension_types);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}