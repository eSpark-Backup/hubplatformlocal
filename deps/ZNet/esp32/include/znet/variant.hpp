#pragma once

#include <znet/znet_visitors.hpp>

namespace znet {

namespace utils {

template <typename... Args>
class variant {
    template <typename V, typename F>
    friend class details::VariantIndexHandler;

public:
    static constexpr size_t ElementsCount = sizeof...( Args );
    using FirstType = typename std::tuple_element<0, std::tuple<Args...> >::type;
    using Index = typename std::conditional < ElementsCount<256, uint8_t, uint16_t>::type;

    template <typename Front = FirstType,
              typename std::enable_if<std::is_default_constructible<Front>::value>::type* = nullptr>
    variant() noexcept( std::is_nothrow_default_constructible<Front>::value ) {
        new( m_data ) Front();
    }

    template <
        typename Value,
        typename std::enable_if<utils::HasType<typename std::decay<Value>::type, Args...>::value>::type* = nullptr>
    variant( Value&& value )
    : m_index( GetTypeIndex<typename std::decay<Value>::type, Args...>::value ) {
        new( m_data ) typename std::decay<Value>::type( std::forward<Value>( value ) );
    }

    ~variant() {
        visitors::visit( details::VariantDeleter(), *this );
    }

    variant( const variant& other )
    : m_index( other.m_index ) {
        visitors::visit( details::VariantCreator{ m_data }, other );
    }

    variant( variant&& other ) noexcept(
        utils::Accumulate<utils::Sum, std::is_nothrow_move_constructible<Args>::value...>::value == ElementsCount )
    : m_index( other.m_index ) {
        visitors::visit( details::VariantCreator{ m_data }, std::move( other ) );
    }

    template <
        typename Value,
        typename std::enable_if<utils::HasType<typename std::decay<Value>::type, Args...>::value>::type* = nullptr>
    variant& operator=( Value&& value ) {
        assign( std::forward<Value>( value ) );
        return *this;
    }

    variant& operator=( const variant& other ) {
        assign( other );
        return *this;
    }

    variant& operator=( variant&& other ) noexcept(
        utils::Accumulate<utils::Sum, std::is_nothrow_move_assignable<Args>::value...>::value == ElementsCount ) {
        assign( std::move( other ) );
        return *this;
    }

    template <typename Type>
    Type* getPtr() {
        return m_index == GetTypeIndex<typename std::remove_cv<Type>::type, Args...>::value ?
                   reinterpret_cast<Type*>( m_data ) :
                   nullptr;
    }

    template <typename Type>
    const Type* getPtr() const {
        return m_index == GetTypeIndex<typename std::remove_cv<Type>::type, Args...>::value ?
                   reinterpret_cast<const Type*>( m_data ) :
                   nullptr;
    }

    template <typename Type>
    Type& get() & {
        checkType<Type>();
        return *reinterpret_cast<Type*>( m_data );
    }

    template <typename Type>
    Type get() && {
        checkType<Type>();
        return std::move( *reinterpret_cast<Type*>( m_data ) );
    }

    template <typename Type>
    const Type& get() const {
        checkType<Type>();
        return *reinterpret_cast<const Type*>( m_data );
    }

    Index getIndex() const {
        return m_index;
    }

    bool operator==( const variant& other ) const {
        return m_index == other.m_index && visitors::visit( details::VariantEqual{ m_data }, other );
    }

    bool operator!=( const variant& other ) const {
        return !( *this == other );
    }

    bool operator<( const variant& other ) const {
        return m_index < other.m_index ||
               ( m_index == other.m_index && visitors::visit( details::VariantLess{ m_data }, other ) );
    }

    template <typename Variant>
    typename std::enable_if<IsVariant<typename std::decay<Variant>::type>::value>::type assign( Variant&& other ) {
        if( m_index == other.m_index ) {
            visitors::visit( details::VariantAssigner{ m_data }, std::forward<Variant>( other ) );
        }
        else {
            m_index = other.m_index;
            visitors::visit( details::VariantDeleter(), *this );
            visitors::visit( details::VariantCreator{ m_data }, std::forward<Variant>( other ) );
        }
    }

    template <typename Value>
    typename std::enable_if<utils::HasType<typename std::decay<Value>::type, Args...>::value>::type assign(
        Value&& value ) {
        using BaseType = typename std::decay<Value>::type;
        constexpr auto newIndex = GetTypeIndex<BaseType, Args...>::value;
        if( m_index == newIndex ) {
            getUnsafe<BaseType>() = std::forward<Value>( value );
        }
        else {
            m_index = newIndex;
            visitors::visit( details::VariantDeleter(), *this );
            new( m_data ) BaseType( std::forward<Value>( value ) );
        }
    }

private:
    template <typename Type>
    void checkType() {
        if( m_index != GetTypeIndex<typename std::remove_cv<Type>::type, Args...>::value ) {
            std::abort();
        }
    }

    template <typename Type>
    Type& getUnsafe() & {
        return *reinterpret_cast<Type*>( m_data );
    }

    template <typename Type>
    Type getUnsafe() && {
        return std::move( *reinterpret_cast<Type*>( m_data ) );
    }

    template <typename Type>
    const Type& getUnsafe() const {
        return *reinterpret_cast<const Type*>( m_data );
    }

private:
    Index m_index = 0;

    static constexpr size_t MaxSize = utils::Accumulate<utils::Max, sizeof( Args )...>::value;
    uint8_t m_data[MaxSize];
};

}  // namespace utils
}  // namespace znet
