#pragma once

#include <tuple>
#include <type_traits>
#include <cassert>

namespace znet {

namespace utils {

template <typename T, std::size_t N>
constexpr std::size_t arraySize( const T ( &array )[N] ) {
    return N;
}

template <template <size_t, size_t, typename...> class F, size_t Val, size_t... Vals>
struct Accumulate : std::integral_constant<size_t, Val> {};

template <template <size_t, size_t, typename...> class F, size_t A, size_t B, size_t... Vals>
struct Accumulate<F, A, B, Vals...> : Accumulate<F, F<A, B>::value, Vals...> {};

template <size_t A, size_t B>
struct Max : std::integral_constant<size_t, ( A > B ) ? A : B> {};

template <size_t A, size_t B>
struct Sum : std::integral_constant<size_t, A + B> {};

template <size_t A, size_t B>
struct GetTypeIndexHelper : std::integral_constant<size_t, ( A & 1 ) ? A : B ? A + 1 : A + 2> {};

template <typename Type, typename... Other>
struct GetTypeIndex
: std::integral_constant<size_t, Accumulate<GetTypeIndexHelper, 0, std::is_same<Type, Other>::value...>::value / 2> {};

template <typename... Types>
using AreTypesUnique = std::integral_constant<bool,
                                              Accumulate<Sum, utils::GetTypeIndex<Types, Types...>::value...>::value ==
                                                  sizeof...( Types ) * ( sizeof...( Types ) - 1 ) / 2>;

template <typename Type, typename... Types>
using HasType = std::integral_constant<bool, ( GetTypeIndex<Type, Types...>::value < sizeof...( Types ) )>;

template <template <typename, size_t> class F,
          template <typename...> class C,
          typename,
          typename = C<>,
          size_t Index = 0>
struct RemoveArgs;

template <template <typename, size_t> class F,
          template <typename...> class C,
          typename... ResultArgs,
          typename Arg,
          typename... Args,
          size_t Index>
struct RemoveArgs<F, C, C<Arg, Args...>, C<ResultArgs...>, Index>
: RemoveArgs<F,
             C,
             C<Args...>,
             typename std::conditional<F<Arg, Index>::value, C<ResultArgs...>, C<ResultArgs..., Arg> >::type,
             Index + 1> {};

template <template <typename, size_t> class F, template <typename...> class C, typename... ResultArgs, size_t Index>
struct RemoveArgs<F, C, C<>, C<ResultArgs...>, Index> {
    using type = C<ResultArgs...>;
};

template <template <typename...> class C, typename...>
struct EnsureElementsUnique;

template <template <typename...> class C, typename... Args>
struct EnsureElementsUnique<C, C<Args...> > {
    template <typename Arg, size_t Index>
    struct Helper {
        static constexpr bool value = GetTypeIndex<Arg, Args...>::value != Index;
    };

