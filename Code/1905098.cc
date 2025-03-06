#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/propagation-module.h"
#include "ns3/point-to-point-dumbbell.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Task-1");

class TutorialApp : public Application
{
  public:
    TutorialApp();
    ~TutorialApp() override;

    /**
     * Register this type.
     * \return The TypeId.
     */
    static TypeId GetTypeId();

    /**
     * Setup the socket.
     * \param socket The socket.
     * \param address The destination address.
     * \param packetSize The packet size to transmit.
     * \param simulationTime The number of packets to transmit.
     * \param dataRate the data rate to use.
     */
    void Setup(Ptr<Socket> socket,
               Address address,
               uint32_t packetSize,
               uint32_t simulationTime,
               DataRate dataRate);

  private:
    void StartApplication() override;
    void StopApplication() override;

    /// Schedule a new transmission.
    void ScheduleTx();
    /// Send a packet.
    void SendPacket();

    Ptr<Socket> m_socket;   //!< The transmission socket.
    Address m_peer;         //!< The destination address.
    uint32_t m_packetSize;  //!< The packet size.
    uint32_t m_simulationTime;    //!< The number of packets to send.
    DataRate m_dataRate;    //!< The data rate to use.
    EventId m_sendEvent;    //!< Send event.
    bool m_running;         //!< True if the application is running.
    uint32_t m_packetsSent; //!< The number of packets sent.
};


/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */



TutorialApp::TutorialApp()
    : m_socket(nullptr),
      m_peer(),
      m_packetSize(0),
      m_simulationTime(0),
      m_dataRate(0),
      m_sendEvent(),
      m_running(false),
      m_packetsSent(0)
{
}

TutorialApp::~TutorialApp()
{
    m_socket = nullptr;
}

/* static */
TypeId
TutorialApp::GetTypeId()
{
    static TypeId tid = TypeId("TutorialApp")
                            .SetParent<Application>()
                            .SetGroupName("Tutorial")
                            .AddConstructor<TutorialApp>();
    return tid;
}

void
TutorialApp::Setup(Ptr<Socket> socket,
                   Address address,
                   uint32_t packetSize,
                   uint32_t simulationTime,
                   DataRate dataRate)
{
    m_socket = socket;
    m_peer = address;
    m_packetSize = packetSize;
    m_simulationTime = simulationTime;
    m_dataRate = dataRate;
}

void
TutorialApp::StartApplication()
{
    m_running = true;
    m_packetsSent = 0;
    m_socket->Bind();
    m_socket->Connect(m_peer);
    SendPacket();
}

void
TutorialApp::StopApplication()
{
    m_running = false;

    if (m_sendEvent.IsRunning())
    {
        Simulator::Cancel(m_sendEvent);
    }

    if (m_socket)
    {
        m_socket->Close();
    }
}

void
TutorialApp::SendPacket()
{
    Ptr<Packet> packet = Create<Packet>(m_packetSize);
    m_socket->Send(packet);

    if (Simulator::Now().GetSeconds() < m_simulationTime)
    {
        ScheduleTx();
    }
}

void
TutorialApp::ScheduleTx()
{
    if (m_running)
    {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double>(m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &TutorialApp::SendPacket, this);
    }
}

static void
CwndChange(Ptr<OutputStreamWrapper>stream, uint32_t oldCwnd, uint32_t newCwnd)
{
    *stream->GetStream()<<Simulator::Now().GetSeconds()<< " " <<newCwnd<<"\n";
}



