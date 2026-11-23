#import "template.typ": conf

#show: conf.with(
  title: "45. Bundeswettbewerb Informatik, Aufgabe 1",
  team-id: "00291",
  team-name: "Einzelteam",
  authors: (
    "Erwin Raupach",
  ),
)

= Lösungsidee

Um eine valide Konfiguration von vier Wörtern zu finden, welche ein Kreuzwortkreuz erfüllt, wird wie folgt vorgegangen:

Das erste Wort der Wortliste wird an Stelle des ersten Wortes des Kreuzwortkreuzes eingesetzt, und dann wird geprüft ob das Kreuzwortkreuz noch lösbar ist, also ob die aktuelle Konfiguration an Wörtern die gegebene Maske verletzt oder nicht. Falls es nicht lösbar ist, wird das nächste Wort der Wortliste an die selbe Stelle eingesetzt. Falls es doch Lösbar ist, beginnt das Einsetzen der Wörter der Wortliste rekursiv erneut, diesmal jedoch für das zweite Wort des Kreuzwortkreuzes. Falls dieses dann wieder eins findet, bei dem das Kreuzwortkreuz lösbar ist, wird rekursiv das Einsetzen des dritten Wortes des Kreuzwortkreuzes begonnen, usw. Sobald alle Wörter der Wortliste an eine Stelle eingesetzt wurden, und keins als lösbar galt, gibt dieser rekursive Aufruf "unlösbar" zurück, und der Aufruf eine ebene höher geht zum nächsten Wort über. Wenn der oberste Aufruf "unlösbar" zurückgibt, dann ist das Kreuzwortkreuz als ganzes mit der gegebenen Wortliste unlösbar. Sobald eine valide Konfiguration gefunden wurde, geben alle Aufrufe "lösbar" zurück, und das Kreuzwortkreuz ist gelöst.

= Umsetzung

Das Kreuzwortkreuz besteht aus 4 Wörtern a 4 Buchstaben, dargestellt als 4x4 Array. Jeder Buchstabe ist ein 32-bit unsigned integer, welches den Unicode-Codepoint des Zeichens repräsentiert. Dies ist notwendig, da die Wortliste die deutschen Umlaute enthält, welche nicht in den Standard 8-bit `char` Typ passen. Der Wert `0` erhält dabei allerdings eine besondere Rolle, während er normalerweise als Null-Terminator am ende von Strings verwendet wird, wird er hier als Sentinel für "nicht besetzt" verwendet, damit die `cross_check` Funktion weiß, welche Buchstaben und Teile der Maske (noch) nicht belegt sind. Die Maske ist ein eindimensionales Array aus 12 Buchstaben, welches das kreuz von links nach rechts, und oben nach unten abdeckt. Die C-Implementation des Structs ist zu finden in @kreuz_struct

Zum Überprüfen der Maske an den Rändern des Kreuzes wird die Funktion `check_single` (@check_single) verwendet. Diese prüft ob am gegebenen Punkt Maske und Wort-Buchstabe identisch sind, solange beide nicht `0` sind.

Zum Überprüfen der Maske in der Mitte des Kreuzes wird die Funktion `check_double` (@check_double) verwendet. Diese prüft, falls die Maske und beide Wort-Buchstaben gegeben sind, ob mindestens einer der Wort-Buchstaben gleich der Maske ist. Wenn die Maske nicht gegeben ist, prüft sie ob beide Wort-Buchstaben (solange beide nicht `0`) gleich sind.

In der Funktion `cross_check` (@cross_check) sind diese Vergleiche über die einzelnen Positionen hardcoded, sie gibt zurück ob das Kreuz mit den aktuell eingesetzten Wörtern noch lösbar ist.

Die `decode_utf8` Funktion ist nicht Standard-konform, bspw. lässt sie die surrogate values `U+D800` bis `U+DFFF` problemlos durch, und akzeptiert auch überlange Sequenzen. Da die gegebenen Beispiele aber alle valide UTF-8 Sequenzen sind, ist das egal.

Die bereitgestellte Wortliste enthält $651$ Wörter, somit gibt es 

$
651^4 = 179.607.287.601
$

Möglichkeiten für ein Kreuzwortkreuz, dennoch ist diese Menge aufgrund der frühen Terminierung von Versuchen (es wird geschaut ob ein Kreuzwortkreuz noch lösbar ist, obwohl es erst teilweise gefüllt ist) kein Problem aus der Perspektive der Laufzeit, zumindest nicht bei den gegebenen Beispielen.

= Werkzeuge

