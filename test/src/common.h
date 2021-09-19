#ifndef TEST_COMMON
#define TEST_COMMON

static inline void test_assert_illuminatir_error( illuminatir_error_t expected, illuminatir_error_t actual, unsigned line )
{
	UNITY_TEST_ASSERT_EQUAL_UINT( expected, actual, line, illuminatir_error_toString(actual) );
}

#define TEST_ASSERT_ILLUMINATIR_ERROR(expected,actual) test_assert_illuminatir_error((expected), (actual), __LINE__)

#endif
