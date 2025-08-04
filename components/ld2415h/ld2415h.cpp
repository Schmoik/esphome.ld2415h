#include "ld2415h.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ld2415h {

static const char *const TAG = "ld2415h";

void LD2415HComponent::setup() {
  ESP_LOGI(TAG, "Setting up LD2415H...");
  this->flush();  // UART-Puffer leeren
}

void LD2415HComponent::loop() {
  while (this->available()) {
    char c = this->read();
    if (fill_buffer_(c)) {
      parse_buffer_();
    }
  }
}

void LD2415HComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "LD2415H:");
  LOG_SENSOR("  ", "Speed", this->speed_sensor_);
}

bool LD2415HComponent::fill_buffer_(char c) {
  if (response_buffer_index_ >= sizeof(response_buffer_)) {
    response_buffer_index_ = 0;  // Buffer-Overflow vermeiden
    return false;
  }

  response_buffer_[response_buffer_index_++] = c;

  // Prüfen auf gültiges Telegramm (z. B. Start- und Endbytes)
  if (response_buffer_index_ >= 12 &&
      static_cast<uint8_t>(response_buffer_[0]) == 0xFD &&
      static_cast<uint8_t>(response_buffer_[1]) == 0xFC) {
    return true;
  }

  return false;
}

void LD2415HComponent::parse_buffer_() {
  if (response_buffer_index_ < 12) {
    response_buffer_index_ = 0;
    return;
  }

  // Geschwindigkeit ist an Position 8 & 9 im Datenpaket (typisch für LD2415-Protokoll)
  uint8_t speed_lsb = response_buffer_[8];
  uint8_t speed_msb = response_buffer_[9];
  float speed = (speed_msb << 8 | speed_lsb) / 100.0f;

  this->speed_ = speed;

  if (this->speed_sensor_ != nullptr) {
    this->speed_sensor_->publish_state(speed_);
  }

  ESP_LOGD(TAG, "Geschwindigkeit: %.2f", speed_);

  response_buffer_index_ = 0;  // Buffer zurücksetzen
}

}  // namespace ld2415h
}  // namespace esphome
