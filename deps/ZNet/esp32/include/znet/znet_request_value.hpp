#pragma once

#include <znet/iznet.hpp>
#include <znet/znet_request_fail.hpp>
#include <functional>
#include <znet/znet_job.hpp>
#include <memory>

namespace znet {

struct RequestValue;
struct RequestValueTermostatSetpoint;
struct RequestValueUserCode;
struct RequestValueAlarmSensor;
struct RequestValueAssociation;
struct RequestValueTime;

#define REQUEST_VALUE_BASE_CAST_METHODS( Inheritor ) \
    virtual const Inheritor* to##Inheritor() const { \
        return nullptr;                              \
    }                                                \
    virtual Inheritor* to##Inheritor() {             \
        return nullptr;                              \
    }

#define REQUEST_VALUE_CAST_METHODS( Inheritor )       \
    const Inheritor* to##Inheritor() const override { \
        return this;                                  \
    }                                                 \
    Inheritor* to##Inheritor() override {             \
        return this;                                  \
    }

struct IRequestValue {
    virtual ~IRequestValue() {}

    REQUEST_VALUE_BASE_CAST_METHODS( RequestValue )
    REQUEST_VALUE_BASE_CAST_METHODS( RequestValueTermostatSetpoint )
    REQUEST_VALUE_BASE_CAST_METHODS( RequestValueUserCode )
    REQUEST_VALUE_BASE_CAST_METHODS( RequestValueAlarmSensor )
    REQUEST_VALUE_BASE_CAST_METHODS( RequestValueAssociation )
    REQUEST_VALUE_BASE_CAST_METHODS( RequestValueTime )
};

struct RequestValue : public IRequestValue {
    REQUEST_VALUE_CAST_METHODS( RequestValue )

    WeakJobContext jobContext;

    using Callback = std::function<void( RequestFailTypeOpt )>;
    using CallbackPtr = std::shared_ptr<Callback>;
    Callback resultCallback;
};

struct RequestValueTermostatSetpoint : public RequestValue {
    REQUEST_VALUE_CAST_METHODS( RequestValueTermostatSetpoint )

    uint8_t setpointType{ 0 };
};

struct RequestValueUserCode : public RequestValue {
    REQUEST_VALUE_CAST_METHODS( RequestValueUserCode )

    znet_supported_users_t userId{ 0 };
    bool multipleRequest{ false };
};

struct RequestValueAlarmSensor : public RequestValue {
    REQUEST_VALUE_CAST_METHODS( RequestValueAlarmSensor )

    uint8_t sensorType{ 0 };
};

struct RequestValueAssociation : public RequestValue {
    REQUEST_VALUE_CAST_METHODS( RequestValueAssociation )

    uint8_t groupNum{ 0 };
};

}  // namespace znet
