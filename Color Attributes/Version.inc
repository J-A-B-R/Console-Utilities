#define VER_MAJOR_NUMBER 1
#define VER_MINOR_NUMBER 0
#define VER_DELTA_NUMBER 1
#define VER_BUILD_NUMBER 8

#define STR(value) #value
#define STRINGIZE(value) STR(value)
#define VER_FULLVERSION STRINGIZE(VER_MAJOR_NUMBER.VER_MINOR_NUMBER.VER_DELTA_NUMBER.VER_BUILD_NUMBER)
