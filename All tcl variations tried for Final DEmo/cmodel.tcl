# Credit System(Queie Limit Adjustment) with Pareto Model to Calculate RTO

# ======================================================================
# Define options
# ======================================================================
 set val(chan)         Channel/WirelessChannel  ;# channel type
 set val(prop)         Propagation/TwoRayGround ;# radio-propagation model
 set val(ant)          Antenna/OmniAntenna      ;# Antenna type
 set val(ll)           LL                       ;# Link layer type
 set val(ifq)          Queue/DropTail/PriQueue  ;# Interface queue type
 set val(ifqlen)       50                       ;# max packet in ifq
 set val(netif)        Phy/WirelessPhy          ;# network interface type
 set val(mac)          Mac/802_11               ;# MAC type
 set val(nn)           3                        ;# number of mobilenodes
 set val(rp)	       DSDV                    ;# routing protocol
 set val(x)            800
 set val(y)            800

set ns [new Simulator]


#ns-random 0

set f [open 1_out.tr w]
$ns trace-all $f
set namtrace [open 1_out.nam w]
$ns namtrace-all-wireless $namtrace $val(x) $val(y)
set f0 [open proj_out0.tr w]
set f1 [open proj_out1.tr w]
set f2 [open proj_out2.tr w]
set f3 [open proj_out3.tr w]

set topo [new Topography]
$topo load_flatgrid 800 800

create-god $val(nn)

set chan_1 [new $val(chan)]
set chan_2 [new $val(chan)]


# CREATE NODES

$ns node-config  -adhocRouting $val(rp) \
 		 -llType $val(ll) \
                 -macType $val(mac) \
                 -ifqType $val(ifq) \
                 -ifqLen $val(ifqlen) \
                 -antType $val(ant) \
                 -propType $val(prop) \
                 -phyType $val(netif) \
                 #-channelType $val(chan) \
                 -topoInstance $topo \
                 -agentTrace ON \
                 -routerTrace ON \
                 -macTrace ON \
                 -movementTrace OFF \
                 -channel $chan_1   \
                 -channel $chan_2   

proc finish {} {
	global ns f f0 f1 namtrace 
	$ns flush-trace
        close $namtrace   
	close $f0
        close $f1
        #exec xgraph proj_out0.tr proj_out1.tr proj_out2.tr proj_out3.tr 
        exec nam -r 5m 1_out.nam &
	exit 0
}

proc record {} {
  global sink0 sink1 f0 f1 
   #Get An Instance Of The Simulator
   set ns [Simulator instance]
   
   #Set The Time After Which The Procedure Should Be Called Again
   set time 0.05
   #How Many Bytes Have Been Received By The Traffic Sinks?
   set bw0 [$sink0 set npkts_]
   set bw1 [$sink0 set nlost_]
   
   #Get The Current Time
   set now [$ns now]
   
   #Save Data To The Files
   puts $f0 "$now [expr $bw0]"
   puts $f1 "$now [expr $bw1]"

   #Re-Schedule The Procedure
   $ns at [expr $now+$time] "record"
  }
 
# define color index
$ns color 0 blue
$ns color 1 red
$ns color 2 chocolate
$ns color 3 red
$ns color 4 brown
$ns color 5 tan
$ns color 6 gold
$ns color 7 black
                        
set n(0) [$ns node]
$ns at 0.0 "$n(0) color red"
$n(0) color "0"
$n(0) shape "circle"
$n(0) label "Sender(0)"

set n(1) [$ns node]
$ns at 0.0 "$n(1) color green"
$n(1) color "green"
$n(1) shape "circle"
$n(1) label "Receiver(0)"

set n(2) [$ns node]
$ns at 0.0 "$n(2) color blue"
$n(2) color "blue"
$n(2) shape "circle"
$n(2) label "Dummy"

proc LChange {} {
    puts "Hello, Welcome"
}
for {set i 0} {$i < $val(nn)} {incr i} {
	$ns initial_node_pos $n($i) 30+i*100
}


$ns at 0.0 "$n(0) setdest 100.0 200.0 3000.0"
$ns at 0.0 "$n(1) setdest 250.0 200.0 3000.0"


# CONFIGURE AND SET UP A FLOW

#define the layout topology

set sink0 [new Agent/LossMonitor]

#set sink1 [new Agent/LossMonitor]
#set sink2 [new Agent/LossMonitor]
#set sink3 [new Agent/LossMonitor]
#$ns attach-agent $n(0) $sink0

