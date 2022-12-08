#pragma once

#include "opkg_utils/feeds/feeds_configuration.h"

namespace HubPlatform {
namespace Opkg {

class IFeedsUpdater {
public:
    virtual ~IFeedsUpdater() = default;

    virtual bool update( const std::string& source, int snapshot ) = 0;
    virtual void update( const std::string& imageUrl ) = 0;

    virtual void resetCurrent() = 0;
    virtual void resetToCurrent() = 0;

    virtual std::string getFeedsSource() const = 0;
    virtual std::string getExpectedUpdateVersion() const = 0;
    virtual std::string getFeedsUpdateVersion() const = 0;
    virtual int getFeedsSnapshot() const = 0;
    virtual std::string generateImageUrl() const = 0;
    virtual std::string generateRecoveryImageUrl() const = 0;
};

}  // namespace Opkg
}  // namespace HubPlatform
