#include "ws-handler.hpp"
#include "vp-settings.hpp"
#include "obs-websocket-api.h"

#include <obs-module.h>

#include <filesystem>
#include <string>

static obs_websocket_vendor g_vendor = nullptr;

static void handle_create_folder(obs_data_t *request_data, obs_data_t *response_data, void * /*priv*/)
{
	const char *folderName = obs_data_get_string(request_data, "folderName");

	if (!folderName || folderName[0] == '\0') {
		obs_data_set_bool(response_data, "success", false);
		obs_data_set_string(response_data, "error", "folderName is required");
		return;
	}

	const std::string &root = vp_settings_get_workspace_root();
	if (root.empty()) {
		obs_data_set_bool(response_data, "success", false);
		obs_data_set_string(response_data, "error",
				    "Workspace root is not configured. Use Tools > VP Plugin Settings.");
		return;
	}

	// Reject anything with a path separator, traversal, or non-printable
	// characters — must be a single plain name component only.
	std::string name(folderName);
	bool has_bad_char = false;
	for (unsigned char c : name) {
		if (c < 0x20 || c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' ||
		    c == '>' || c == '|') {
			has_bad_char = true;
			break;
		}
	}
	if (has_bad_char || name == ".." || name == ".") {
		obs_data_set_bool(response_data, "success", false);
		obs_data_set_string(response_data, "error",
				    "folderName must be a plain name with no path separators or traversal sequences");
		return;
	}

	std::error_code ec;
	std::filesystem::path workspaceRoot = std::filesystem::weakly_canonical(std::filesystem::path(root), ec);
	if (ec) {
		obs_data_set_bool(response_data, "success", false);
		obs_data_set_string(response_data, "error", "Invalid workspace root path");
		return;
	}

	std::filesystem::path target = workspaceRoot / name;

	// Canonical check: resolved target must be a direct child of workspace root
	auto canonical_target = std::filesystem::weakly_canonical(target, ec);
	if (ec || canonical_target.parent_path() != workspaceRoot) {
		obs_data_set_bool(response_data, "success", false);
		obs_data_set_string(response_data, "error", "folderName must resolve inside the workspace root");
		return;
	}

	std::filesystem::create_directories(target, ec);

	if (ec) {
		obs_data_set_bool(response_data, "success", false);
		obs_data_set_string(response_data, "error", ec.message().c_str());
	} else {
		obs_data_set_bool(response_data, "success", true);
		obs_data_set_string(response_data, "path", target.string().c_str());
	}
}

void ws_handler_register()
{
	g_vendor = obs_websocket_register_vendor("vp-plugin");
	if (!g_vendor) {
		blog(LOG_WARNING, "[vp-plugin] obs-websocket vendor registration failed — is obs-websocket loaded?");
		return;
	}

	obs_websocket_vendor_register_request(g_vendor, "VPPlugin_Workspace_CreateFolder", handle_create_folder,
					      nullptr);

	blog(LOG_INFO, "[vp-plugin] WebSocket vendor request VPPlugin_Workspace_CreateFolder registered");
}

void ws_handler_unregister()
{
	g_vendor = nullptr;
}
