#include "gtest/gtest.h"

#include "znet/znet_utils.hpp"
#include "znet/znet.h"

static bool operator==( const znet_date_time_t& a, const znet_date_time_t& b ) {
    return a.date.year == b.date.year && a.date.month == b.date.month && a.date.day == b.date.day &&

           a.time.hour == b.time.hour && a.time.minute == b.time.minute && a.time.second == b.time.second;
}

static void PrintTo( const znet_date_time_t& dateTime, ::std::ostream* os ) {
    *os << static_cast<int>( dateTime.date.year ) << ':' << static_cast<int>( dateTime.date.month ) << ':'
        << static_cast<int>( dateTime.date.day ) << " " << static_cast<int>( dateTime.time.hour ) << ':'
        << static_cast<int>( dateTime.time.minute ) << ':' << static_cast<int>( dateTime.time.second );
}

namespace std {
static void PrintTo( const std::chrono::seconds& s, ::std::ostream* os ) {
    *os << s.count() << "s";
}
}  // namespace std

namespace znet {
namespace utils {
namespace ut {

namespace {

void testInvalidDateTime( const znet_date_time_t& invalidDateTime ) {
    znet_date_time_t validDateTime;
    validDateTime.date.year = 2019;
    validDateTime.date.month = 1;
    validDateTime.date.day = 29;
    validDateTime.time.hour = 1;
    validDateTime.time.minute = 3;
    validDateTime.time.second = 59;

    EXPECT_EQ( invalidDateTime, addTimeDifference( invalidDateTime, std::chrono::seconds( 100 ) ) );
    EXPECT_EQ( std::chrono::seconds( 0 ), getTimeDifference( invalidDateTime, validDateTime ) );
    EXPECT_EQ( std::chrono::seconds( 0 ), getTimeDifference( validDateTime, invalidDateTime ) );
}

void testDateTimeDifference( const znet_date_time_t& end,
                             const znet_date_time_t& begin,
                             std::chrono::seconds timeDiff ) {
    EXPECT_EQ( timeDiff, getTimeDifference( end, begin ) );
    EXPECT_EQ( -timeDiff, getTimeDifference( begin, end ) );

    EXPECT_EQ( end, addTimeDifference( begin, timeDiff ) );
    EXPECT_EQ( begin, addTimeDifference( end, -timeDiff ) );
}

}  // namespace

TEST( ZNetUtilsTestSuite, getTimeDifferenceSimple ) {
    znet_date_time_t dateTime1;
    dateTime1.date.year = 2019;
    dateTime1.date.month = 3;
    dateTime1.date.day = 3;
    dateTime1.time.hour = 1;
    dateTime1.time.minute = 3;
    dateTime1.time.second = 59;

    znet_date_time_t dateTime2;
    dateTime2.date.year = 2019;
    dateTime2.date.month = 3;
    dateTime2.date.day = 3;
    dateTime2.time.hour = 1;
    dateTime2.time.minute = 4;
    dateTime2.time.second = 1;

    testDateTimeDifference( dateTime2, dateTime1, std::chrono::seconds( 2 ) );
}

TEST( ZNetUtilsTestSuite, getTimeDifferenceInvalid ) {
    znet_date_time_t invalidDateTime;
    invalidDateTime.date.year = 2019;
    invalidDateTime.date.month = 2;
    invalidDateTime.time.hour = 1;
    invalidDateTime.time.minute = 3;
    invalidDateTime.time.second = 59;

    invalidDateTime.date.day = 29;
    testInvalidDateTime( invalidDateTime );

    invalidDateTime.date.day = 0;
    testInvalidDateTime( invalidDateTime );

    invalidDateTime.date.day = 1;
    invalidDateTime.date.month = 0;
    testInvalidDateTime( invalidDateTime );

    invalidDateTime.date.month = 13;
    testInvalidDateTime( invalidDateTime );

    invalidDateTime.date.month = 1;
    invalidDateTime.time.hour = 24;
    testInvalidDateTime( invalidDateTime );

    invalidDateTime.time.hour = 0;
    invalidDateTime.time.minute = 60;
    testInvalidDateTime( invalidDateTime );

    invalidDateTime.time.minute = 0;
    invalidDateTime.time.second = 60;
    testInvalidDateTime( invalidDateTime );
}

class ZNetYearsMonthsTestSuite
: public ::testing::TestWithParam<
      std::tuple<bool, znet_cmd_time_parameters_year_t, znet_cmd_time_parameters_month_t> > {
protected:
    static znet_cmd_time_parameters_day_t getDaysInMonth( znet_cmd_time_parameters_month_t month, bool isLeap ) {
        static constexpr znet_cmd_time_parameters_month_t monthsDays[] = { 31, 28, 31, 30, 31, 30,
                                                                           31, 31, 30, 31, 30, 31 };
        return month >= 1 && month <= 12 ? ( monthsDays[month - 1] + ( isLeap && month == 2 ) ) : 0;
    }

    static uint16_t getDaysInMonths( znet_cmd_time_parameters_month_t monthStart,
                                     znet_cmd_time_parameters_month_t monthEnd,
                                     bool isLeap ) {
        uint16_t result = 0;
        for( auto month = monthStart; month < monthEnd; ++month )
            result += getDaysInMonth( month, isLeap );

        return result;
    }
};

TEST( ZNetUtilsTestSuite, getClockWeekDay ) {
    EXPECT_EQ( 0, getClockWeekDay( { 2020, 3, 0 } ) );  // Invalid day
    EXPECT_EQ( 0, getClockWeekDay( { 2020, 0, 1 } ) );  // Invalid day

    EXPECT_EQ( 1, getClockWeekDay( { 2020, 3, 30 } ) );  // Monday
    EXPECT_EQ( 2, getClockWeekDay( { 2020, 3, 31 } ) );  // Thuesday
    EXPECT_EQ( 3, getClockWeekDay( { 2020, 5, 6 } ) );   // Wednesday
    EXPECT_EQ( 4, getClockWeekDay( { 2021, 2, 4 } ) );   // Thursday
    EXPECT_EQ( 5, getClockWeekDay( { 2021, 6, 4 } ) );   // Friday
    EXPECT_EQ( 6, getClockWeekDay( { 2021, 6, 5 } ) );   // Saturday
    EXPECT_EQ( 7, getClockWeekDay( { 2022, 1, 2 } ) );   // Sunday
    EXPECT_EQ( 1, getClockWeekDay( { 2022, 1, 3 } ) );   // Monday
}

TEST_P( ZNetYearsMonthsTestSuite, getTimeDifferenceMonthShiftInLeapYears ) {
    bool isLeap;

    znet_date_time_t dateTime1;

    std::tie( isLeap, dateTime1.date.year, dateTime1.date.month ) = GetParam();
    dateTime1.date.day = getDaysInMonth( dateTime1.date.month, isLeap );

    dateTime1.time.hour = 23;
    dateTime1.time.minute = 59;
    dateTime1.time.second = 58;

    znet_date_time_t dateTime2;
    dateTime2.date.year = dateTime1.date.year + dateTime1.date.month / 12;
    dateTime2.date.month = dateTime1.date.month % 12 + 1;
    dateTime2.date.day = 1;

    dateTime2.time.hour = 0;
    dateTime2.time.minute = 0;

    dateTime2.time.second = 1;
    testDateTimeDifference( dateTime2, dateTime1, std::chrono::seconds( 3 ) );

    dateTime2.time.second = 0;
    testDateTimeDifference( dateTime2, dateTime1, std::chrono::seconds( 2 ) );
}

TEST_P( ZNetYearsMonthsTestSuite, timeDifferenceWithDst ) {
    bool isLeap;
    znet_date_time_t dateTime2;

    std::tie( isLeap, dateTime2.date.year, dateTime2.date.month ) = GetParam();

    dateTime2.date.day = 28;

    dateTime2.time.hour = 12;
    dateTime2.time.minute = 6;
    dateTime2.time.second = 5;

    znet_date_time_t dateTime1 = dateTime2;
    dateTime1.date.month = 1;

    znet_date_time_t dateTime3 = dateTime1;
    dateTime3.date.month = 12;

    testDateTimeDifference(
        dateTime2,
        dateTime1,
        std::chrono::hours( getDaysInMonths( dateTime1.date.month, dateTime2.date.month, isLeap ) * 24 ) );
    testDateTimeDifference(
        dateTime3,
        dateTime2,
        std::chrono::hours( getDaysInMonths( dateTime2.date.month, dateTime3.date.month, isLeap ) * 24 ) );
}

INSTANTIATE_TEST_CASE_P( NotLeapYears,
                         ZNetYearsMonthsTestSuite,
                         ::testing::Combine(::testing::Values( false ),
                                            ::testing::Values( 1599, 1700, 1800, 1900, 1951, 1999, 2001, 2021 ),
                                            ::testing::Values( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ) ) );

INSTANTIATE_TEST_CASE_P( LeapYears,
                         ZNetYearsMonthsTestSuite,
                         ::testing::Combine(::testing::Values( true ),
                                            ::testing::Values( 1600, 1604, 1996, 2000, 2020, 2092, 2208 ),
                                            ::testing::Values( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ) ) );

TEST( ZNetUtilsTestSuite, actuatorTimeToSeconds ) {
    EXPECT_EQ( actuatorReportTimeToSeconds( 0 ), SecondsOpt( Seconds( 0 ) ) );
    EXPECT_EQ( actuatorReportTimeToSeconds( 50 ), SecondsOpt( Seconds( 50 ) ) );
    EXPECT_EQ( actuatorReportTimeToSeconds( 127 ), SecondsOpt( Seconds( 127 ) ) );
    EXPECT_EQ( actuatorReportTimeToSeconds( 128 ), SecondsOpt( std::chrono::minutes( 1 ) ) );
    EXPECT_EQ( actuatorReportTimeToSeconds( 0xFD ), SecondsOpt( std::chrono::minutes( 0xFD - 127 ) ) );
    EXPECT_EQ( actuatorReportTimeToSeconds( 0xFE ), SecondsOpt{} );
    EXPECT_EQ( actuatorReportTimeToSeconds( 0xFF ), SecondsOpt{} );
}

TEST( ZNetUtilsTestSuite, actuatorSetTimeFromSeconds ) {
    EXPECT_EQ( actuatorSetTimeFromSeconds( {} ), 0xFF );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 0 ) ), 0 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 1 ) ), 1 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 100 ) ), 100 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 127 ) ), 127 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 128 ) ), 127 + 2 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 129 ) ), 127 + 2 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 60 * 3 ) ), 127 + 3 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 60 * 100 ) ), 127 + 100 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 60 * 126 ) ), 127 + 126 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 60 * 127 ) ), 127 + 127 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 60 * 128 ) ), 127 + 127 );
    EXPECT_EQ( actuatorSetTimeFromSeconds( Seconds( 60 * 130 ) ), 127 + 127 );
}

