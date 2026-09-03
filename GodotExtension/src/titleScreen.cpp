// Change this to the matching header filename.
#include "titleScreen.h"

// Add other includes when needed.
// #include <godot_cpp/variant/vector2.hpp>
// #include <godot_cpp/variant/utility_functions.hpp>
// #include <godot_cpp/classes/input.hpp>

using namespace godot;


// Constructor.
TitleScreen::TitleScreen() {
    // Set initial values here when needed.
}


// Destructor.
TitleScreen::~TitleScreen() {
    // Add cleanup code here when needed.
}


// Exposes C++ methods, properties, and signals to Godot.
void TitleScreen::_bind_methods() {
    // Leave empty if nothing needs to be exposed.
}


// Runs once when the node is ready in the scene.
void TitleScreen::_ready() {
    // Add starting or scene setup here.
}


// Runs every rendered frame.
void TitleScreen::_process(double delta) {
    // Remove this line when delta is used.
    (void)delta;

    // Add frame-based behaviour here.
}


// Use this instead of _process() for movement and physics.
// It must also be declared in the .h file.
// void TitleScreen::_physics_process(double delta) {
//     (void)delta;
//
//     // Add movement, gravity, and physics behaviour here.
// }


// Define custom functions here.
// void TitleScreen::take_damage(int damage) {
// }
//
// void TitleScreen::attack() {
// }
//
// bool TitleScreen::is_alive() const {
//     return true;
// }