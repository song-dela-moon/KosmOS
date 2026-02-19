/**
 * @file layer.hpp
 *
 * Provides layering functionality.
 */

#pragma once

#include <memory>
#include <map>
#include <vector>

#include "graphics.hpp"
#include "frame_buffer.hpp"
#include "window.hpp"

/** @brief Layer represents a single layer.
 *
 * Currently it is designed to hold only one window,
 * but in the future it may hold multiple windows.
 */
// #@@range_begin(layer)
class Layer {
 public:
  /** @brief Creates a layer with the specified ID. */
  Layer(unsigned int id = 0);
  /** @brief Returns this instance's ID. */
  unsigned int ID() const;

  /** @brief Sets the window. The existing window is detached from this layer. */
  Layer& SetWindow(const std::shared_ptr<Window>& window);
  /** @brief Returns the currently set window. */
  std::shared_ptr<Window> GetWindow() const;
  /** @brief Returns the layer's origin coordinate. */
  Vector2D<int> GetPosition() const;

  /** @brief Updates the layer position to the specified absolute coordinates. Does not redraw. */
  Layer& Move(Vector2D<int> pos);
  /** @brief Updates the layer position by the specified relative offset. Does not redraw. */
  Layer& MoveRelative(Vector2D<int> pos_diff);

  /** @brief Draws the content of the currently set window to the given target. */
  void DrawTo(FrameBuffer& screen, const Rectangle<int>& area) const;

 private:
  unsigned int id_;
  Vector2D<int> pos_;
  std::shared_ptr<Window> window_;
};
// #@@range_end(layer)

/** @brief LayerManager manages multiple layers. */
// #@@range_begin(layer_manager)
class LayerManager {
 public:
  /** @brief Sets the render target used by Draw and related methods. */
  void SetWriter(FrameBuffer* screen);
  /** @brief Creates a new layer and returns a reference to it.
   *
   * The actual layer object is stored in an internal container of LayerManager.
   */
  Layer& NewLayer();

  /** @brief Draws all layers that are currently visible. */
  void Draw(const Rectangle<int>& area) const;
  /** @brief Redraws the window area of the specified layer. */
  void Draw(unsigned int id) const;

  /** @brief Updates the specified layer's position to the given absolute coordinates and redraws. */
  void Move(unsigned int id, Vector2D<int> new_pos);
  /** @brief Updates the specified layer's position by the given relative offset and redraws. */
  void MoveRelative(unsigned int id, Vector2D<int> pos_diff);

  /** @brief Changes the z-order (height) of a layer to the specified position.
   *
   * If `new_height` is negative, the layer becomes hidden.
   * If it is 0 or greater, that value becomes the new height.
   * If it is greater than or equal to the current number of layers, the layer becomes the front-most.
   * */
  void UpDown(unsigned int id, int new_height);
  /** @brief Hides the specified layer. */
  void Hide(unsigned int id);

  /** @brief Searches for the topmost layer at the specified coordinates. */
  Layer* FindLayerByPosition(Vector2D<int> pos, unsigned int exclude_id) const;

// #@@range_begin(layermgr_fields)
 private:
  FrameBuffer* screen_{nullptr};
  mutable FrameBuffer back_buffer_{};
// #@@range_end(layermgr_fields)
  std::vector<std::unique_ptr<Layer>> layers_{};
  std::vector<Layer*> layer_stack_{};
  unsigned int latest_id_{0};

  Layer* FindLayer(unsigned int id);
};

extern LayerManager* layer_manager;
// #@@range_end(layer_manager)
