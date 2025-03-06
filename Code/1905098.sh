#!/bin/bash
filename="throughput_bottleneck.txt"
filename2="congestion_window_reno.txt";
filename3="congestion_window_other.txt";
filename4="fairness_reno.txt"

rm -f $filename
rm -f $filename4
touch $filename
touch $filename4
    #ns3::TcpAdaptiveReno
    #ns3::TcpWestwoodPlus
    #ns3::TcpHighSpeed





#filename5="fairness_other.txt"

mkdir -p "VS_TCP_HIGHSPEED"
mkdir -p "VS_TCP_ADAPTIVE_RENO"
mkdir -p "VS_TCP_WEST_WOOD_PLUS"


for i in {1..301..50}  #bottleneck data rate
do
    ../ns3 run "1905098.cc --algo="ns3::TcpHighSpeed" --bottleneckRate=${i} --error=-6"

    if [ $i == 51 ]; 
    then
       gnuplot -c "1905098_2.gnuplot" "VS_TCP_HIGHSPEED/congestion_window_vs_time.png" "congestion window vs time" "${filename2}" "${filename3}" 1 2 1 2 "TCP HIGH SPEED"
    fi

    


    echo "Bottleneck : $i  is done"
done

gnuplot -c "1905098_3.gnuplot" "VS_TCP_HIGHSPEED/fairness_vs_bottleneck_data_rate.png" "fairness vs bottleneck data rate" "bottleneck data rate" "fairness" "${filename4}" 2 1 "TCP NEW RENO + TCP HIGH SPEED"
gnuplot -c "1905098_1.gnuplot" "VS_TCP_HIGHSPEED/throughput_vs_bottleneck_data_rate.png" "throughput vs bottleneck data rate" "bottleneck data rate" "throughput (Kbps)" "${filename}" 3 1 4 "TCP HIGH SPEED"

#gnuplot -c "1905098.gnuplot" "throughput_vs_bottleneck_data_rate.png" "throughput vs bottleneck data rate" "bottleneck data rate2" "throughput (Kbps)" "${filename}" 4 1




rm -f $filename
rm -f $filename4



touch $filename
touch $filename4


for i in {-6..-2}  #bottleneck data rate
do
    ../ns3 run "1905098.cc --algo="ns3::TcpHighSpeed" --error=${i} --bottleneckRate=50"

    

    echo "error model  : $i  is done"
done

gnuplot -c "1905098_3.gnuplot" "VS_TCP_HIGHSPEED/fairness_vs_packet_loss_rate.png" "fairness vs packet loss rate" "packet loss rate" "fairness" "${filename4}" 3 1 "TCP NEW RENO + TCP HIGH SPEED"
gnuplot -c "1905098_1.gnuplot" "VS_TCP_HIGHSPEED/throughput_vs_packet_loss_rate.png" "throughput vs packet loss rate" "packet loss rate" "throughput (Kbps)" "${filename}" 6 1 4 "TCP HIGH SPEED"







rm -f $filename
rm -f $filename4


touch $filename
touch $filename4

##################################


for i in {1..301..50}  #bottleneck data rate
do
    ../ns3 run "1905098.cc --algo="ns3::TcpWestwoodPlus" --bottleneckRate=${i} --error=-6"

    if [ $i == 51 ]; 
    then
       gnuplot -c "1905098_2.gnuplot" "VS_TCP_WEST_WOOD_PLUS/congestion_window_vs_time.png" "congestion window vs time" "${filename2}" "${filename3}" 1 2 1 2 "TCP WEST WOOD PLUS"
    fi

    
    echo "Bottleneck : $i  is done"
done

gnuplot -c "1905098_3.gnuplot" "VS_TCP_WEST_WOOD_PLUS/fairness_vs_bottleneck_data_rate.png" "fairness vs bottleneck data rate" "bottleneck data rate" "fairness" "${filename4}" 2 1 "TCP NEW RENO + TCP WEST WOOD PLUS"

