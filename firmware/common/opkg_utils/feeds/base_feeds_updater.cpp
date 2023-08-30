#include "opkg_utils/feeds/base_feeds_updater.h"
#include "opkg_utils/feeds/base_url_utils.h"

namespace HubPlatform {
namespace Opkg {

BaseFeedsUpdater::BaseFeedsUpdater( std::unique_ptr< Opkg::FeedsConfiguration > feedsConfiguration )
: m_feedsConfiguration( std::move( feedsConfiguration ) ) {}

bool BaseFeedsUpdater::update( const std::string& source, int snapshot ) {
    std::string currentBaseUrl;
    if( source.empty() || snapshot == 0 ) {
        currentBaseUrl = getCurrentBaseUrl();
        if( currentBaseUrl.empty() )
            return false;
    }

    auto actualSource = source.empty() ? BaseUrlUtils::getSource( currentBaseUrl ) : source;
    auto actualSnapshot = snapshot == 0 ? BaseUrlUtils::getSnapshot( currentBaseUrl ) : snapshot;
    auto baseUrl = BaseUrlUtils::composeBaseUrl( actualSource, getHubType(), getUpdateVersion(), actualSnapshot );

    updateFromBaseUrl( baseUrl );
    return true;
}

void BaseFeedsUpdater::update( const std::string& imageUrl ) {
    auto baseUrl = extractBaseUrl( imageUrl );
    updateFromBaseUrl( baseUrl );
}

void BaseFeedsUpdater::resetCurrent() {
    m_feedsConfiguration->removeBackup();
}

void BaseFeedsUpdater::resetToCurrent() {
    m_feedsConfiguration->tryRestore();
}

std::string BaseFeedsUpdater::getFeedsSource() const {
    return BaseUrlUtils::getSource( getCurrentBaseUrl() );
}

std::string BaseFeedsUpdater::getExpectedUpdateVersion() const {
    return getUpdateVersion();
}

std::string BaseFeedsUpdater::getFeedsUpdateVersion() const {
    return BaseUrlUtils::getUpdateVersion( getCurrentBaseUrl() );
}

int BaseFeedsUpdater::getFeedsSnapshot() const {
    return BaseUrlUtils::getSnapshot( getCurrentBaseUrl() );
}

Opkg::FeedsConfiguration& BaseFeedsUpdater::getFeedsConfiguration() {
    return *m_feedsConfiguration;
}

const Opkg::FeedsConfiguration& BaseFeedsUpdater::getFeedsConfiguration() const {
    return *m_feedsConfiguration;
}

}  // namespace Opkg
}  // namespace HubPlatform