    using type = typename RemoveArgs<Helper, C, C<Args...> >::type;
};

template <size_t Index, size_t... Indexes>
struct TupleApplier : TupleApplier<Index - 1, Index - 1, Indexes...> {};

template <size_t... Indexes>
struct TupleApplier<0, Indexes...> {
    template <typename F, typename T, typename... BackArgs>
    auto operator()( F&& f, T&& t, BackArgs&&... backArgs )
        -> decltype( f( std::get<Indexes>( std::forward<T>( t ) )..., std::forward<BackArgs>( backArgs )... ) ) {
        return f( std::get<Indexes>( std::forward<T>( t ) )..., std::forward<BackArgs>( backArgs )... );
    }
};

template <typename F, typename T, typename... BackArgs>
auto applyTuple( F&& f, T&& t, BackArgs&&... backArgs ) -> decltype(
    TupleApplier<std::tuple_size<typename std::decay<T>::type>::value>()( std::forward<F>( f ),
                                                                          std::forward<T>( t ),
                                                                          std::forward<BackArgs>( backArgs )... ) ) {
    return TupleApplier<std::tuple_size<typename std::decay<T>::type>::value>()(
        std::forward<F>( f ), std::forward<T>( t ), std::forward<BackArgs>( backArgs )... );
}

template <typename T>
T derefIfNeeded( T t ) {
    return std::forward<T>( t );
}

template <typename T>
T& derefIfNeeded( typename std::add_pointer<T>::type t ) {
    assert( t );
    return *t;
}

namespace details {

template <typename Result, size_t Index, size_t Max, typename F>
typename std::enable_if<( Index < Max ), Result>::type runIndexCase( F&& f ) {
    return f.template operator()<Index>();
}

template <typename Result, size_t Index, size_t Max, typename F>
typename std::enable_if<( Index >= Max ), Result>::type runIndexCase( F&& ) {
    std::abort();
}

}  // namespace details

#define INDEX_CASE_0( Index, MaxIndex, Result, F, f ) \
    case Index:                                       \
        return details::runIndexCase<Result, Index, MaxIndex>( std::forward<F>( f ) );

#define INDEX_ADJUCENT_ARG( Index, Extent ) ( Index + ( 1 << Extent ) )
#define INDEX_CASE_1( Index, ... ) \
    INDEX_CASE_0( Index, __VA_ARGS__ ) INDEX_CASE_0( INDEX_ADJUCENT_ARG( Index, 0 ), __VA_ARGS__ )
#define INDEX_CASE_2( Index, ... ) \
    INDEX_CASE_1( Index, __VA_ARGS__ ) INDEX_CASE_1( INDEX_ADJUCENT_ARG( Index, 1 ), __VA_ARGS__ )
#define INDEX_CASE_3( Index, ... ) \
    INDEX_CASE_2( Index, __VA_ARGS__ ) INDEX_CASE_2( INDEX_ADJUCENT_ARG( Index, 2 ), __VA_ARGS__ )
#define INDEX_CASE_4( Index, ... ) \
    INDEX_CASE_3( Index, __VA_ARGS__ ) INDEX_CASE_3( INDEX_ADJUCENT_ARG( Index, 3 ), __VA_ARGS__ )
#define INDEX_CASE_5( Index, ... ) \
    INDEX_CASE_4( Index, __VA_ARGS__ ) INDEX_CASE_4( INDEX_ADJUCENT_ARG( Index, 4 ), __VA_ARGS__ )
#define INDEX_CASE_6( Index, ... ) \
    INDEX_CASE_5( Index, __VA_ARGS__ ) INDEX_CASE_5( INDEX_ADJUCENT_ARG( Index, 5 ), __VA_ARGS__ )
#define INDEX_CASE_7( Index, ... ) \
    INDEX_CASE_6( Index, __VA_ARGS__ ) INDEX_CASE_6( INDEX_ADJUCENT_ARG( Index, 6 ), __VA_ARGS__ )
#define INDEX_CASE_8( Index, ... ) \
    INDEX_CASE_7( Index, __VA_ARGS__ ) INDEX_CASE_7( INDEX_ADJUCENT_ARG( Index, 7 ), __VA_ARGS__ )
#define INDEX_CASE_9( Index, ... ) \
    INDEX_CASE_8( Index, __VA_ARGS__ ) INDEX_CASE_8( INDEX_ADJUCENT_ARG( Index, 8 ), __VA_ARGS__ )

#define DEFINE_INDEX_HANDLER( Extent )                                                                               \
    template <size_t MaxIndex, typename F, typename Result = decltype( std::declval<F>().template operator()<0>() )> \
    typename std::enable_if<MaxIndex >= ( 1 << ( Extent - 1 ) ) && ( MaxIndex < ( 1 << Extent ) ), Result>::type     \
    applyIndexHandler( size_t index, F&& f ) {                                                                       \
        switch( index ) { INDEX_CASE_##Extent( 0, MaxIndex, Result, F, f ) }                                         \
        return details::runIndexCase<Result, 0, 0>( nullptr );                                                       \
    }

DEFINE_INDEX_HANDLER( 1 )
DEFINE_INDEX_HANDLER( 2 )
DEFINE_INDEX_HANDLER( 3 )
DEFINE_INDEX_HANDLER( 4 )
DEFINE_INDEX_HANDLER( 5 )
DEFINE_INDEX_HANDLER( 6 )
DEFINE_INDEX_HANDLER( 7 )
DEFINE_INDEX_HANDLER( 8 )
DEFINE_INDEX_HANDLER( 9 )

}  // namespace utils
}  // namespace znet
