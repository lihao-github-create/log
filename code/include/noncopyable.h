/*
 * =====================================================================================
 *
 *       Filename:  noncopyable.h
 *
 *    Description:  此类的派生类的对象不可拷贝
 *
 *        Version:  1.0
 *        Created:
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *        Company:
 *
 * =====================================================================================
 */

#ifndef COMMON_SYS_NONCOPYABLE_H_
#define COMMON_SYS_NONCOPYABLE_H_

#define NOCOPYABLE_DECLARE(typename)                                           \
public:                                                                        \
  typename(const typename &nc) = delete;                                       \
  typename &operator=(const typename &nc) = delete;

#endif // COMMON_SYS_NONCOPYABLE_H_
