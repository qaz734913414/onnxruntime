// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "test/providers/provider_test_utils.h"
#include "core/util/math.h"

namespace onnxruntime {
namespace test {

template <typename T>
void RunReverseTest(const std::initializer_list<T>& input_vals,
                    const std::vector<int64_t>& input_dimensions,
                    const std::vector<int64_t>& reverse_axes,
                    const std::initializer_list<T>& expected_vals,
                    const std::vector<int64_t>& expected_dimensions,
                    OpTester::ExpectResult expect_result = OpTester::ExpectResult::kExpectSuccess,
                    const std::string& expected_err_str = "") {
  OpTester test("Reverse", 1, onnxruntime::kMSDomain);

  // Attribute
  if (reverse_axes.size() != 0)
	test.AddAttribute("axes", reverse_axes);

  // Input
  test.AddInput<T>("input", input_dimensions, input_vals);

  // Output
  test.AddOutput<T>("output", expected_dimensions, expected_vals);

  // Run test and check results
  test.Run(expect_result, expected_err_str);
}

template <typename T>
void RunReverseTestWrapper() {
  // Test 1 (scalar)
  std::initializer_list<T> input_vals_1 = {1};
  std::vector<int64_t> input_dimensions_1;
  std::vector<int64_t> reverse_axes_1;
  std::initializer_list<T> expected_vals_1 = {1};
  std::vector<int64_t> expected_dimensions_1;
  RunReverseTest<T>(input_vals_1, input_dimensions_1, reverse_axes_1, expected_vals_1, expected_dimensions_1);

  // Test 2 (default axes)
  std::initializer_list<T> input_vals_2 = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int64_t> input_dimensions_2 = {2, 4};
  std::vector<int64_t> reverse_axes_2;
  std::initializer_list<T> expected_vals_2 = {8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<int64_t> expected_dimensions_2 = {2, 4};
  RunReverseTest<T>(input_vals_2, input_dimensions_2, reverse_axes_2, expected_vals_2, expected_dimensions_2);

  // Test 3 (explicit axes)
  std::initializer_list<T> input_vals_3 = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int64_t> input_dimensions_3 = {2, 4};
  std::vector<int64_t> reverse_axes_3 = {0, 1};
  std::initializer_list<T> expected_vals_3 = {8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<int64_t> expected_dimensions_3 = {2, 4};
  RunReverseTest<T>(input_vals_3, input_dimensions_3, reverse_axes_3, expected_vals_3, expected_dimensions_3);

  // Test 4 (explicit axes with negative axis)
  std::initializer_list<T> input_vals_4 = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int64_t> input_dimensions_4 = {2, 4};
  std::vector<int64_t> reverse_axes_4 = {-1, 0};
  std::initializer_list<T> expected_vals_4 = {8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<int64_t> expected_dimensions_4 = {2, 4};
  RunReverseTest<T>(input_vals_4, input_dimensions_4, reverse_axes_4, expected_vals_4, expected_dimensions_4);

  // Test 5 (explicit axes with explicit duplicate values)
  std::initializer_list<T> input_vals_5 = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int64_t> input_dimensions_5 = {2, 4};
  std::vector<int64_t> reverse_axes_5 = {0, 0};
  std::initializer_list<T> expected_vals_5 = {8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<int64_t> expected_dimensions_5 = {2, 4};
  RunReverseTest<T>(input_vals_5, input_dimensions_5, reverse_axes_5, expected_vals_5, expected_dimensions_5, 
  OpTester::ExpectResult::kExpectFailure, "axes attribute has duplicates in Reverse operator");

  // Test 6 (explicit axes with implicit duplicate values)
  std::initializer_list<T> input_vals_6 = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int64_t> input_dimensions_6 = {2, 4};
  std::vector<int64_t> reverse_axes_6 = {-1, 1};
  std::initializer_list<T> expected_vals_6 = {8, 7, 6, 5, 4, 3, 2, 1};
  std::vector<int64_t> expected_dimensions_6 = {2, 4};
  RunReverseTest<T>(input_vals_6, input_dimensions_6, reverse_axes_6, expected_vals_6, expected_dimensions_6, 
  OpTester::ExpectResult::kExpectFailure, "axes attribute has duplicates in Reverse operator");

  // Test 7 (subset of axes)
  std::initializer_list<T> input_vals_7 = {1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int64_t> input_dimensions_7 = {2, 4};
  std::vector<int64_t> reverse_axes_7 = {0};
  std::initializer_list<T> expected_vals_7 = {5, 6, 7, 8, 1, 2, 3, 4};
  std::vector<int64_t> expected_dimensions_7 = {2, 4};
  RunReverseTest<T>(input_vals_7, input_dimensions_7, reverse_axes_7, expected_vals_7, expected_dimensions_7);
}

template <>
void RunReverseTestWrapper<std::string>() {
  // Test 1 (scalar)
  std::initializer_list<std::string> input_vals_1 = {"a"};
  std::vector<int64_t> input_dimensions_1;
  std::vector<int64_t> reverse_axes_1;
  std::initializer_list<std::string> expected_vals_1 = {"a"};
  std::vector<int64_t> expected_dimensions_1;
  RunReverseTest<std::string>(input_vals_1, input_dimensions_1, reverse_axes_1, expected_vals_1, expected_dimensions_1);

  // Test 2 (default axes)
  std::initializer_list<std::string> input_vals_2 = {"a", "b", "c", "d", "e", "f", "g", "h"};
  std::vector<int64_t> input_dimensions_2 = {2, 4};
  std::vector<int64_t> reverse_axes_2;
  std::initializer_list<std::string> expected_vals_2 = {"h", "g", "f", "e", "d", "c", "b", "a"};
  std::vector<int64_t> expected_dimensions_2 = {2, 4};
  RunReverseTest<std::string>(input_vals_2, input_dimensions_2, reverse_axes_2, expected_vals_2, expected_dimensions_2);

  // Test 3 (explicit axes)
  std::initializer_list<std::string> input_vals_3 = {"a", "b", "c", "d", "e", "f", "g", "h"};
  std::vector<int64_t> input_dimensions_3 = {2, 4};
  std::vector<int64_t> reverse_axes_3 = {0, 1};
  std::initializer_list<std::string> expected_vals_3 = {"h", "g", "f", "e", "d", "c", "b", "a"};
  std::vector<int64_t> expected_dimensions_3 = {2, 4};
  RunReverseTest<std::string>(input_vals_3, input_dimensions_3, reverse_axes_3, expected_vals_3, expected_dimensions_3);

  // Test 4 (explicit axes with negative axis)
  std::initializer_list<std::string> input_vals_4 = {"a", "b", "c", "d", "e", "f", "g", "h"};
  std::vector<int64_t> input_dimensions_4 = {2, 4};
  std::vector<int64_t> reverse_axes_4 = {-1, 0};
  std::initializer_list<std::string> expected_vals_4 = {"h", "g", "f", "e", "d", "c", "b", "a"};
  std::vector<int64_t> expected_dimensions_4 = {2, 4};
  RunReverseTest<std::string>(input_vals_4, input_dimensions_4, reverse_axes_4, expected_vals_4, expected_dimensions_4);
}

template <>
void RunReverseTestWrapper<bool>() {
  // Test 1 (scalar)
  std::initializer_list<bool> input_vals_1 = {true};
  std::vector<int64_t> input_dimensions_1;
  std::vector<int64_t> reverse_axes_1;
  std::initializer_list<bool> expected_vals_1 = {true};
  std::vector<int64_t> expected_dimensions_1;
  RunReverseTest<bool>(input_vals_1, input_dimensions_1, reverse_axes_1, expected_vals_1, expected_dimensions_1);

  // Test 2 (default axes)
  std::initializer_list<bool> input_vals_2 = {true, true, false, false, true, true, false, false};
  std::vector<int64_t> input_dimensions_2 = {2, 4};
  std::vector<int64_t> reverse_axes_2;
  std::initializer_list<bool> expected_vals_2 = {false, false, true, true, false, false, true, true};
  std::vector<int64_t> expected_dimensions_2 = {2, 4};
  RunReverseTest<bool>(input_vals_2, input_dimensions_2, reverse_axes_2, expected_vals_2, expected_dimensions_2);

  // Test 3 (explicit axes)
  std::initializer_list<bool> input_vals_3 = {true, true, false, false, true, true, false, false};
  std::vector<int64_t> input_dimensions_3 = {2, 4};
  std::vector<int64_t> reverse_axes_3 = {0, 1};
  std::initializer_list<bool> expected_vals_3 = {false, false, true, true, false, false, true, true};
  std::vector<int64_t> expected_dimensions_3 = {2, 4};
  RunReverseTest<bool>(input_vals_3, input_dimensions_3, reverse_axes_3, expected_vals_3, expected_dimensions_3);

  // Test 4 (explicit axes with negative axis)
  std::initializer_list<bool> input_vals_4 = {true, true, false, false, true, true, false, false};
  std::vector<int64_t> input_dimensions_4 = {2, 4};
  std::vector<int64_t> reverse_axes_4 = {-1, 0};
  std::initializer_list<bool> expected_vals_4 = {false, false, true, true, false, false, true, true};
  std::vector<int64_t> expected_dimensions_4 = {2, 4};
  RunReverseTest<bool>(input_vals_4, input_dimensions_4, reverse_axes_4, expected_vals_4, expected_dimensions_4);
}

const MLFloat16 ConvertFloatToMLFloat16(const float& f) {
  return MLFloat16(math::floatToHalf(f));
}

template <>
void RunReverseTestWrapper<MLFloat16>() {
  // Test 1 (scalar)
  std::initializer_list<MLFloat16> input_vals_1 = {ConvertFloatToMLFloat16(1)};
  std::vector<int64_t> input_dimensions_1;
  std::vector<int64_t> reverse_axes_1;
  std::initializer_list<MLFloat16> expected_vals_1 = {ConvertFloatToMLFloat16(1)};
  std::vector<int64_t> expected_dimensions_1;
  RunReverseTest<MLFloat16>(input_vals_1, input_dimensions_1, reverse_axes_1, expected_vals_1, expected_dimensions_1);

  // Test 2 (default axes)
  std::initializer_list<MLFloat16> input_vals_2 = {ConvertFloatToMLFloat16(1), ConvertFloatToMLFloat16(2), 
  ConvertFloatToMLFloat16(3), ConvertFloatToMLFloat16(4), ConvertFloatToMLFloat16(5), 
  ConvertFloatToMLFloat16(6), ConvertFloatToMLFloat16(7), ConvertFloatToMLFloat16(8)};
  std::vector<int64_t> input_dimensions_2 = {2, 4};
  std::vector<int64_t> reverse_axes_2;
  std::initializer_list<MLFloat16> expected_vals_2 = {ConvertFloatToMLFloat16(8), ConvertFloatToMLFloat16(7), 
  ConvertFloatToMLFloat16(6), ConvertFloatToMLFloat16(5), ConvertFloatToMLFloat16(4), 
  ConvertFloatToMLFloat16(3), ConvertFloatToMLFloat16(2), ConvertFloatToMLFloat16(1)};
  std::vector<int64_t> expected_dimensions_2 = {2, 4};
  RunReverseTest<MLFloat16>(input_vals_2, input_dimensions_2, reverse_axes_2, expected_vals_2, expected_dimensions_2);

  // Test 3 (explicit axes)
  std::initializer_list<MLFloat16> input_vals_3 = {ConvertFloatToMLFloat16(1), ConvertFloatToMLFloat16(2), 
  ConvertFloatToMLFloat16(3), ConvertFloatToMLFloat16(4), ConvertFloatToMLFloat16(5), 
  ConvertFloatToMLFloat16(6), ConvertFloatToMLFloat16(7), ConvertFloatToMLFloat16(8)};
  std::vector<int64_t> input_dimensions_3 = {2, 4};
  std::vector<int64_t> reverse_axes_3 = {0, 1};
  std::initializer_list<MLFloat16> expected_vals_3 = {ConvertFloatToMLFloat16(8), ConvertFloatToMLFloat16(7), 
  ConvertFloatToMLFloat16(6), ConvertFloatToMLFloat16(5), ConvertFloatToMLFloat16(4), 
  ConvertFloatToMLFloat16(3), ConvertFloatToMLFloat16(2), ConvertFloatToMLFloat16(1)};
  std::vector<int64_t> expected_dimensions_3 = {2, 4};
  RunReverseTest<MLFloat16>(input_vals_3, input_dimensions_3, reverse_axes_3, expected_vals_3, expected_dimensions_3);

  // Test 4 (explicit axes with negative axis)
  std::initializer_list<MLFloat16> input_vals_4 = {ConvertFloatToMLFloat16(1), ConvertFloatToMLFloat16(2), 
  ConvertFloatToMLFloat16(3), ConvertFloatToMLFloat16(4), ConvertFloatToMLFloat16(5), 
  ConvertFloatToMLFloat16(6), ConvertFloatToMLFloat16(7), ConvertFloatToMLFloat16(8)};
  std::vector<int64_t> input_dimensions_4 = {2, 4};
  std::vector<int64_t> reverse_axes_4 = {-1, 0};
  std::initializer_list<MLFloat16> expected_vals_4 = {ConvertFloatToMLFloat16(8), ConvertFloatToMLFloat16(7), 
  ConvertFloatToMLFloat16(6), ConvertFloatToMLFloat16(5), ConvertFloatToMLFloat16(4), 
  ConvertFloatToMLFloat16(3), ConvertFloatToMLFloat16(2), ConvertFloatToMLFloat16(1)};
  std::vector<int64_t> expected_dimensions_4 = {2, 4};
  RunReverseTest<MLFloat16>(input_vals_4, input_dimensions_4, reverse_axes_4, expected_vals_4, expected_dimensions_4);
}

TEST(ReverseOperator, FloatType) {
  RunReverseTestWrapper<float>();
}

TEST(ReverseOperator, DoubleType) {
  RunReverseTestWrapper<double>();
}

TEST(ReverseOperator, Uint8Type) {
  RunReverseTestWrapper<uint8_t>();
}

TEST(ReverseOperator, Int8Type) {
  RunReverseTestWrapper<int8_t>();
}

TEST(ReverseOperator, Uint16Type) {
  RunReverseTestWrapper<uint16_t>();
}

TEST(ReverseOperator, Int16Type) {
  RunReverseTestWrapper<int16_t>();
}

TEST(ReverseOperator, Uint32Type) {
  RunReverseTestWrapper<uint32_t>();
}

TEST(ReverseOperator, Int32Type) {
  RunReverseTestWrapper<int32_t>();
}

TEST(ReverseOperator, Uint64Type) {
  RunReverseTestWrapper<uint64_t>();
}

TEST(ReverseOperator, Int64Type) {
  RunReverseTestWrapper<int64_t>();
}

TEST(ReverseOperator, StringType) {
  RunReverseTestWrapper<std::string>();
}

TEST(ReverseOperator, BoolType) {
  RunReverseTestWrapper<bool>();
}

TEST(ReverseOperator, MLFLoat16Type) {
  RunReverseTestWrapper<MLFloat16>();
}
}  // namespace test
}  // namespace onnxruntime