# Звіт з лабораторної роботи: статичний аналіз коду

## 1. Мета роботи

Метою роботи є запуск статичного аналізатора коду для проєкту, який використовувався в попередніх лабораторних роботах, аналіз отриманих результатів, опис одного з правил аналізатора, визначення основних метрик коду та формування пропозицій щодо рефакторингу для покращення показника Maintainability.

## 2. Обраний інструмент

Для аналізу використано **SonarCloud** від SonarSource. SonarCloud виконує статичний аналіз коду, знаходить помилки, вразливості, code smells, security hotspots, дублювання коду та обчислює метрики складності й технічного боргу.

Проєкт аналізувався у SonarCloud як C++-проєкт.

Основна інформація про аналіз:

| Параметр | Значення |
| --- | --- |
| Проєкт | `leitsyshyn_cslex` |
| Репозиторій/директорія | `cslex` |
| Гілка | `main` |
| Дата останнього аналізу | `2026-04-27 15:21:29 UTC` |
| Commit | `9ed0a073d4b03a4dc79b322711288be15e7a9462` |
| Аналізатор | SonarCloud / SonarSource |
| Мова проєкту | C++ |

## 3. Загальні результати аналізу

За результатами SonarCloud у проєкті не виявлено bugs або vulnerabilities, однак знайдено code smells та один security hotspot.

| Категорія | Значення |
| --- | ---: |
| Bugs | 0 |
| Vulnerabilities | 0 |
| Code Smells | 72 |
| Security Hotspots | 1 |
| Maintainability Rating | A |
| Reliability Rating | A |
| Security Rating | A |
| Technical Debt | 636 хвилин |
| Debt Ratio | 0.9% |
| Duplicated Lines | 74 |
| Duplicated Lines Density | 2.5% |

Розподіл виявлених code smells за важливістю:

| Severity | Кількість |
| --- | ---: |
| Critical | 17 |
| Major | 30 |
| Minor | 25 |
| Усього | 72 |

Усі 72 знайдені issues мають тип **Code Smell**, тобто вони не є безпосередніми помилками виконання або вразливостями, але погіршують читабельність, супровід, модифікованість і довгострокову якість коду.

## 4. Основні метрики коду

SonarCloud також надав кількісні метрики проєкту.

| Метрика | Значення |
| --- | ---: |
| Lines | 3002 |
| NCLOC | 2340 |
| Files | 45 |
| Classes | 87 |
| Functions | 350 |
| Statements | 1988 |
| Cyclomatic Complexity | 629 |
| Cognitive Complexity | 347 |
| Comment Lines | 56 |
| Comment Lines Density | 2.3% |

Пояснення ключових метрик:

| Метрика | Значення метрики |
| --- | --- |
| NCLOC | Кількість рядків коду без коментарів і порожніх рядків. У проєкті 2340 таких рядків. |
| Cyclomatic Complexity | Показує кількість незалежних шляхів виконання у коді. Чим більше умов, циклів і розгалужень, тим вища складність. Значення 629 для всього проєкту вказує на наявність ділянок із досить складною логікою. |
| Cognitive Complexity | Показує, наскільки важко людині зрозуміти код. На відміну від цикломатичної складності, сильніше штрафує вкладеність умов і складні конструкції. Загальне значення 347 вказує, що окремі функції варто спростити. |
| Technical Debt | Орієнтовний час, потрібний для виправлення знайдених maintainability-проблем. У цьому проєкті SonarCloud оцінив борг у 636 хвилин. |
| Debt Ratio | Відношення технічного боргу до загального обсягу коду. Значення 0.9% відповідає рейтингу Maintainability A. |

## 5. Файли з найбільшим впливом на Maintainability

Найбільше проблем підтримуваності зосереджено в кількох файлах.

