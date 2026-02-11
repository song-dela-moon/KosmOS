/**
 * @file font.cpp
 *
 * A collection of font drawing programs.
 */

#include "font.hpp"

// #@@range_begin(font_text_bin)
extern const uint8_t _binary_font_text_bin_start;
extern const uint8_t _binary_font_text_bin_end;
extern const uint8_t _binary_font_text_bin_size;

const uint8_t* GetFont(char c) {
  auto index = 16 * static_cast<unsigned int>(c);
  if (index >= reinterpret_cast<uintptr_t>(&_binary_font_text_bin_size)) {
    return nullptr;
  }
  return &_binary_font_text_bin_start + index;
}
// #@@range_end(font_text_bin)

void WriteAscii(PixelWriter& writer, Vector2D<int> pos, char c, const PixelColor& color) {
  const uint8_t* font = GetFont(c);
  if (font == nullptr) {
    return;
  }
  for (int dy = 0; dy < 16; ++dy) {
    for (int dx = 0; dx < 8; ++dx) {
      if ((font[dy] << dx) & 0x80u) {
        writer.Write(pos + Vector2D<int>{dx, dy}, color);
      }
    }
  }
}

// #@@range_begin(write_string)
void WriteString(PixelWriter& writer, Vector2D<int> pos, const char* s, const PixelColor& color) {
  for (int i = 0; s[i] != '\0'; ++i) {
    WriteAscii(writer, pos + Vector2D<int>{8 * i, 0}, s[i], color);
  }
}
// #@@range_end(write_string)
