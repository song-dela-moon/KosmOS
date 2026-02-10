/**
 * @file window.hpp
 *
 * Provides the Window class that represents a drawable region.
 */

#pragma once

#include <vector>
#include <optional>
#include "graphics.hpp"

/** @brief The Window class represents a graphical drawing region.
 *
 * It is used not only for windows with titles and menus, but also for regions such as the mouse cursor.
 */
// #@@range_begin(window)
class Window {
 public:
  /** @brief WindowWriter provides a PixelWriter associated with a Window.
   */
  // #@@range_begin(windowwriter)
  class WindowWriter : public PixelWriter {
   public:
    WindowWriter(Window& window) : window_{window} {}
    /** @brief Draws the specified color at the given position. */
    virtual void Write(int x, int y, const PixelColor& c) override {
      window_.At(x, y) = c;
    }
    /** @brief Returns the width of the associated Window in pixels. */
    virtual int Width() const override { return window_.Width(); }
    /** @brief Returns the height of the associated Window in pixels. */
    virtual int Height() const override { return window_.Height(); }

   private:
    Window& window_;
  };
  // #@@range_end(windowwriter)

  /** @brief Creates a 2D drawing region of the specified size in pixels. */
  Window(int width, int height);
  ~Window() = default;
  Window(const Window& rhs) = delete;
  Window& operator=(const Window& rhs) = delete;

  /** @brief Draws this window's contents onto the given PixelWriter.
   *
   * @param writer   Destination to draw to.
   * @param position Draw position relative to the top-left of `writer`.
   */
  void DrawTo(PixelWriter& writer, Vector2D<int> position);
  /** @brief Sets the transparent color. */
  void SetTransparentColor(std::optional<PixelColor> c);
  /** @brief Returns the WindowWriter associated with this instance. */
  WindowWriter* Writer();

  /** @brief Returns the pixel at the specified position. */
  PixelColor& At(int x, int y);
  /** @brief Returns the pixel at the specified position (const overload). */
  const PixelColor& At(int x, int y) const;

  /** @brief Returns the width of the drawing region in pixels. */
  int Width() const;
  /** @brief Returns the height of the drawing region in pixels. */
  int Height() const;

 private:
  int width_, height_;
  std::vector<std::vector<PixelColor>> data_{};
  WindowWriter writer_{*this};
  std::optional<PixelColor> transparent_color_{std::nullopt};
};
// #@@range_end(window)
