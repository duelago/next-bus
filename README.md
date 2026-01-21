# Next Bus
Visar när nästa buss går på en GeekMagic Ultra<p>

Kräver api-nyckel för ResRobot v2.1 https://developer.trafiklab.se/project/list<p>
Sök hållplats-ID här: http://www.windflag.se/buss.php<p>

<h1>Uppdatera din GeekMagic i två steg</h1>
GeekMagic går inte att programmera via usb-porten. Enklaste sätttet är därför att göra det OTA (over the air) i webbinterfacet. Om du har kört enheten som väderstation måste vi först rensa EEPROM så att enheten blir helt ren och därmed inte blir korrupt. Har du inte kört enheten som väderstation går det bra att flasha korrekt firmware direkt. <p></p>
Kör därför först <b>cleaner-firmware.bin</b> och gå in i det nya webinterfacet och välj att rensa EEPROM-inställningarna. I steg två flashar du <b>buss.bin</b><p></p>
Jag använder WifiManager för att ge enheten en IP-adress och för att lägga till den på ditt wifi-nätverk. Kolla under accesspunkter så dyker enheten upp.<p></p>
IP-adressen till webinterfacet för att konfigurera enheten syns på bildskärmen under boot. Navigera till denna adress i en browser med http (ej https). Ofta något i stil med http://192.168.1.123 (<- Fast inte exakt så här) 
