# Aflevering

## GitHub-repository

Her er linket til projektet:

- [mwsk75996/q_api_case](https://github.com/mwsk75996/q_api_case)

Nyttige links:

- [Kode på main](https://github.com/mwsk75996/q_api_case/tree/main)
- [GitHub Actions](https://github.com/mwsk75996/q_api_case/actions)
- [Defects-mappe](https://github.com/mwsk75996/q_api_case/tree/main/defects)

## Defect reports

Jeg testede API'et manuelt og fandt 5 fejl, som jeg har skrevet defect reports til:

1. [DEF-001](https://github.com/mwsk75996/q_api_case/blob/main/defects/DEF-001.md) – `/api/grade` giver `F` ved `score=60`, men burde give `D`
2. [DEF-002](https://github.com/mwsk75996/q_api_case/blob/main/defects/DEF-002.md) – `/api/discount` giver `0` ved `amount=100`, men grænsen burde udløse rabat
3. [DEF-003](https://github.com/mwsk75996/q_api_case/blob/main/defects/DEF-003.md) – `/api/booking` accepterer `seats=6` uden override, men 6 burde afvises
4. [DEF-004](https://github.com/mwsk75996/q_api_case/blob/main/defects/DEF-004.md) – `/api/username` returnerer en tom streng ved kun mellemrum i stedet for en fejl
5. [DEF-005](https://github.com/mwsk75996/q_api_case/blob/main/defects/DEF-005.md) – `/api/sensor-average` mister decimaler fordi der bruges heltalsdivision

## Unit tests

Testfilen ligger her:

- [tests/quality_service_test.cpp](https://github.com/mwsk75996/q_api_case/blob/main/tests/quality_service_test.cpp)

Jeg kørte de eksisterende tests og tilføjede tre nye, som dækker de fejl jeg fandt under den manuelle test:

1. `GradeBoundaryNeighborsAroundSixty` – tjekker at `score=59`, `60` og `61` giver de rigtige karakterer
2. `UsernameRejectsEmptyAndTooLongInput` – tjekker at tomme og for lange brugernavne afvises
3. `SensorAverageKeepsFractionsForLongerSeries` – tjekker at gennemsnittet ikke taber decimaler

## Pipeline

Pipelineen kører i GitHub Actions og kan ses her: [Actions-fanen](https://github.com/mwsk75996/q_api_case/actions)

Den gør tre ting i rækkefølge:

1. **Build** – projektet konfigureres med CMake og bygges
2. **Test** – unit tests køres med `ctest`
3. **Deploy** – hvis build og test er grønne, publiceres browserklienten i `web/` til GitHub Pages

Det betyder at man ved et push til `main` automatisk kan se om noget er gået galt, i stedet for at opdage det bagefter.

## Refleksion

Det der overraskede mig lidt var, hvor nemt det var at overse fejl ved manuel test, hvis man ikke er systematisk. Grænseværdier af typen "hvad sker der præcist på 60 vs. 61" er svære at huske at tjekke manuelt, men trivielle at skrive en test til.

Den manuelle black box-test var god til at finde fejl ud fra, hvad man forventer API'et gør, uden at kigge i koden. Automatiserede tests er så gode til at holde fast i de fejl, man har fundet, så man ikke opdager dem igen ved en fejl tre uger senere.

Samlet set gav kombinationen af begge dele en bedre dækning, end hvis man kun havde brugt den ene tilgang.
