#include "vp-settings.hpp"

#include <obs-module.h>
#include <util/config-file.h>

#define SECTION "VPPlugin"
#define KEY_WORKSPACE_ROOT "WorkspaceRoot"

static std::string g_workspaceRoot;

void vp_settings_load()
{
	char *config_path = obs_module_config_path("vp-plugin.ini");
	if (!config_path)
		return;

	config_t *cfg = nullptr;
	if (config_open(&cfg, config_path, CONFIG_OPEN_ALWAYS) == CONFIG_SUCCESS) {
		const char *val = config_get_string(cfg, SECTION, KEY_WORKSPACE_ROOT);
		if (val)
			vp_settings_set_workspace_root(val);
		config_close(cfg);
	}
	bfree(config_path);
}

void vp_settings_save()
{
	char *config_path = obs_module_config_path("vp-plugin.ini");
	if (!config_path)
		return;

	config_t *cfg = nullptr;
	if (config_open(&cfg, config_path, CONFIG_OPEN_ALWAYS) == CONFIG_SUCCESS) {
		config_set_string(cfg, SECTION, KEY_WORKSPACE_ROOT, g_workspaceRoot.c_str());
		config_save_safe(cfg, "tmp", nullptr);
		config_close(cfg);
	}
	bfree(config_path);
}

const std::string &vp_settings_get_workspace_root()
{
	return g_workspaceRoot;
}

void vp_settings_set_workspace_root(const std::string &path)
{
	// Normalize to forward slashes and strip trailing separator
	std::string normalized = path;
	for (char &c : normalized)
		if (c == '\\')
			c = '/';
	while (!normalized.empty() && normalized.back() == '/')
		normalized.pop_back();
	g_workspaceRoot = normalized;
}
