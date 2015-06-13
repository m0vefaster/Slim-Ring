#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
//#include "ns3/visualizer-module.h"
#include <string>
#include <sstream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

void add_static_host_route(Ptr<Node> node, std::string dst, std::string nhop, uint32_t intf){
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> staticRouting1 = ipv4RoutingHelper.GetStaticRouting (node->GetObject<Ipv4>());
  staticRouting1->AddHostRouteTo (Ipv4Address (dst.c_str()), Ipv4Address (nhop.c_str()), intf);
}

void add_static_network_route(Ptr<Node> node, std::string network, std::string mask, std::string nhop, uint32_t intf, uint32_t metric){
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> staticRouting1 = ipv4RoutingHelper.GetStaticRouting (node->GetObject<Ipv4>());
  staticRouting1->AddNetworkRouteTo (Ipv4Address (network.c_str()), Ipv4Mask(mask.c_str()), Ipv4Address (nhop.c_str()), intf, metric);
}

ApplicationContainer bulk_tcp_app(std::string d_ip, uint32_t d_port, Ptr<Node> src){
  uint32_t maxBytes = 0;
   BulkSendHelper source ("ns3::TcpSocketFactory", Address (InetSocketAddress (Ipv4Address (d_ip.c_str()), d_port)));
   // Set the amount of data to send in bytes.  Zero is unlimited.
   source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
   source.SetAttribute ("SendSize", UintegerValue (1460));
   ApplicationContainer app = source.Install(src);
   return app;
}

//main 
int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  Config::SetDefault ("ns3::ArpCache::PendingQueueSize", UintegerValue (100));


  /*Inner Ring*/
  int num1 = 12;
  NodeContainer nodes1;
  nodes1.Create (num1);

  PointToPointHelper pointToPoint1;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer devices1;

  for (int x = 0; x < num1-1; ++x)
    {
       devices1.Add (pointToPoint1.Install (nodes1.Get (x), nodes1.Get (x+1)));
    }

    devices1.Add(pointToPoint1.Install (nodes1.Get (num1-1), nodes1.Get (0)));


  InternetStackHelper stack1;
  stack1.Install (nodes1);

  Ipv4AddressHelper address1;
  address1.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
      
  /* On-Off application */
  OnOffHelper clientHelper ("ns3::TcpSocketFactory", Address (InetSocketAddress (Ipv4Address ("10.1.2.2"), 8080)));
  clientHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  clientHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  clientHelper.SetAttribute ("DataRate",StringValue ("2Mbps"));
  clientHelper.SetAttribute ("PacketSize",UintegerValue(1280));
  //ApplicationContainer app = clientHelper.Install(nodes2.Get(0));


  /*Mesh connections*/
  Ipv4AddressHelper address4;
  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("1ms"));

  NodeContainer node_cont;
  std::string temp;
    for(int i = 0; i < num1/2; i++){
        std::ostringstream ss1;
        ss1 << 2*i+2;
        std::ostringstream ss2;
        ss2 << 2*i+3;
        for (int x = 0; x < num1; x++){
            std::ostringstream ss;
            ss << x+1;
            if(x == i){
                continue;
            }
            temp = "10."+ss1.str() + "." + ss.str() + ".0";
            address4.SetBase (temp.c_str(), "255.255.255.0");
            node_cont = NodeContainer(nodes1.Get (i), nodes1.Get (x));
            address4.Assign(pointToPoint2.Install(node_cont));

            if(x!= num1/2 + i){
                temp = "10."+ss2.str() + "." + ss.str() + ".0";
                address4.SetBase (temp.c_str(), "255.255.255.0");
                node_cont = NodeContainer(nodes1.Get (x), nodes1.Get (i+num1/2));
                address4.Assign(pointToPoint2.Install(node_cont));
            }
        }
    }

  //Add static routes
  //ApplicationContainer app1;
  ApplicationContainer app2[num1][num1];
    for(int i = 0; i<num1/2; i++){
        std::ostringstream ss1;
        ss1 << 2*i+2;
        std::ostringstream ss2;
        ss2 << 2*i+3;
        int start = 2*i + 3;
        for (int x = 0; x < num1; x++){
            if(x == i){
                continue;
            }

            std::ostringstream ss;
            ss << x+1;

            std::string temp = "10." + ss2.str()+ "." + ss.str();       //10.3.2.0
            std::string temp1 = "10." + ss1.str()+ "." + ss.str();      //10.2.2.0

            add_static_network_route(nodes1.Get(i), (temp+ ".0").c_str() , "/24", (temp1+ ".2").c_str(), start, 0);
            if(x != i+num1/2){
                add_static_network_route(nodes1.Get(i + num1/2), (temp1+".0").c_str(), "/24", (temp+".1").c_str(), start , 0);
            }
            start++;

            if(x != i + num1/2){
                app2[i][x] = bulk_tcp_app((temp+".2").c_str(), 8080, nodes1.Get(i));
            }else{
                app2[i][x] = bulk_tcp_app((temp1+".2").c_str(), 8080, nodes1.Get(i));
            }

            app2[i][x].Start(Seconds(1.0));
            app2[i][x].Stop(Seconds(3.0));
        }
    }

  /* Add sink */
    NodeContainer nodes2;
    for(int i = 0; i < num1/2; i++){
        nodes2.Add(nodes1.Get(i+num1/2));
    }
  PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 8080));
  ApplicationContainer sinkApp = sinkHelper.Install (nodes2);
  sinkApp.Start (Seconds (0));
  sinkApp.Stop (Seconds (30.0));


    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop (Seconds (10.0));
    Simulator::Run ();

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
    {
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

      //if (t.sourceAddress == Ipv4Address("10.4.3.1") && t.destinationAddress == Ipv4Address("10.5.3.2")){
      //if(iter->first > 0 && iter->first < 12){
          NS_LOG_UNCOND("Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress);
          NS_LOG_UNCOND("Tx Packets = " << iter->second.txPackets);
          NS_LOG_UNCOND("Rx Packets = " << iter->second.rxPackets);
          NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps");
        //}
    }
  monitor->SerializeToXmlFile("lab-5.flowmon", true, true);


    Simulator::Destroy ();
    return 0;
}
