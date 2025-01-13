#include <boost/bind.hpp>
#include <vel_try/rovo2.hpp>
#include <cmath>

using namespace inmach::mattro;

Rovo2::Rovo2()
: m_ios()
, m_sock_stream ( m_ios )
, m_is_opened ( false )
, m_is_started ( false )
{
  m_rpdo2.rc_emergency_stop_disable = 0;
  m_rpdo2.driving_mode_selection = 0; 
}

Rovo2::~Rovo2()
{
  this->close();
}
    
void Rovo2::open ( const char* can_device )
{
  struct sockaddr_can addr;
  struct ifreq ifr; 
  this->close();
  
  m_sock_fd = socket ( PF_CAN, SOCK_RAW, CAN_RAW );
  strcpy ( ifr.ifr_name, can_device );
  ioctl ( m_sock_fd, SIOCGIFINDEX, &ifr );

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  if ( bind ( m_sock_fd, (struct sockaddr *)&addr, sizeof(addr) ) < 0 )
  {
    perror("Error in socket bind");
    ::close ( m_sock_fd );
    m_sock_fd = -1;
  }
  else
  {
    m_is_opened = true;
    m_sock_stream.assign ( m_sock_fd );
    m_thread.reset ( new std::thread ( std::bind ( &Rovo2::m_thread_func, this ) ) );
  }
}

void Rovo2::close()
{
  if ( m_is_started )
  {
    this->stop();
  }

  if ( m_is_opened )
  {
    m_ios.post ( boost::bind ( &Rovo2::m_close_socket, this ) );
    m_thread->join();
    m_thread.reset ( nullptr );
    ::close ( m_sock_fd );
    m_sock_fd = -1;
    m_is_opened = false;
  }
}

void Rovo2::start()
{
  if ( m_is_opened )
  {
    m_rpdo2.driving_mode_selection = 0; 

    m_is_started = true;
  }
}

void Rovo2::stop()
{
  m_is_started = false;

  m_rpdo1.activation_message_request = 0x00;
  m_rpdo1.auth_challenge_key = m_tpdo1.get_auth_challenge_key();
  m_async_send ( m_rpdo1.encode() );

  m_rpdo2.driving_mode_selection = 0; 
  m_rpdo2.set_speed( 0, 0 );
  m_async_send ( m_rpdo2.encode() );
}

void Rovo2::set_velocity ( const double vr, const double vl )
{
  if ( m_is_opened )
  {
    m_ios.post ( boost::bind ( &Rovo2::m_set_velocity, this, vr, vl ) );
  }
}

void Rovo2::m_async_send ( std::shared_ptr<struct can_frame> frame )
{
    m_sock_stream.async_write_some
      ( boost::asio::buffer ( frame.get(), sizeof ( struct can_frame ) )
      , boost::bind ( &Rovo2::m_handle_send, this, frame  )
      );
  
}

void Rovo2::m_handle_send ( std::shared_ptr<struct can_frame> frame )
{
  (void)frame;
}


void Rovo2::m_async_recv()
{
  if ( m_is_opened )
  {
    m_sock_stream.async_read_some
      ( boost::asio::buffer ( &m_rx_frame, sizeof ( m_rx_frame ) )
      , boost::bind ( &Rovo2::m_handle_recv, this )
      );
  }
}

void Rovo2::m_handle_recv()
{
  switch ( m_rx_frame.can_id )
  {
    case tpdo1::cob_id: 
    {
      m_tpdo1.decode_from ( m_rx_frame );
      
      if ( m_is_started )
      {
        if ( m_tpdo1.activation_state < 0x03 )
        {
          m_rpdo1.activation_message_request = 0x02; 
          m_rpdo1.auth_challenge_key = m_tpdo1.get_auth_challenge_key();
          m_async_send ( m_rpdo1.encode() );
        }
        else
        {
            m_rpdo2.driving_mode_selection = 1; 
        }
      }
      
      break;
    }
    case tpdo2::cob_id:
    {
      m_tpdo2.decode_from ( m_rx_frame );
      
      if ( m_listener )
      {
        double gear_ratio{7.0};
        double wheel_radius{0.17};
        double rpm_r = m_tpdo2.rotational_speed_right;
        double rpm_l = m_tpdo2.rotational_speed_left;
        double omega_right = rpm_r * 2.0 * M_PI / 60.0;
        double omega_left = rpm_l * 2.0 * M_PI / 60.0;
        double translational_speed_right = wheel_radius * omega_right / gear_ratio;
        double translational_speed_left = -wheel_radius * omega_left / gear_ratio; 
        m_listener->update ( translational_speed_right, translational_speed_left, std::chrono::steady_clock::now() );
      }
      break;
    }

    case tpdo3::cob_id:
    {
      m_tpdo3.decode_from ( m_rx_frame );
      break;
    }
    case tpdo4::cob_id:
    {
      m_tpdo4.decode_from ( m_rx_frame );
      break;
    }

    default:
    {
      /* ignore */
      break;
    }
  }
  m_async_recv();

}

/*
 * convert linear velocity in [m/s] to rovo2 integer velocity
 */
static inline int32_t linear2rovo ( const double v )
{
  enum
  {
    v_max_km_h = 30,
    rovo_max = 1000
  };

  static const double scale = ( rovo_max * 3600.0 ) / ( v_max_km_h * 1000.0 );

  return std::round ( v * scale );
}

void Rovo2::m_set_velocity ( const double vr, const double vl )
{
  m_rpdo2.set_speed ( linear2rovo ( vr ), linear2rovo ( vl ) );


  m_async_send ( m_rpdo2.encode() );
}

void Rovo2::m_close_socket()
{
  m_sock_stream.release();
}

void Rovo2::m_thread_func()
{
  this->m_async_recv();
  m_ios.run();
}