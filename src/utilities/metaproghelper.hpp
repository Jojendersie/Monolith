#pragma once

// The following construct allows to inherit or not, based on a compile time bool.
// It can be used as:
// class B : public inherit_if<A, ...> {};
// such that B might have members of A or have no more than some compile time overhead.
template<bool, typename T>
class inherit_if {};

template<typename T>
class inherit_if<true, T> : public T {};


// Structure to inherit conditional to be able to replace with a simplified base
// which defaults to dummys for some expected members.
// Inherits from A if the condition is true.
template<bool, typename A, typename B>
class inherit_conditional : public A {};

template<typename A, typename B>
class inherit_conditional<false, A, B> : public B {};
