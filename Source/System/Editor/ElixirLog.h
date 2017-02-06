#pragma once

#include "../../../External Soruce/imgui/imgui.h"
#include <fstream>
#include <iostream>
#include <string>

class ElixirLogger
{	
public:
	ElixirLogger() {};

	//Refresh Frequency sets how many frames it will skip reading the log files
	//Default value is 2. It will skip 2 frames for each RefreshLog()
	//Currently only supports engine specific log types
	void Draw(const char* title, bool* p_open = NULL, int refreshFreq = 2)
	{
		if (*p_open == true)
		{
			if (m_refreshCounter == refreshFreq)
			{
				RefreshLog();
			}

			m_refreshCounter++;

			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiSetCond_FirstUseEver);
			ImGui::Begin(title, p_open);
			if (ImGui::Button("Clear")) Clear();
			ImGui::SameLine();

			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);
			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (Filter.IsActive())
			{
				const char* buf_begin = Buf.begin();
				const char* line = buf_begin;
				for (int line_no = 0; line != NULL; line_no++)
				{
					const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
					if (Filter.PassFilter(line, line_end))
						ImGui::TextUnformatted(line, line_end);
					line = line_end && line_end[1] ? line_end + 1 : NULL;
				}
			}
			else
			{
				ImGui::TextUnformatted(Buf.begin());
			}

			if (ScrollToBottom)
				ImGui::SetScrollHere(1.0f);
			ScrollToBottom = false;
			ImGui::EndChild();
			ImGui::End();
		}
	}

private:
	void Clear() { Buf.clear(); LineOffsets.clear(); }

	void RefreshLog()
	{
		std::ifstream file(LOG_DEFAULT + ".log", std::ios::in);
		std::string line;
		if (file.is_open())
		{
			while (std::getline(file, line))
			{
				int old_size = Buf.size();
				std::string newLine = line;
				//newLine.append("\n");
				Buf.append(newLine.c_str());
				Buf.append("\n");

				
			}

			file.close();
		}

		ScrollToBottom = true;

		std::ofstream cleanFile(LOG_DEFAULT + ".log", std::ios::trunc);
		if (cleanFile.is_open())
		{
			cleanFile << "";

			cleanFile.close();
		}

		m_refreshCounter = 0;
	}

private: 
	ImGuiTextBuffer     Buf;
	ImGuiTextFilter     Filter;
	ImVector<int>       LineOffsets;        // Index to lines offset
	bool                ScrollToBottom;

	int m_refreshCounter = 0;
};