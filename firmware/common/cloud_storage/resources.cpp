#include "cloud_storage/resources.h"

namespace HubPlatform {
namespace Common {
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

namespace Http {
namespace Options {
namespace Headers {
const char* const Xapp = "X-APP";
const char* const Type = "p-type";
const char* const Filename = "p-filename";
const char* const FileFormat = "p-file_format";
const char* const Meta = "p-meta";
const char* const Uuid = "p-uuid";
const char* const TransferEncoding = "transfer-encoding";
}  // namespace Headers
}  // namespace Options
namespace Response {
const char* const Code = "code";
const char* const Data = "data";
const char* const Key = "key";
const char* const Uuid = "uuid";
const char* const Reason = "reason";
const char* const Text = "text";
}  // namespace Response
}  // namespace Http

}  // namespace Resources
}  // namespace Common
}  // namespace HubPlatform
