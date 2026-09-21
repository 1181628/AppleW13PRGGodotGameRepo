#include "pastRecords.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>

#include <cstdio>

using namespace godot;

void PastRecords::_bind_methods() {
    // Register the callback so Callable can find it by name
    ClassDB::bind_method(D_METHOD("on_return_pressed"), &PastRecords::on_return_pressed);
}

void PastRecords::_ready() {
    // Do not load saved results while editing the scene
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    // Find the existing return button.
    Button *return_button = get_node<Button>("PanelContainer/MarginContainer/VBoxContainer/Return");
    // Hide the records panel when the button is pressed
    return_button->connect("pressed",Callable(this, "on_return_pressed"));
    display_records();
}

void PastRecords::display_records() {
    // Find the container holding the three record boxes
    Node *records_container = get_node<Node>(
        "PanelContainer/MarginContainer/VBoxContainer/"
        "MarginContainer/VBoxContainer"
    );

    // Store references to the existing boxes in display order
    Control *boxes[3] = {
        records_container->get_node<Control>("LastRun1"),
        records_container->get_node<Control>("LastRun2"),
        records_container->get_node<Control>("LastRun3")
    };

    // Hide all boxes first
    // Only boxes with a saved result will be shown later
    for (int i = 0; i < 3; i++) {
        boxes[i]->hide();
    }

    // No history file means there are no results to display
    if (!FileAccess::file_exists("user://past_records.json")) {
        return;
    }

    // Open the history file created by SaveManager
    Ref<FileAccess> file = FileAccess::open("user://past_records.json", FileAccess::READ);

    // Stops before reading a file that could not be opened
    if (file.is_null()) {
        return;
    }

    // Parse the JSON text before closing the file
    Variant data = JSON::parse_string(file->get_as_text());
    file->close();

    // The history must contain a Dictionary.
    if (data.get_type() != Variant::DICTIONARY) {
        return;
    }

    Dictionary history = data;

    // Read the array stored under the "records" key
    Variant saved_records = history.get("records", Array());

    // Checks the records collection before converting it into an Array
    if (saved_records.get_type() != Variant::ARRAY) {
        return;
    }

    Array records = saved_records;

    // Display up to three results
    for (int i = 0; i < 3 && i < records.size(); i++) {
        Variant entry = records[i];

        // Skip entries that are not valid dictionaries.
        if (entry.get_type() != Variant::DICTIONARY) {
            continue;
        }

        Dictionary record = entry;

        // Find the three value labels inside this box
        Label *count_label = boxes[i]->get_node<Label>("HBoxContainer/Label Counts");
        Label *time_label = boxes[i]->get_node<Label>("HBoxContainer/VBoxContainer/Label Time");
        Label *floor_label = boxes[i]->get_node<Label>("HBoxContainer/VBoxContainer2/Label Floor");

        // Display fixed positions: RUN 01, RUN 02 and RUN 03
        char text[64];
        std::snprintf(text, sizeof(text), "   RUN %02d", i + 1);
        count_label->set_text(text);
        // Convert elapsed seconds into hours, minutes and seconds
        int total_seconds = int(record.get("time", 0.0));
        int hours = total_seconds / 3600;
        int minutes = (total_seconds % 3600) / 60;
        int seconds = total_seconds % 60;
        // Format the time as HH:MM:SS.
        std::snprintf(text, sizeof(text),"%02d:%02d:%02d", hours, minutes, seconds);
        time_label->set_text(text);

        // Display the saved room ID using at least three digits
        int floor = int(record.get("floor", 0));

        std::snprintf(text, sizeof(text), "%03d", floor);
        floor_label->set_text(text);

        // Show this box after its labels have been updated
        boxes[i]->show();
    }
}

void PastRecords::on_return_pressed() {
    // Hide this panel to reveal the title screen underneath
    hide();
}