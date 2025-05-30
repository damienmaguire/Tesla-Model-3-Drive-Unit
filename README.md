# Tesla-Model-3-Drive-Unit
Open Source logic board for the Tesla Model 3 rear drive unit. PCB Files in DesignSpark 9 and Kicad 8 format.Based on the inverter designed by Johannes Heubner using FOC control.
<br>
![Screenshot from 2025-01-08 18-51-52](https://github.com/user-attachments/assets/b7f1be79-28bd-49b1-98d1-5fdaf6de3171)

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


Please note design source files will be only available via Patreon for a period of time:

https://www.patreon.com/evbmw

02/10/20 : V2 board now fully tested in an M3 Rear drive unit and working very well on the bench.
<br>At this time I have decided to do a full opensource release of all design sources:)
<br>More updates and video to following including testing in a M3 front DU.

18/11/20 : Uploaded spreadsheet work in progress for mapping the pin assignments of the TMS320F28377D MCU.

21/11/20 : Discovered an eeprom on the back of the pcb. Logic analyser captures uploaded. All logic analyser captures made using Saleae logic. Free software available here :https://www.saleae.com/downloads/

02/12/20 : Big thanks to Colin Kidder (https://github.com/collin80) for the explanation of the STGAP1AS SPI gate driver traffic. PDF uploaded to the Gate SPI folder.

11/12/20 : Big thanks again to Colin Kidder (https://github.com/collin80) for the explanation of the TLF SPI power supply traffic. PDF uploaded to the TLF SPI folder.

08/02/21 : We have a new modboard : https://vimeo.com/509198907

08/05/21 : Added V3 modboard pdf schematic and board layout. Turns out Mr.Musk almost made a fool of me....again! 

18/10/21 : Added design files for USB to JTAG adapter board.

19/03/23 : Add first temp sensor calibration data

30/03/23 : Running : https://vimeo.com/809120859

15/08/24 : Added a very early schematic of the gate driver section for use in the PCB replacement project.

24/10/24 : Updated schematic of the gate driver power supply section for use in the PCB replacement project.

17/01/25 : Updated schematic and PCB layout to latest "complete" V1 version.

07/05/25 : First prototypes undergoing test.


![board_jlc_powerup](https://github.com/user-attachments/assets/40a378ad-6385-4834-8268-7bbb4e7ee89f)

28/05/25 : Testing continues. First motor spins :

https://youtu.be/JKbsiIRJ8mo
