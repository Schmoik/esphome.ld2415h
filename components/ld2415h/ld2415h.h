#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ld2415h {

enum TrackingMode : uint8_t { APPROACHING_AND_RETREATING = 0x00, APPROACHING = 0x01, RETREATING = 0x02 };
enum UnitOfMeasure : uint8_t { KPH = 0x00, MPH = 0x01, MPS = 0x02 };

class LD2415HComponent : public Component, public uart::UARTDevice {
 public:
  LD2415HComponent() = default;

  void setup() override;
  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  void set_speed_sensor(sensor::Sensor *sensor) { this->speed_sensor_ = sensor; }

 protected:
  sensor::Sensor *speed_sensor_{nullptr};

  // interner Buffer
  char response_buffer_[64];
  uint8_t response_buffer_index_ = 0;

  // Statuswerte
  double speed_ = 0;
  double velocity_ = 0;

  // Konfiguration
  uint8_t min_speed_threshold_ = 1;
  uint8_t compensation_angle_ = 0;
  uint8_t sensitivity_ = 10;
  TrackingMode tracking_mode_ = TrackingMode::APPROACHING_AND_RETREATING;
  UnitOfMeasure unit_of_measure_ = UnitOfMeasure::KPH;

  void parse_buffer_();
  void parse_speed_();
  bool fill_buffer_(char c);
  void clear_remaining_buffer_(uint8_t pos);
};

}  // namespace ld2415h
}  // namespace esphome
