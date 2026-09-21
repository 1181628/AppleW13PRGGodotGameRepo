#include "rewardCard.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

using namespace godot;

void RewardCard::_bind_methods() {
}

void RewardCard::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Store the shared path
    String path = "MarginContainer/VBoxContainer/";

    // Find the display nodes and cast them to their expected types
    rarity_label = Object::cast_to<Label>(get_node_or_null(NodePath(path + "RarityLabel")));
    name_label = Object::cast_to<Label>(get_node_or_null(NodePath(path + "NameLabel")));
    description_label = Object::cast_to<Label>(get_node_or_null(NodePath(path + "DescriptionLabel")));
    icon = Object::cast_to<TextureRect>(get_node_or_null(NodePath(path + "Icon")));
}

// Return true if any required display node is missing
bool RewardCard::detect_invalid() const {
    return rarity_label == nullptr
        || name_label == nullptr
        || description_label == nullptr
        || icon == nullptr;
}

// Read the existing image for the temporary Apple rewards
Ref<Texture2D> RewardCard::get_preview_icon() const {
    // Stop before accessing missing display nodes
    if (icon == nullptr) {
        return Ref<Texture2D>();
    }

    return icon->get_texture();
}

// Update the card with the selected reward's rarity, text, and icon
void RewardCard::display_reward(const String &rarity, const RewardData &reward) {
    // Stop if any required label or image node is missing
    if (detect_invalid()) {
        return;
    }

    // Update the reward's rarity, name, and description
    rarity_label->set_text(rarity);
    name_label->set_text(reward.name);
    description_label->set_text(reward.description);

    // Create a texture using this reward's region of the shared image
    Ref<AtlasTexture> reward_icon;
    reward_icon.instantiate();
    reward_icon->set_atlas(reward.icon);
    reward_icon->set_region(reward.icon_region);

    // Show the cropped icon on the card
    icon->set_texture(reward_icon);
}
