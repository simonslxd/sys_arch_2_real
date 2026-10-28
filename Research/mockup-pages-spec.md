# Krav og avklaringer: mock-up for laboratorieutlån

## Formål

Lage en statisk mock-up med plassholdere og tydelig fiktive eksempeldata for et
system som viser og administrerer utlån av laboratorieutstyr. Mock-upen skal kunne
publiseres på GitHub Pages.

## Avklarte krav

- Én responsiv side med utstyrsoversikt, status og enkle handlinger for utlån og retur.
- Bruk vanlig HTML, CSS og JavaScript.
- Grensesnittet skal være på norsk.
- Bruk diagrammenes tekniske navn i grensesnittet, også der navngivningen avviker
  mellom diagrammene.
- Bruk generiske, tydelig fiktive eksempeldata.
- Utlån og retur skal oppdatere demodata lokalt i nettleseren.
- Data skal ikke bevares ved oppdatering av siden; oppdatering tilbakestiller
  demodataene.
- Ingen backend er nødvendig for mock-upen.
- GitHub Actions skal publisere til GitHub Pages ved push til `main`.
- Det skal ikke pushes endringer uten brukerens uttrykkelige godkjenning.
- Lagre denne kravoppsummeringen i `reseach/`.

## Oppsummering av diagrammene

Diagrammene ble lest som XML fra:

- `project/diagrams/input/lab_utlaan_klassediagram(3).drawio`
- `project/diagrams/input/TilstandDiagram.drawio`
- `project/diagrams/input/Sekvens.drawio`

### Klassediagram

Klassene som vises er:

- `Lab`: lister over utstyr, studenter og utlån; metoder for å legge til utstyr,
  registrere studenter, låne ut, registrere retur og finne tilgjengelig utstyr.
- `Lab_Utstyr`: navn, totalt antall og tilgjengelig antall; metoder for å sjekke
  tilgjengelighet og øke/redusere tilgjengelig antall.
- `Student`: studentnummer, navn og egne lån; metoder for å legge til, fjerne og
  hente lån.
- `Utlaan`: låntaker, utlånt utstyr og antall; metoder for å hente disse verdiene.

Diagrammet viser også relasjoner mellom disse klassene, med multipliciteter.
UI-en skal bruke diagrammenes klassenavn der de vises.

### Tilstandsdiagram

Tilstandene for utstyr er `LEDIG` og `UTLÅNT`. Overgangene er merket
`regUtlaan()` fra ledig til utlånt og `regRetur()` fra utlånt til ledig.
Dette gir grunnlag for statusvisning og de enkle utlåns-/returhandlingene i
mock-upen.

### Sekvensdiagram

Sekvensen viser en `Lab Ansatt` som starter `Laan(Objekt, Student)` i `Lab`.
Deretter kaller `Lab` `finnObjekt(Objekt)` på `UtlaansObjekter`,
`erTilgjengelig()` på `Objekt` og `finnStudent(Student)` på `Studenter`.
Diagrammet viser også `settStudentPaaLaan()` på `Objekt`.

Sekvensdiagrammets lifeline-navn (`UtlaansObjekter`, `Objekt` og `Studenter`)
avviker fra klassediagrammets navn (`Lab`, `Lab_Utstyr`, `Student` og `Utlaan`).
Avklart valg for mock-upen er å speile diagrammenes tekniske navn fremfor å
normalisere dem til ett felles begrepssett.

## Publisering

GitHub Actions-workflowen skal konfigureres for GitHub Pages med `main` som
push-trigger. Selve pushingen av endringene er en separat handling som krever
uttrykkelig godkjenning.

## Avgrensning

Dette dokumentet lagrer kravene og diagramoppsummeringen; det innebærer ikke at
mock-upen, workflowen eller en publisering er opprettet. Diagrammene er lest
som XML, men layout, tekstplassering og linjeføring er ikke visuelt kontrollert
ved rendering.
