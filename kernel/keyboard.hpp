/**
 * @file keyboard.hpp
 *
 * Keyboard control program.
 */

#pragma once

#include <deque>
#include "message.hpp"

void InitializeKeyboard(std::deque<Message>& msg_queue);
