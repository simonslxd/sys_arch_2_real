# Implementeringsplan: laboratorieutlån-mock-up

## Mål

Lage én responsiv, norsk mock-up med fiktive eksempeldata, basert på
klassediagrammet, tilstandsdiagrammet, sekvensdiagrammet og den godkjente
spesifikasjonen. Samsvar med kildematerialet er kritisk. Alle avvik skal
identifiseres, dokumenteres og rapporteres før arbeidet erklæres ferdig.

Koden legges i `src/`. Mock-upen bruker vanlig HTML, CSS og JavaScript uten
backend eller persistent lagring. GitHub Actions skal konfigureres for GitHub
Pages ved push til `main`. Ingen endring pushes eller deployes uten separat,
uttrykkelig godkjenning.

## Styring og beslutningsregler

- Etter hver fase stopper arbeidet ved en **Human Gate**. Neste fase starter
  først når brukeren uttrykkelig har godkjent resultatet fra den foregående.
- Mindre presentasjonsjusteringer kan gis som tekstlige instruksjoner.
- En beslutning som påvirker modellfortolkning, funksjonell atferd, brukerflyt,
  teknologi/arkitektur eller publisering/sikkerhet krever en ADR med kontekst,
  alternativer, beslutning, konsekvenser og kildegrunnlag. ADR-en må godkjennes
  før beslutningen implementeres.
- `AGENTS.md` skal inneholde en vedvarende, generell regel om kildesamsvar for
  alle implementeringer i repositoryet. Den regelen supplerer eksisterende
  C++/UML-instruksjoner.
- Kravdokument: `project/process/specs/mockup-pages.md`.
- Utforsknings- og avklaringsdokument: `Research/mockup-pages-spec.md`.
- UML-kilder: `project/diagrams/input/lab_utlaan_klassediagram(3).drawio`,
  `project/diagrams/input/TilstandDiagram.drawio` og
  `project/diagrams/input/Sekvens.drawio`.

## Faser

### Fase 0 — Kildesporbarhet og varige instruksjoner

- Kryssjekk alle krav mot de tre UML-kildene; skill modellfakta fra antakelser.
- Lag en sporbarhetsoversikt for klasser, felt, operasjoner, relasjoner,
  tilstandsoverganger og sekvensmeldinger.
- Oppdater `AGENTS.md` med regelen om kildefidelitet og obligatorisk rapportering
  av avvik.
- Ikke opprett mock-up-kode.

**Human Gate 0:** Vis sporbarhetsoversikten, AGENTS-endringen, uklarheter og
antakelser. Stopp til brukeren godkjenner at arbeidet kan gå videre.

### Fase 1 — UI-ramme og beslutninger

- Avklar navngiving, informasjonsstruktur, filinndeling under `src/` og
  presentasjon av sekvensdiagrammets alternative lifeline-navn.
- Behandle mindre visuelle valg som tekstinstruksjoner.
- Opprett og få godkjent ADR-er for større beslutninger etter terskelen ovenfor.
- Bekreft akseptansekriterier og hvordan funksjoner og kildesamsvar kontrolleres.

**Human Gate 1:** Brukeren godkjenner UI-rammen og relevante ADR-er før
implementasjon.

### Fase 2 — Statisk mock-up

- Implementer én responsiv norsk side i `src/` uten funksjonelle utlåns- eller
  returhandlinger.
- Vis relevante modellklasser og attributter med navn og verdier sporet til
  kildene eller eksplisitt merkede plassholderantakelser.
- Ikke introduser domeneklasser, attributter eller regler utover modellen.
- Kontroller visning på smal og bred viewport.

**Human Gate 2:** Presenter den kjørbare statiske siden, visuell kontroll og
eventuelle avvik. Vent på godkjenning før interaksjoner implementeres.

### Fase 3 — Lokale utlån og retur

- Implementer godkjente utlåns- og returhandlinger med tilstand kun i
  nettleserminnet.
- Følg sekvensdiagrammets meldingsrekkefølge og de tegnede `LEDIG`/`UTLÅNT`
  overgangene; ikke bruk localStorage eller backend.
- Test utlån, retur, utilgjengelig utstyr og refresh/reset.
- Hold tekniske UI-valgmekanismer adskilt fra domenemodellen.

**Human Gate 3:** Presenter interaksjonene og testresultatene. Vent på
godkjenning før verifikasjons-/publiseringsarbeid fortsetter.

### Fase 4 — Verifikasjon og avviksrapport

- Sammenlign implementasjonen punktvis med klassediagram, tilstandsdiagram,
  sekvensdiagram og godkjente krav/ADR-er.
- Kjør relevante tester og kontroller med eksisterende verktøy; unngå
  unødvendige avhengigheter.
- Rapporter kildeelement, faktisk implementasjon, type avvik, begrunnelse og
  status for alle manglende, endrede, ekstra, tvetydige eller avvikende forhold.
- Rett avvik innen godkjent scope. Større endringer går til ADR og Human Gate.
- Ikke erklær samsvar eller ferdig status når avvik er uforklarte eller åpne.

**Human Gate 4:** Brukeren gjennomgår samsvarsmatrise, tester og avviksrapport.
Arbeidet kan bare omtales som ferdig etter godkjenning og uten uavklarte avvik.

### Fase 5 — GitHub Pages og publiseringsklarhet

- Konfigurer GitHub Actions for Pages på push til `main`, med minst nødvendige
  rettigheter og standard artifact/deploy-flyt.
- Kontroller workflow, branch-filter, artifact-sti og Pages-forutsetninger.
- Valider lokalt der det er mulig; ikke push eller utløs deploy automatisk.

**Human Gate 5:** Presenter workflow og valideringsresultater. Push eller deploy
krever separat, uttrykkelig godkjenning; godkjenning av plan eller fase er ikke
push-godkjenning.

## Ferdigkriterier

- Alle faser og Human Gates er gjennomført.
- Implementasjonen er kontrollert mot alt grunnlagsmateriale og godkjente
  beslutninger.
- Test- og samsvarsresultater er rapportert; ingen uavklarte avvik står igjen.
- Endelig oppsummering peker til implementasjon, ADR-er, tester og
  avviksrapport. Ikke-pushet arbeid merkes som lokalt og ikke publisert.
