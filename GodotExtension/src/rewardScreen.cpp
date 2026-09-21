#include "rewardScreen.h"
#include "playerStatus.h"
#include "playerStatusBar.h"
#include "roomManager.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>
#include <godot_cpp/variant/rect2.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>

using namespace godot;

void RewardScreen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("open_reward"), &RewardScreen::open_reward);
    ClassDB::bind_method(D_METHOD("on_reroll_pressed"), &RewardScreen::on_reroll_pressed);
    ClassDB::bind_method(D_METHOD("on_card_pressed", "index"), &RewardScreen::on_card_pressed);
    ClassDB::bind_method(D_METHOD("on_animation_finished", "animation_name"), &RewardScreen::on_animation_finished);
    ClassDB::bind_method(D_METHOD("on_next_floor_pressed"), &RewardScreen::on_next_floor_pressed);
}

// =================================== REWARD SCREEN INITIALISATION ===================================
// Find the nodes and connect their signals.
void RewardScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Keep the reward interface active while gameplay is paused
    set_process_mode(Node::PROCESS_MODE_ALWAYS);
    // Hide the reward screen until the room is cleared
    hide();

    String base = "PanelContainer/MarginContainer/VBoxContainer/";

    // Find the three reward cards by adding their numbers to the node path
    for (int i = 0; i < 3; i++) {
        String path = base + "RewardsRow/RewardCard" + String::num_int64(i + 1);
        cards[i] = Object::cast_to<RewardCard>(get_node_or_null(NodePath(path)));
    }

    // Find the two navigation buttons and the entrance animation player
    reroll_button = Object::cast_to<BaseButton>(get_node_or_null(NodePath(base + "BottomRow/RerollButton")));
    next_floor_button = Object::cast_to<BaseButton>(get_node_or_null(NodePath(base + "BottomRow/NextFloorButton")));
    animation_player = Object::cast_to<AnimationPlayer>(get_node_or_null(NodePath("AnimationPlayer")));

    // Disable keyboard focus on the two navigation buttons
    reroll_button->set_focus_mode(Control::FOCUS_NONE);
    next_floor_button->set_focus_mode(Control::FOCUS_NONE);
    // Disable card focus and pass each card's index to its click handler
    for (int i = 0; i < 3; i++) {
        cards[i]->set_focus_mode(Control::FOCUS_NONE);
        cards[i]->connect("pressed", Callable(this, "on_card_pressed").bind(i));
    }
    
    // Refresh the rewards when the reroll button is pressed
    reroll_button->connect("pressed", Callable(this, "on_reroll_pressed"));
    // Handle the end of an animation so reward selection can be enabled
    animation_player->connect("animation_finished", Callable(this, "on_animation_finished"));
    next_floor_button->connect("pressed", Callable(this, "on_next_floor_pressed"));

    // Fill the reward pools
    create_reward_data();
    // Prevent the player from continuing before choosing a reward
    next_floor_button->set_disabled(true);

    // Mark the initial setup as complete
    ready = true;
    // Allow other nodes to find this screen
    add_to_group("reward_screen");
}

// Prepare a new reward selection and play the entrance animation
void RewardScreen::open_reward() {
    // Reject requests made before setup or while the screen is already open
    if (!ready || reward_open) {
        return;
    }

    // Mark the screen as open
    reward_open = true;
    entering = true;
    selected_card = -1;
    PlayerStatus *status = Object::cast_to<PlayerStatus>(get_node_or_null(NodePath("/root/PlayerStatusData")));
    // Adds earned bonus rerolls to the three available for each reward screen
    rerolls = 3 + (status != nullptr ? status->bonusRerolls : 0);

    // Generate and display the three rewards
    randomise_rewards();

    // Prevent card selection during the entrance animation
    for (int i = 0; i < 3; i++) {
        cards[i]->set_disabled(true);
    }
    // Keep Next Floor unavailable
    next_floor_button->set_disabled(true);
    // Show the reset reroll count
    update_reroll_button();

    // Pause gameplay and make the reward interface visible
    get_tree()->set_pause(true);
    show();

    // Restart the entrance animation
    animation_player->stop();
    animation_player->play(entrance_animation);
    animation_player->advance(0.0);
}

// Allow reward selection once the entrance animation has finished
void RewardScreen::on_animation_finished(StringName animation_name) {
    // Ignore unrelated animations or calls made outside the entrance stage
    if (!ready || !entering ||
        animation_name != entrance_animation) {
        return;
    }

    // Record that the entrance animation is no longer running
    entering = false;
    // Make all three reward cards available for selection
    for (int i = 0; i < 3; i++) {
        cards[i]->set_disabled(false);
    }
    // Enable rerolling if the player still meets its requirements
    update_reroll_button();
}

// =================================== REWARD DATA AND POOLS ===================================
// Load item icons and organise rewards by rarity
void RewardScreen::create_reward_data() {
    // Load the large image containing all item icons.
    Ref<Texture2D> item_sheet = ResourceLoader::get_singleton()->load("res://assets/gameObjects/IconSet.png");

    // Define the available rewards in two rarity pools
    //
    // Rules:
    // 1. Pool 0 contains common rewards and Pool 1 contains rare rewards
    // 2. Each reward stores its name, description, image, effect amount and icon region
    // 3. All reward icons use different regions of the same shared image
    // 4. The selected reward's effect is applied when the Player chooses its card, not here
    reward_pools[0] = {
        {"APPLE", "+1 Health", item_sheet, 1, Rect2(64, 866, 30, 32)},
        {"STRAWBERRY", "+3 Health", item_sheet, 3, Rect2(0, 866, 30, 30)},
        {"CARROT", "+1 Max Health", item_sheet, 1, Rect2(256, 866, 30, 30)},
        {"WHETSTONE", "+1 Attack", item_sheet, 100, Rect2(290, 2176, 30, 32)},
        {"LONG BLADE", "+2 Attack", item_sheet, 200, Rect2(480, 1440, 32, 32)},
        {"LIGHT BOOTS", "+1 Speed", item_sheet, 10, Rect2(224, 3874, 30, 30)}
    };

    reward_pools[1] = {
        {"JADE PUMPKIN", "+3 Max Health", item_sheet, 3, Rect2(416, 866, 30, 30)},
        {"WARRIOR'S SEAL", "+3 Attack", item_sheet, 300, Rect2(288, 1440, 32, 32)},
        {"REBIRTH RING", "Restore Full Health", item_sheet, 100, Rect2(448, 1856, 30, 30)},
        {"FORTUNE GEM", "+1 Forever Reroll Chance", item_sheet, 1, Rect2(384, 1824, 32, 32)}
    };
}

