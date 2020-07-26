# Tesla-Model-3-Drive-Unit
Open Source logic board for the Tesla Model 3 rear drive unit. PCB Files in DesignSpark 8 format.Based on the inverter designed by Johannes Heubner using slip control already in use on Model S large and small drive units.
<br>
<br>
13/03/20 : Added CAN logs from both CAN busses from the rear drive unit inverter powered on the bench.

04/05/20 : Added pdf schematic and board layout for the V1 logic board design. Untested as of this date. Given recent developments the design files will not be released at this time. For more information see : <br>
https://openinverter.org/forum/viewtopic.php?f=10&t=575


26/07/20 : After initial testing of V1 in a rear drive unit inverter some issues were found. V2 now released. Changes :

-Added resolver signal preconditioning circuit. Resolver signal needs to be amplified to 1v p-p and offset to 1.6v before feeding to the Tesla resolvver power amp.

-Added opamp to combine differential HV voltage measurement to unipolar.

-Deleted Tagconnect Jtag.

-Added 3v3 pin for single wire programming via st-link.

-Added solder jumper for optional single 5v power supply.

