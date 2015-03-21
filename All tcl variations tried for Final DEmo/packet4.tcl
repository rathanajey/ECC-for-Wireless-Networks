#======================================================================
# Define options
# ======================================================================

set val(chan)       Channel/WirelessChannel
set val(prop)       Propagation/TwoRayGround
set val(netif)      Phy/WirelessPhy
set val(mac)        Mac/802_11
set val(ifq)        Queue/RED
set val(ll)         LL
set val(ant)        Antenna/OmniAntenna
set val(x)              600   ;# X dimension of the topography
set val(y)              600   ;# Y dimension of the topography
set val(ifqlen)         2000          ;# max packet in ifq
set val(adhocRouting)   AODV
set val(nn)             20            ;# how many nodes are simulated 
set val(stop)         15.0           ;# simulation time

# =====================================================================
# Main Program
# ======================================================================

#
# Initialize Global Variables
#
 
# create simulator instance

set ns_		    [new Simulator]

# setup topography object

set topo	[new Topography]

# create trace object for ns and nam

set tracefd	[open out.tr w]
set namtrace    [open out.nam w]

$ns_ trace-all $tracefd
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)


# define topology
$topo load_flatgrid $val(x) $val(y)


#
# Create God
#
set god_ [create-god $val(nn)]


#
# define how node should be created
#

#global node setting

$ns_ node-config -adhocRouting AODV \
                 -llType $val(ll) \
                 -macType $val(mac) \
                 -ifqType $val(ifq) \
                 -ifqLen $val(ifqlen) \
                 -antType $val(ant) \
                 -propType $val(prop) \
                 -phyType $val(netif) \
                 -channelType $val(chan) \
		 -topoInstance $topo \
		 -agentTrace ON \
                 -routerTrace ON \
		# -energyModel EnergyModel \
		#-initialEnergy  \
		# -rxPower  \
		# -txPower  \
                 -macTrace OFF 


#
#  Create the specified number of nodes [$val(nn)] and "attach" them
#  to the channel. 



for {set i 0} {$i < $val(nn) } {incr i} {
	set node_($i) [$ns_ node]	
			
}
$node_(0) set X_ 399
$node_(0) set Y_ 197
$node_(0) set Z_ 0.0

$node_(1) set X_ -263
$node_(1) set Y_ 69
$node_(1) set Z_ 0.0

$node_(2) set X_ 13
$node_(2) set Y_ 223
$node_(2) set Z_ 0.0

$node_(3) set X_ -81
$node_(3) set Y_ 42
$node_(3) set Z_ 0.0

$node_(4) set X_ 382
$node_(4) set Y_ -1.0
$node_(4) set Z_ 0.0

$node_(5) set X_ 352
$node_(5) set Y_ 385
$node_(5) set Z_ 0.0

$node_(6) set X_ 344
$node_(6) set Y_ 493
$node_(6) set Z_ 0.0

$node_(7) set X_ -200
$node_(7) set Y_ 502
$node_(7) set Z_ 0.0

$node_(8) set X_ -245
$node_(8) set Y_ 338
$node_(8) set Z_ 0.0

$node_(9) set X_ -72
$node_(9) set Y_ 404
$node_(9) set Z_ 0.0

$node_(10) set X_ 41
$node_(10) set Y_ 540
$node_(10) set Z_ 0.0

$node_(11) set X_ 103
$node_(11) set Y_ 436
$node_(11) set Z_ 0.0

$node_(12) set X_ 151
$node_(12) set Y_ 36
$node_(12) set Z_ 0.0

$node_(13) set X_ 594
$node_(13) set Y_ 144
$node_(13) set Z_ 0.0

$node_(14) set X_ 565
$node_(14) set Y_ 327
$node_(14) set Z_ 0.0

$node_(15) set X_ -307
$node_(15) set Y_ 210
$node_(15) set Z_ 0.0

$node_(16) set X_ -156
$node_(16) set Y_ 190
$node_(16) set Z_ 0.0

$node_(17) set X_ -152
$node_(17) set Y_ 244
$node_(17) set Z_ 0.0

$node_(18) set X_ -403
$node_(18) set Y_ 417
$node_(18) set Z_ 0.0

$node_(19) set X_ -164
$node_(19) set Y_ 317
$node_(19) set Z_ 0.0
 





# Define node initial position in nam

for {set i 0} {$i < $val(nn) } {incr i} {

    $ns_ initial_node_pos $node_($i) 30
}


#
# Tell nodes when the simulation ends
#
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop).0 "$node_($i) reset";
}

#packet Transferring

set tcp [new Agent/TCP]
$tcp set class_ 2
set sink [new Agent/TCPSink]
$ns_ attach-agent $node_(1) $tcp
$ns_ attach-agent $node_(11) $sink
$ns_ connect $tcp $sink
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ns_ at 0.3 "$ftp start" 



$ns_ at 0.0 "$node_(1) label SOURCE"
$ns_ at 0.0 "$node_(11) label DESTINATION"
$ns_ at 0.0 "$node_(1) add-mark m pink circle"
$ns_ at 0.0 "$node_(11) add-mark m blue circle"

$ns_ at $val(stop) "$ns_ nam-end-wireless $val(stop)"
$ns_ at $val(stop).01 "stop"
$ns_ at  $val(stop).0002 "puts \"NS EXITING...\" ; $ns_ halt"

puts $tracefd "M 0.0 nn $val(nn) x $val(x) y $val(y) rp "
puts $tracefd "M 0.0 prop $val(prop) ant $val(ant)"

proc stop {} {
global ns tracefd namtrace
$ns flush-trace
close $tracefd
close $namtrace
}
puts "Starting Simulation..."
$ns_ run