int main(int argc, char *argv[])
{



    std::ofstream MyFile1;
    std::ofstream MyFile2;

    std::ofstream MyFile3;
    std::ofstream MyFile4;
   

    MyFile1.open("./scratch/throughput_bottleneck.txt", std::ios_base::app);
    MyFile2.open("./scratch/congestion.txt", std::ios_base::app);

    MyFile3.open("./scratch/fairness_reno.txt", std::ios_base::app);
    MyFile4.open("./scratch/fairness_other.txt", std::ios_base::app);



    int bottleNeckRate = 50;
    int bottleNeckDelay = 100;
    double error = 0.000001;
    int error_power = -6;
    std::string algo;
    CommandLine cmd(__FILE__);
    cmd.AddValue("bottleneckRate", "Bottle neck datarate", bottleNeckRate);
    cmd.AddValue("error", "Packet loss rate", error_power);
    cmd.AddValue("algo","Select algo",algo);

    //cmd.AddValue("pkt", "Packet Per second", packetsPerSecond);
    //cmd.AddValue("coverage", "Coverage Area factor", transmission_range_factor);
    
    cmd.Parse(argc, argv);

    error = pow(10,error_power);
    
    std::cout<<bottleNeckRate<<" "<<error<<"\n";

    int nLeft = 2;
    int nRight = 2;

    
    int packetSize = 1024;

    

    uint32_t sim_time = 60;

    uint32_t senderDataRate = 1e9;


    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue (packetSize));

    PointToPointHelper leftP2PHelper;
    PointToPointHelper rightP2PHelper;
    PointToPointHelper bottleNeckHelper;


    NodeContainer totalNodes;

    leftP2PHelper.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    leftP2PHelper.SetChannelAttribute("Delay", StringValue("1ms"));
    

    rightP2PHelper.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    rightP2PHelper.SetChannelAttribute("Delay", StringValue("1ms"));

    bottleNeckHelper.SetDeviceAttribute("DataRate", StringValue(std::to_string(bottleNeckRate) + "Mbps"));
    bottleNeckHelper.SetChannelAttribute("Delay", StringValue(std::to_string(bottleNeckDelay) + "ms"));

    bottleNeckHelper.SetQueue("ns3::DropTailQueue", "MaxSize",StringValue(std::to_string ((bottleNeckRate * bottleNeckDelay * 1000/packetSize)) + "p"));

    PointToPointDumbbellHelper pointopoint_dumbel_helper(nLeft, leftP2PHelper, nRight, rightP2PHelper, bottleNeckHelper);

    //Error model setup
    Ptr<RateErrorModel> rate_errorModel = CreateObject<RateErrorModel>();
    rate_errorModel->SetAttribute("ErrorRate", DoubleValue(error));

    pointopoint_dumbel_helper.m_routerDevices.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(rate_errorModel));

    pointopoint_dumbel_helper.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(rate_errorModel));

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));

    InternetStackHelper stack_tcp_reno;

    stack_tcp_reno.Install(pointopoint_dumbel_helper.GetLeft(0));
    stack_tcp_reno.Install(pointopoint_dumbel_helper.GetRight(0));

    stack_tcp_reno.Install(pointopoint_dumbel_helper.GetLeft());
    stack_tcp_reno.Install(pointopoint_dumbel_helper.GetRight());

    //ns3::TcpAdaptiveReno
   // ns3::TcpWestwoodPlus
    //ns3::TcpHighSpeed

    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue (algo));

    InternetStackHelper stack_tcp_westwood;

    stack_tcp_westwood.Install(pointopoint_dumbel_helper.GetLeft(1));
    stack_tcp_westwood.Install(pointopoint_dumbel_helper.GetRight(1));

    Ipv4AddressHelper leftNodesIP;
    Ipv4AddressHelper rightNodesIP;
    Ipv4AddressHelper bottleNeckIP;

    leftNodesIP.SetBase("10.1.0.0", "255.255.255.0");
    rightNodesIP.SetBase("10.2.0.0", "255.255.255.0");
    bottleNeckIP.SetBase("10.3.0.0", "255.255.255.0");
    
    pointopoint_dumbel_helper.AssignIpv4Addresses(leftNodesIP, rightNodesIP, bottleNeckIP);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();


    FlowMonitorHelper flowmonitor_helper;
    flowmonitor_helper.SetMonitorAttribute("MaxPerHopDelay", TimeValue(Seconds(2)));
    Ptr<FlowMonitor> monitor = flowmonitor_helper.InstallAll();

    uint16_t port = 8888;

    ApplicationContainer sinkApps;

  
    for(int i=0;i<2; i++){

      Address sinkAddress (InetSocketAddress (pointopoint_dumbel_helper.GetRightIpv4Address(i),port));

      Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (pointopoint_dumbel_helper.GetLeft(i), TcpSocketFactory::GetTypeId());

      Ptr<TutorialApp> application = CreateObject<TutorialApp>();

      application->Setup(ns3TcpSocket, sinkAddress, packetSize,sim_time,DataRate(senderDataRate));
      pointopoint_dumbel_helper.GetLeft(i)->AddApplication(application);
      application->SetStartTime(Seconds (1));
      application->SetStopTime(Seconds (sim_time));

      PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(),port));

      sinkApps.Add(packetSinkHelper.Install(pointopoint_dumbel_helper.GetRight(i)));

      port++;
     
      std::ostringstream oss;
    
      if(i % 2 == 0)
      {
        oss<<"./scratch/congestion_window_reno.txt";
      }
      else
      {
        oss<<"./scratch/congestion_window_other.txt";
      }
      
      AsciiTraceHelper asciiTraceHelper;
      Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (oss.str());
      ns3TcpSocket->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback (&CwndChange,stream));

    }

    sinkApps.Start(Seconds (0));
    sinkApps.Stop(Seconds (sim_time + 2));

    Simulator::Stop(Seconds(sim_time + 2)); 

    Simulator::Run ();

    FlowMonitor::FlowStatsContainer flow_stats = monitor->GetFlowStats();

    double throughput_tcp_reno = 0;
    double throughput_tcp_highspeed = 0;
    double totalLostPackets_tcp_reno = 0;
    double totalLostPackets_tcp_highspeed = 0;


    uint32_t totalSentPackets_tcp_reno = 0;
    uint32_t totalSentPackets_tcp_highspeed = 0;


    double fairness_reno_num = 0;
    double fairness_reno_deno = 0;

    //double fairness_other_num = 0;
    //double fairness_other_deno = 0;
    
    double jain_reno = 0;
    //double jain_other = 0;

    int counter = 0;


    for (auto iterator = flow_stats.begin (); iterator != flow_stats.end (); ++iterator)
    {
        
        if(counter % 2 == 0)
        {
            throughput_tcp_reno += iterator->second.rxBytes * 8.0;
            totalLostPackets_tcp_reno += iterator->second.lostPackets;
            totalSentPackets_tcp_reno += iterator->second.txPackets;


            

        }
        else
        {

            throughput_tcp_highspeed += iterator->second.rxBytes * 8.0;
            totalLostPackets_tcp_highspeed += iterator->second.lostPackets;
            totalSentPackets_tcp_highspeed += iterator->second.txPackets;

            
        
        }

        double thr = iterator->second.rxBytes * 8.0/((sim_time + 2)*1000);

        fairness_reno_num += thr;
        fairness_reno_deno+= (thr * thr);

        counter++;
        
    }

    jain_reno = (fairness_reno_num * fairness_reno_num) / (counter * fairness_reno_deno);
    //jain_other = (fairness_other_num * fairness_other_num) / (counter * fairness_other_deno);

    MyFile1<< throughput_tcp_reno / ((sim_time + 2) * 1000)<<" "<<(totalLostPackets_tcp_reno/totalSentPackets_tcp_reno) * 100<<" "<<bottleNeckRate<<" ";
    MyFile1<< throughput_tcp_highspeed / ((sim_time + 2) * 1000)<<" "<<(totalLostPackets_tcp_highspeed/totalSentPackets_tcp_highspeed) * 100 <<" "<<error_power<<"\n";


    MyFile3<<jain_reno<<" "<<bottleNeckRate<<" "<<error_power<<"\n";
    //MyFile4<<jain_other<<" "<<bottleNeckRate<<" "<<error_power<<"\n";

    Simulator::Destroy ();

}