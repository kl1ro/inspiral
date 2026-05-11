#include "ui.hpp"

#include "globals.hpp"

void renderUI() {
  createFrame();
  drawFPS();
  drawToolbar();
  renderFrame();
}

void createFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void drawFPS() {
  ImGui::SetNextWindowPos(
    ImVec2(ImGui::GetIO().DisplaySize.x - 10, 10),
    ImGuiCond_Always,
    ImVec2(1, 0)
  );

  ImGui::SetNextWindowBgAlpha(1.0f);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.133f, 0.133f, 0.149f, 1.0f));

  ImGui::Begin("##fps", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
  ImGui::Text("FPS: %.0f", Globals::time.fps);
  ImGui::End();

  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}

void drawToolbar() {
  ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(1.0f);
  ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 6.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 4.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 8.0f);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.133f, 0.133f, 0.149f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.133f, 0.133f, 0.149f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.22f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.28f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.133f, 0.133f, 0.149f, 1.0f));
  ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);

  if (ImGui::Button("File")) {
    ImGui::SetNextWindowPos(
      ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y + 8.0f),
      ImGuiCond_Always
    );

    ImGui::OpenPopup("file_menu");
  }

  drawFilePopover();

  ImGui::SameLine();

  if (ImGui::Button("About"))
    ImGui::OpenPopup("About");

  ImVec2 center = ImGui::GetMainViewport()->GetCenter();
  ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
  ImGui::SetNextWindowSize(ImVec2(500, 350), ImGuiCond_Appearing);

  if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_NoResize)) {
    ImGui::Text(
      "The game engine application is written by Ivan Prosvirov.\n\nVersion: 0.01\n\nAll rights "
      "reserved."
    );

    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 buttonSize = ImVec2(60, 24);
    ImGui::SetCursorPos(ImVec2(windowSize.x - buttonSize.x - 8, windowSize.y - buttonSize.y - 8));

    if (ImGui::Button("Close", buttonSize))
      ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
  }

  ImGui::End();
  ImGui::PopStyleVar(5);
  ImGui::PopStyleColor(5);
}

void drawFilePopover() {
  if (ImGui::BeginPopup("file_menu")) {
    if (ImGui::MenuItem("Open")) {
      std::string path = openFileDialog();
      if (!path.empty())
        openGltf(path);
    }

    ImGui::MenuItem("Save");
    ImGui::Separator();

    if (ImGui::BeginMenu("Import")) {
      if (ImGui::MenuItem(".obj")) {
        std::string path = openFileDialog("*.obj");

        if (!path.empty())
          importObj(path);
      }

      ImGui::EndMenu();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Exit"))
      glfwSetWindowShouldClose(Globals::window, true);

    ImGui::EndPopup();
  }
}

void renderFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
