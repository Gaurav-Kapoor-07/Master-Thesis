#ifndef INMACH_MATTRO_ROVO2_HPP_INCLUDED
#define INMACH_MATTRO_ROVO2_HPP_INCLUDED

#include <memory>
#include <boost/asio.hpp>
#include <thread>
#include <vel_try/types.hpp>
#include <vel_try/pdo.hpp>
#include <vel_try/util/listener.hpp>

namespace inmach {
namespace mattro {

class Rovo2
{
public:

    Rovo2();
    virtual ~Rovo2();
    
    /**
     * Register a callback to receive wheel angle updates.
     * The callback shall accepts a \c pair_type and a \c timeval argument.
     * The values in the  pair are angle increments in [rad] units.
     * The callback shall be implemented in a thread safe way, 
     * since it will be called from a different (internal) thread.
     *
     * Example:
     * \code
     * #include <functional>
     *
     * class MyClass
     * {
     * public:
     *   MyClass()
     *   {
     *     using namespace std::placeholders;
     *     m_rovo2.connect ( std::bind (&MyClass::update,this,_1,_2,_3) );
     *     // ...
     *   }
     *   // ...
     * private:
     *   double m_wheel_radius;
     *   inmach::mattro::Rovo2 m_rovo2;
     *   void update ( const double vr, const double vl, inmach::mattro::timestamp_type const& ts )
     *   {
     *     // ...
     *   }
     *   // ...
     * }
     * \endcode
     */
    template < typename Fn >
    void get_velocity_async ( Fn const& cb )
    {
        m_listener.reset ( new util::Listener < Fn > ( cb ) );
    }
    
    /**
     * Open communication on the CAN bus, using the given \a can_device name.
     */
    void open ( const char* can_device );
    
    /**
     * Close communication on the CAN bus.
     */
    void close();

    /**
     * Start controlling the Rovo2 using velocity commands.
     * The Rovo2 will be unlocked after the call to \c start().
     */
    void start();
    
    /**
     * Stop controlling the Rovo2 using velocity commands.
     * The Rovo2 will be locked again after the call to \c stop();
     */
    void stop();
    
    /**
     * Send a velocity command to the Rovo2.
     * The values in the \a omega pair are angular velocites in [rad/s] units.
     */
    void set_velocity ( const double vr, const double vl );
    
private:
    double m_wheel_base;
    double m_wheel_radius;

    std::shared_ptr < util::ListenerInterface > m_listener;
    
    boost::asio::io_service m_ios;
    int m_sock_fd;
    boost::asio::posix::basic_stream_descriptor<> m_sock_stream;
    std::unique_ptr < std::thread > m_thread;
    void m_thread_func();

    bool m_is_opened;
    bool m_is_started;

    struct can_frame m_rx_frame;

    tpdo1 m_tpdo1;
    tpdo2 m_tpdo2;
    tpdo3 m_tpdo3;
    tpdo4 m_tpdo4;

    rpdo1 m_rpdo1;
    rpdo2 m_rpdo2;
    
    void m_async_send ( std::shared_ptr<struct can_frame> frame );
    void m_handle_send( std::shared_ptr<struct can_frame> frame );
    
    void m_async_recv();
    void m_handle_recv();
    
    void m_set_velocity ( const double vr, const double vl );
    void m_close_socket();
};

} // namespace mattro
} // namespace inmach

#endif // INMACH_MATTRO_ROVO2_HPP_INCLUDED