#include <Arduino.h>
#include "icons.h"

const uint8_t shakeXBM[] = {
  9,	   9,
  0x70, 0x00, 0x80, 0x00, 0x30, 0x01, 0x40, 0x01, 0x45, 0x01, 0x05, 0x00,
  0x19, 0x00, 0x02, 0x00, 0x1C, 0x00, };

const uint8_t tempXBM[] = {
  5,	   9,
  0x04, 0x0A, 0x0A, 0x0A, 0x0A, 0x0E, 0x1F, 0x1F, 0x0E, };

const uint8_t voltXBM[] = {
  6,	   9,
  0x20, 0x18, 0x0C, 0x06, 0x3F, 0x18, 0x0C, 0x06, 0x01, };

const uint8_t warningXBM[] = {
  9,	   8,
  0x10, 0x00, 0x28, 0x00, 0x54, 0x00, 0x54, 0x00, 0x82, 0x00, 0x92, 0x00,
  0x01, 0x01, 0xFF, 0x01, };

const uint8_t ironXBM[] = {
  105,	   7,
  0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x07, 0x00, 0x01, 0x3E, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x1C, 0x00, 0x01, 0x3E, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x01, 0x3E, 0xF8, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x01, 0x01, 0x3E, 0xF8, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x01, 0x3E,
  0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x00,
  0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0x07, 0x00,};

const uint8_t x_markXBM[] = {
  12,	  15,
  0x03, 0x0C, 0x07, 0x0E, 0x06, 0x06, 0x0C, 0x03, 0x9C, 0x03, 0xF8, 0x01,
  0xF0, 0x00, 0x60, 0x00, 0xF0, 0x00, 0xF8, 0x01, 0x9C, 0x03, 0x0C, 0x03,
  0x06, 0x06, 0x07, 0x0E, 0x03, 0x0C };

// not truncated
