# Spoje

## Hardware

### Napajeni

vsechna ctyri napeti maji svoji indikaci na prednim panelu a svuj vypinac

na vstupnich 230VAC je pojistka

POZOR! IEC konektor na vstupu nema standardizovane zapojeni, resp. ruzne kabely jsou ruzne. je tedy treba mit na pameti, ze L a N mohou byt prohozene (a nikdo s tim nic neudela)

### DC

Meanwell RD-35B (+5/+24V)

5V napaji arduino

24V napaji telefonni linku

### Trafo

pouziva se pro vyzvaneci AC

je to trafo 400->110, tedy 230->63 voltu, coz je optimalni

### Zapojeni

viz `spoje_pcb` -- projekt v kicadu

v 'doc/` je vyexportovane schema a PCB v kicadu

vsechny draty jsou popsane

## Ovladani

### Vypinace

napajeni ctyr napeti (master 230VAC, vyzvaneci trafo, dva vystupy z DC zdroje)

cervene reset tlacitko (hardware reset na RESET pinu)

### Zvoneni

zluta tlacitka jsou na vyzvaneni na prislusnych linkach. je to SW zvoneni, aktivace stavu POKE.

## Literatura

specifikace vyzvanecich tonu:

https://www.ctu.cz/sites/default/files/obsah/clanky/navrh-sitoveho-planu-signalizace-verejnych-telekomunikacnich-siti-poskytujicich-verejne-dostupnou/soubory/priloha-7-specifikace-tonu-1113656123.pdf

https://vyvoj.hw.cz/navrh-obvodu/rozhrani/telefonni-linky-v-cr-a-sr.html

https://www.ranecommercial.com/legacy/note150.html

http://home.zcu.cz/~vargaj/U%20S%20T/Telefonni%20pristroje.pdf

http://mcu.cz/news.php?extend.8
http://mcu.cz/images/newspost_images/00/26/00-26.pdf

https://forum.arduino.cc/t/intercom-using-two-old-dial-telephones/606457