| Файл | Code Smells | Cyclomatic Complexity | Cognitive Complexity | Technical Debt |
| --- | ---: | ---: | ---: | ---: |
| `tests/source_repository_tests.cpp` | 6 | 39 | 2 | 88 хв |
| `io/SourceRepository.cpp` | 7 | 20 | 27 | 74 хв |
| `lexers/fsm/processors/NumberProcessor.h` | 2 | 67 | 85 | 67 хв |
| `lexers/fsm/processors/InterpolatedStringProcessor.h` | 6 | 22 | 40 | 62 хв |
| `app/LexerComparisonApp.cpp` | 6 | 49 | 40 | 52 хв |

Особливо важливими є `NumberProcessor.h` та `InterpolatedStringProcessor.h`, оскільки вони мають високу cognitive complexity. Це означає, що логіка в цих файлах складна для розуміння, тестування та подальшої модифікації.

## 6. Приклади виявлених проблем

У проєкті були знайдені такі типові проблеми:

| Правило | Опис | Приклади файлів |
| --- | --- | --- |
| `cpp:S3776` | Cognitive Complexity функції не повинна бути надто високою | `NumberProcessor.h`, `InterpolatedStringProcessor.h` |
| `cpp:S134` | Надмірна вкладеність `if`, `for`, `while`, `switch` | `SourceRepository.cpp`, `RegexLexer.cpp`, `InterpolatedStringProcessor.h` |
| `cpp:S1003` | Не слід використовувати using-directive у header-файлах | `Token.h`, `FSMLexer.h`, `IProcessor.h`, `RegexLexer.h`, `RegexPatterns.h` |
| `cpp:S1181` | Не слід ловити занадто загальні винятки | `LexerComparisonApp.cpp`, `source_repository_tests.cpp` |
| `cpp:S6003` | Рекомендовано використовувати `emplace_back` замість `push_back` | `main.cpp`, `FSMLexer.cpp`, `RegexLexer.cpp` |
| `cpp:S5945` | Рекомендовано використовувати `std::array` або `std::vector` замість C-style array | `Operators.h`, `token_tests.cpp` |
| `cpp:S7034` | Рекомендовано використовувати `contains` замість `find` для перевірки наявності елемента | `OperatorProcessor.h`, `SeparatorProcessor.h` |

Також виявлено один security hotspot:

| Файл | Рядок | Правило | Повідомлення |
| --- | ---: | --- | --- |
| `tests/source_repository_tests.cpp` | 21 | `cpp:S5443` | Make sure publicly writable directories are used safely here. |

Цей security hotspot пов'язаний із використанням тимчасової або публічно доступної директорії в тестах. SonarCloud не класифікує це як підтверджену вразливість, але вимагає ручної перевірки безпечності такого використання.

## 7. Опис правила аналізатора

Для детального опису обрано правило **`cpp:S3776` — Cognitive Complexity of functions should not be too high**.

| Параметр | Значення |
| --- | --- |
| Rule Key | `cpp:S3776` |
| Назва | Cognitive Complexity of functions should not be too high |
| Тип | Code Smell |
| Severity | Critical |
| Вплив | Maintainability, High |
| Стандартний поріг | 25 |
| Оцінка виправлення | 1 хвилина за кожну одиницю складності понад поріг + базові 5 хвилин |

Суть правила полягає в тому, що функція не повинна бути занадто складною для розуміння. Cognitive complexity збільшується через вкладені умови, цикли, складні логічні переходи та конструкції керування потоком. Якщо функція має багато вкладених `if`, `else`, `while`, `switch` або складні комбінації умов, її важче читати, тестувати й безпечно змінювати.

У цьому проєкті правило спрацювало для таких файлів:

| Файл | Cognitive Complexity | Дозволений поріг |
| --- | ---: | ---: |
| `lexers/fsm/processors/NumberProcessor.h` | 85 | 25 |
| `lexers/fsm/processors/InterpolatedStringProcessor.h` | 40 | 25 |

Проблема полягає в тому, що обробники лексера містять багато логіки в одній функції. Такий код складно перевіряти, змінювати та розширювати, наприклад під час додавання нових правил обробки чисел або інтерпольованих рядків.

## 8. Аналіз Maintainability

Попри те, що загальний Maintainability Rating має значення **A**, у проєкті є 72 code smells та 636 хвилин технічного боргу. Це означає, що загальний стан проєкту прийнятний, але є конкретні ділянки, які варто покращити.

