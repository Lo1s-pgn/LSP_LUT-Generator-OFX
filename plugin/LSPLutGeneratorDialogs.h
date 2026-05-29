#pragma once

#include <string>

/* Folder chooser for export path; must run on the UI thread (OFX). Empty if cancelled. */
std::string LSPLutGenShowChooseFolderDialog(const char* p_DefaultDir);
