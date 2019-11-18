#include "AllEarsManager.h"

#include "Step.h"

#include <json.hpp>
#include <imgui.h>

#include <fstream>

AllEarsManager::AllEarsManager()
{
	std::ifstream file("assets/steps.json");

	nlohmann::json json = nlohmann::json::parse(file);

	for (const auto& index : json) {
		std::string key(index.begin().key());

		if (key == "NPC") {

			std::vector<std::string> dialogs;
			for (const auto& subject : index["subjects"]) {
				dialogs.push_back(subject);
			}

			steps_.push_back(std::make_unique<NpcStep>(index["NPC"], dialogs ));
		}
		else if (key == "destination") {
			steps_.push_back(std::make_unique<TravelStep>(index["destination"]));
		}
		else if (key == "event") {
			steps_.push_back(std::make_unique<EventStep>(index["event"]));
		}
		else {
			std::cout << "JSON syntax error";
		}
	}
}

void AllEarsManager::Render()
{
	steps_[current_step_]->Render();

	ImGui::Separator();

	if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
		DecrementStep();
	}
	ImGui::SameLine();
	ImGui::Text("Step: %d / %d", current_step_ + 1, steps_.size());
	ImGui::SameLine();
	if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
		IncrementStep();
	}
}

void AllEarsManager::IncrementStep()
{
	if ((current_step_ + 1) < steps_.size()) {

		//reset dialog checkboxes
		auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
		if (npc_step != nullptr) {
			for (auto& dialog : npc_step->dialogs_) {
				dialog.completed = false;
			}
		}

		++current_step_;
	}
	else {
		std::cout << "AT END OF STEPS";
	}
}

void AllEarsManager::DecrementStep()
{
	if (current_step_ > 0) {
		--current_step_;
	}
	else {
		std::cout << "AT THE BEGINNING OF STEPS";
	}
}

std::string AllEarsManager::GetDestination()
{
	auto travel_step = dynamic_cast<TravelStep*>(steps_[current_step_].get());
	if (travel_step != nullptr) {
		return travel_step->destination_;
	}

	return "";
}

bool AllEarsManager::StepIsComplete()
{
	auto npc_step = dynamic_cast<NpcStep*>(steps_[current_step_].get());
	if (npc_step != nullptr) {
		bool all_dialog_complete = true;

		for (const auto& dialog : npc_step->dialogs_) {
			if (!dialog.completed) {
				all_dialog_complete = false;
				break;
			}
		}
		if (all_dialog_complete) {
			return true;
		}
	}

	auto event_step = dynamic_cast<EventStep*>(steps_[current_step_].get());
	if (event_step != nullptr) {

		if (event_step->completed_) {
			event_step->completed_ = false;
			return true;
		}
	}
	return false;
}