Основні причини технічного боргу:

- надмірна cognitive complexity у `NumberProcessor.h` та `InterpolatedStringProcessor.h`;
- надмірна вкладеність умов у `SourceRepository.cpp`, `RegexLexer.cpp` та processor-файлах;
- using-directive у header-файлах;
- використання generic exceptions замість спеціалізованих типів винятків;
- використання C-style arrays;
- дрібні modern C++ issues: `push_back` замість `emplace_back`, `find` замість `contains`, ручна конкатенація рядків замість `std::format`.

Найбільший ризик для подальшої підтримки становлять файли з високою cognitive complexity, оскільки будь-які зміни в них мають підвищену ймовірність регресій.

## 9. Запропонований рефакторинг

Для покращення Maintainability доцільно виконати такі зміни.

| Напрям рефакторингу | Очікуваний ефект |
| --- | --- |
| Розбити складні функції у `NumberProcessor.h` на менші функції: обробка префіксів, десяткових чисел, hex/binary форматів, суфіксів | Зменшення cognitive complexity, спрощення тестування |
| Розбити логіку `InterpolatedStringProcessor.h` на окремі кроки: читання символів, обробка escape-послідовностей, обробка вкладених виразів | Зменшення вкладеності та підвищення читабельності |
| Використати guard clauses замість глибокої вкладеності `if/else` | Зменшення `cpp:S134`, простіша структура керування |
| Прибрати using-directive з header-файлів | Зменшення ризику конфліктів імен у файлах, які підключають ці headers |
| Замінити generic exceptions на спеціалізовані exception-класи | Краще розділення типів помилок і точніша обробка винятків |
| Замінити C-style arrays на `std::array` або `std::vector` | Безпечніший і сучасніший C++-код |
| Використати `contains` замість `find(...) != end()` | Коротший і зрозуміліший код |
| Використати `emplace_back` замість `push_back` там, де об'єкт створюється на місці | Незначне покращення продуктивності та відповідність modern C++ style |
| Використати `std::format` замість ручної конкатенації повідомлень | Краща читабельність форматування рядків |
| Перевірити security hotspot у `source_repository_tests.cpp` і переконатися, що тимчасова директорія створюється безпечно | Усунення або документування потенційного security-ризику |

Пріоритет рефакторингу:

1. `NumberProcessor.h` — найбільша cognitive complexity: 85.
2. `InterpolatedStringProcessor.h` — cognitive complexity: 40.
3. `SourceRepository.cpp` — найбільша кількість code smells серед production-файлів: 7.
4. Header-файли з `using namespace` або using-directive.
5. Дрібні modern C++ improvements: `contains`, `emplace_back`, `std::array`, `std::format`.

## 10. Очікуваний результат після рефакторингу

Після запропонованого рефакторингу очікується:

- зменшення кількості Critical code smells;
- зменшення cognitive complexity у processor-файлах;
- зменшення technical debt;
- покращення локальної читабельності коду;
- зниження ризику регресій під час зміни логіки лексера;
- збереження або покращення Maintainability Rating A;
- зменшення часу, потрібного для подальшої підтримки проєкту.

## 11. Висновок

У межах лабораторної роботи було виконано статичний аналіз проєкту `leitsyshyn_cslex` за допомогою SonarCloud. Аналіз показав, що проєкт не має виявлених bugs або vulnerabilities, а загальні рейтинги Reliability, Security та Maintainability мають значення A.

Водночас SonarCloud виявив 72 code smells, 1 security hotspot і 636 хвилин технічного боргу. Основні проблеми пов'язані не з коректністю виконання, а з підтримуваністю: високою cognitive complexity, надмірною вкладеністю, using-directives у header-файлах і використанням менш сучасних конструкцій C++.

Найбільш доцільним напрямом покращення є рефакторинг складних processor-файлів, насамперед `NumberProcessor.h` та `InterpolatedStringProcessor.h`. Це дозволить зменшити складність коду, покращити maintainability і зробити подальший розвиток проєкту безпечнішим та простішим.
