// Utilities for unpacking files
// PackLab - CS213 - Northwestern University

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "unpack-utilities.h"


// --- public functions ---

void error_and_exit(const char* message) {
  fprintf(stderr, "%s", message);
  exit(1);
}

void* malloc_and_check(size_t size) {
  void* pointer = malloc(size);
  if (pointer == NULL) {
    error_and_exit("ERROR: malloc failed\n");
  }
  return pointer;
}

void parse_header(uint8_t* input_data, size_t input_len, packlab_config_t* config) {
  // TODO
  // Validate the header and set configurations based on it
  // Look at unpack-utilities.h to see what the fields of config are
  // Set the is_valid field of config to false if the header is invalid
  // or input_len (length of the input_data) is shorter than expected
  config->is_valid = true;
  
  if(input_data[0] != 0x02 || input_data[1] != 0x13 || input_data[2] != 0x03){
    config->is_valid = false;
    return;
  }
  config->header_len = 20;
  // set flags
  uint8_t flags = input_data[3];
  if(flags >= 128) {
    config->is_compressed = true;
    config->header_len = config->header_len + 16;
  }
  else {
    config->is_compressed = false;
  }
  flags = flags << 1;

  if (flags >= 128){
    config->is_encrypted = true;
  } 
  else {
    config->is_encrypted = false;
  }
  flags = flags << 1;
  
  if (flags >= 128){
    config->is_checksummed = true;
    config->header_len = config->header_len + 2;
  } 
  else {
    config->is_checksummed = false;
  }
  flags = flags << 1;

  if (flags >= 128){
    config->should_continue = true;
  } 
  else {
    config->should_continue = false;
  }
  flags = flags << 1;

  if (flags >= 128){
    config->should_float = true;
  } 
  else {
    config->should_float = false;
  }
  flags = flags << 1;

  if (flags >= 128){
    config->should_float3 = true;
  } 
  else {
    config->should_float3 = false;
  }
  flags = flags << 1;

  if (flags >= 128){
    config->is_valid = false;
    return;
  } 
  flags = flags << 1;

  if (flags>=128){
    config->is_valid = false;
    return;
  }

  // sum data sizes
  config->orig_data_size = 0;
  config->data_size = 0;
  for (int i = 0; i < 8; i++){
    config->orig_data_size = config->orig_data_size | ((uint64_t) input_data[i + 4] << (8 * i));
    config->data_size = config->data_size | ((uint64_t)input_data[i + 12] << (8 * i));
  }


  // pull out compression dictionary
  if (config->is_compressed == true){
    for (int i = 0; i < 16; i++){
      config->dictionary_data[i] = input_data[i + 20];
    }
  }

  // pull out the checksum value
  if (config->is_checksummed == true){
    if (config->is_compressed)
      config->checksum_value = (input_data[36] << 8) | input_data[37];
    else 
      config->checksum_value = (input_data[20] << 8) | input_data[21];
  }

  
}

uint16_t calculate_checksum(uint8_t* input_data, size_t input_len) {

  // TODO
  // Calculate a checksum over input_data
  // Return the checksum value
  uint16_t sum = 0;
  for (int i = 0; i < input_len; i++){
    sum += input_data[i];
  }

  return sum;
  

  return 0;
}

uint16_t lfsr_step(uint16_t oldstate) {
  // TODO
  // Calculate the new LFSR state given previous state
  // Return the new LFSR state
  int zero = 0;
  int six = 0; 
  int nine = 0;
  int thirteen = 0;
  if (oldstate & (1 << 0)) {
    zero = 1;
  }
  if (oldstate & (1 << 6)) {
    six = 1;
  }
  if (oldstate & (1 << 9)) {
    nine = 1;
  }
  if (oldstate & (1 << 13)) {
    thirteen = 1;
  }
  int bit = zero ^ six ^ nine ^ thirteen;
  oldstate = oldstate >> 1;
  oldstate = oldstate | (bit << 15);
  
  return oldstate;
}

void decrypt_data(uint8_t* input_data, size_t input_len,
                  uint8_t* output_data, size_t output_len,
                  uint16_t encryption_key) {
  // TODO
  // Decrypt input_data and write result to output_data
  // Uses lfsr_step() to calculate psuedorandom numbers, initialized with encryption_key
  // Step the LFSR once before encrypting data
  // Apply psuedorandom number with an XOR in little-endian order
  // Beware: input_data may be an odd number of bytes
  encryption_key = lfsr_step(encryption_key);
  uint8_t left = encryption_key >> 8;
  uint8_t right = (encryption_key << 8) >> 8;
  
  for (int i = 0; i < input_len; i+=2){ 
    output_data[i] = input_data[i] ^ right;
    if (!(i + 1 >= input_len)){
      output_data[i + 1] = input_data[i + 1] ^ left;
    }

    encryption_key = lfsr_step(encryption_key);
    left = encryption_key >> 8;
    right = (encryption_key << 8) >> 8;
  }
}

size_t decompress_data(uint8_t* input_data, size_t input_len,
                       uint8_t* output_data, size_t output_len,
                       uint8_t* dictionary_data) {

  // TODO
  // Decompress input_data and write result to output_data
  // Return the length of the decompressed data
  int j = 0;
  for (int i = 0; i < input_len - 1; i++){
    if (input_data[i] != 0x07){
      output_data[j] = input_data[i];
      j++;
    }
    else {
      if (input_data[i + 1] == 0x00) {
        output_data[j] = 0x07;
        j++;
      }
      else {
        uint8_t dictI = (input_data[i + 1] & (1 | (1 << 1) | (1 << 2) | (1 << 3)));
        for (int k = 0; k < (input_data[i + 1] >> 4); k++) {
          output_data[j] = dictionary_data[dictI];
          j++;
        }
      }
      i++;
    }
  }
  if (j < output_len) {
    output_data[j] = input_data[input_len - 1];
    j++;
  }
  return j;
}

void join_float_array(uint8_t* input_signfrac, size_t input_len_bytes_signfrac,
                      uint8_t* input_exp, size_t input_len_bytes_exp,
                      uint8_t* output_data, size_t output_len_bytes) {

  // TODO
  // Combine two streams of bytes, one with signfrac data and one with exp data,
  // into one output stream of floating point data
  // Output bytes are in little-endian order
  for (int i = 0; i < input_len_bytes_exp; i++) {
    output_data[4 * i] = input_signfrac[3 * i];
    output_data[4 * i + 1] = input_signfrac[3 * i + 1];
    int lastExpBit = 1 & input_exp[i];
    output_data[4 * i + 2] = (input_signfrac[3 * i + 2]) | (lastExpBit << 7);
    int signBit = (input_signfrac[3 * i + 2] >> 7);
    output_data[4 * i + 3] = (input_exp[i] >> 1) | (signBit << 7);
  }
  return;

}
/* End of mandatory implementation. */

/* Extra credit */
void join_float_array_three_stream(uint8_t* input_frac,
                                   size_t   input_len_bytes_frac,
                                   uint8_t* input_exp,
                                   size_t   input_len_bytes_exp,
                                   uint8_t* input_sign,
                                   size_t   input_len_bytes_sign,
                                   uint8_t* output_data,
                                   size_t   output_len_bytes) {

  // TODO
  // Combine three streams of bytes, one with frac data, one with exp data,
  // and one with sign data, into one output stream of floating point data
  // Output bytes are in little-endian order

  
}

