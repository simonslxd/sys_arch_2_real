# Implementeringsspesifikasjon: laboratorieutlån-mock-up

## Hensikt og status

Beskriv implementeringen av en statisk, interaktiv mock-up basert på de tre
innsendte UML-diagrammene. Dette dokumentet spesifiserer brukergrensesnittet,
demodataene og den lokale simuleringen. Det endrer ikke klassene, attributtene,
operasjonene, relasjonene eller tilstandene i modellen.

Sluttresultatet skal være en mock-up, ikke et produksjonssystem: ingen backend,
autentisering, lagring eller reell student-/utstyrskatalog.

## Grunnlagsmateriale

- `project/diagrams/input/lab_utlaan_klassediagram(3).drawio`
- `project/diagrams/input/TilstandDiagram.drawio`
- `project/diagrams/input/Sekvens.drawio`
- Avklaringer i `Research/mockup-pages-spec.md`

Klassediagrammet og tilstandsdiagrammet gir domenemodellen og utstyrstilstandene.
Sekvensdiagrammet gir rekkefølgen på utlånsscenarioets meldinger. Ved navne- eller
detaljforskjeller beholdes den relevante tegnede betegnelsen og forskjellen
dokumenteres; navn skal ikke stilltiende normaliseres.

## Avklarte produktkrav

- Én responsiv side med utstyrsoversikt, status og enkle utlåns-/returhandlinger.
- Norsk grensesnitt. Bruk klassediagrammets tekniske navn som vist, inkludert
  `Lab`, `Lab_Utstyr`, `Student` og `Utlaan`.
- Vanlig HTML, CSS og JavaScript, uten rammeverk eller backend.
- Bruk tydelig fiktive, generiske eksempeldata.
- Interaksjoner oppdaterer kun data i nettleserens minne. Oppdatering av siden
  gjenoppretter de opprinnelige eksempeldataene; ikke bruk persistent lagring.
- GitHub Pages publiseres gjennom GitHub Actions ved push til `main`.
- Ikke push kode eller workflow uten brukerens uttrykkelige godkjenning.

## Modellen som skal gjengis

### Klasser, attributter og operasjoner

Modellnavn og signaturer nedenfor skal ikke utvides eller erstattes i
presentasjonen av modellinformasjon.

| Klasse | Attributter | Operasjoner |
|---|---|---|
| `Lab` | `utstyrsliste: Lab_Utstyr[0..*]`; `student_liste: Student[0..*]`; `utlaansliste: Utlaan[0..*]` | `legg_til_utstyr(utstyr: Lab_Utstyr): void`; `registrer_student(student: Student): void`; `laan_ut(student: Student, utstyr: Lab_Utstyr): Utlaan`; `registrer_retur(utlaan: Utlaan): void`; `finn_tilgjengelig_utstyr(utstyr: Lab_Utstyr): Bool` |
| `Lab_Utstyr` | `utstyr_navn: string`; `antall_totalt: int`; `antall_tilgjengelig: int` | `er_tilgjengelig(): bool`; `oek_antall_tilgjengelig(): void`; `reduser_antall_tilgjengelig(): void`; `get_navn(): string` |
| `Student` | `studentnummer: string`; `navn: string`; `mine_laan: Utlån[0..*]` | `legg_til_laan(Utlaan): void`; `fjern_laan(Utlaan): void`; `get_laan(): Utlaan[0..*]` |
| `Utlaan` | `laanetaker: Student`; `utlaant_utstyr: Lab_Utstyr`; `antall: int` | `get_laanetaker(): Student`; `get_utstyr(): Lab_Utstyr`; `get_antall(): int` |

Diagrammet bruker `Utlaan` i operasjoner og relasjoner, men `Utlån` på
`Student.mine_laan`. Behold original skrivemåte i gjengivelse av hvert
modellmedlem. Ikke presenter dette som to separate domeneklasser.

### Relasjoner

Gjengi relasjonene som vist i klassediagrammet, uten å innføre nye typer,
identifikatorer eller relasjoner:

- `Lab`–`Lab_Utstyr`: én `Lab` har null eller flere utstyrselementer
  (`utstyrsliste`); diagrammet tegner aggregasjon.
