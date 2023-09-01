#pragma once

#include "2gig/device_status.h"

#include "core/packets/pin_code_transmission.h"
#include "utils/pipeline/pipe_consumer.h"

namespace TwoGig {

struct Transmission;
struct Client;
class Logger;

class TransmissionHandler : public PipeConsumer< Transmission >, public PipeConsumer< PinCodeTransmission > {
public:
    TransmissionHandler( const Logger& logger, Client& client );

    void processPayload( Transmission packet ) override;

    void processPayload( PinCodeTransmission packet ) override;

private:
    template < typename TransmissionT >
    void printTransmissionInfo( TransmissionT&& transmission, const char* message );

private:
    const Logger& m_logger;

    Client& m_client;
};

}  // namespace TwoGig
