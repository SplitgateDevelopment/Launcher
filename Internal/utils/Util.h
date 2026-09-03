#pragma once
#include <cstdint>
#include <Windows.h>

/**
 * @file
 * @brief Byte-pattern (AOB signature) scanning helpers used to locate code/data in a module.
 */

/**
 * Compares a byte buffer against a signature.
 * @param data buffer to test.
 * @param sig  signature bytes.
 * @param size number of bytes to compare.
 * @return true if @p data matches @p sig for @p size bytes.
 */
bool Compare(byte* data, byte* sig, uint64_t size);

/**
 * Scans [start, end) for the first occurrence of a signature.
 * @return pointer to the match, or nullptr if not found.
 */
byte* FindSignature(byte* start, byte* end, byte* sig, uint64_t size);

/**
 * Finds a signature within a module and applies an offset to the match.
 * @param mod      module to scan.
 * @param sig      signature bytes.
 * @param size     signature length.
 * @param addition byte offset added to the match address before returning.
 * @return the resolved pointer, or nullptr if the signature wasn't found.
 */
void* FindPointer(HMODULE mod, byte* sig, uint64_t size, int addition);