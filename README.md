# Quality API - Test & CI/CD Case

Denne case handler om at teste et C++-baseret kvalitetssystem API og opsætte en moderne CI/CD-pipeline. Arbejdet går fra manuel **black box-test** til **automatiseret test** og **GitHub Actions**.

## Case-idé

Du arbejder med et simpelt, men realistisk **industrielt kvalitetssystem**, hvor et C++ API behandler data fra sensorer, beregner karakterer, rabatter og booking-logik. Systemet har komplekse regler, som er svære at holde styr på manuelt.

**Målet**: Lære forskellen mellem manuel test, unit test og CI/CD-automatisering.

## Endpoints og forventede krav

Browserklienten (`web/index.html`) kalder disse API-endpoints. Når I laver **black box-test**, skal I teste både gyldige intervaller, ugyldige værdier og **grænseværdier**.

### `/api/grade`
**Formål**: Karakterberegning ud fra score.

| Input | Forventet interval | Grænseværdier at teste |
|-------|-------------------|----------------------|
| `score` | 0-100 | `-1, 0, 1`, `59, 60, 61`, `99, 100, 101` |
| `production_mode` | true/false | true, false |

### `/api/discount`
**Formål**: Rabatberegning.

| Input | Forventet interval | Grænseværdier at teste |
|-------|-------------------|----------------------|
| `amount` | ≥ 0 | `-1, 0, 1`, `99, 100, 101`, `499, 500, 501` |
| `coupon` | true/false | true, false |
| `loyal_customer` | true/false | true, false |

### `/api/booking`
**Formål**: Booking-validering.

| Input | Forventet interval | Grænseværdier at teste |
|-------|-------------------|----------------------|
| `seats` | 1 til `max_capacity` | `-1, 0, 1`, `max_capacity-1, max_capacity, max_capacity+1` |
| `max_capacity` | > 0 | `0, 1, 5` |

### `/api/username`
**Formål**: Brugernavn-validering.

| Input | Forventet interval | Grænseværdier at teste |
|-------|-------------------|----------------------|
| `name` | 1-20 tegn | `""`, `"a"`, `"ab"`, 19/20/21 tegn |

### `/api/sensor-average`
**Formål**: Gennemsnit af sensorværdier.

| Input | Forventet interval | Grænseværdier at teste |
|-------|-------------------|----------------------|
| `values[]` | ≥ 0 | tom liste, 1 værdi, flere værdier |

### `/api/sensor-health`
**Formål**: Sensor sundhedscheck.

| Input | Forventet interval | Grænseværdier at teste |
|-------|-------------------|----------------------|
| `readings[]` | ≥ 0 | tom liste, værdier tæt på grænser |
| `maintenance_mode` | true/false | true, false |

## Forventede krav i opgaven

1. **Del 1**: Black box-test → **mindst 5 defect reports** med steps to reproduce
2. **Del 2**: Unit test → kør eksisterende tests + **tilføj 3 nye tests**
3. **Del 3**: CI/CD → GitHub Actions med **build, test, deploy af web**

## Lokal kørsel

### Backend (C++ API)
```bash
cmake -S . -B build
cmake --build build
./build/quality_api
```

**Note**: Projektet henter automatisk **GoogleTest** via `FetchContent`, hvis det ikke er installeret lokalt.

### Frontend (Browser)
1. Start backend ovenfor
2. Åbn `web/index.html` i browser

## VS Code Extensions (anbefalet)

### Live Server / Live Preview
Installer **"Live Server"** eller **"Live Preview"** i VS Code:

1. `Ctrl+Shift+X` → Extensions
2. Søg "Live Server" (Ritwick Dey) eller "Live Preview" 
3. Installer → genstart VS Code
4. Højreklik på `web/index.html` → **"Open with Live Server"**

**Fordel**: Browseren genindlæser automatisk ved ændringer.

### C++ Extensions
- **C/C++** (Microsoft) → IntelliSense, debugging
- **CMake Tools** → CMake integration
- **CMake** → Syntax highlighting

## Test kørsel lokalt

### Unit tests (GoogleTest)
```bash
ctest --test-dir build --output-on-failure
```

### Black box-test hints
- Test **grænseværdier**: `59, 60, 61`, `999, 1000, 1001`
- Test **kombinationer**: `production_mode=true` + `maintenance_mode=true`
- Brug `materialer/defect-report-template.md` til fejlregistrering

## Defect report eksempel
Defect ID: #001
Titel: Grade calculator fejler ved score=101
Endpoint: /api/grade
Steps to reproduce:

score=101, production_mode=false
Expected: Error eller max 100
Actual: Forkert karakter
Severity: Major

## CI/CD Pipeline

Opgaven afslutter med **GitHub Actions** i `.github/workflows/ci-cd.yml`:
✅ Build (CMake + Ninja)
✅ Test (GoogleTest + CTest)
✅ Deploy (GitHub Pages for web/)

**Note**: GitHub Pages deployer kun `web/` - C++ API kører lokalt.

## Tips til black box-test

- Start med **ækvivalenspartitionering**: gyldige/ugyldige intervaller
- Test **grænseværdier**: værdier lige under, på og over grænsen
- Test **kombinationer** af boolske flags
- Dokumentér alt i **defect reports** med steps to reproduce

---

**God arbejdslyst!** 