$ns attach-agent $n(1) $sink0

#$ns attach-agent $n(2) $sink2
#$ns attach-agent $n(3) $sink3


#$ns attach-agent $sink2 $sink3



set tcp0 [new Agent/TCP]
$ns attach-agent $n(0) $tcp0

set tcp1 [new Agent/TCPSink]
$ns attach-agent $n(1) $tcp1


#Connect the Traffic Sources with the Traffic Sinks
#$ns connect $tcp0 $tcp1 



#Create FTP Applications and attach them to agents

#set ftp1 [new Application/FTP]
#$ftp1 attach-agent $tcp0 

proc attach-CBR-trafficEFree { node sink size interval } {
   #Get an instance of the simulator
   set ns [Simulator instance]
   
   #Create a CBR  agent and attach it to the node
   set cbr [new Agent/CBR]
   $ns attach-agent $node $cbr
 
   $cbr set packetSize_ $size
   $cbr set interval_ $interval
   $cbr set window_ 50
   #Attach CBR source to sink;
   $ns connect $cbr $sink

   return $cbr
  }

proc attach-CBR-trafficError1 { node sink size interval wsize qlimit } {
     #Get an instance of the Simulator
     set ns [Simulator instance]
    
     #Create a CBR agent and attach it to the node
     set cbr [new Agent/CBR]
     $ns attach-agent $node $cbr
     #$ns queue-limit $node $sink 10
     $cbr set packetSize_ $size
     $cbr set interval_ $interval
     $cbr set window_ $wsize
     
     $sink set packetSize_ 20 
     #Attach CBR Source to sink
     $ns connect $cbr $sink 
}

#set cbr0[attach-CBR-traffic $n(0) $sink0 Packetsize interval wsize

set cbr0 [attach-CBR-trafficEFree $n(0) $sink0 1000 .015]
set cbr1 [attach-CBR-trafficError1 $n(0) $sink0 2750 .01279 3 4]
set cbr2 [attach-CBR-trafficError1 $n(0) $sink0 3000 .014405 6 4]
set cbr3 [attach-CBR-trafficError1 $n(0) $sink0 3500 .015011 7 4]
set cbr4 [attach-CBR-trafficError1 $n(0) $sink0 1000 .015518 8 8]
set cbr5 [attach-CBR-trafficError1 $n(0) $sink0 1500 .015951 9 8]
set cbr6 [attach-CBR-trafficEFree $n(0) $sink0 1000 .015]


$ns at 0.0 "record"

#Error Free Channel Setup
$ns at 0.0 "$n(2) label \"Error Free\""
$ns at 0.5 "$cbr0 start"
$ns at 1.5 "$cbr0 stop"

#Error - Startup
$ns at 1.51 "$n(2) label \"cbr1\""
$ns at 1.51 "$cbr1 start"
$ns at 2.5 "$cbr1 stop"

$ns at 2.51 "$n(2) label \"cbr2\""
$ns at 2.51 "$cbr2 start"
$ns at 3.5 "$cbr2 stop"

$ns at 3.51 "$n(2) label \"cbr3\""
$ns at 3.51 "$cbr3 start"
$ns at 4.0 "$cbr3 stop"

$ns at 4.6 "$n(2) label \"cbr4\""
$ns at 4.6 "$cbr4 start"
$ns at 5.1 "$cbr4 stop"

$ns at 5.2 "$n(2) label \"cbr5\""
$ns at 5.2 "$cbr5 start"
$ns at 6.0 "$cbr5 stop"

#Error - Error Free Channel Setup
$ns at 6.1 "$n(2) label \"Error Free\""
$ns at 6.1 "$cbr6 start"
$ns at 8.5 "$cbr6 stop"

$ns at 0.0 "$n(2) setdest 50.0 700.0 3000.0"
$ns at 0.5 "$n(0) setdest 400.0 150.0 3000.0"
$ns at 1.51 "$n(0) setdest 410.0 160.0 3000.0"
$ns at 2.51 "$n(0) setdest 420.0 170.0 3000.0"
$ns at 3.51 "$n(0) setdest 430.0 180.0 3000.0"
$ns at 4.6 "$n(0) setdest 440.0 190.0 3000.0"
$ns at 5.2 "$n(0) setdest 450.0 200.0 3000.0"
$ns at 6.1 "$n(0) setdest 460.0 210.0 3000.0"
$ns at 8.5 "finish"

puts "Start of simulation.."
$ns run