TEST( ZNetUtilsTestSuite, hexToBytes ) {
    using Bytes = std::vector<uint8_t>;
    using BytesOpt = utils::optional<Bytes>;

    EXPECT_EQ( hexToBytes( "" ), BytesOpt( Bytes{} ) );
    EXPECT_EQ( hexToBytes( "0" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "9" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "F" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "a" ), BytesOpt() );

    EXPECT_EQ( hexToBytes( "g" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "z" ), BytesOpt() );

    EXPECT_EQ( hexToBytes( "0A" ), BytesOpt( Bytes{ 0x0A } ) );
    EXPECT_EQ( hexToBytes( "CB" ), BytesOpt( Bytes{ 0xCB } ) );
    EXPECT_EQ( hexToBytes( "F1" ), BytesOpt( Bytes{ 0xF1 } ) );
    EXPECT_EQ( hexToBytes( "9F" ), BytesOpt( Bytes{ 0x9F } ) );

    EXPECT_EQ( hexToBytes( "1234567890abcdefABCDEF" ),
               BytesOpt( Bytes{ 0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0xab, 0xcd, 0xef } ) );
    EXPECT_EQ( hexToBytes( "11234567890abcdefABCDEF" ), BytesOpt() );

    EXPECT_EQ( hexToBytes( "+1234567890abcdefABCDEF" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "-1234567890abcdefABCDEF" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "11234567890abgcdefABCDEF" ), BytesOpt() );

    EXPECT_EQ( hexToBytes( "+123467890abcdefABCDEF" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "-123467890abcdefABCDEF" ), BytesOpt() );
    EXPECT_EQ( hexToBytes( "1123467890abgcdefABCDEF" ), BytesOpt() );
}