- `Lab`–`Student`: én `Lab` har null eller flere studenter
  (`student_liste`); diagrammet tegner aggregasjon.
- `Lab`–`Utlaan`: én `Lab` har null eller flere utlån
  (`utlaansliste`); diagrammet tegner komposisjon.
- `Student`–`Utlaan`: én student har null eller flere lån, og hvert tegnet lån
  hører til én student; diagrammet tegner assosiasjon.
- `Utlaan`–`Lab_Utstyr`: ett utlån peker på ett utstyr, og utstyr kan være knyttet
  til null eller flere utlån; diagrammet tegner assosiasjon.

### Tilstandsmaskin

Tilstandsdiagrammet inneholder tilstandene `LEDIG` og `UTLÅNT`:

- `LEDIG` → `UTLÅNT` ved `regUtlaan()`.
- `UTLÅNT` → `LEDIG` ved `regRetur()`.
- Starttilstanden er `LEDIG`.

Ingen andre tilstander eller overganger skal vises eller simuleres.

### Utlånsscenario

Sekvensdiagrammets synlige aktør og livslinjer er `Lab Ansatt`, `Lab`,
`UtlaansObjekter`, `Objekt` og `Studenter`. Tegnede meldinger skal i mock-upens
scenarioforklaring beholdes i denne rekkefølgen:

1. `Lab Ansatt` starter `Laan(Objekt, Student)` på `Lab`.
2. `Lab` kaller `finnObjekt(Objekt)` på `UtlaansObjekter`; returverdien er `Objekt`.
3. `Lab` kaller `erTilgjengelig()` på `Objekt`; returverdien er `Bool`.
4. `Lab` kaller `finnStudent(Student)` på `Studenter`; returverdien er `Student`.
5. `Lab` kaller `settStudentPaaLaan()` på `Objekt`.
6. Sekvensdiagrammet har en stiplet returpil merket `Laan`. I draw.io-XML-en
   peker pilens `target` på `Lab`, ikke på aktøren. Mottaker er derfor
   motstridende/uklar og må avklares ved Human Gate før scenarioet simuleres.

Ikke presenter sekvensdiagrammets livlinjenavn som nye klasser i
klassediagrammet. Behold dem som scenariobetegnelser.

## Side og innhold

Én side skal gi en enkel oversikt over modellens eksempeldata:

1. **Toppfelt:** vis mock-upens tittel og en tydelig merking om at innholdet er
   eksempeldata.
2. **Utstyrsoversikt (`Lab_Utstyr`):** vis `utstyr_navn`, `antall_totalt`,
   `antall_tilgjengelig` og den modellerte tilgjengelighetstilstanden
   (`LEDIG`/`UTLÅNT`).
3. **Student- og utlånsoversikt (`Student`, `Utlaan`):** vis `studentnummer`,
   `navn`, `laanetaker`, `utlaant_utstyr` og `antall` der det er relevant.
4. **Utlån:** la brukeren velge en eksisterende student og et eksisterende
   utstyrselement og starte den modellerte utlånsflyten.
5. **Retur:** la brukeren velge et eksisterende aktivt utlån og starte
   `registrer_retur(utlaan)`.

Bruk de tekniske diagramnavnene på modellseksjonene og statusene. Menneskelesbare
beskrivelser kan stå ved siden av navnene, men må ikke erstatte eller endre
modellbetegnelsene. Utforming, typografi og fargevalg er fri så lenge
presentasjonen er ryddig, responsiv og ikke gir inntrykk av egenskaper som ikke
finnes i modellen.

## Interaksjoner og tilstandsoppdatering

- Utlån skal følge sekvensdiagrammets angitte meldingsrekkefølge og kun kunne
  gjennomføres når `er_tilgjengelig()`/`erTilgjengelig()` gir sant.
- Ved utlån skal mock-upen vise tilstandsovergangen `LEDIG` → `UTLÅNT` via
  `regUtlaan()`.
- Retur av et eksisterende utlån skal vise overgangen `UTLÅNT` → `LEDIG` via
  `regRetur()`.
