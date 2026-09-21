#include "pauseScreen.h"
#include "rewardScreen.h"

#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/audio_stream_player.hpp>

using namespace godot;

void PauseScreen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("resume_game"), &PauseScreen::resume_game);
    ClassDB::bind_method(D_METHOD("on_main_menu_pressed"), &PauseScreen::on_main_menu_pressed);
    ClassDB::bind_method(D_METHOD("on_quit_game_pressed"), &PauseScreen::on_quit_game_pressed);
}

void PauseScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Draw above the tutorial CanvasLayer (layer 10)
    set_layer(20);
    add_to_group("pause_screen");

    // The menu must keep receiving input after the scene tree is paused
    set_process_mode(Node::PROCESS_MODE_ALWAYS);
    hide();

    // Find the menu buttons
    Button *resume = get_node<Button>("PanelContainer/MarginContainer/VBoxContainer/Resume");
    Button *main_menu = get_node<Button>("PanelContainer/MarginContainer/VBoxContainer/Main Menu");
    Button *quit_game = get_node<Button>("PanelContainer/MarginContainer/VBoxContainer/Quit Game");
    // Connect each button
    resume->connect("pressed", Callable(this, "resume_game"));
    main_menu->connect("pressed", Callable(this, "on_main_menu_pressed"));
    quit_game->connect("pressed", Callable(this, "on_quit_game_pressed"));
}

// Toggle the pause menu when the cancel action is pressed
void PauseScreen::_unhandled_input(const Ref<InputEvent> &event) {
    RewardScreen *reward = Object::cast_to<RewardScreen>(get_tree()->get_first_node_in_group("reward_screen"));

    // Blocks pause-menu input while the reward screen controls gameplay
    if (reward != nullptr && reward->is_reward_open()) {
        get_viewport()->set_input_as_handled();
        return;
    }

    // Ignores key-repeat events so holding Escape does not repeatedly toggle pause
    if (event->is_action_pressed("ui_cancel") && !event->is_echo()) {
        if (is_visible()) {
            resume_game();
        } else {
            pause_game();
        }
        // Prevent other nodes from processing this input event
        get_viewport()->set_input_as_handled();
    }
}

// Show the menu and pause gameplay
void PauseScreen::pause_game() {
    show();
    get_tree()->set_pause(true);

    get_node<AnimationPlayer>("AnimationPlayer")->play("enter");
}

// Resume gameplay and hide the menu
void PauseScreen::resume_game() {
    RewardScreen *reward = Object::cast_to<RewardScreen>(get_tree()->get_first_node_in_group("reward_screen"));

    hide();

    // Keep gameplay paused while rewards are open
    if (reward != nullptr && reward->is_reward_open()) {
        return;
    }

    get_tree()->set_pause(false);
    hide();
}

// Resume the scene tree before returning to the title screen
void PauseScreen::on_main_menu_pressed() {
    get_tree()->set_pause(false);
    get_tree()->change_scene_to_file("res://scenes/title_screen.tscn");
}

// Close the game
void PauseScreen::on_quit_game_pressed() {
    get_tree()->quit();
}