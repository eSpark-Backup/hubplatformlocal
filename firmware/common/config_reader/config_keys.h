#pragma once

namespace HubPlatform {

namespace ConfigKeys {

constexpr const char Serial[] = "firmware/dev/serial";
constexpr const char HubMac[] = "firmware/dev/mac";
constexpr const char fwVersion[] = "firmware/dev/fw_version";
constexpr const char Coordinator[] = "cloud/server/coordinator";
constexpr const char NmaUrlCacheOn[] = "cloud/server/server_url_cache";
constexpr const char CustomServices[] = "custom_services/services";

constexpr const char CloudTimeout[] = "cloud/server/timeout";
constexpr const char CloudReconnectPeriod[] = "cloud/server/reconnect_period";
constexpr const char NmaUrl[] = "cloud/server/url";
constexpr const char StopEmotionsOnShutdown[] = "firmware/general/stop_emotions_on_shutdown";
constexpr const char PluginStorageLimit[] = "firmware/general/plugin_storage_limit";
constexpr const char LogLevel[] = "firmware/general/log_level";
constexpr const char LogColor[] = "firmware/general/log_color";
constexpr const char LogIndent[] = "firmware/general/log_indent";
constexpr const char dbPathKey[] = "firmware/storage/db_path";
constexpr const char LogPathKey[] = "firmware/storage/log_path";
constexpr const char HouseModes[] = "house_modes/house_modes";
constexpr const char HouseModesDefault[] = "house_modes/settings/default";
constexpr const char HouseModeSwitchToDelay[] = "house_modes/settings/switchToDelay";
constexpr const char HouseModeAlarmDelay[] = "house_modes/settings/alarmDelay";
constexpr const char LuadScriptTimeout[] = "luad/limits/scriptTimeout";
constexpr const char LuadScriptMinTimeout[] = "luad/limits/scriptMinTimeout";
constexpr const char Settings[] = "settings/settings";

constexpr const char scriptRunTimeout[] = "backup/script/run_timeout_sec";
constexpr const char backupIntervalMin[] = "backup/daily/everyday_time_min";

constexpr const char CamerasRtspServerPort[] = "cameras/rtsp_server/port";
constexpr const char CamerasRtspServerPacketBufferSize[] = "cameras/rtsp_server/packet_buffer_size";
constexpr const char CamerasLocalStorageMaxSizeMb[] = "cameras/local_storage/max_size_mb";
constexpr const char CamerasRecordingBufferSize[] = "cameras/recording/packet_buffer_size";
constexpr const char CamerasPreRecordingBufferSize[] = "cameras/pre_recording/packet_buffer_size";
constexpr const char CamerasSnapshoterBufferSize[] = "cameras/snapshot/packet_buffer_size";
constexpr const char CamerasSnapshoterQueueLimit[] = "cameras/snapshot/queue_limit";
constexpr const char CamerasSnapshoterTimeout[] = "cameras/snapshot/timeout_sec";
constexpr const char CamerasStreamingMaxDelayMsec[] = "cameras/streaming/max_delay_msec";

}  // namespace ConfigKeys
}  // namespace HubPlatform