- Oppdater relevante viste eksempelverdier fra de eksisterende attributtene.
  Ikke introduser nye domeneattributter for å holde UI-tilstanden.
- Etter hver handling skal oversiktene vise oppdatert lokal tilstand. En
  sideoppdatering skal nullstille endringene til de opprinnelige demodataene.
- Ved utilgjengelig utstyr skal utlån ikke gjennomføres; vis en kort norsk
  tilbakemelding. Ikke legg til nye feilkategorier eller forretningsregler.
- UI-et skal bare tilby handlinger diagrammene støtter. Ingen søk, filtrering,
  oppretting/sletting av studenter, historikk, datoer eller innlogging med mindre
  disse kravene senere blir eksplisitt godkjent.

## Teknisk og publisering

- Implementer som statiske filer med HTML, CSS og JavaScript; skal kunne lastes
  uten server-side kjøring.
- Publiser de statiske filene til GitHub Pages med en GitHub Actions-workflow
  som kjører på push til `main`.
- Workflowen skal bruke GitHub Pages' artifact-/deploy-flyt og nødvendige
  rettigheter for Pages-deploy. Ikke legg inn hemmeligheter eller tokens i
  repositoryet.
- Ingen faktisk push, deploy eller endring av diagramfilene inngår i denne
  spesifikasjonen.

## Antakelser og avvik som må være synlige

Punktene under er ikke definert fullstendig av diagrammene. De er eksplisitte
implementeringsantakelser for en brukbar mock-up, og må ikke forveksles med
modellfakta:

1. **Navn på sekvenslivslinjer:** `UtlaansObjekter`, `Objekt` og `Studenter` er
   ikke klasser i klassediagrammet. Mock-upen viser dem kun i forklaringen av
   scenariet. For visning av klassedata brukes `Lab_Utstyr`, `Student` og
   `Utlaan`; dette knytter scenariet til de nærmeste klassediagramrollene, men
   diagrammene sier ikke eksplisitt at navnene er synonymer.
2. **Skrivemåte for lån:** `Utlån` på `Student.mine_laan` antas å være ment som
   den samme klassen som `Utlaan` andre steder i diagrammet. Den opprinnelige
   skrivemåten beholdes der attributtet omtales.
3. **Status kontra antall:** Tilstandsdiagrammet har én binær tilstand for
   utstyr, mens `Lab_Utstyr` har totalt og tilgjengelig antall. For mock-upens
   samlede utstyrspost vises `LEDIG` når `antall_tilgjengelig > 0`, ellers
   `UTLÅNT`. Diagrammene spesifiserer ikke hvordan en enkelt tilstandsmaskin
   gjelder for utstyr med flere eksemplarer.
4. **Utlånt antall:** `laan_ut(student, utstyr)` har ingen antallsparameter,
   mens `Utlaan` har attributtet `antall`. Mock-upen antar at ett utlån låner
   ett eksemplar (`antall = 1`); antall kan ikke velges separat i skjemaet.
5. **Utstyr identifiseres i demodata:** klassediagrammet gir ikke utstyr-ID.
   Hvert eksempelutstyr identifiseres internt av sin unike `utstyr_navn` kun for
   å kunne velges i mock-upen. Dette er en UI-teknisk nøkkel, ikke et nytt
   synlig modellattributt; eksempelnavnene må være unike.
6. **Returvalg:** `registrer_retur` tar et `Utlaan`, men diagrammene definerer
   ikke et returregister eller kriterier for aktivt lån. Mock-upen tilbyr retur
   for de lånene som er opprettet i gjeldende sideøkt, og fjerner det valgte
   lånet fra studentens og Labs viste lånelister.
7. **Demoverdier:** navn, studentnummer, utstyrsnavn og startantall er ikke gitt
   i diagrammene. De er fiktive plassholderverdier, konsistente med typene og
   relasjonene. De skal merkes som eksempeldata og skal ikke ligne ekte
   personopplysninger.
