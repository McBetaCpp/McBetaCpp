#pragma once

#include <type_traits>
#include <memory>

namespace Util
{

// Size implementation
template<class C>
constexpr auto size(const C &c) -> decltype(c.size())
{
	return c.size();
}

template<class T, std::size_t N>
constexpr std::size_t size(const T(&array)[N]) noexcept
{
	return N;
}

// Taken from here https://isocpp.org/files/papers/N3656.txt
// Make unique implementation
template<class T> struct _Unique_if {
	typedef std::unique_ptr<T> _Single_object;
};

template<class T> struct _Unique_if<T[]> {
	typedef std::unique_ptr<T[]> _Unknown_bound;
};

template<class T, size_t N> struct _Unique_if<T[N]> {
	typedef void _Known_bound;
};

template<class T, class... Args>
typename _Unique_if<T>::_Single_object
make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename _Unique_if<T>::_Unknown_bound
make_unique(size_t n) {
	typedef typename std::remove_extent<T>::type U;
	return std::unique_ptr<T>(new U[n]());
}

template<class T, class... Args>
typename _Unique_if<T>::_Known_bound
make_unique(Args&&...) = delete;

// Make shared implementation
template<class T> struct _Shared_if {
	typedef std::shared_ptr<T> _Single_object;
};

template<class T> struct _Shared_if<T[]> {
	typedef std::shared_ptr<T[]> _Unknown_bound;
};

template<class T, size_t N> struct _Shared_if<T[N]> {
	typedef void _Known_bound;
};

template<class T, class... Args>
typename _Shared_if<T>::_Single_object
make_shared(Args&&... args) {
	return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename _Shared_if<T>::_Unknown_bound
make_shared(size_t n) {
	typedef typename std::remove_extent<T>::type U;
	return std::shared_ptr<T>(new U[n]());
}

template<class T, class... Args>
typename _Shared_if<T>::_Known_bound
make_shared(Args&&...) = delete;

}