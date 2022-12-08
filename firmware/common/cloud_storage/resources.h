#pragma once

namespace HubPlatform {
namespace Common {
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

namespace Http {
namespace Options {
namespace Headers {
extern const char* const Xapp;
extern const char* const Type;
extern const char* const Filename;
extern const char* const FileFormat;
extern const char* const Meta;
extern const char* const Uuid;
extern const char* const TransferEncoding;
}  // namespace Headers
}  // namespace Options
namespace Response {
extern const char* const Code;
extern const char* const Data;
extern const char* const Key;
extern const char* const Uuid;
extern const char* const Reason;
extern const char* const Text;
}  // namespace Response
}  // namespace Http

}  // namespace Resources
}  // namespace Common
}  // namespace HubPlatform
