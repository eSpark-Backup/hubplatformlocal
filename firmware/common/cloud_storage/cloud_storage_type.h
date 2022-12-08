#pragma once

#include "json_utils/json_wrapper.h"
#include <string>

namespace HubPlatform {
namespace Common {
namespace CloudStorageInterface {

enum class StorageType {
    Backup,
    Multimedia,
    ControllerBackup,
    ArchiveControllerBackup,
    ControllerLog,
    ArchiveControllerLog
};
enum class PrepareUploadType { UploadAndPreview };
enum class OwnerEntityType { Controller, User, Location };
enum class LoadType { Upload, UploadLogs, Download };
enum class Errors { Ok, InvalidUrl, InvalidToken, InvalidXapp, RequestError, WrongPathFormat, WrongFileFormat };

inline std::string toString( StorageType storageType ) {
    switch( storageType ) {
    case StorageType::Backup:
        return "backup";
    case StorageType::Multimedia:
        return "multimedia";
    case StorageType::ControllerBackup:
        return "controller_backup";
    case StorageType::ArchiveControllerBackup:
        return "archive_controller_backup";
    case StorageType::ControllerLog:
        return "controller_log";
    case StorageType::ArchiveControllerLog:
        return "archive_controller_log";
    }
    return "";
}

inline std::string toString( PrepareUploadType prepareUploadType ) {
    switch( prepareUploadType ) {
    case PrepareUploadType::UploadAndPreview:
        return "upload-and-preview";
    }
    return "";
}

inline std::string toString( OwnerEntityType ownerEntityType ) {
    switch( ownerEntityType ) {
    case OwnerEntityType::User:
        return "user";
    case OwnerEntityType::Location:
        return "location";
    case OwnerEntityType::Controller:
        return "controller";
    }
    return "";
}

inline std::string toString( Errors error ) {
    switch( error ) {
    case Errors::Ok:
        return "ok";
    case Errors::InvalidUrl:
        return "invalid_url";
    case Errors::InvalidToken:
        return "invalid_token";
    case Errors::InvalidXapp:
        return "invalid_xapp";
    case Errors::RequestError:
        return "request_error";
    case Errors::WrongPathFormat:
        return "wrong_path_format";
    case Errors::WrongFileFormat:
        return "wrong_file_format";
    }
    return "";
}

struct FileKey {
    FileKey() = default;
    FileKey( std::string fileKey, std::string fileUuid )
    : key( std::move( fileKey ) )
    , uuid( std::move( fileUuid ) ) {}

    std::string key;
    std::string uuid;

    Json toJson() const {
        return JSON( "key" << key << "uuid" << uuid );
    }
};

struct UploadFileData {
    explicit UploadFileData( std::string filePath )
    : file( std::move( filePath ) ) {}
    UploadFileData( std::string filePath, std::string fileUuid, Json metadata )
    : file( std::move( filePath ) )
    , uuid( std::move( fileUuid ) )
    , metadata( std::move( metadata ) ) {}

    std::string file;
    std::string uuid;
    Json metadata;
};

}  // namespace CloudStorageInterface
}  // namespace Common
}  // namespace HubPlatform
