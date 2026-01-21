# Next Bus
Visar när nästa buss går på en GeekMagic Ultra<p>

Kräver api-nyckel för ResRobot v2.1 https://developer.trafiklab.se/project/list<p>
Sök hållplats-ID här: http://www.windflag.se/buss.php<p>

<h1>Uppdatera din GeekMagic i två steg</h1>
GeekMagic går inte att programmera via usb-porten. Enklaste sätttet är därför att göra det OTA (over the air) i webbinterfacet. Om du har kört enheten som väderstation måste vi först rensa EEPROM så att enheten blir helt ren och därmed inte blir korrupt. <p></p>
Kör därför först cleaner-firmware.bin och i steg två flashar vi buss.bin<p></p>
Jag använder WifiManager för att ge enheten en IP-adress och för att lägga till den på ditt wifi-nätverk. Kolla under accesspunkter så dyker enheten upp.
