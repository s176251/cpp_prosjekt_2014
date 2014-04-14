Prosjektoppgave C++ 2014
================

####Martin(s176251) og Jonas(s176250)

##Ide
� lage et peer-to-peer chatte-program i likhet med msn/skype/irc, etc., men serverl�st.
S� lenge to maskiner har programmet kj�rende skal man kunne sende meldinger kun ved � skrive inn ip'en,
evt. dele en "profil" generert av programmet som man deler via dropbox, etc.

##Beskrivelse av hovedfunksjonaliteten

####Minimumsm�l
* Skal kunne broadcaste meldinger p� LAN og lytte p� broadcastede meldinger.(ergo ingen ip n�dvendig p� samme nettverk) 
* Skal kunne scanne LAN (pinge all iper) og deretter sjekke om de har programmet kj�rende.
    * Disse skal vises i en liste i programmet
* Man skal kunne sende til _alle_, en gruppe eller �n.
* Velge om man skal kommunisere via UDP eller TCP.
* Ha ulike samtaler i ulike faner, el.l.
* GUI i qt

####Utvidede m�l
* Kommunisere via internet.
* Lagre bruker med navn og ip, og mulighet for � endre/slette bruker.
* Indikator "farge/lys" ved siden av en bruker som indikerer om vedkommende er "p�".
* Velge om meldingene skal krypteres eller ikke
* Ha tjeneste/daemon kj�rende(uavhengig av programmet) som "snapper"-opp tilsendte meldinger(valgfritt).
* Mulighet for sending av meldinger utenom GUI.

####BHAG (Big Hairy Audacious Goals)
* VoIP (lite sannsynlig)
* text-to-speech
* mulighet 

####Designm�l
1. Brukervennlighet(innenfor m�lgruppen aka. ikke bestemor.) 
2. Effektivitet (men ikke p� bekostning av brukervennlighet)
3. Lett utvidbart (men ikke p� bekostning av effektivitet)


------
Bra prosjekt - men: det blir utfordrende å få dette til uten at det oppfattes som portscanning. Husk at på skolen er alle IP'er offentlige (selv de man får på trådløst nett) og hvis man da sniffer porter for å sjekke om de har noe som lytter på en bestemt port blir det trøbbel. Det blir da også litt utfordrende for meg å teste løsningen, men jeg kan gjøre det ved å sette opp to VM'er. 

En alternativ løsning kunne være at man må vite om IP'en eller url'en (bruk feks. dyndns) til den man vil chatte med, og at man så bare prøver å connecte. Får man ikke kontakt vil forbindelsen bare time ut.

Uansett et bra prosjekt, gå videre med det.


- Alfred

