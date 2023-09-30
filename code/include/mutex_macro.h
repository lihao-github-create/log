#ifndef __MUTEX_MACRO_H__
#define __MUTEX_MACRO_H__

#define GUARDED_BY(x) __attribute__((guarded_by(x)))

#endif