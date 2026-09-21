#include "titleScreen.h"
#include "playerStatus.h"
#include "saveManager.h"
#include "playerStatusBar.h"
#include "pastRecords.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

TitleScreen::TitleScreen() {
}

TitleScreen::~TitleScreen() {
}

void TitleScreen::_bind_methods() {
    // Register the buttons callback
    ClassDB::bind_method(D_METHOD("on_new_game_pressed"), &TitleScreen::on_new_game_pressed);
    ClassDB::bind_method(D_METHOD("on_load_game_pressed"), &TitleScreen::on_load_game_pressed);
    ClassDB::bind_method(D_METHOD("on_exit_game_pressed"), &TitleScreen::on_exit_game_pressed);
    ClassDB::bind_method(D_METHOD("on_past_records_pressed"), &TitleScreen::on_past_records_pressed);
}

void TitleScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Hide all nodes in the player health bar & past records panel
    get_tree()->call_group("health_bar_visual", "hide");
    get_node<PastRecords>("PastRecords")->hide();


    // Intro animations
    AnimationPlayer *intro_player = get_node<AnimationPlayer>(NodePath("IntroAnimationPlayer"));
    AnimationPlayer *rotation_player = get_node<AnimationPlayer>(NodePath("RotationAnimationPlayer"));
    intro_player->play("intro");
    intro_player->advance(0.0);
    rotation_player->play("rotation");
    rotation_player->advance(0.0);

    // Retrieve the buttons
    Button *new_game = get_node<Button>("V/NewGame");
    Button *load_game = get_node<Button>("V/LoadGame");
    Button *exit_game = get_node<Button>("V/ExitGame");
    Button *records_button = get_node<Button>("Button");
    // Connect the buttons to its callback
    new_game->connect("pressed", Callable(this, "on_new_game_pressed"));
    load_game->connect("pressed", Callable(this, "on_load_game_pressed"));
    exit_game->connect("pressed", Callable(this, "on_exit_game_pressed"));
    records_button->connect("pressed", Callable(this, "on_past_records_pressed"));
}

// Restore gameplay display settings when the title screen leaves the scene tree
void TitleScreen::_exit_tree() {
    // Show all nodes in the health bar visual group
    get_tree()->call_group("health_bar_visual", "show");
}

// Reset player data and open the main scene for a new game
void TitleScreen::on_new_game_pressed() {
    // Reset the player's data to its initial values
    PlayerStatus *status = Object::cast_to<PlayerStatus>(get_node_or_null(NodePath("/root/PlayerStatusData")));
    status->reset_for_new_game();

    // Refresh the status bar after resetting player data
    PlayerStatusBar *status_bar = Object::cast_to<PlayerStatusBar>(get_node_or_null(NodePath("/root/StatusBar")));
    status_bar->refresh_player_status();

    // Request a scene change and store the returned error code
    Error result = get_tree()->change_scene_to_file("res://scenes/main_scene.tscn");

}

// Load saved player data and open the main scene
void TitleScreen::on_load_game_pressed() {
    PlayerStatus *status = Object::cast_to<PlayerStatus>(get_node_or_null(NodePath("/root/PlayerStatusData")));

    // Stop if the player status node is missing or has an incompatible type
    if (status == nullptr) {
        return;
    }

    // Keeps the Player on the menu if loading fails instead of opening an invalid game
    if (!SaveManager::load_game(status)) {
        return;
    }

    // Refresh the status bar after restoring the saved values
    PlayerStatusBar *status_bar = Object::cast_to<PlayerStatusBar>(get_node_or_null(NodePath("/root/StatusBar")));
    if (status_bar != nullptr) {
        status_bar->refresh_player_status();
    }

    // Open the main scene
    get_tree()->change_scene_to_file("res://scenes/main_scene.tscn");
}

// Close the game when the exit button is pressed
void TitleScreen::on_exit_game_pressed() {
    // Request application shutdown
    get_tree()->quit();
}

// Open the past records page when the button is pressed
void TitleScreen::on_past_records_pressed() {
    PastRecords *records = get_node<PastRecords>("PastRecords");

    // Refresh the saved results before displaying the panel
    records->display_records();
    records->show();
}