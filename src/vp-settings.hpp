#pragma once
#include <string>

void vp_settings_load();
void vp_settings_save();

const std::string &vp_settings_get_workspace_root();
void vp_settings_set_workspace_root(const std::string &path);
