#include "register_types.h"
#include "example_node.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

//include and recognise new functions I added
#include "player.h"

using namespace godot;

void initialize_gdextension_types() {
    //registe the new function
    ClassDB::register_class<Player>();
}

void uninitialize_gdextension_types() {}

extern "C" {
GDExtensionBool GDE_EXPORT my_extension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_gdextension_types);
    init_obj.register_uninitializer(uninitialize_gdextension_types);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}