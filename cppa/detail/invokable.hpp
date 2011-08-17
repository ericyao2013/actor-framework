#ifndef INVOKABLE_HPP
#define INVOKABLE_HPP

#include <vector>
#include <cstddef>

#include "cppa/invoke.hpp"
#include "cppa/any_tuple.hpp"
#include "cppa/detail/intermediate.hpp"

// forward declaration
namespace cppa { class any_tuple; }

namespace cppa { namespace detail {

class invokable
{

    invokable(const invokable&) = delete;
    invokable& operator=(const invokable&) = delete;

 public:

    invokable() = default;
    virtual ~invokable();
    virtual bool invoke(const any_tuple&) const = 0;
    virtual intermediate* get_intermediate(const any_tuple&) const = 0;

};

template<class TupleView, class MatchFunction, class TargetFun>
class invokable_impl : public invokable
{

    MatchFunction m_match;
    TargetFun m_target;

 public:

    //invokable_impl(MatchFunction&& mm, TargetFun&& mt)
    //    : m_match(std::move(mm)), m_target(std::move(mt))
    //{
    //}

    invokable_impl(MatchFunction&& mm, const TargetFun& mt)
        : m_match(std::move(mm)), m_target(mt)
    {
    }

    bool invoke(const any_tuple& data) const
    {
        std::vector<size_t> mv;
        if (m_match(data, &mv))
        {
            TupleView tv(data.vals(), std::move(mv));
            cppa::invoke(m_target, tv);
            return true;
        }
        return false;
    }

    intermediate* get_intermediate(const any_tuple& data) const
    {
        struct iimpl : intermediate
        {
            TupleView m_args;
            const TargetFun& m_target;

            iimpl(TupleView&& tv, const TargetFun& tf)
                : m_args(std::move(tv)), m_target(tf)
            {
            }

            void invoke() // override
            {
                cppa::invoke(m_target, m_args);
            }

        };
        std::vector<size_t> mv;
        return (m_match(data, &mv)) ? new iimpl(TupleView(data.vals(),
                                                          std::move(mv)),
                                                m_target)
                                    : nullptr;
    }

};

template<template<class...> class TupleClass, class MatchFunction, class TargetFun>
class invokable_impl<TupleClass<>, MatchFunction, TargetFun> : public invokable
{

    MatchFunction m_match;
    TargetFun m_target;

 public:

    //invokable_impl(MatchFunction&& mm, TargetFun&& mt)
    //    : m_match(std::move(mm)), m_target(std::move(mt))
    //{
    //}

    invokable_impl(MatchFunction&& mm, const TargetFun& mt)
        : m_match(std::move(mm)), m_target(mt)
    {
    }

    bool invoke(const any_tuple& data) const
    {
        if (m_match(data, nullptr))
        {
            m_target();
            return true;
        }
        return false;
    }

    intermediate* get_intermediate(const any_tuple& data) const
    {
        struct iimpl : intermediate
        {
            const TargetFun& m_target;

            iimpl(const TargetFun& tf) : m_target(tf)
            {
            }

            void invoke()
            {
                m_target();
            }
        };
        return m_match(data, nullptr) ? new iimpl(m_target) : nullptr;
    }
};

} } // namespace cppa::detail

#endif // INVOKABLE_HPP
