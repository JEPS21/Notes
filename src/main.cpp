#include <iostream>

#include <fmt/core.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_md.h>
#include <SDL3/SDL_main.h>

#include <editor/ogl.hpp>
#include <editor/editor.hpp>

std::string src;

typedef struct NoteInfo
{
   std::string filename;
   std::string content;
} NoteInfo;

static std::vector<NoteInfo> _notes;
static int current_note = -1;

bool update_markdown_preview = false;

int main(int argc, char **argv)
{
   int32_t error_code = Editor::init();
   if (error_code)
   {
      return error_code;
   }

   window.setClearColor({1, 1, 1, 1});

   while (window.keep_open)
   {
      Editor::Window::pollEvents();
      Editor::startFrame();
      ImVec2 size = ImGui::GetMainViewport()->Size;
      size.x *= 0.9;
      size.y *= 0.9;

      ImGui::SetNextWindowSize(size);
      if (ImGui::Begin("EDITOR"))
      {
         if (ImGui::BeginChild("SOURCE", ImVec2{ImGui::GetContentRegionAvail().x * 0.5f - 6, 0}, 1))
         {
            if (_notes.size() == 0)
            {
               if (ImGui::Button("New"))
               {
                  _notes.emplace_back();
                  NoteInfo &note = _notes.back();
                  fmt::format_to(std::back_inserter(note.filename), "{}", _notes.size());
               }
            }
            else
            {
               if (ImGui::BeginTabBar("Notes"))
               {
                  for (int i = 0; i < (int)_notes.size(); ++i)
                  {
                     ImGui::PushID(i);
                     if (ImGui::BeginTabItem(_notes[i].filename.c_str()))
                     {
                        if(ImGui::InputTextMultiline("SRC", &_notes[i].content, ImGui::GetContentRegionAvail())){
                           
                        }
                        current_note = i;
                        ImGui::EndTabItem();
                     }
                     ImGui::PopID();
                  }
                  if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing))
                  {
                     NoteInfo &note = _notes.emplace_back();
                     fmt::format_to(std::back_inserter(note.filename), "{}", _notes.size());
                  }
               }
               ImGui::EndTabBar();
            }
         }
         ImGui::EndChild();
         ImGui::SameLine();
         if (ImGui::BeginChild("MARKDOWN", ImVec2{0, 0}, 1))
         {
            if (current_note >= 0)
            {
               // Editor::markdown(current_note->content.begin().base(), current_note->content.end().base());
               Editor::markdown(_notes[current_note].content.begin().base(), _notes[current_note].content.end().base());
            }
         }
         ImGui::EndChild();
      }
      ImGui::End();

      Editor::clear();
      Editor::EndFrame();
      window.swap();
   }

   Editor::terminate();
}