# Next Bus
Visar när nästa buss går på en GeekMagic Ultra https://www.aliexpress.com/item/1005010417578863.html<p>

Kräver api-nyckel för ResRobot v2.1 https://developer.trafiklab.se/project/list<p>
Sök hållplats-ID här: http://www.windflag.se/buss.php<p>

<h1>Känd begränsning</h1>
Använd inte skärmen för att visa angångar från <b>Stockholm Central</b>, <b>Malmö Central</b> och andra riktigt stora knutpunkter. Det är för mycket data för den lille mikrokontrollern. Du kan dock visa en avgång i riktning Malmö Central

<h1>Uppdatera din GeekMagic i två steg</h1>
GeekMagic går inte att programmera via usb-porten. Enklaste sätttet är därför att göra det OTA (over the air) i webbinterfacet. Det är lämpligt att först rensa EEPROM så att enheten blir helt ren och därmed inte blir korrupt, samt att vi säkerställer att vi har tillräckligt med plats att ladda upp vår mjukvara.<p></p>
Kör därför först <b>cleaner-firmware.bin</b> och gå in i det nya webinterfacet och välj att rensa EEPROM-inställningarna. I steg två flashar du <b>buss.bin</b><p></p>
Jag använder WifiManager för att ge enheten en IP-adress och för att lägga till den på ditt wifi-nätverk. Kolla under accesspunkter så dyker enheten upp.<p></p>
IP-adressen till webinterfacet för att konfigurera enheten syns på bildskärmen under boot. Navigera till denna adress i en browser med http (ej https). Ofta något i stil med http://192.168.1.123 (<- Fast inte exakt så här) 
<p></p>
<h1>Webbinterfacet</h1>
Fyll i din <b></b>api-nyckel</b> som du skaffat dig via <b>ResRobot</b>.<p>
Därefter skriver du in din hållplats <b>StationsID</b> som är ett nummer. Jag har byggt en webbplats som det länkas till i Webbinterfacet där du kan hitta ID till din hållplats<p>
  Sen behöver vi veta i vilken riktning du reser. Har bussen slutdestination Ystad så är det orten du matar in som <b></b>destination</b></p></p>
  Mjukvaran visar när nästa buss även vilken linje som nästa ankommande buss trafikerar (150 i exemplet här)<br>
Nattläget sparar på API-anrop och stänger ner skärmen så den blir svart. Det går fortfarande att nå webbinterfacet.
<img width="907" height="960" alt="vellinge" src="https://github.com/user-attachments/assets/a25df43a-ed15-484c-9ec5-5721656abf2e" />
