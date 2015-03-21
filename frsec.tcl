
#Create a simulator object
set ns [new Simulator]

#Define different colors for data flows (for NAM)
$ns color 1 Blue
$ns color 2 Red

#Open a trace file
set nf [open outsec.nam w]
$ns namtrace-all $nf

#Define a 'finish' procedure
proc finish {} {
        global ns nf
        $ns flush-trace
        close $nf
        exec nam outsec.nam &
        exit 0
}

#Create four nodes
set n0 [$ns node]
set n1 [$ns node]
set n2 [$ns node]
set n3 [$ns node]
set n4 [$ns node]
set n5 [$ns node]

#Create links between the nodes
$ns duplex-link $n0 $n2 5Mb 10ms DropTail
$ns duplex-link $n1 $n2 5Mb 10ms DropTail
$ns duplex-link $n2 $n3 1.5Mb 10ms DropTail
$ns duplex-link $n3 $n4 5Mb 10ms DropTail
$ns duplex-link $n3 $n5 5Mb 10ms DropTail

#Set Queue Size of link (n2-n3) to 100
$ns queue-limit $n2 $n3 100

$ns duplex-link-op $n0 $n2 orient right-down
$ns duplex-link-op $n1 $n2 orient right-up
$ns duplex-link-op $n2 $n3 orient right
$ns duplex-link-op $n3 $n4 orient right-up
$ns duplex-link-op $n3 $n5 orient right-down


#Create two security agents and attach them to the nodes n0 and n2
set p0 [new Agent/Security_packet]
$ns attach-agent $n0 $p0
$p0 set class_ 1


set p3 [new Agent/Security_packet]
$ns attach-agent $n5 $p3
$p3 set class_ 2

#Connect the two agents
$ns connect $p0 $p3
#$ns connect $p1 $p2


#Schedule events

$ns at 0.5 "$p0 send srm"
$ns at 1.0 "finish"

#Run the simulation
$ns run
