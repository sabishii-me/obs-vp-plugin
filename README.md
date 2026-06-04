# OBS Virtual Production Plugin

An OBS Studio plugin for virtual production workflows. Provides a workspace root configuration and a WebSocket API for remote tools to create project folders inside that workspace.

## Features

- Set a **workspace root folder** via Tools menu in OBS
- Remote tools create **subfolders** under the workspace root via obs-websocket v5
- Path traversal and invalid characters are rejected — remote callers cannot escape the workspace root

## Installation

Run the installer:
```
obs-vp-plugin-<version>-windows-x64-Installer.exe
```
The installer detects your OBS Studio path automatically and places files in the correct locations.

## Setup

1. Open OBS Studio
2. Go to **Tools → VP Plugin Settings...**
3. Click **Browse...** and select your workspace root folder
4. Click **OK** — the path is saved and persists across restarts

## WebSocket API

The plugin registers vendor requests through the built-in **obs-websocket v5** (OBS 28+). No separate server or port needed.

### Authentication

Connect to obs-websocket as normal (default port `4455`). See the [obs-websocket docs](https://github.com/obsproject/obs-websocket/blob/master/docs/generated/protocol.md) for the authentication handshake.

### How to call a vendor request

All plugin requests use the standard `CallVendorRequest` type:

```json
{
  "op": 6,
  "d": {
    "requestType": "CallVendorRequest",
    "requestId": "your-id",
    "requestData": {
      "vendorName": "vp-plugin",
      "requestType": "<REQUEST_NAME>",
      "requestData": { }
    }
  }
}
```

Check the response inside `responseData.responseData`:
- `success: true` — operation succeeded, additional fields vary per request
- `success: false` — operation failed, `error` field contains the reason

---

### `VPPlugin_Workspace_CreateFolder`

Creates a subfolder inside the configured workspace root.

**Request data**

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `folderName` | string | yes | Name of the folder to create. Must be a plain name — no path separators, no `..`, no control characters, no Windows-reserved characters (`/ \ : * ? " < > |`) |

**Response data**

| Field | Type | Description |
|-------|------|-------------|
| `success` | bool | `true` if the folder was created |
| `path` | string | Absolute path of the created folder (on success) |
| `error` | string | Error message (on failure) |

**Example request**

```json
{
  "op": 6,
  "d": {
    "requestType": "CallVendorRequest",
    "requestId": "create-shot-001",
    "requestData": {
      "vendorName": "vp-plugin",
      "requestType": "VPPlugin_Workspace_CreateFolder",
      "requestData": {
        "folderName": "SHOT_001_T001"
      }
    }
  }
}
```

**Example success response**

```json
{
  "op": 7,
  "d": {
    "requestId": "create-shot-001",
    "requestType": "CallVendorRequest",
    "requestStatus": { "code": 100, "result": true },
    "responseData": {
      "vendorName": "vp-plugin",
      "requestType": "VPPlugin_Workspace_CreateFolder",
      "responseData": {
        "success": true,
        "path": "C:\\Users\\colin\\Downloads\\SHOT_001_T001"
      }
    }
  }
}
```

**Example error response**

```json
{
  "responseData": {
    "success": false,
    "error": "folderName must be a plain name with no path separators or traversal sequences"
  }
}
```

**Error cases**

| Error | Cause |
|-------|-------|
| `folderName is required` | Empty or missing `folderName` |
| `folderName must be a plain name...` | Name contains `/`, `\`, `:`, `*`, `?`, `"`, `<`, `>`, `\|`, control characters, or is `.` / `..` |
| `Workspace root is not configured` | No workspace root set in Tools → VP Plugin Settings |
| `Invalid workspace root path` | The saved workspace root path is invalid |
| `folderName must resolve inside the workspace root` | Canonical path check failed |

---

## Building from Source

**Requirements:** Visual Studio 2022, CMake 3.28+

```bash
git clone <repo>
cd obs-vp-plugin
cmake --preset windows-x64
cmake --build build_x64 --config RelWithDebInfo
```

To produce an installer:
```bash
cmake --install build_x64 --prefix release/RelWithDebInfo --config RelWithDebInfo
cp -r release/RelWithDebInfo/. release/Package/
iscc build_x64/installer-Windows.generated.iss
```

## Version History

| Version | Changes |
|---------|---------|
| 1.3.0 | Block control characters and Windows-reserved chars in folderName |
| 1.2.0 | Fix crash on bad folderName; wrap filesystem calls in error_code |
| 1.1.0 | Fix settings not persisting (ensure plugin config dir exists on load) |
| 1.0.0 | Initial release — workspace root setting, VPPlugin_Workspace_CreateFolder API |
