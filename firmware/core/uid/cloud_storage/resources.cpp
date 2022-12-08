#include "uid/cloud_storage/resources.h"

namespace HubPlatform {
namespace Uid {
namespace Resources {
namespace DB {

namespace Fields {
const char* const Id = "_id";
const char* const Xapp = "xapp";
const char* const Token = "token";
const char* const TokenExpirationDate = "tokenExpirationDate";
const char* const UploadUrls = "uploadUrls";
const char* const UploadLogsUrls = "uploadLogsUrls";
const char* const DownloadUrls = "downloadUrls";
}  // namespace Fields

}  // namespace DB

namespace Api {
namespace Fields {
const char* const Id = "id";
const char* const Method = "method";
const char* const Api = "api";
const char* const Params = "params";
const char* const Endpoints = "endpoints";
const char* const Download = "download";
const char* const Upload = "upload";
const char* const UploadLogs = "upload-logs";
const char* const Token = "token";
const char* const String = "string";
const char* const ExpiresTs = "expires_ts";
const char* const Xapp = "xapp";
const char* const Result = "result";
const char* const StorageType = "type";
const char* const DownloadUrl = "downloadUrl";
const char* const UploadUrl = "uploadUrl";
}  // namespace Fields
namespace Methods {
const char* const cloudStorageControllerToken = "cloud.storage_controller_token";
}  // namespace Methods
}  // namespace Api

}  // namespace Resources
}  // namespace Uid
}  // namespace HubPlatform