// Choose a rarity and then an item from that rarity for each card
void RewardScreen::randomise_rewards() {
    // Randomly select a reward for each of the three cards
    //
    // Rules:
    // 1. Each card has a 70% chance of common and a 30% chance of rare
    // 2. Each item in the selected rarity pool has an equal chance of being chosen
    // 3. Cards are selected independently, so duplicate rewards are allowed
    // 4. Each selected reward is stored and then displayed on its matching card
    for (int i = 0; i < 3; i++) {
        int roll = UtilityFunctions::randi_range(1, 100);
        int rarity;

        // Give each card a 70% chance of common and a 30% chance of rare
        if (roll <= 70) {
            rarity = 0;
        } else {
            rarity = 1;
        }

        // Choose a random item index from the selected reward pool
        int item_index = UtilityFunctions::randi_range(0, static_cast<int>(reward_pools[rarity].size()) - 1);
        // Store the chosen reward so it matches the card's displayed contents
        displayed_rewards[i] = reward_pools[rarity][item_index];
        // Displays the stored reward so the visible card matches the reward applied
        cards[i]->display_reward(rarity_names[rarity], displayed_rewards[i]);
    }
}

// =================================== REWARD SELECTION AND APPLICATION ===================================
// Refresh the cards without replaying the entrance animation
void RewardScreen::on_reroll_pressed() {
    // Reject rerolls if the screen is closed, entering, already selected, or out of rerolls
    if (!reward_open || entering ||
        selected_card != -1 || rerolls <= 0) {
        return;
    }

    rerolls-= 1;

    // Replace all three rewards with newly generated choices
    randomise_rewards();
    // Refresh the remaining count and check whether another reroll is allowed
    update_reroll_button();
}

// Update the remaining count and availability of rerolling
void RewardScreen::update_reroll_button() {
    String text = String("REROLL (") + String::num_int64(rerolls) + " LEFT)";
    Button *text_button = Object::cast_to<Button>(reroll_button);

    // Display the count as button text
    if (text_button != nullptr) {
        text_button->set_text(text);
    } else {
        // TextureButtons display the count through a tooltip.
        reroll_button->set_tooltip_text(text);
    }
    // Disable rerolling during the entrance, after selection, or when none remain
    reroll_button->set_disabled(
        entering || selected_card != -1 || rerolls <= 0
    );
}

// Record one choice and enable Next Floor
void RewardScreen::on_card_pressed(int index) {
    // Ignore clicks while closed, during the entrance, or after a reward is selected
    if (!reward_open || entering || selected_card != -1) {
        return;
    }

    // Reject indices outside the three-card array
    if (index < 0 || index >= 3) {
        return;
    }

    PlayerStatus *status = Object::cast_to<PlayerStatus>(get_node_or_null(NodePath("/root/PlayerStatusData")));
    const RewardData &reward = displayed_rewards[index];
    // Applies the selected reward's effect to the Player's stats
    if (reward.name == "APPLE" || reward.name == "STRAWBERRY") {
        status->heal(reward.status_amount);
    } else if (reward.name == "CARROT" || reward.name == "JADE PUMPKIN") {
        status->maxHealth = std::min(10, status->maxHealth + reward.status_amount);
    } else if (reward.name == "WHETSTONE" || reward.name == "LONG BLADE" || reward.name == "WARRIOR'S SEAL") {
        status->attackDamage += reward.status_amount;
    } else if (reward.name == "LIGHT BOOTS") {
        status->maxHorizontalSpeed += reward.status_amount;
    } else if (reward.name == "REBIRTH RING") {
        status->health = status->maxHealth;
    } else if (reward.name == "FORTUNE GEM") {
    status->bonusRerolls += reward.status_amount;
    }

    PlayerStatusBar *status_bar = Object::cast_to<PlayerStatusBar>(get_node_or_null(NodePath("/root/StatusBar")));

    if (status_bar != nullptr) {
        status_bar->refresh_player_status();
    }

    // Once a reward is selected, it cannot be re-rolled or claimed again
    selected_card = index;

    // Lock all cards
    for (int i = 0; i < 3; i++) {
        cards[i]->set_disabled(true);
    }

    update_reroll_button();
    // Enable Next Floor button
    next_floor_button->set_disabled(false);
}

void RewardScreen::on_next_floor_pressed() {
    // Prevents continuing before the entrance finishes and a reward is chosen
    if (!reward_open || entering || selected_card == -1) {
        return;
    }

    RoomManager *room_manager = Object::cast_to<RoomManager>(get_tree()->get_first_node_in_group("room_manager"));

    // Resumes gameplay before loading the next room
    get_tree()->set_pause(false);
    room_manager->go_to_next_room(); 
    reward_open = false;
    next_floor_button->set_disabled(true);
    hide();
}