#ifndef TCP_ADAPTIVE_RENO_H
#define TCP_ADAPTIVE_RENO_H

#include "tcp-congestion-ops.h"
#include "tcp-westwood-plus.h"
#include "ns3/tcp-recovery-ops.h"
#include "ns3/sequence-number.h"
#include "ns3/traced-value.h"
#include "ns3/event-id.h"

namespace ns3 {

class Packet;
class TcpHeader;
class Time;
class EventId;

/**
 * \ingroup congestionOps
 *
 * \brief An implementation of TCP ADAPTIVE RENO.
 *
 */
class TcpAdaptiveReno : public TcpWestwoodPlus
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpAdaptiveReno ();
  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpAdaptiveReno (const TcpAdaptiveReno& sock);
  ~TcpAdaptiveReno () override;

  /**
   * \brief Filter type (None or Tustin)
   */
  enum FilterType 
  {
    NONE,
    TUSTIN
  };

  uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb,uint32_t bytesInFlight) override;

  void PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked,const Time& rtt) override;

  Ptr<TcpCongestionOps> Fork () override;

private:
  // void UpdateAckedSegments (int acked);

  //void EstimateBW(const Time& rtt, Ptr<TcpSocketState> tcb);

  double EstimateCongestionLevel();

  void EstimateIncWnd(Ptr<TcpSocketState> tcb);

  void CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked) override;

protected:
                  
  Time  m_RTT_CURR;
  Time  m_RTT_PKT_LOSS_J;
  Time  m_RTT_MIN;         
  Time  m_RTT_CONG_J;              
  Time  m_RTT_CONG_J_1;           

  // Window

  int32_t    m_W_inc;                
  uint32_t   m_W_base;                
  int32_t    m_W_probe;              
  uint32_t m_ackedSegments;


};

} // namespace ns3

#endif /*TCP_ADAPTIVE_RENO*/