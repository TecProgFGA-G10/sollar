/*
* Defines the function to implement the log on the system.
* To use the function simply use the function print_log the same way as the printf function.
*
*/
#define print_error_log(...) print_error_log(__FILE__, __LINE__, __VA_ARGS__ )
#define print_debug_log(...) print_debug_log(__FILE__, __LINE__, __VA_ARGS__ )
