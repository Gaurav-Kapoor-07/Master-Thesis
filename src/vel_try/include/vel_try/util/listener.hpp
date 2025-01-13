#ifndef INMACH_MATTRO_UTIL_LISTENER_HPP_INCLUDED
#define INMACH_MATTRO_UTIL_LISTENER_HPP_INCLUDED

#include <memory>
#include <vel_try/types.hpp>

namespace inmach {
namespace mattro {
namespace util {

/**
 * Interface of a listener class receiving wheel angle increments.
 * The update method shall be implemented in a thread safe way 
 * since it will be called from a different thread.
 */
class ListenerInterface
{
public:

    virtual ~ListenerInterface();
    
    /**
     * Receive a wheel angle increment.
     * The values in the \a delta pair are angle increments in [rad] units.
     */
    virtual void update ( const double vr, const double vl, timestamp_type const& ts ) = 0;
};

template < typename Fn >
class Listener: public ListenerInterface
{
public:
    Listener ( Fn const& fn ) : m_fn ( fn ) {}
    virtual ~Listener() {}
    
    virtual void update ( const double vr, const double vl, timestamp_type const& ts )
    {
        m_fn ( vr, vl, ts );
    }

private:
    Fn m_fn;
};

template < typename Fn >
std::shared_ptr<ListenerInterface> make_listener ( Fn const& fn )
{
    return std::shared_ptr<ListenerInterface> ( new Listener<Fn> ( fn ) );
}

} // namespace util
} // namespace mattro
} // namespace inmach

#endif  // INMACH_MATTRO_UTIL_LISTENER_HPP_INCLUDED