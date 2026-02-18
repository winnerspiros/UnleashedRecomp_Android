#pragma once

struct RenderCommand;

void InitImGuiBackend();
void DrawImGui();
void ProcDrawImGui(const RenderCommand& cmd);