8. **Layout og tilbakemelding:** diagrammene spesifiserer ikke sideoppsett,
   responsiv oppførsel eller tekst etter handlinger. Én side, responsiv
   presentasjon og korte bekreftelser/feilmeldinger er valgt for å oppfylle
   avklarte mock-upkrav; de skal ikke utgi seg for å være domeneoperasjoner.
9. **Sekvensretur:** meldingen `Laan` er tegnet som stiplet retur, men XML-en
   angir `Lab` som pilens mål. Ikke anta at den returneres til aktøren; avklar
   mottakeren før sekvensflyten implementeres.

Hvis produktet senere skal behandle flere eksemplarer per utlån, vedlikeholde
utstyr-ID-er eller definere en entydig kobling mellom sekvens- og klassediagram,
krever det en oppdatert modell eller eksplisitt brukerbeslutning. Ikke utled
ytterligere regler utover antakelsene ovenfor.

## Akseptansekriterier

- Mock-upen er én responsiv norsk side med utstyrsstatus samt utlåns- og
  returhandlinger.
- Klasse-, attributt-, operasjons-, relasjons- og tilstandsnavn samsvarer med
  grunnlagsdiagrammene; navneforskjeller er ikke skjult.
- Utlånsscenarioets meldingsrekkefølge samsvarer med sekvensdiagrammet.
- Utlån/retur oppdaterer kun lokal demotilstand og gir de tegnede
  tilstandsovergangene; refresh gjenoppretter startdata.
- All data er tydelig fiktiv; ingen backend eller persistent lagring finnes.
- Pages-workflowen er konfigurert for push til `main`. Ingen endring pushes uten
  uttrykkelig godkjenning.

## Kildesporbarhet og åpne modellspørsmål

| Implementeringsområde | Kilde | Hva som kan spores | Avgrensning eller åpent spørsmål |
|---|---|---|---|
| Modellklasser og medlemmer | `project/diagrams/input/lab_utlaan_klassediagram(3).drawio` | Klassene `Lab`, `Lab_Utstyr`, `Student` og `Utlaan`, samt viste attributter, operasjoner, navn og relasjoner | `Student.mine_laan` bruker typen `Utlån`, mens resten bruker `Utlaan`; behold skrivemåten fra diagrammet og ikke opprett to klasser uten godkjenning. |
| Utstyrstilstand | `project/diagrams/input/TilstandDiagram.drawio` | Starttilstand `LEDIG`; `regUtlaan()` fører til `UTLÅNT`; `regRetur()` fører tilbake til `LEDIG` | Diagrammet fastsetter ikke hvordan binær tilstand skal gjelde for en lagerpost med flere eksemplarer. |
| Utlånsscenario | `project/diagrams/input/Sekvens.drawio` | Lifeline-navnene `Lab`, `UtlaansObjekter`, `Objekt`, `Studenter`; meldingsetikettene `Laan(Objekt, Student)`, `finnObjekt(Objekt)`, `erTilgjengelig()`, `finnStudent(Student)`, `settStudentPaaLaan()` og `Laan` | Sekvenslivslinjene samsvarer ikke med klassene. Den stiplede `Laan`-returpilen har XML-målet `Lab`, ikke aktøren. Mottaker og kobling til klassene krever eksplisitt godkjenning. |
| Utlånt mengde og retur | Klassediagrammet og tilstand/sekvensdiagrammene | `Utlaan.antall`, utlåns- og returrelaterte operasjoner/overganger | `laan_ut` tar ikke antall; eksemplar-ID og returidentifikasjon er ikke modellert. Én enhet per lån og retur bare av øktens lån er dokumenterte antakelser, ikke kildedefinerte regler. |
| Side, demodata og deploy | `Research/mockup-pages-spec.md` og brukeravklaringer | Én norsk side, eksempeldata, lokal tilstand som resettes ved refresh, `src/`, GitHub Pages ved push til `main` | Utforming og verdier er ikke definert i UML. Ingen push eller deploy uten separat godkjenning. |

Samsvarskontroll skal rapportere hvert ekstra UI-konsept, manglende modellfelt,
endret navn, endret meldingsrekkefølge og hver uavklart fortolkning. Punktene i
siste kolonne er ikke avgjort av UML-kildene og må ikke skjules som modellfakta.
