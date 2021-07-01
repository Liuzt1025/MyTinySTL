#ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等 

#include <cstddef>

#include "type_traits.h"

namespace mystl
{

// move

template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) noexcept
{
  return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

// forward

template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
  return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
{
  static_assert(!std::is_lvalue_reference<T>::value, "bad forward");	// std::is_lvalue_reference 这个是什么东西？
  return static_cast<T&&>(arg);
}

// swap

template <class Tp>
void swap(Tp& lhs, Tp& rhs)
{
  auto tmp(mystl::move(lhs));
  lhs = mystl::move(rhs);		// 这个地方为什么需要使用右值引用？如果是普通类型或者左值，那么相当与拷贝， 如果是右值此举有利于提高效率
  rhs = mystl::move(tmp);
}

template <class ForwardIter1, class ForwardIter2>
ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
{
  for (; first1 != last1; ++first1, (void) ++first2)
    mystl::swap(*first1, *first2);
  return first2;
}

template <class Tp, size_t N>		// 模板好像是提供了这个东西， 什么情况调用这个模板？
void swap(Tp(&a)[N], Tp(&b)[N])
{
  mystl::swap_range(a, a + N, b);
}

// --------------------------------------------------------------------------------------
// pair

// 结构体模板 : pair
// 两个模板参数分别表示两个数据的类型
// 用 first 和 second 来分别取出第一个数据和第二个数据
template <class Ty1, class Ty2>
struct pair
{
  typedef Ty1    first_type;
  typedef Ty2    second_type;

  first_type first;    // 保存第一个数据
  second_type second;  // 保存第二个数据

  // default constructiable
  template <class Other1 = Ty1, class Other2 = Ty2,
    typename = typename std::enable_if<								// enable_if: 第一个参数有效的时候才有定义，否则编译错误
    std::is_default_constructible<Other1>::value &&
    std::is_default_constructible<Other2>::value, void>::type>		// is_default_constructible: 测试类型是否具有默认构造函数
    constexpr pair()												// 常量表达式是什么意思？
    : first(), second()
  {
  }

  // implicit constructiable for this type
  template <class U1 = Ty1, class U2 = Ty2,
    typename std::enable_if<
    std::is_copy_constructible<U1>::value &&
    std::is_copy_constructible<U2>::value &&
    std::is_convertible<const U1&, Ty1>::value &&
    std::is_convertible<const U2&, Ty2>::value, int>::type = 0>			// 在模板符号里面给一个typename赋值会怎么样？
    constexpr pair(const Ty1& a, const Ty2& b)
    : first(a), second(b)
  {
  }

  // explicit constructible for this type
  template <class U1 = Ty1, class U2 = Ty2,									// 逻辑：如果不能转化，这个函数被编译出来， 反之，报错
    typename std::enable_if<
    std::is_copy_constructible<U1>::value &&
    std::is_copy_constructible<U2>::value &&								// is_convertible<A, B> 判断A是否可以转化为B
    (!std::is_convertible<const U1&, Ty1>::value ||							// is_convertible是为了干什么？
     !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
    explicit constexpr pair(const Ty1& a, const Ty2& b)						// constexpr是什么意思？
    : first(a), second(b)
  {
  }

  pair(const pair& rhs) = default;
  pair(pair&& rhs) = default;				// 都设置为默认编译器都会合成吗？

  // implicit constructiable for other type
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, Other1>::value &&
    std::is_constructible<Ty2, Other2>::value &&
    std::is_convertible<Other1&&, Ty1>::value &&
    std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
    constexpr pair(Other1&& a, Other2&& b)
    : first(mystl::forward<Other1>(a)),
    second(mystl::forward<Other2>(b))
  {
  }

  // explicit constructiable for other type
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, Other1>::value &&
    std::is_constructible<Ty2, Other2>::value &&
    (!std::is_convertible<Other1, Ty1>::value ||
     !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
    explicit constexpr pair(Other1&& a, Other2&& b)
    : first(mystl::forward<Other1>(a)),
    second(mystl::forward<Other2>(b))
  {
  }

  // implicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, const Other1&>::value &&
    std::is_constructible<Ty2, const Other2&>::value &&
    std::is_convertible<const Other1&, Ty1>::value &&
    std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
    constexpr pair(const pair<Other1, Other2>& other)
    : first(other.first),
    second(other.second)
  {
  }

  // explicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, const Other1&>::value &&
    std::is_constructible<Ty2, const Other2&>::value &&
    (!std::is_convertible<const Other1&, Ty1>::value ||
     !std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
    explicit constexpr pair(const pair<Other1, Other2>& other)
    : first(other.first),
    second(other.second)
  {
  }

  // implicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, Other1>::value &&
    std::is_constructible<Ty2, Other2>::value &&
    std::is_convertible<Other1, Ty1>::value &&
    std::is_convertible<Other2, Ty2>::value, int>::type = 0>
    constexpr pair(pair<Other1, Other2>&& other)
    : first(mystl::forward<Other1>(other.first)),
    second(mystl::forward<Other2>(other.second))
  {
  }

  // explicit constructiable for other pair
  template <class Other1, class Other2,
    typename std::enable_if<
    std::is_constructible<Ty1, Other1>::value &&
    std::is_constructible<Ty2, Other2>::value &&
    (!std::is_convertible<Other1, Ty1>::value ||
     !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
    explicit constexpr pair(pair<Other1, Other2>&& other)
    : first(mystl::forward<Other1>(other.first)),
    second(mystl::forward<Other2>(other.second))
  {
  }

  // copy assign for this pair
  pair& operator=(const pair& rhs)
  {
    if (this != &rhs)
    {
      first = rhs.first;
      second = rhs.second;
    }
    return *this;
  }

  // move assign for this pair
  pair& operator=(pair&& rhs)			//这个还是和普通的一样的吧？
  {
    if (this != &rhs)
    {
      first = mystl::move(rhs.first);		// 转化为右值有什么用处吗？这个就依靠first这个类型里面实现的移动操作了
      second = mystl::move(rhs.second);
    }
    return *this;
  }

  // copy assign for other pair
  template <class Other1, class Other2>
  pair& operator=(const pair<Other1, Other2>& other)
  {
    first = other.first;
    second = other.second;
    return *this;
  }

  // move assign for other pair
  template <class Other1, class Other2>
  pair& operator=(pair<Other1, Other2>&& other)
  {
    first = mystl::forward<Other1>(other.first);
    second = mystl::forward<Other2>(other.second);
    return *this;
  }

  ~pair() = default;

  void swap(pair& other)
  {
    if (this != &other)
    {
      mystl::swap(first, other.first);
      mystl::swap(second, other.second);
    }
  }

};

// pair的比较规则： 字典序
// 重载比较操作符 
template <class Ty1, class Ty2>
bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class Ty1, class Ty2>
bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class Ty1, class Ty2>
bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return !(lhs == rhs);
}

template <class Ty1, class Ty2>
bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return rhs < lhs;
}

template <class Ty1, class Ty2>
bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return !(rhs < lhs);
}

template <class Ty1, class Ty2>
bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
{
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Ty1, class Ty2>		// 这些函数是以友元的身份存在的吗？
void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
{
  lhs.swap(rhs);
}

// 全局函数，让两个数据成为一个 pair
template <class Ty1, class Ty2>
pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
{
  return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
}

}	// endif mystl

#endif // !MYTINYSTL_UTIL_H_

