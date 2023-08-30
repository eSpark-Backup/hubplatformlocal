#pragma once

#include "cameras/input_output/camera_snapshoter.h"
#include "cameras/defs.h"

#include "json_utils/json_fwd.h"

#include <memory>

namespace HubPlatform {

class ISnapshotManager {
public:
    using Response = std::function< void( const Json& ) >;
    virtual void makeSnapshotForCloud( const Json& metaData, SaveFileCallback saveFile, Response responseCallback ) = 0;
    virtual void makeSnapshotForUI( std::shared_ptr< Resolution > resolution, Response responseCallback ) = 0;

    virtual ~ISnapshotManager() = default;
};

}  // namespace HubPlatform
