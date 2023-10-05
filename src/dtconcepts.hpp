#ifndef __DSO_DATETIME_CONCEPTS20_HPP__
#define __DSO_DATETIME_CONCEPTS20_HPP__

#if __cplusplus >= 202002L /* concepts only available in C++20 */
namespace dso {
namespace gconcepts {

/** A concept to denote arithmetic types (integral or floating-point) */
template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

/** Concept has_const_ref_dt -> Make sure that the class has a member function
 * __member_const_ref__()
 */
template <typename DType>
concept has_const_ref_dt = requires(DType t) { t.__member_const_ref__(); };

/** Concept has_const_ref_dt -> Make sure that the class has a member function
 * __member_ref__()
 */
template <typename DType>
concept has_ref_dt = requires(DType t) { t.__member_ref__(); };

/** Concept is_fundamental_dt -> Make sure that the class has a static member
 * variable called DType::is_dt_fundamental_type and it is set to true
 */
template <typename DType>
concept is_fundamental_dt =
    requires(DType) { DType::is_dt_fundamental_type == true; };

/** Concept is_sec_dt -> Make sure that the class has a static member variable
 * called DType::is_of_sec_type and it is set to true
 */
template <typename DType>
concept is_sec_dt = requires(DType) { DType::is_of_sec_type == true; };

/** Concept is_fundamental_and_has_const_ref
 * Class is fundamental (i.e. has is_dt_fundamental_type==true) and we can
 * access its member (i.e. has __member_const_ref__() method)
 */
template <typename DType>
concept is_fundamental_and_has_const_ref =
    is_fundamental_dt<DType> && has_const_ref_dt<DType>;

/** Concept is_fundamental_and_has_ref
 * Class is fundamental (i.e. has is_dt_fundamental_type==true) and we can
 * access its member (i.e. has __member_ref__() method)
 */
template <typename DType>
concept is_fundamental_and_has_ref =
    is_fundamental_dt<DType> && has_ref_dt<DType>;

} /* namespace gconcepts */

} /* namespace dso */

#endif
#endif
