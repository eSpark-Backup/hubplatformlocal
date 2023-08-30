#pragma once

#include <znet/type_utils.hpp>

namespace znet {

namespace utils {

template <typename... Args>
class variant;

template <size_t, typename...>
struct VariantElement;

template <size_t Index, typename... Args>
struct VariantElement<Index, variant<Args...> > : std::tuple_element<Index, std::tuple<Args...> > {};

template <typename... Args>
struct IsVariant : std::integral_constant<bool, false> {};

template <typename... Args>
struct IsVariant<variant<Args...> > : std::integral_constant<bool, true> {};

namespace details {

template <typename V, typename F>
struct VariantIndexHandler {
    using Variant = typename std::decay<V>::type;

    template <size_t Index>
    decltype(
        std::declval<F>()( std::declval<V>().template getUnsafe<typename VariantElement<Index, Variant>::type>() ) )
    operator()() {
        using Element = typename VariantElement<Index, Variant>::type;
        return std::forward<F>( wrapped )( std::forward<V>( variant ).template getUnsafe<Element>() );
    }

    V&& variant;
    F&& wrapped;
};

template <typename V, typename F>
VariantIndexHandler<V, F> makeVariantIndexHandler( V&& v, F&& f ) {
    return { std::forward<V>( v ), std::forward<F>( f ) };
}

struct VariantDeleter {
    template <typename T>
    void operator()( T& value ) const {
        value.~T();
    }
};

struct VariantCreator {
    void* data;

    template <typename T>
    void operator()( T&& value ) const {
        using BaseType = typename std::decay<T>::type;
        new( data ) BaseType( std::forward<T>( value ) );
    }
};

struct VariantAssigner {
    void* data;

    template <typename T>
    void operator()( T&& value ) const {
        using BaseType = typename std::decay<T>::type;
        *reinterpret_cast<BaseType*>( data ) = std::forward<T>( value );
    }
};

struct VariantEqual {
    const void* data;

    template <typename T>
    bool operator()( const T& value ) const {
        return *reinterpret_cast<const T*>( data ) == value;
    }
};

struct VariantLess {
    const void* data;

    template <typename T>
    bool operator()( const T& value ) const {
        return *reinterpret_cast<const T*>( data ) < value;
    }
};

}  // namespace details
}  // namespace utils
}  // namespace znet