- Code Editor: Code - OSS 1.138.0
- C-Compiler: Clang 22.1.8
- Build System: GNU Make 4.4.1
- Version Control: Git 2.55.0
- Verwendete Libraries:
  - glibc (C Standartbibliothek)

= Beispiele

== Kreuz 0a

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz0a.txt 
  Solved cross:
  Word 0: ABER
  Word 1: BUND
  Word 2: USUS
  Word 3: BÖSE
  ```
]

== Kreuz 0b

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz0b.txt 
  Solved cross:
  Word 0: ABER
  Word 1: JAVA
  Word 2: AFFE
  Word 3: BABY
  ```
]

== Kreuz 01

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz01.txt 
  Solved cross:
  Word 0: ABER
  Word 1: AFFE
  Word 2: BIER
  Word 3: HOFE
  ```
]

== Kreuz 02

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz02.txt 
  The cross is unsolveable with the given wordlist!
  ```
]

== Kreuz 03

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz03.txt 
  Solved cross:
  Word 0: ALOE
  Word 1: ISST
  Word 2: ALSO
  Word 3: POST
  ```
]

== Kreuz 04

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz04.txt 
  Solved cross:
  Word 0: ABER
  Word 1: SCAN
  Word 2: APEX
  Word 3: AGAR
  ```
]

== Kreuz 05

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz05.txt 
  Solved cross:
  Word 0: BALG
  Word 1: GROB
  Word 2: MARS
  Word 3: VLOG
  ```
]

== Kreuz 06

#align(center)[
  ```
  $ bin/aufgabe1 assets/wortliste.txt assets/kreuz06.txt 
  The cross is unsolveable with the given wordlist!
  ```
]

= Quellcode

== Kreuzwortkreuz Struct <kreuz_struct>

```c
// word3 -------v
// word2 -----v
//            X X
// word0 -> X X X X
// word1 -> X X X X
//            X X

typedef struct _Cross {
    u32 words[4][4];
    u32 wmask[12];
} Cross;
```

== Check Single Funktion <check_single>

```c
static bool check_single(u32 p_c, u32 p_mask) {
    return (p_c == p_mask) || !(p_c && p_mask);
}
```

== Check Double Funktion <check_double>

```c
static bool check_double(u32 p_c0, u32 p_c1, u32 p_mask) {
    if (p_mask) {
        return (p_c0 == p_mask) || (p_c1 == p_mask) || !(p_c0 && p_c1);
    }
    else {
        return (p_c0 == p_c1) || !(p_c0 && p_c1);
    }
}
```

== Cross Check Funktion <cross_check>

#text(size: 0.9em)[
  ```c
  static bool cross_check(Cross* p_cross) {
      bool matches = true;
      matches = matches && check_double(p_cross->words[0][1], p_cross->words[2][1], p_cross->wmask[3]);
      matches = matches && check_double(p_cross->words[0][2], p_cross->words[3][1], p_cross->wmask[4]);
      matches = matches && check_double(p_cross->words[1][1], p_cross->words[2][2], p_cross->wmask[7]);
      matches = matches && check_double(p_cross->words[1][2], p_cross->words[3][2], p_cross->wmask[8]);
      matches = matches && check_single(p_cross->words[2][0], p_cross->wmask[ 0]);
      matches = matches && check_single(p_cross->words[3][0], p_cross->wmask[ 1]);
      matches = matches && check_single(p_cross->words[0][0], p_cross->wmask[ 2]);
      matches = matches && check_single(p_cross->words[0][3], p_cross->wmask[ 5]);
      matches = matches && check_single(p_cross->words[1][0], p_cross->wmask[ 6]);
      matches = matches && check_single(p_cross->words[1][3], p_cross->wmask[ 9]);
      matches = matches && check_single(p_cross->words[2][3], p_cross->wmask[10]);
      matches = matches && check_single(p_cross->words[3][3], p_cross->wmask[11]);
      return matches;
  }
  ```
]

== Solve Cross Funktion

```c
static bool solve_cross_impl(Cross* p_cross, u32 p_idx) {
    if (p_idx == 4) {
        return true;
    }
    for (u32 i = 0; i < words_len; i++) {
        set_word(p_cross, p_idx, i);
        bool res = cross_check(p_cross);
        if (res) {
            res = solve_cross_impl(p_cross, p_idx + 1);
            if (res) { return true; }
        }
    }
    clear_word(p_cross, p_idx);
    return false;
}

static bool solve_cross(Cross* p_cross) {
    return solve_cross_impl(p_cross, 0);
}
```