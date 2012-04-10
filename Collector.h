/**
 * @file
 * @author Brian Trammell <trammell@tik.ee.ethz.ch>
 *
 * @section DESCRIPTION
 * 
 * Defines the abstract collector interface.
 *
 * To receive IPFIX Messages, client code should create an instance
 * of a Collector subclass for the necessary transport, register
 * a SetReceiver for each type of message you want to receive, and
 * continually call receiveMessage() in the runloop to receive each
 * next message.
 */


#ifndef IPFIX_COLLECTOR_H // idem
#define IPFIX_COLLECTOR_H // hack

#include "Session.h"
#include "MBuf.h"
#include "IETemplate.h"
#include "SetReceiver.h"

namespace IPFIX {

  class Collector {
    
  public:

    Collector() {}
    virtual ~Collector() {}
    
    /**
     * Register a SetReceiver instance for a given minimal template. 
     * As new templates are received at the collector, they will be
     * compared to this minimal template. The first minimal template found
     * (in order of calls to registerReceiver()) such that all the IEs
     * in the minimal template appear in the newly received template 
     * will bind the new template to this receiver.
     *
     * @param mintmpl minimal template associated with the receiver.
     * @param receiver receiver to bind; caller retains ownership.
     */
     void registerReceiver(const IETemplate* mintmpl, 
                           SetReceiver* receiver);

    /**
     * Receive and process the next message sent to this collector.
     *
     * Calls the receive() method on any registered receiver matching
     * templates in the received message. Allows the caller to pass in
     * a message buffer by reference; for instance, to save on message
     * buffer reallocation or to implement delayed message processing.
     *
     * @param mbuf message buffer to use for; will be overwritten with
     *             received message content.
     * @return true if a message was received, false otherwise.
     */
    bool receiveMessage(MBuf& mbuf);
    
    /**
     * Receive and process the next message sent to this collector.
     *
     * Calls the receive() method on any registered receiver matching
     * templates in the received message. Uses a transient message
     * buffer.
     *
     * @return true if a message was received, false otherwise.
     */
    bool receiveMessage() {
      MBuf transient_mbuf;
      return this->receiveMessage(transient_mbuf);
    }
    
    void restart() {
      // FIXME support collecting process restart
    }
    
  protected:
    
    /**
     * Receive and deframe the next message into the given message buffer;
     * return the session on which it was received by reference.
     *
     * Implemented by subclasses to provide transport-specific deframing.
     *
     * @param mbuf buffer to deframe message into
     * @param session (returned by reference) shared pointer to session 
                      from which message was received
     * @return true if message available, false otherwise
     */
    virtual bool _receiveMessage(MBuf& mbuf, std::tr1::shared_ptr<Session>& session) = 0;
    
    /**
     * Get a session for a given session key. Creates a new session if
     * no session exists for the given key.
     *
     * Used by subclasses to map session keys to sessions.
     *
     * FIXME really need to fix how session handling is done; a one-size fits
     * all approach is wrong. these should be keyed by active socket on TCP/SCTP,
     * and by... what? on UDP. Each Collector subclass should handle its own
     * Session management.
     *
     * @param sessionKey string identifying session
     * @return shared pointer to the session
     */
    std::tr1::shared_ptr<Session> getSession(const std::string& sk);
    
    /**
     * Discard the session for a given session key.
     *
     * Used by subclasses to end sessions on session shutdown.
     *
     * @param sessionKey string identifying session
     */
    void endSession(const std::string sessionKey);
    
  private:

    /**
     * Get the receiver for a given wire template. 
     * Caches the receiver if necessary.
     *
     * @param wt template to get receiver for
     * @return shared pointer to the receiver (or to NULL if no receiver registered)
     */
    SetReceiver* receiverForTemplate(const WireTemplate* wt);
    
    /**
     * Signal that a given wire template has been withdrawn.
     *
     * Removes any cached receivers for this template.
     *
     * @param tk template to decache.
     */
    void templateWithdrawn(WireTemplateKey& tk) {
      receiver_cache_.erase(tk);
    }
            
    std::map<std::string, std::tr1::shared_ptr<Session> >           sessions_;
    std::map<const IETemplate*, SetReceiver* >              receivers_;
    std::map<WireTemplateKey, SetReceiver* >                receiver_cache_;
  };
  
} // namespace IPFIX

#endif // idem hack