TEST( ZNetUtilsTestSuite, bytesToHex ) {
    EXPECT_EQ( bytesToHex( {} ), "" );
    EXPECT_EQ( bytesToHex( { 0x00 } ), "00" );
    EXPECT_EQ( bytesToHex( { 0x01 } ), "01" );
    EXPECT_EQ( bytesToHex( { 0x09 } ), "09" );
    EXPECT_EQ( bytesToHex( { 0x0A } ), "0A" );
    EXPECT_EQ( bytesToHex( { 0x0C } ), "0C" );
    EXPECT_EQ( bytesToHex( { 0x0F } ), "0F" );

    EXPECT_EQ( bytesToHex( { 0x00 } ), "00" );
    EXPECT_EQ( bytesToHex( { 0x10 } ), "10" );
    EXPECT_EQ( bytesToHex( { 0x90 } ), "90" );
    EXPECT_EQ( bytesToHex( { 0xA0 } ), "A0" );
    EXPECT_EQ( bytesToHex( { 0xC0 } ), "C0" );
    EXPECT_EQ( bytesToHex( { 0xF0 } ), "F0" );

    EXPECT_EQ( bytesToHex( { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF } ), "0123456789ABCDEF" );
    EXPECT_EQ( bytesToHex( { 0xF1, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xC3, 0xE0 } ), "F123456789ABC3E0" );
}

}  // namespace ut
}  // namespace utils
}  // namespace znet
