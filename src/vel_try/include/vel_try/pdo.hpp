#ifndef INMACH_MATTRO_UTIL_PDO_HPP_INCLUDED
#define INMACH_MATTRO_UTIL_PDO_HPP_INCLUDED

#include <memory>

#include <linux/can.h>
#include <boost/cstdint.hpp>

namespace inmach {
namespace mattro {

struct tpdo1
{
    enum { cob_id = 0x215 };
    boost::uint8_t activation_state;
    boost::uint8_t auth_challenge_seed;
    boost::uint8_t displacement_value;
    boost::uint8_t current_driving_mode;
    boost::uint16_t state_of_charge;
    
    bool decode_from ( struct can_frame const& frame );

    inline boost::uint8_t get_auth_challenge_key() const
    {
        return ( auth_challenge_seed >> displacement_value );
    }
};

struct tpdo2
{
    enum { cob_id = 0x315 };
    boost::int32_t rotational_speed_left;
    boost::int32_t rotational_speed_right;

    bool decode_from ( struct can_frame const& frame );
};

struct tpdo3
{
    enum { cob_id = 0x415 };
    boost::int16_t x_axis_1;
    boost::int16_t y_axis_1;
    boost::int16_t x_axis_2;
    boost::int16_t y_axis_2;

    bool decode_from ( struct can_frame const& frame );
};

struct tpdo4
{
    enum { cob_id = 0x515 };
    enum { rc_data_size = 7 };
    boost::uint8_t rc_data [ rc_data_size ]; 

    enum
    {
        rc_buttons_0 = 0,
        rc_buttons_1 = 1, rc_potentiometer_0 = 1, /* alternative use */
        rc_buttons_2 = 2, rc_potentiometer_1 = 2, /* alternative use */
        rc_buttons_3 = 3, rc_potentiometer_2 = 3, /* alternative use */
        rc_buttons_4 = 4,
        rc_buttons_5 = 5,
        rc_buttons_6 = 6,
    };

    bool decode_from ( struct can_frame const& frame );
};

struct rpdo1
{
    enum { cob_id = 0x195 };
    boost::uint8_t activation_message_request;
    boost::uint8_t auth_challenge_key;

    std::shared_ptr < struct can_frame >  encode() const;
};

struct rpdo2
{
    enum { cob_id = 0x295 };
    
    boost::uint8_t driving_mode_and_direction;
    boost::uint16_t rotation_speed_motor_left;
    boost::uint16_t rotation_speed_motor_right;
    boost::uint8_t driving_mode_selection;
    boost::uint8_t rc_emergency_stop_disable;

    std::shared_ptr < struct can_frame >  encode() const;

    void set_speed ( int32_t right, int32_t left );
};

} // namespace mattro
} // namespace inmach

#endif // INMACH_MATTRO_UTIL_PDO_HPP_INCLUDED