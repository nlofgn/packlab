// Application to test unpack utilities
// PackLab - CS213 - Northwestern University

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unpack-utilities.h"



// create a test for parse_header, using the minimal header.
int test_minimal_header_parse(void) {
  // Create input data to test with
  // If you wanted to test a header, these would be bytes of the header with
  //    meaningful bytes in appropriate places
  // If you want to test one of the other functions, they can be any bytes
  uint8_t input_data[] = {0x02, 0x13, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  // Actually run your code
  // Note that `sizeof(input_data)` actually returns the number of bytes for the
  //    array because it's a local variable (`sizeof()` generally doesn't return
  //    buffer lengths in C for arrays that are passed in as arguments)
  packlab_config_t config;
  parse_header(input_data, 20, &config);

  // Compare the results
  // This might need to be multiple comparisons or even a loop that compares many bytes
  // `memcmp()` in the C standard libary might be a useful function here!
  // Note, you don't _need_ the CHECK() functions like we used in CS211, you
  //    can just return 1 then print that there was an error
  if (config.is_valid == false){
    printf("Input data incorrectly identified as invalid.\n");
    return 1;
  }
  else if (config.is_compressed == true || config.is_encrypted == true || config.is_checksummed == true || config.should_continue == true || config.should_float == true || config.should_float3 == true){
    printf("Incorrect flag markings.\n");
    return 1;
  }
  else if (config.orig_data_size != 0x0100){
    printf("original data size incorrect, it is %li\n", config.orig_data_size);
    return 1;
  }
  else if (config.data_size != 0x0100){
    printf("new data size incorrect\n");
    return 1;
  }
  else if (config.header_len != 20){
    printf("header length incorrect\n");
    return 1;
  }
  // Test succeeded! Return 0 to signify success
  printf("**SUCCESS**\n");
  printf("test_minimal_header_parser succeeded.\n");
  return 0;
}

int test_parse_header_compression_and_float(void) {

  uint8_t input_data[] = {0x02, 0x13, 0x03, (1 << 7) | (1 << 3), 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0F,
    0x10, 0x01};
  packlab_config_t config;
  parse_header(input_data, 20, &config);

  if (!config.is_compressed || !config.should_float || config.is_checksummed || config.is_encrypted || !config.is_valid) {
    printf("error: incorrect flags for compression and float.");
    return 1;
  }
  printf("**SUCCESS**\n");
  printf("test_float_and_compression_test_successful\n");
  return 0;
}

// create a test for parse_header, using a checksum and no dict.
int test_checksum_header_parse(void) {
  // Create input data to test with
  // If you wanted to test a header, these would be bytes of the header with
  //    meaningful bytes in appropriate places
  // If you want to test one of the other functions, they can be any bytes
  uint8_t input_data[] = {0x02, 0x13, 0x03, (1 << 5), 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};

  // Actually run your code
  // Note that `sizeof(input_data)` actually returns the number of bytes for the
  //    array because it's a local variable (`sizeof()` generally doesn't return
  //    buffer lengths in C for arrays that are passed in as arguments)
  packlab_config_t config;
  parse_header(input_data, 20, &config);

  // Compare the results
  // This might need to be multiple comparisons or even a loop that compares many bytes
  // `memcmp()` in the C standard libary might be a useful function here!
  // Note, you don't _need_ the CHECK() functions like we used in CS211, you
  //    can just return 1 then print that there was an error
  if (config.is_valid == false){
    printf("Input data incorrectly identified as invalid.\n");
    return 1;
  }
  else if (config.is_compressed == true || config.is_encrypted == true || config.is_checksummed == false || config.should_continue == true || config.should_float == true || config.should_float3 == true){
    printf("Incorrect flag markings.\n");
    return 1;
  }
  else if (config.orig_data_size != 0x0100){
    printf("original data size incorrect, it is %li\n", config.orig_data_size);
    return 1;
  }
  else if (config.data_size != 0x0100){
    printf("new data size incorrect\n");
    return 1;
  }
  else if (config.header_len != 22){
    printf("header length incorrect\n");
    return 1;
  }
  else if (config.checksum_value != (1 << 8)){
    printf("checksum value incorrect, value is %i\n", config.checksum_value);
    return 1;
  }
  // Test succeeded! Return 0 to signify success
  printf("**SUCCESS**\n");
  printf("test_checksum_header_parser succeeded.\n");
  return 0;
}

// create a test for parse_header, using the minimal header.
int test_compression_header_parse(void) {
  // Create input data to test with
  // If you wanted to test a header, these would be bytes of the header with
  //    meaningful bytes in appropriate places
  // If you want to test one of the other functions, they can be any bytes
  uint8_t input_data[] = {0x02, 0x13, 0x03, (1 << 7), 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x11, 0xF0,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x10};

  // Actually run your code
  // Note that `sizeof(input_data)` actually returns the number of bytes for the
  //    array because it's a local variable (`sizeof()` generally doesn't return
  //    buffer lengths in C for arrays that are passed in as arguments)
  packlab_config_t config;
  parse_header(input_data, 36, &config);

  // Compare the results
  // This might need to be multiple comparisons or even a loop that compares many bytes
  // `memcmp()` in the C standard libary might be a useful function here!
  // Note, you don't _need_ the CHECK() functions like we used in CS211, you
  //    can just return 1 then print that there was an error
  if (config.is_valid == false){
    printf("Input data incorrectly identified as invalid.\n");
    return 1;
  }
  else if (config.is_compressed == false || config.is_encrypted == true || config.is_checksummed == true || config.should_continue == true || config.should_float == true || config.should_float3 == true){
    printf("Incorrect flag markings.\n");
    return 1;
  }
  else if (config.orig_data_size != 0x0100){
    printf("original data size incorrect, it is %li\n", config.orig_data_size);
    return 1;
  }
  else if (config.data_size != 0x0100){
    printf("new data size incorrect\n");
    return 1;
  }
  else if (config.header_len != 36){
    printf("header length incorrect\n");
    return 1;
  }
  else if (config.dictionary_data[3] != 0xF0 || config.dictionary_data[0] != 0x00 || config.dictionary_data[15] != 0x10){
    printf("Dictonary values incorrect\n");
    return 1;
  }
  // Test succeeded! Return 0 to signify success
  printf("**SUCCESS**\n");
  printf("test_compression_header_parser succeeded.\n");
  return 0;
}


// create a test for parse_header, using the compression and checksum header.
int test_compression_and_checksum_header_parse(void) {
  // Create input data to test with
  // If you wanted to test a header, these would be bytes of the header with
  //    meaningful bytes in appropriate places
  // If you want to test one of the other functions, they can be any bytes
  uint8_t input_data[] = {0x02, 0x13, 0x03, (1 << 7) | (1 << 5), 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x01, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0F,
  0x10, 0x01};

  // Actually run your code
  // Note that `sizeof(input_data)` actually returns the number of bytes for the
  //    array because it's a local variable (`sizeof()` generally doesn't return
  //    buffer lengths in C for arrays that are passed in as arguments)
  packlab_config_t config;
  parse_header(input_data, 38, &config);

  // Compare the results
  // This might need to be multiple comparisons or even a loop that compares many bytes
  // `memcmp()` in the C standard libary might be a useful function here!
  // Note, you don't _need_ the CHECK() functions like we used in CS211, you
  //    can just return 1 then print that there was an error
  if (config.is_valid == false){
    printf("Input data incorrectly identified as invalid.\n");
    return 1;
  }
  else if (config.is_compressed == false || config.is_encrypted == true || config.is_checksummed == false || config.should_continue == true || config.should_float == true || config.should_float3 == true){
    printf("Incorrect flag markings.\n");
    return 1;
  }
  else if (config.orig_data_size != 0x0100){
    printf("original data size incorrect, it is %li\n", config.orig_data_size);
    return 1;
  }
  else if (config.data_size != 0x0100){
    printf("new data size incorrect\n");
    return 1;
  }
  else if (config.header_len != 38){
    printf("header length incorrect\n");
    return 1;
  }
  else if (config.dictionary_data[0] != 0x01 || config.dictionary_data[15] != 0x0F){
    printf("dictionary data incorrect\n");
    return 1;
  }
  else if(config.checksum_value != (1 << 12) + 1){
    printf("checksum incorrect\n");
    return 1;
  }
  // Test succeeded! Return 0 to signify success
  printf("**SUCCESS**\n");
  printf("test_compressed_and_checksummed_header_parser succeeded.\n");
  return 0;
}

int test_lfsr_step(void) {
  // A properly created LFSR should do two things
  //  1. It should generate specific new state based on a known initial state
  //  2. It should iterate through all 2^16 integers, once each (except 0)

  // Create an array to track if the LFSR hit each integer (except 0)
  // 2^16 (65536) possibilities
  bool* lfsr_states = malloc_and_check(65536);
  memset(lfsr_states, 0, 65536);

  // Initial 16 LFSR states
  uint16_t correct_lfsr_states[16] = {
    0x1337, 0x099B, 0x84CD, 0x4266,
    0x2133, 0x1099, 0x884C, 0xC426,
    0x6213, 0xB109, 0x5884, 0x2C42,
    0x1621, 0x0B10, 0x8588, 0x42C4
  };

  // Step the LFSR until a state repeats
  bool repeat        = false;
  size_t steps       = 0;
  uint16_t new_state = 0x1337; // known initial state
  while (!repeat) {

    // Iterate LFSR
    steps++;
    new_state = lfsr_step(new_state);

    // Check if this state has already been reached
    repeat = lfsr_states[new_state];
    lfsr_states[new_state] = true;

    // Check first 16 LFSR steps
    if (steps < 16) {
      if (new_state != correct_lfsr_states[steps]) {
        printf("ERROR: at step %lu, expected state 0x%04X but received state 0x%04X\n",
            steps, correct_lfsr_states[steps], new_state);
        free(lfsr_states);
        return 1;
      }
    }
  }

  // Check that all integers were hit. Should take 2^16 (65536) steps (2^16-1 integers, plus a repeat)
  if (steps != 65536) {
    printf("ERROR: expected %d iterations before a repeat, but ended after %lu steps\n", 65536, steps);
    free(lfsr_states);
    return 1;
  }

  // Cleanup
  free(lfsr_states);
  return 0;
}

// Here's an example testcase
// It's written for the `calculate_checksum()` function, but the same ideas
//  would work for any function you want to test
// Feel free to copy it and adapt it to create your own tests
int example_test(void) {
  // Create input data to test with
  // If you wanted to test a header, these would be bytes of the header with
  //    meaningful bytes in appropriate places
  // If you want to test one of the other functions, they can be any bytes
  uint8_t input_data[] = {0x01, 0x03, 0x04, };

  // Create an "expected" result to compare against
  // If you're testing header parsing, you will likely need one of these for
  //    each config field. If you're testing decryption or decompression, this
  //    should be an array of expected output_data bytes
  uint16_t expected_checksum_value = 0x0008;

  // Actually run your code
  // Note that `sizeof(input_data)` actually returns the number of bytes for the
  //    array because it's a local variable (`sizeof()` generally doesn't return
  //    buffer lengths in C for arrays that are passed in as arguments)
  uint16_t calculated_checksum_value = calculate_checksum(input_data, sizeof(input_data));

  // Compare the results
  // This might need to be multiple comparisons or even a loop that compares many bytes
  // `memcmp()` in the C standard libary might be a useful function here!
  // Note, you don't _need_ the CHECK() functions like we used in CS211, you
  //    can just return 1 then print that there was an error
  if (calculated_checksum_value != expected_checksum_value) {
    // Test failed! Return 1 to signify failure
    return 1;
  }

  // Test succeeded! Return 0 to signify success
  return 0;
}

int test_compression(void) {
  uint8_t input[] = {0x01, 0x07, 0x42};
  uint8_t inputDict[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};
  size_t input_len = 3;
  size_t output_len = 5;
  uint8_t output[5];

  decompress_data(input, input_len, output, output_len, inputDict);
  if (output[0] != 0x01 || output[1] != 0x32 || output[2] != 0x32 || output[3] != 0x32 || output[4] != 0x32){
    printf("incorrect values in output\n");
    for (int i = 0; i < output_len; i++){
    }
    return 1;
  }
  printf("encryption test successful!\n");
  return 0;
}

int test_float(void) {
  uint8_t input_signfrac[] = {0x00, 0x00, 0x16};
  uint8_t iput_exp[] = {0x87};
  uint8_t output_data[4];

  join_float_array(input_signfrac, 3, iput_exp, 1, output_data, 4);
  return 0;
}

int main(void) {
  
  // Test the LFSR implementation
  int result = test_lfsr_step();
  if (result != 0) {
    printf("Error when testing LFSR implementation\n");
    return 1;
  }
  
  // TODO - add tests here for other functionality
  // You can craft arbitrary array data as inputs to the functions
  // Parsing headers, checksumming, decryption, and decompressing are all testable
  result = test_minimal_header_parse();

  result = test_parse_header_compression_and_float();
  
  result = test_checksum_header_parse();

  result = test_compression_header_parse();

  result = test_compression_and_checksum_header_parse();

  result = test_compression();

  result = test_float();
  // Here's an example test
  // Note that it's going to fail until you implement the `calculate_checksum()` function
  result = example_test();
  if (result != 0) {
    // Make sure to print the name of which test failed, so you can go find it and figure out why
    printf("ERROR: example_test_setup failed\n");
    return 1;
  }

  printf("All tests passed successfully!\n");
  return 0;
}

