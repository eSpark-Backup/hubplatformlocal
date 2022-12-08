#pragma once

#include <znet/IFile.hpp>

#include <znet/type_utils.hpp>
#include <znet/znet_error_message.hpp>

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <functional>
#include <vector>
#include <string>
#include <sstream>

struct znet_date_time_t;
struct znet_time_zone_settings_t;
struct znet_dst_time_point_t;
struct znet_cmd_schedule_entry_lock_time_offset_t;
struct znet_date_t;
struct znet_clock_value_t;
using znet_clock_weekday_value_t = std::uint8_t;

namespace znet {

using Seconds = std::chrono::seconds;
using SecondsOpt = utils::optional<Seconds>;

namespace utils {

float valueToFloat( int32_t value, uint8_t precision );
int32_t valueFromFloat( float value, uint8_t precision );

/// Ranged list utils

template <typename Index, typename Data>
struct RangeAndList {
    Index m_firstIndex;
    std::initializer_list<const Data> m_list;
};

template <typename Index, typename Data>
using RangedList = std::initializer_list<const RangeAndList<Index, Data> >;

template <typename Index, typename Data>
const Data* findInRangedList( const RangedList<Index, Data>& rangedList, Index index ) {
    auto it = std::upper_bound(
        rangedList.begin(), rangedList.end(), index, []( Index index, const RangeAndList<Index, Data>& range ) {
            return index < range.m_firstIndex;
        } );

    if( it == rangedList.begin() ) {
        return nullptr;
    }

    const auto& range = *std::prev( it );
    const auto currentIndex = static_cast<size_t>( index ) - static_cast<size_t>( range.m_firstIndex );

    return currentIndex < range.m_list.size() ? &*( range.m_list.begin() + currentIndex ) : nullptr;
}

template <typename Index, typename Data, typename Predicate>
utils::optional<Index> findIndexInRangedList( const RangedList<Index, Data>& rangedList, Predicate predicate ) {
    for( const auto& range : rangedList ) {
        auto index = static_cast<size_t>( range.m_firstIndex );
        for( const auto& value : range.m_list ) {
            if( predicate( value ) ) {
                return static_cast<Index>( index );
            }
            ++index;
        }
    }
    return {};
}

/// Bit operation utils

template <typename Predicate>
utils::optional<std::uint8_t> findBit( const std::uint8_t* data,
                                       std::uint8_t size,
                                       Predicate&& predicate,
                                       std::uint8_t firstBit = 0 ) {
    assert( size <= 256 / 8 );
    auto bitShift = firstBit % 8;
    for( std::uint8_t i = firstBit / 8; i < size; ++i ) {
        std::uint8_t index = i * 8 + bitShift;
        for( auto val = ( data[i] >> bitShift ); val; val >>= 1, ++index ) {
            if( ( val & 1 ) && predicate( std::uint8_t( index ) ) ) {
                return index;
            }
        }

        bitShift = 0;
    }

    return {};
}

template <typename Handler>
void forEachBit( const std::uint8_t* data, std::uint8_t size, Handler&& handler ) {
    findBit( data, size, [&]( std::uint8_t index ) {
        handler( index );
        return false;
    } );
}

inline void setBit( std::uint8_t* data, std::uint8_t index ) {
    data[index / 8] |= 1 << ( index % 8 );
}

inline void unsetBit( std::uint8_t* data, std::uint8_t index ) {
    data[index / 8] &= ~( 1 << ( index % 8 ) );
}

inline bool checkBit( const std::uint8_t* data, std::uint8_t index ) {
    return ( data[index / 8] & ( 1 << ( index % 8 ) ) ) != 0;
}

inline bool checkBit( const std::uint8_t* data, std::uint8_t size, std::uint8_t index ) {
    assert( size <= 256 / 8 );
    return index < size * 8 && checkBit( data, index );
}

template <typename Iterator, typename IndexOptGetter>
uint8_t writeBitmaskByRange( Iterator rangeBegin,
                             Iterator rangeEnd,
                             uint8_t* bitmask,
                             uint8_t bitmaskSize,
                             IndexOptGetter indexOptGetter ) {
    memset( bitmask, 0, bitmaskSize );
    uint8_t result = 0;

    for( auto it = rangeBegin; it != rangeEnd; ++it ) {
        const auto indexOpt = indexOptGetter( *it );
        if( indexOpt ) {
            const auto byteIndex = static_cast<uint8_t>( *indexOpt ) / 8;
            const auto bitIndex = static_cast<uint8_t>( *indexOpt ) % 8;

            if( byteIndex < bitmaskSize ) {
                bitmask[byteIndex] |= 1 << bitIndex;
                result = std::max( static_cast<uint8_t>( byteIndex + 1 ), result );
            }
        }
    }

    return result;
}

template <typename Iterator, typename IndexOptGetter, typename UnknownValueHandler>
uint8_t writeBitmaskByRange( Iterator rangeBegin,
                             Iterator rangeEnd,
                             uint8_t* bitmask,
                             uint8_t bitmaskSize,
                             IndexOptGetter indexOptGetter,
                             UnknownValueHandler unknownValueHandler ) {
    return writeBitmaskByRange( rangeBegin, rangeEnd, bitmask, bitmaskSize, [&]( decltype( *rangeBegin ) value ) {
        const auto indexOpt = indexOptGetter( value );
        if( !indexOpt ) {
            unknownValueHandler( value );
        }

        return indexOpt;
    } );
}

std::uint8_t getBitsCount( const std::uint8_t* data, std::uint8_t size );

utils::optional<uint8_t> findFirstBit( const std::uint8_t* data, std::uint8_t size, std::uint8_t bitShift = 0 );

/// Time utils

znet_clock_weekday_value_t getClockWeekDay( const znet_date_t& date );

std::chrono::seconds getTimeDifference( const znet_date_time_t& end, const znet_date_time_t& begin );

znet_date_time_t addTimeDifference( const znet_date_time_t& dateTime, const std::chrono::seconds& timeDiff );

template <typename Duration>
Duration getTimeDifference( const znet_date_time_t& end, const znet_date_time_t& begin ) {
    return std::chrono::duration_cast<Duration>( getTimeDifference( end, begin ) );
}

std::string bitmaskToString( const std::uint8_t* data, std::uint8_t size );

const char* boolToYesNo( bool value );

const char* progressToStringStatus( bool isOk, uint8_t progress );

SecondsOpt actuatorReportTimeToSeconds( uint8_t time );

uint8_t actuatorSetTimeFromSeconds( const SecondsOpt& seconds );

utils::optional<std::vector<uint8_t> > hexToBytes( const std::string& hex );

std::string bytesToHex( const std::vector<uint8_t>& bytes );

inline std::stringstream& stringstream( std::stringstream& s ) {
    return s;
}

template <typename Arg, typename... Args>
std::stringstream& stringstream( std::stringstream& s, const Arg& arg, const Args&... args ) {
    s << arg;
    return stringstream( s, args... );
}

template <typename... Args>
std::string stringify( const Args&... args ) {
    std::stringstream stream;
    return stringstream( stream, args... ).str();
}

template <>
std::string stringify<err::znet_error_message_t>( const err::znet_error_message_t& err );

template <typename VectorType>
bool writeVectorToFile( IFile& file, const std::vector<VectorType>& src ) {
    const auto size = static_cast<uint8_t>( src.size() );
    bool result = file.write( &size, sizeof( size ) );
    result &= file.write( src.data(), src.size() * sizeof( VectorType ) );
    return result;
}

template <typename VectorType>
bool readVectorFromFile( IFile& file, std::vector<VectorType>& dest ) {
    uint8_t size = 0;
    bool result = file.read( &size, sizeof( size ) );
    dest.resize( size );
    result &= file.read( dest.data(), size * sizeof( VectorType ) );
    return result;
}

template <typename Type>
bool writeOptionalToFile( IFile& file, const utils::optional<Type>& src ) {
    const bool hasValue = src.has_value();
    bool result = file.write( &hasValue, sizeof( hasValue ) );
    if( hasValue ) {
        result = result && file.write( &*src, sizeof( Type ) );
    }
    return result;
}

template <typename Type>
bool readOptionalFromFile( IFile& file, utils::optional<Type>& dst ) {
    bool hasValue = false;
    bool result = file.read( &hasValue, sizeof( hasValue ) );

    if( hasValue ) {
        dst = Type{};
        result = result && file.read( &*dst, sizeof( Type ) );
    }

    return result;
}

template <size_t length>
inline std::array<uint8_t, length> toStdArray( const uint8_t* ptr ) {
    std::array<uint8_t, length> array;
    std::memcpy( array.data(), ptr, array.size() );
    return array;
}

template <typename EnumType>
inline typename std::underlying_type<EnumType>::type toUnderlyingType( EnumType value ) {
    return static_cast<typename std::underlying_type<EnumType>::type>( value );
}

}  // namespace utils
}  // namespace znet

bool operator==( const znet_time_zone_settings_t& a, const znet_time_zone_settings_t& b );

bool operator==( const znet_dst_time_point_t& a, const znet_dst_time_point_t& b );

bool operator==( const znet_cmd_schedule_entry_lock_time_offset_t& a,
                 const znet_cmd_schedule_entry_lock_time_offset_t& b );

bool operator==( const znet_clock_value_t& a, const znet_clock_value_t& b );
