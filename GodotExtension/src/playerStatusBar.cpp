#include "playerStatusBar.h"
#include "playerStatus.h"

#include <algorithm>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace {

// Load a texture from the project's status image directory
Ref<Texture2D> load_status_texture(const String &filename) {return ResourceLoader::get_singleton()->load("res://assets/status/" + filename);}

// Build a numbered PNG filename
String numbered_png(const String &prefix, int number) {
    return prefix + String::num_int64(number) + ".png";
}

// Create an overlay sprite and add it to the status bar
Sprite2D *make_overlay(PlayerStatusBar *bar, const String &name) {
    Sprite2D *sprite = memnew(Sprite2D);
    sprite->set_name(name);
    sprite->set_centered(false);
    sprite->add_to_group("health_bar_visual");
    bar->add_child(sprite);
    return sprite;
}
} // namespace

PlayerStatusBar::PlayerStatusBar() = default;
PlayerStatusBar::~PlayerStatusBar() = default;

void PlayerStatusBar::_bind_methods() {
}

void PlayerStatusBar::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Configure the base sprite already present in the scene
    Sprite2D *base = get_node<Sprite2D>("Sprite2D");
    base->set_centered(false);
    base->set_scale(Vector2(0.25f, 0.25f));
    base->set_position(Vector2(0, 0));
    base->add_to_group("health_bar_visual");

    // Create overlays for health, attack, and speed
    make_overlay(this, "HealthOverlay");
    make_overlay(this, "AttackOverlay");
    make_overlay(this, "SpeedOverlay");

    // Display the player's current status
    refresh_player_status();
}

// Update the status bar
void PlayerStatusBar::refresh_player_status() {
    PlayerStatus *status = Object::cast_to<PlayerStatus>(get_node_or_null(NodePath("/root/PlayerStatusData")));
    // Stops the update if the Player's status data is unavailable
    if (status == nullptr) {
        return;
    }

    // Limits display values to the available images without changing the actual stats
    const int max_health = std::clamp(status->maxHealth, 5, 10);
    const int health = std::clamp(status->health, 0, max_health);
    // Convert attack damage and maximum horizontal speed to icon levels 1–5
    const int attack_level = std::clamp(1 + (status->attackDamage - 100) / 200, 1, 5);
    const int speed_level = std::clamp(1 + static_cast<int>((status->maxHorizontalSpeed - 120.0) / 50.0), 1, 5);

    // Load the images that correspond to the current values
    Ref<Texture2D> base_texture = load_status_texture(numbered_png("status base", max_health - 5));
    // Leaves the heart texture empty at zero health so no filled hearts are shown
    Ref<Texture2D> health_texture;
    if (health > 0) {
        health_texture = load_status_texture(numbered_png("health", health));
    }
    Ref<Texture2D> attack_texture = load_status_texture(numbered_png("", attack_level));
    Ref<Texture2D> speed_texture = load_status_texture(numbered_png("", speed_level));

    // Keep the current display if any required image has not loaded
    if (base_texture.is_null() || (health > 0 && health_texture.is_null()) ||
        attack_texture.is_null() || speed_texture.is_null()) {
        return;
    }

    // Update the textures
    Sprite2D *base = get_node<Sprite2D>("Sprite2D");
    Sprite2D *hearts = get_node<Sprite2D>("HealthOverlay");
    Sprite2D *attack = get_node<Sprite2D>("AttackOverlay");
    Sprite2D *speed = get_node<Sprite2D>("SpeedOverlay");
    base->set_texture(base_texture);
    hearts->set_texture(health_texture);
    attack->set_texture(attack_texture);
    speed->set_texture(speed_texture);

    const Vector2 base_scale = base->get_scale();

    // Align the health overlay with the base sprite
    hearts->set_scale(base_scale);
    hearts->set_position(Vector2(0, 0));

    // Set the attack icon's size and position
    const float attack_size = 1.0f;
    const Vector2 attack_position = Vector2(0, 0);
    attack->set_scale(base_scale * attack_size);
    attack->set_position(attack_position);

    // Offset the speed icon by 185 original pixels horizontally and 1 vertically
    const float speed_size = 1.0f;
    const Vector2 speed_position = Vector2(185.0f * base_scale.x, 1.0f * base_scale.y);
    speed->set_scale(base_scale * speed_size);
    speed->set_position(speed_position);
}
