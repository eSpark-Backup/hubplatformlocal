#pragma once

#include <type_traits>
#include <utility>

#include <znet/type_utils.hpp>
#include <znet/variant_details.hpp>

namespace znet {

namespace visitors {

namespace details {

template <typename...>
struct Visitor;

template <>
struct Visitor<> {
    virtual ~Visitor() = default;
};

template <typename Acceptor, typename... Acceptors>
struct Visitor<Acceptor, Acceptors...> : Visitor<Acceptors...> {
    using Visitor<Acceptors...>::operator();
    virtual void operator()( Acceptor ) = 0;
};

template <typename Acceptor>
struct Visitor<Acceptor> : Visitor<> {
    virtual void operator()( Acceptor ) = 0;
};

template <typename T>
struct VisitorResultWrapper {
    template <typename V, typename... Args>
    void applyResult( V&& v, Args&&... args ) {
        value = v( std::forward<Args>( args )... );
    }

    T takeResult() {
        return std::move( value );
    }

    T value{};
};

template <>
struct VisitorResultWrapper<void> {
    template <typename V, typename... Args>
    void applyResult( V&& v, Args&&... args ) {
        v( std::forward<Args>( args )... );
    }

    void takeResult() {}
};

template <typename...>
struct VisitorWithResult;

template <typename Base, typename Overloads, typename Acceptor, typename... Acceptors>
struct VisitorWithResult<Base, Overloads, Acceptor, Acceptors...> : VisitorWithResult<Base, Overloads, Acceptors...> {
    using VisitorWithResult<Base, Overloads, Acceptors...>::VisitorWithResult;
    using VisitorWithResult<Base, Overloads, Acceptors...>::operator();
    void operator()( Acceptor& acceptor ) override {
        this->applyResult( this->overloads, acceptor );
    }
};

template <typename Base, typename Overloads, typename Acceptor>
struct VisitorWithResult<Base, Overloads, Acceptor>
: Base, VisitorResultWrapper<decltype( std::declval<Overloads>()( std::declval<Acceptor&>() ) )> {
    VisitorWithResult<Base, Overloads, Acceptor>( Overloads&& v )
    : overloads( std::forward<Overloads>( v ) ) {}

    void operator()( Acceptor& acceptor ) override {
        this->applyResult( overloads, acceptor );
    }

    Overloads overloads;
};

template <typename Functor, typename... Functors>
struct FunctorsOverloads : Functor, FunctorsOverloads<Functors...> {
    FunctorsOverloads( Functor&& functor, Functors&&... functors )
    : Functor( std::move( functor ) )
    , FunctorsOverloads<Functors...>( std::move( functors )... ) {}

    using Functor::operator();
    using FunctorsOverloads<Functors...>::operator();
};

template <typename Functor>
struct FunctorsOverloads<Functor> : Functor {
    FunctorsOverloads( Functor&& f )
    : Functor( std::move( f ) ) {}
    using Functor::operator();
};

template <typename Functor>
struct DefaultOverload : Functor {
    DefaultOverload( Functor&& f )
    : Functor( std::move( f ) ) {}

    template <typename... Args>
    decltype( std::declval<Functor>()() ) operator()( Args&&... ) const {
        return Functor::operator()();
    }
};

template <typename Overloads, typename... Acceptors>
struct VisitorWithResult<Visitor<Acceptors...>, Overloads>
: VisitorWithResult<Visitor<Acceptors...>, Overloads, Acceptors...> {
    using VisitorWithResult<Visitor<Acceptors...>, Overloads, Acceptors...>::VisitorWithResult;
};

template <typename Base, typename Overloads>
typename std::enable_if<!std::is_base_of<Visitor<>, typename std::decay<Overloads>::type>::value,
                        VisitorWithResult<Base, Overloads>>::type
makeVisitorWithResult( Overloads&& overloads ) {
    return { std::forward<Overloads>( overloads ) };
}

}  // namespace details

template <typename... Acceptors>
using Visitor = details::Visitor<Acceptors...>;

template <typename... Acceptors>
struct BaseAcceptor {
    using Visitor = visitors::Visitor<Acceptors&...>;
    using ConstVisitor = visitors::Visitor<const Acceptors&...>;

    virtual ~BaseAcceptor() = default;

