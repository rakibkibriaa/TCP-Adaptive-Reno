#include "1905098_tcp-adaptive-reno.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE ("TcpAdaptiveReno");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpAdaptiveReno")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpAdaptiveReno>()
    .AddAttribute("FilterType", "Use this to choose no filter or Tustin's approximation filter",
                  EnumValue(TcpAdaptiveReno::TUSTIN), MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                  MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
    .AddTraceSource("EstimatedBW", "The estimated bandwidth",
                    MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                    "ns3::TracedValueCallback::Double")
  ;
  return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno():
  TcpWestwoodPlus(),

  m_RTT_CURR(Time(0)),
  m_RTT_PKT_LOSS_J(Time(0)),
  m_RTT_MIN(Time(0)),
  m_RTT_CONG_J(Time(0)),
  m_RTT_CONG_J_1(Time(0)),

  //window
  m_W_inc(0),
  m_W_base(0),
  m_W_probe(0),
  m_ackedSegments(0)
{
  NS_LOG_FUNCTION (this);
}

TcpAdaptiveReno::TcpAdaptiveReno (const TcpAdaptiveReno& sock) :
  TcpWestwoodPlus(sock),

  m_RTT_MIN(sock.m_RTT_MIN),
  m_RTT_CURR(sock.m_RTT_CURR),
  m_RTT_PKT_LOSS_J(sock.m_RTT_PKT_LOSS_J),
  m_RTT_CONG_J(sock.m_RTT_CONG_J),
  m_RTT_CONG_J_1(sock.m_RTT_CONG_J_1),

  //window
  m_W_inc (sock.m_W_inc),
  m_W_base (sock.m_W_base),
  m_W_probe (sock.m_W_probe),
  m_ackedSegments(sock.m_ackedSegments)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}

TcpAdaptiveReno::~TcpAdaptiveReno ()
{

}

void
TcpAdaptiveReno::PktsAcked(Ptr<TcpSocketState> tcb, uint32_t packetsAcked,const Time& rtt)
{
  NS_LOG_FUNCTION(this);

  if (rtt.IsZero())
    {
      NS_LOG_WARN("RTT measured is zero!");
      return;
    }

    m_ackedSegments += packetsAcked;  

    if(m_RTT_MIN > rtt || m_RTT_MIN.IsZero())
    {
        m_RTT_MIN = rtt;
    }

    m_RTT_CURR = rtt;

    TcpWestwoodPlus::EstimateBW(rtt, tcb);
}


double
TcpAdaptiveReno::EstimateCongestionLevel()
{

  float exponential_smoothing_factor = 0.85; // exponential smoothing factor

  if(m_RTT_CONG_J_1 < m_RTT_MIN)
  {
     exponential_smoothing_factor = 0; 
  } 
  
  double RTT_CONG_J = exponential_smoothing_factor * m_RTT_CONG_J_1.GetSeconds() + ( 1 - exponential_smoothing_factor) * m_RTT_PKT_LOSS_J.GetSeconds();

  double c = std::min((m_RTT_CURR.GetSeconds() - m_RTT_MIN.GetSeconds()) / (RTT_CONG_J - m_RTT_MIN.GetSeconds()),1.0);

  m_RTT_CONG_J = Seconds(RTT_CONG_J); //update

  return c;
}


void 
TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb)
{

  double MSS = tcb->m_segmentSize * tcb->m_segmentSize;

  double m_max_W_inc = (m_currentBW.Get()).GetBitRate() / 1000 * MSS;

  double alpha = 10; 

  double beta = 2 * m_max_W_inc * ((1/alpha) - ((1/alpha + 1) / (std::exp(alpha))));

  double gamma = 1 - (2 * m_max_W_inc * (( 1 / alpha) - ((1 / alpha + 0.5) / (std::exp(alpha)))));

  double congestion = EstimateCongestionLevel();

  m_W_inc = (int)((m_max_W_inc / std::exp(congestion * alpha)) + (congestion * beta) + gamma);

}


void
TcpAdaptiveReno::CongestionAvoidance(Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{

  if (segmentsAcked > 0)
  {
      EstimateIncWnd(tcb);
    
      double MSS = tcb->m_segmentSize * tcb->m_segmentSize;

      double val = std::max(MSS/tcb->m_cWnd.Get(),1.0);

      m_W_base = m_W_base + val;

      m_W_probe = std::max((m_W_probe + m_W_inc / (int32_t)tcb->m_cWnd.Get()),0);
      
      tcb->m_cWnd = m_W_base + m_W_probe;
    
  }

}

uint32_t
TcpAdaptiveReno::GetSsThresh(Ptr<const TcpSocketState> tcb,uint32_t bytesInFlight)
{

  m_RTT_CONG_J_1 = m_RTT_CONG_J;
  m_RTT_PKT_LOSS_J = m_RTT_CURR; 
  
  double congestion = EstimateCongestionLevel();
 
  uint32_t ssthresh = std::max (2 * tcb->m_segmentSize, (uint32_t) (tcb->m_cWnd / (1.0 + congestion)));
  
  m_W_base = ssthresh;

  m_W_probe = 0;
  
  return ssthresh;

}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork ()
{
  return CreateObject<TcpAdaptiveReno> (*this);
}

} // namespace ns3