/**
 * @file window.hpp
 *
 * Provides the Window class that represents a drawable region.
 */

#pragma once

#include <vector>
#include <optional>
#include <string>
#include "graphics.hpp"
#include "frame_buffer.hpp"

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
    virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
      window_.Write(pos, c);
    }
    /** @brief Returns the width of the associated Window in pixels. */
    virtual int Width() const override { return window_.Width(); }
    /** @brief Returns the height of the associated Window in pixels. */
    virtual int Height() const override { return window_.Height(); }

   private:
    Window& window_;
  };
  // #@@range_end(windowwriter)

  /** @brief Create a flat drawing area with the specified number of pixels. */
  Window(int width, int height, PixelFormat shadow_format);
  virtual ~Window() = default;
  Window(const Window& rhs) = delete;
  Window& operator=(const Window& rhs) = delete;

  /** @brief Draws this window's contents onto the given FrameBuffer.
   *
   * @param dst       Destination to draw to.
   * @param position  Draw position relative to the top-left of `dst`.
   */
  void DrawTo(FrameBuffer& dst, Vector2D<int> pos, const Rectangle<int>& area);
  /** @brief Sets the transparent color. */
  void SetTransparentColor(std::optional<PixelColor> c);
  /** @brief Returns the WindowWriter associated with this instance. */
  WindowWriter* Writer();

  /** @brief Returns the pixel at the specified position. */
  const PixelColor& At(Vector2D<int> pos) const;
  /** @brief Writes a pixel at the specified position. */
  void Write(Vector2D<int> pos, PixelColor c);

  /** @brief Returns the width of the drawing region in pixels. */
  int Width() const;
  /** @brief Returns the height of the drawing region in pixels. */
  int Height() const;
  /** @brief Returns the size of the drawing region in pixels. */
  Vector2D<int> Size() const;

  /** @brief Moves a rectangular region within this window's drawing area.
   *
   * @param dst_pos  Origin of the destination position.
   * @param src      Source rectangle (position and size).
   */
  void Move(Vector2D<int> dst_pos, const Rectangle<int>& src);

  virtual void Activate() {}
  virtual void Deactivate() {}

 private:
  int width_, height_;
  std::vector<std::vector<PixelColor>> data_{};
  WindowWriter writer_{*this};
  std::optional<PixelColor> transparent_color_{std::nullopt};
  FrameBuffer shadow_buffer_{};
};
// #@@range_end(window)

// #@@range_begin(window_consts)
class ToplevelWindow : public Window {
 public:
  static constexpr Vector2D<int> kTopLeftMargin{4, 24};
  static constexpr Vector2D<int> kBottomRightMargin{4, 4};
  static constexpr int kMarginX = kTopLeftMargin.x + kBottomRightMargin.x;
  static constexpr int kMarginY = kTopLeftMargin.y + kBottomRightMargin.y;
// #@@range_end(window_consts)

  class InnerAreaWriter : public PixelWriter {
   public:
    InnerAreaWriter(ToplevelWindow& window) : window_{window} {}
    virtual void Write(Vector2D<int> pos, const PixelColor& c) override {
      window_.Write(pos + kTopLeftMargin, c);
    }
    virtual int Width() const override {
      return window_.Width() - kTopLeftMargin.x - kBottomRightMargin.x; }
    virtual int Height() const override {
      return window_.Height() - kTopLeftMargin.y - kBottomRightMargin.y; }

   private:
    ToplevelWindow& window_;
  };

  ToplevelWindow(int width, int height, PixelFormat shadow_format,
                 const std::string& title);

  virtual void Activate() override;
  virtual void Deactivate() override;

  InnerAreaWriter* InnerWriter() { return &inner_writer_; }
  Vector2D<int> InnerSize() const;

 private:
  std::string title_;
  InnerAreaWriter inner_writer_{*this};
};

void DrawWindow(PixelWriter& writer, const char* title);
void DrawTextbox(PixelWriter& writer, Vector2D<int> pos, Vector2D<int> size);
void DrawTerminal(PixelWriter& writer, Vector2D<int> pos, Vector2D<int> size);
void DrawWindowTitle(PixelWriter& writer, const char* title, bool active);