    virtual void accept( Visitor& ) = 0;
    virtual void accept( ConstVisitor& ) const = 0;
};

#define ZNET_DEFINE_ACCEPT()                        \
    void accept( Visitor& v ) override {            \
        v( *this );                                 \
    }                                               \
    void accept( ConstVisitor& v ) const override { \
        v( *this );                                 \
    }

template <typename Acceptor>
using VisitorOfAcceptor = typename std::
    conditional<std::is_const<Acceptor>::value, typename Acceptor::ConstVisitor, typename Acceptor::Visitor>::type;

template <typename... Functors>
details::FunctorsOverloads<Functors...> makeOverloads( Functors... functors ) {
    return { std::move( functors )... };
}

template <typename Functor>
details::DefaultOverload<Functor> makeDefaultOverload( Functor f ) {
    return { std::move( f ) };
}

template <typename V, typename Acceptor>
auto visit( V&& visitor, Acceptor&& acceptor ) -> decltype( acceptor.accept( std::forward<V>( visitor ) ) ) {
    return acceptor.accept( std::forward<V>( visitor ) );
}

template <typename Overloads,
          typename Acceptor,
          typename V = VisitorOfAcceptor<typename std::remove_reference<Acceptor>::type>>
auto visit( Overloads&& overloads, Acceptor&& acceptor )
    -> decltype( details::makeVisitorWithResult<V>( std::forward<Overloads>( overloads ) ).takeResult() ) {
    auto visitorWithResult = details::makeVisitorWithResult<V>( std::forward<Overloads>( overloads ) );
    visit( visitorWithResult, std::forward<Acceptor>( acceptor ) );
    return visitorWithResult.takeResult();
}

template <typename F, typename Variant>
decltype( utils::applyIndexHandler<1>( std::declval<Variant>().getIndex(),
                                       utils::details::makeVariantIndexHandler( std::declval<Variant>(),
                                                                                std::declval<F>() ) ) )
visit( F&& f, Variant&& v ) {
    using Type = typename std::decay<Variant>::type;
    return utils::applyIndexHandler<Type::ElementsCount>(
        v.getIndex(), utils::details::makeVariantIndexHandler( std::forward<Variant>( v ), std::forward<F>( f ) ) );
}

namespace details {

template <typename F, typename ArgsTuple, typename... Acceptors>
struct MultiVisitingHelper;

template <typename F, typename ArgsTuple, typename... Acceptors>
MultiVisitingHelper<F, ArgsTuple, Acceptors...> makeMultiVisitingHelper( F&& f,
                                                                         ArgsTuple&& argsTuple,
                                                                         Acceptors&&... acceptors ) {
    return { std::forward<F>( f ),
             std::forward<ArgsTuple>( argsTuple ),
             std::tuple<Acceptors&&...>{ std::forward<Acceptors>( acceptors )... } };
}

template <typename F, typename ArgsTuple, typename Acceptor, typename... Acceptors>
struct MultiVisitingHelper<F, ArgsTuple, Acceptor, Acceptors...> {
    F&& f;
    ArgsTuple argsTuple;
    std::tuple<Acceptor&&, Acceptors&&...> acceptorsTuple;

    template <typename Arg, typename... Args>
    static std::tuple<Args&&..., Arg&&> handleArgs( std::tuple<Args...>&& tuple, Arg&& arg ) {
        return utils::applyTuple( &std::forward_as_tuple<Args&&..., Arg&&>, tuple, std::forward<Arg>( arg ) );
    }

    template <typename Arg>
    decltype( visit( makeMultiVisitingHelper( std::declval<F>(),
                                              handleArgs( std::move( argsTuple ), std::declval<Arg>() ),
                                              std::declval<Acceptors>()... ),
                     std::declval<Acceptor>() ) )
    operator()( Acceptor&& acceptor, Acceptors&&... acceptors, Arg&& arg ) {
        return visit( makeMultiVisitingHelper( std::forward<F>( f ),
                                               handleArgs( std::move( argsTuple ), std::forward<Arg>( arg ) ),
                                               std::forward<Acceptors>( acceptors )... ),
                      std::forward<Acceptor>( acceptor ) );
    }

    template <typename Arg>
    auto operator()( Arg&& arg )
        -> decltype( utils::applyTuple( *this, std::move( acceptorsTuple ), std::forward<Arg>( arg ) ) ) {
        return utils::applyTuple( *this, std::move( acceptorsTuple ), std::forward<Arg>( arg ) );
    }
};

template <typename F, typename ArgsTuple>
struct MultiVisitingHelper<F, ArgsTuple> {
    F&& f;
    ArgsTuple argsTuple;
    std::tuple<> acceptorsTuple;

    template <typename Arg, typename... Args>
    static auto handleArgs( F&& f, std::tuple<Args...>&& tuple, Arg&& arg ) -> decltype(
        utils::applyTuple( std::declval<F>(), std::declval<std::tuple<Args...>>(), std::declval<Arg>() ) ) {
        return utils::applyTuple( std::forward<F>( f ), std::move( tuple ), std::forward<Arg>( arg ) );
    }

    template <typename Arg>
    decltype( handleArgs( std::declval<F>(), std::declval<ArgsTuple>(), std::declval<Arg>() ) ) operator()(
        Arg&& arg ) {
        return handleArgs( std::forward<F>( f ), std::move( argsTuple ), std::forward<Arg>( arg ) );
    }
};

}  // namespace details

template <typename Overloads, typename Acceptor1, typename Acceptor2, typename... Acceptors>
decltype( visit( details::makeMultiVisitingHelper( std::declval<Overloads>(),
                                                   std::declval<std::tuple<>>(),
                                                   std::declval<Acceptor2>(),
                                                   std::declval<Acceptors>()... ),
                 std::declval<Acceptor1>() ) )
visit( Overloads&& overloads, Acceptor1&& acceptor1, Acceptor2&& acceptor2, Acceptors&&... acceptors ) {
    return visit( details::makeMultiVisitingHelper( std::forward<Overloads>( overloads ),
                                                    std::tuple<>(),
                                                    std::forward<Acceptor2>( acceptor2 ),
                                                    std::forward<Acceptors>( acceptors )... ),
                  std::forward<Acceptor1>( acceptor1 ) );
}

}  // namespace visitors

}  // namespace znet
