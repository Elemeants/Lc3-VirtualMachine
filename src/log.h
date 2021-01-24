#if !defined(__LOG_H__)
#define __LOG_H__

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG(format, ...) printf(" -> %s:%d " format, __FILENAME__, __LINE__, ##__VA_ARGS__);
#define LOG_LN(format, ...) printf(" -> %s:%d " format "\n", __FILENAME__, __LINE__, ##__VA_ARGS__)

#endif  // __LOG_H__
