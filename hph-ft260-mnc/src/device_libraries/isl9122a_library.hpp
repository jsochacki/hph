#ifndef ISL9122A_LIBRARY_H_
#define ISL9122A_LIBRARY_H_

#include <cstdint>

namespace isl9122a
{
   const uint8_t isl9122a_address = 0b00011000;

   const uint8_t vset_address = 0x11;
   const uint8_t conv_address = 0x12;
   const uint8_t intflg_mask_address = 0x13;

   constexpr double vset_value_resolution = 0.025;
   constexpr double vset_value_min_double = 1.8;
   constexpr double vset_value_max_double = 5.375;
   constexpr uint8_t vset_value_min = vset_value_min_double / vset_value_resolution;
   constexpr uint8_t vset_value_max = vset_value_max_double / vset_value_resolution;
   constexpr float vset_range = vset_value_max - vset_value_min;

   enum : uint8_t
   {
      ccea_wake_on_enable_only = (0b0 << 7),
      ccea_start_on_enable = (0b1 << 7),
      ccd_discharge_resistor_disabled = (0b0 << 6),
      ccd_discharge_resistor_enabled = (0b1 << 6),
      ccdvsr_output_slew_rate_3p125mvperus = (0b00 << 4),
      ccdvsr_output_slew_rate_6p25mvperus = (0b01 << 4),
      ccdvsr_output_slew_rate_0p78125mvperus = (0b10 << 4),
      ccdvsr_output_slew_rate_1p5625mvperus = (0b11 << 4),
      ccf_normal_operation = (0b00 << 2),
      ccf_reserved = (0b01 << 2),
      ccf_forced_pwm = (0b10 << 2),
      ccf_forced_bypass = (0b11 << 2),
      cccr_reserved = (0b0 << 1),
      cct_type_1_error_amplifier = 0b0,
      cct_type_2_error_amplifier = 0b1
   };

   constexpr uint8_t default_conv_value = (ccea_start_on_enable
                                           | ccd_discharge_resistor_enabled
                                           | ccdvsr_output_slew_rate_0p78125mvperus
                                           | ccf_normal_operation
                                           | cccr_reserved
                                           | cct_type_2_error_amplifier);

   enum : uint8_t
   {
      imofm_hiccup_mode_with_100ms_wait = (0b00 << 6),
      imofm_shutdown_mode = (0b01 << 6),
      imofm_current_limit_with_no_fault_action_taken = (0b10 << 6),
      imofm_reserved = (0b11 << 6),
      imeo_controlled_by_the_en_pin = (0b0 << 5),
      imeo_held_in_enabled_state = (0b0 << 5)
   };

   constexpr uint8_t default_intflg_value = (imofm_hiccup_mode_with_100ms_wait
                                             | imeo_controlled_by_the_en_pin);

   const uint8_t message_packet_size_in_bytes = 2;

   void initialize_system_fan(hph::ft260_interface &ft260s, uint8_t device_handle);
   void set_fan_to_percent(hph::ft260_interface &ft260s, uint8_t device_handle, float percent);
}   // namespace isl9122a

#endif /* ISL9122A_LIBRARY_H_ */
