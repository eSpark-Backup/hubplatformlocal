#pragma once

namespace HubPlatform {
namespace Uid {
namespace Resources {
namespace DB {

namespace Fields {
extern const char* const Id;
extern const char* const Xapp;
extern const char* const Token;
extern const char* const TokenExpirationDate;
extern const char* const UploadUrls;
extern const char* const UploadLogsUrls;
extern const char* const DownloadUrls;
}  // namespace Fields

}  // namespace DB

namespace Api {
namespace Fields {
extern const char* const Id;
extern const char* const Method;
extern const char* const Api;
extern const char* const Params;
extern const char* const Endpoints;
extern const char* const Download;
extern const char* const Upload;
extern const char* const UploadLogs;
extern const char* const Token;
extern const char* const String;
extern const char* const ExpiresTs;
extern const char* const Xapp;
extern const char* const Result;
extern const char* const StorageType;
extern const char* const DownloadUrl;
extern const char* const UploadUrl;
}  // namespace Fields
namespace Methods {
extern const char* const cloudStorageControllerToken;
}  // namespace Methods
}  // namespace Api

}  // namespace Resources
}  // namespace Uid
}  // namespace HubPlatform