gnuplot -c "1905098_1.gnuplot" "VS_TCP_WEST_WOOD_PLUS/throughput_vs_bottleneck_data_rate.png" "throughput vs bottleneck data rate" "bottleneck data rate" "throughput (Kbps)" "${filename}" 3 1 4 "TCP WEST WOOD PLUS"
#gnuplot -c "1905098.gnuplot" "throughput_vs_bottleneck_data_rate.png" "throughput vs bottleneck data rate" "bottleneck data rate2" "throughput (Kbps)" "${filename}" 4 1




rm -f $filename
rm -f $filename4


touch $filename
touch $filename4


for i in {-6..-2}  #bottleneck data rate
do
    ../ns3 run "1905098.cc --algo="ns3::TcpWestwoodPlus" --error=${i} --bottleneckRate=50"

   

    echo "error model  : $i  is done"
done

 gnuplot -c "1905098_1.gnuplot" "VS_TCP_WEST_WOOD_PLUS/throughput_vs_packet_loss_rate.png" "throughput vs packet loss rate" "packet loss rate" "throughput (Kbps)" "${filename}" 6 1 4 "TCP WEST WOOD PLUS"
 gnuplot -c "1905098_3.gnuplot" "VS_TCP_WEST_WOOD_PLUS/fairness_vs_packet_loss_rate.png" "fairness vs packet loss rate" "packet loss rate" "fairness" "${filename4}" 3 1 "TCP NEW RENO + TCP WEST WOOD PLUS"

rm -f $filename
rm -f $filename4


touch $filename
touch $filename4
#################################################


for i in {1..301..50} #bottleneck data rate
do
    ../ns3 run "1905098.cc --algo="ns3::TcpAdaptiveReno" --bottleneckRate=${i} --error=-6"

    if [ $i == 51 ]; 
    then
       gnuplot -c "1905098_2.gnuplot" "VS_TCP_ADAPTIVE_RENO/congestion_window_vs_time.png" "congestion window vs time" "${filename2}" "${filename3}" 1 2 1 2 "TCP ADAPTIVE RENO"
    fi
    

    echo "Bottleneck : $i  is done"
done
gnuplot -c "1905098_3.gnuplot" "VS_TCP_ADAPTIVE_RENO/fairness_vs_bottleneck_data_rate.png" "fairness vs bottleneck data rate" "bottleneck data rate" "fairness" "${filename4}" 2 1 "TCP NEW RENO + TCP ADAPTIVE RENO"
gnuplot -c "1905098_1.gnuplot" "VS_TCP_ADAPTIVE_RENO/throughput_vs_bottleneck_data_rate.png" "throughput vs bottleneck data rate" "bottleneck data rate" "throughput (Kbps)" "${filename}" 3 1 4 "TCP ADAPTIVE RENO"
#gnuplot -c "1905098.gnuplot" "throughput_vs_bottleneck_data_rate.png" "throughput vs bottleneck data rate" "bottleneck data rate2" "throughput (Kbps)" "${filename}" 4 1




rm -f $filename
rm -f $filename4


touch $filename
touch $filename4


for i in {-6..-2}  #bottleneck data rate
do
    ../ns3 run "1905098.cc --algo="ns3::TcpAdaptiveReno" --error=${i} --bottleneckRate=50"
    
    echo "error model  : $i  is done"
done

gnuplot -c "1905098_1.gnuplot" "VS_TCP_ADAPTIVE_RENO/throughput_vs_packet_loss_rate.png" "throughput vs packet loss rate" "packet loss rate" "throughput (Kbps)" "${filename}" 6 1 4 "TCP ADAPTIVE RENO"
gnuplot -c "1905098_3.gnuplot" "VS_TCP_ADAPTIVE_RENO/fairness_vs_packet_loss_rate.png" "fairness vs packet loss rate" "packet loss rate" "fairness" "${filename4}" 3 1 "TCP NEW RENO + TCP ADAPTIVE RENO"

rm -f $filename
rm -f $filename4


touch $filename
touch $filename4
