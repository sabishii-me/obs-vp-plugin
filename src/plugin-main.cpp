#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/platform.h>
#include <plugin-support.h>

#include "vp-settings.hpp"
#include "settings-dialog.hpp"
#include "ws-handler.hpp"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

static void open_settings_dialog(void * /*priv*/)
{
	SettingsDialog dlg(
		static_cast<QWidget *>(obs_frontend_get_main_window()));
	dlg.exec();
}

bool obs_module_load(void)
{
	// Ensure the plugin config directory exists before loading settings
	char *config_dir = obs_module_config_path(NULL);
	if (config_dir) {
		os_mkdirs(config_dir);
		bfree(config_dir);
	}

	vp_settings_load();

	obs_frontend_add_tools_menu_item("VP Plugin Settings...",
					 open_settings_dialog, nullptr);

	obs_log(LOG_INFO, "plugin loaded successfully (version %s)",
		PLUGIN_VERSION);
	return true;
}

void obs_module_post_load(void)
{
	ws_handler_register();
}

void obs_module_unload(void)
{
	ws_handler_unregister();
	obs_log(LOG_INFO, "plugin unloaded");
}
