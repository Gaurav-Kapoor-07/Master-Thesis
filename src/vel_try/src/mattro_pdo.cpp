#include <algorithm>
#include <vel_try/pdo.hpp>

using namespace inmach::mattro;

struct frame_reader
{
    const boost::uint8_t* iter;
    size_t count;
    bool good;

    inline frame_reader ( struct can_frame const& frame, const canid_t id )
    : iter ( static_cast < const boost::uint8_t* > ( frame.data ) )
    , count ( frame.can_dlc )
    , good ( true )
    {
        if ( id != frame.can_id )
        {
            good = false;
        }
    }
    
    inline frame_reader& operator >> ( boost::uint8_t& u )
    {
        if ( 0 < count )
        {
            u = *iter;
            ++iter;
            --count;
        }
        else
        {
            good = false;
        }
        return *this;
    }

    inline frame_reader& operator >> ( boost::uint16_t& u )
    {
        boost::uint8_t lo, hi;
        *this >> lo >> hi;
        u = hi;
        u = ( u << 8U ) | lo;
        return *this;
    }

    inline frame_reader& operator >> ( boost::uint32_t& u )
    {
        boost::uint16_t lo, hi;
        *this >> lo >> hi;
        u = hi;
        u = ( u << 16U ) | lo;
        return *this;
    }

    inline frame_reader& operator >> ( boost::int8_t& u )
    {
        uint8_t v;
        *this >> v;
        u = v;
        return *this;
    }

    inline frame_reader& operator >> ( boost::int16_t& u )
    {
        uint16_t v;
        *this >> v;
        u = v;
        return *this;
    }

    inline frame_reader& operator >> ( boost::int32_t& u )
    {
        uint32_t v;
        *this >> v;
        u = v;
        return *this;
    }
};

struct frame_writer
{
    struct can_frame& frame;
    bool good;

    inline frame_writer ( struct can_frame& f, const canid_t id )
    : frame ( f )
    , good ( true )
    {
        frame.can_id  = id;
        frame.can_dlc = 0;
    }
    
    inline frame_writer& operator << ( boost::uint8_t const& u )
    {
        if ( frame.can_dlc < CAN_MAX_DLC )
        {
            frame.data [ frame.can_dlc ] = u;
            ++frame.can_dlc;
        }
        else
        {
            good = false;
        }
        return *this;
    }

    inline frame_writer& operator << ( boost::uint16_t const& u )
    {
        const boost::uint8_t lo = ( ( u >> 0U ) & 0x00FF );
        const boost::uint8_t hi = ( ( u >> 8U ) & 0x00FF );
        return *this << lo << hi;
    }

    inline frame_writer& operator << ( boost::uint32_t const& u )
    {
        const boost::uint16_t lo = ( ( u >>  0U ) & 0x0000FFFF );
        const boost::uint16_t hi = ( ( u >> 16U ) & 0x0000FFFF );
        return *this << lo << hi;
    }

    inline frame_writer& operator << ( boost::int8_t const& u )
    {
        return *this << static_cast < boost::uint8_t > ( u );
    }

    inline frame_writer& operator << ( boost::int16_t const& u )
    {
        return *this << static_cast < boost::uint16_t > ( u );
    }

    inline frame_writer& operator << ( boost::int32_t const& u )
    {
        return *this << static_cast < boost::uint32_t > ( u );
    }
};


bool tpdo1::decode_from ( struct can_frame const& frame )
{
    boost::uint8_t skip;
    frame_reader fr ( frame, cob_id );
    fr >> activation_state 
       >> auth_challenge_seed
       >> displacement_value
       >> skip
       >> skip
       >> current_driving_mode
       >> state_of_charge;
    (void)skip;
    return fr.good;
}

bool tpdo2::decode_from ( struct can_frame const& frame )
{
    frame_reader fr ( frame, cob_id );
    fr >> rotational_speed_left
       >> rotational_speed_right;
    return fr.good;
}

bool tpdo3::decode_from ( struct can_frame const& frame )
{
    frame_reader fr ( frame, cob_id );
    fr >> x_axis_1
       >> y_axis_1
       >> x_axis_2
       >> y_axis_2;
    return fr.good;
}

bool tpdo4::decode_from ( struct can_frame const& frame )
{
    frame_reader fr ( frame, cob_id );
    for ( size_t index = 0; index < rc_data_size; ++index )
    {
        fr >> rc_data [ index ];
    }
    return fr.good;
}

std::shared_ptr < struct can_frame >  rpdo1::encode() const
{
  auto frame = std::make_shared < struct can_frame >();
  const boost::uint8_t zero = 0;
  frame_writer fw ( *frame, cob_id );
  fw << activation_message_request 
     << zero
     << zero
     << auth_challenge_key
     << zero
     << zero
     << zero
     << zero;
  return frame;
}

std::shared_ptr < struct can_frame >  rpdo2::encode() const
{
  auto frame = std::make_shared < struct can_frame >();
  const boost::uint8_t zero = 0;
  frame_writer fw ( *frame, cob_id );
  fw << driving_mode_and_direction
     << rotation_speed_motor_left
     << rotation_speed_motor_right
     << driving_mode_selection
     << rc_emergency_stop_disable
     << zero;
  return frame;
}

void rpdo2::set_speed ( int32_t right, int32_t left )
{
  enum
  {
      MOTOR_STOP = 0,
      MOTOR_FWD  = 1,
      MOTOR_BWD  = 2
  };
  enum
  {
      MOTOR_SHIFT_LEFT  = 4,
      MOTOR_SHIFT_RIGHT = 6
  };

  driving_mode_and_direction = 0x03;
  if ( left < 0 )
  {
    driving_mode_and_direction |= ( MOTOR_BWD << MOTOR_SHIFT_LEFT );
    left = - left;
  }
  else if ( left > 0 )
  {
    driving_mode_and_direction |= ( MOTOR_FWD << MOTOR_SHIFT_LEFT );
  }
  else
  {
    driving_mode_and_direction |= ( MOTOR_STOP << MOTOR_SHIFT_LEFT );
  } 
  
  if ( right < 0 )
  {
    driving_mode_and_direction |= ( MOTOR_BWD << MOTOR_SHIFT_RIGHT );
    right = - right;
  }
  else if ( right > 0 )
  {
    driving_mode_and_direction |= ( MOTOR_FWD << MOTOR_SHIFT_RIGHT );
  }
  else
  {
    driving_mode_and_direction |= ( MOTOR_STOP << MOTOR_SHIFT_RIGHT );
  } 

  rotation_speed_motor_left  = std::min ( left,  0xFFFF );
  rotation_speed_motor_right = std::min ( right, 0xFFFF );
  
}