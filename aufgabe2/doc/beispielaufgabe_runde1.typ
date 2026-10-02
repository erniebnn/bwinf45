#import "template.typ": conf


#show: conf.with(
  title: "45. Bundeswettbewerb Informatik, Aufgabe 2",
  team-id: "00291",
  team-name: "Einzelteam",
  authors: (
    "Erwin Raupach",
  ),
)

= Lösungsidee

Um zwei Flaggen zu vergleichen werden drei verschiedene Metriken verwendet. Ein Vergleich eines Histogramms aller in den Flaggen vorhandenen Farben, ein Vergleich der im Bild vorhandenen Kanten, und ein direkter Pixelvergleich. Diese drei Metriken werden dann mit einer Gewichtung zusammengeführt, sodass ein finaler Wert zwischen $0.0$ (Komplett ungleich) und $1.0$ (Komplett identisch) erhalten wird.

== Histogrammvergleich
Für beide Flaggen wird ein Farbhistogramm gebildet, welches die Verteilung der Farben auf dem Flaggen beschreibt. So lassen sich Flaggen als "ähnlich" bezeichnen wenn sie nur ähnliche Farben, an möglicherweise unterschiedlichen Positionen haben.

Die Histogramme zweier Flaggen werden dann verglichen, wie genau steht in der Umsetzung.

== Kantenvergleich
Da viele Flaggen aufgrund ihres "Musters" (z.B. Nordisches Kreuz, ) trotz verschiedener Farben von Menschen dennoch als "ähnlich" wahrgenommen werden, muss auch dieses Muster verglichen werden. Dazu werden aus beiden Flaggen die Kanten extrahiert, und auf Übereinstimmung verglichen.

== Pixelvergleich
Um detaillierte Unterschiede, wie Embleme oder leicht verschiedene Farben, nicht ungeachtet zu lassen, müssen die Flaggen auch anhand ihrer direkten Pixel-Farbwerte verglichen werden. Dazu wird für jeden Pixel eine "Distanz" berechnet, und diese dann als Anteil einer möglichen Maximaldistanz zurückgegeben.

= Umsetzung

Das Programm zum Vergleich ist in C implementiert, und über die Kommandozeile verwendbar. Die Verwendung erklärt das Programm beim Aufruf von selbst. Das Programm ist für Linux geschrieben, und funktioniert aufgrund der Verwendung von `dirent.h` nicht auf Windows.

== Histogrammvergleich
Verwendet wird ein 4x4x4 Histogramm über den RGB-Farbraum. Dieses Histogramm beschreibt die Verteilung der vorhandenen Farben im Bild, jedoch werden alle RGB-Kanäle von 255 auf 4 Buckets aufgeteilt, damit leicht andere Farben nicht als "komplett anders" gesehen werden. Dieser ggf. leichte unterschied wird vom direkten Pixelvergleich sowieso aufgefangen, deswegen ist es nicht schlimm wenn dieser hier verloren geht.

Zur Konstruktion des Histogramms wird für jeden Pixel der zugehörige Bucket bestimmt, und dieser um den (auf 16 Buckets beschränkten) Alphawert des Pixels erhöht. Dies verhindert das durchsichtige Pixel einen Einfluss auf das Histogramm haben. Zusätzlich speichert das Histogramm seine "masse", also die anzahl der Pixel die für das Histogramm verwendet wurden, damit auch Flaggen mit verschiedenen Größen verglichen werden können.

Die Übereinstimmung zweier Histogramme wird mit dieser Formel ermittelt:

$
I = frac(sum_(r, g, b) min(upright(H_0)[r,g,b] dot upright(M)_1, upright(H_1)[r,g,b] dot upright(M)_0), upright(M)_0 dot upright(M)_1)
$

Wobei $upright(H)_0$ und $upright(H)_1$ die beiden Histogramme, und $upright(M)_0$ und $upright(M)_1$ die Masse der jeweiligen Histogramme beschreiben. Diese Formel kann zwei Histogramme mit verschiedenen Massen vergleichen, und wurde angepasst um (bis auf die finale Division) nur Ganzzahloperationen zu verwenden. Das Ergebnis ist eine Kommazahl von $0.0$ bis $1.0$, wobei letzter komplette Übereinstimmung beschreibt.

== Kantenvergleich
Zum Kantenvergleich wird die Flaggen zuerst nach dieser Formel:

$
v = frac(77 dot r + 150 dot g + 29 dot b, 256)
$

in Graustufen umgewandelt (alle werte $0..255$).

Für jeden Pixel einer Flagge wird die Differenz mit dem nächsten Pixel rechts, und dem nächsten Pixel nach unten Gebiltet, und das Ergebnis für jeden Pixel gespeichert. Große Werte deuten dabei auf Kanten hin. Dann wird das Ergebnis auf eine Auflösung von 144x48 verkleinert.

Nun gilt für jeden Pixel, falls der vorhandene Wert Größer als 1 ist, ist dort eine Kante. Zuletzt werden nun zwei dieser "Kantenbilder" mit dieser Formel verglichen:

$
I = 2 dot frac("Anzahl der Pixel welche auf beiden Kantenbildern" >= 1, "Anzahl der Pixel" >= 1 "auf Kantenbild 0" + "Anzahl der Pixel" >= 1 "auf Kantenbild 1")
$

== Pixelvergleich
Zum Pixelvergleich wird für jedes Pixelpaar der beiden Flaggen eine Distanz nach:

$
d = sqrt((r_0-r_1)^2 + (g_0-g_1)^2 + (b_0-b_1)^2)
$

gebildet. Die maximale Distanz ist demnach $d_"max" approx #{calc.round(calc.sqrt(3*255*255))}$. Jede Distanz größer als $65$ wird als Maximaldistanz gezählt. Der finale Wert für den Pixelvergleich berechnet sich nun so:

$
I = 1 - frac(sum_(x,y) d_(x,y), sum_(x,y) d_"max")
$

Komplett durchsichtige Pixel werden ignoriert.

== Finaler Ähnlichkeitswert

Alle drei Metriken bestimmen einen Ähnlichkeitswert $I$, welcher von $0.0$ bis $1.0$ reicht. Diese werden zu einem Gesamtähnlichkeitswert vereint, durch diesen gewichteten Durchschnitt:

$
I_"ges" = 0.35 dot I_"hist" + 0.3 dot I_"edge" + 0.35 dot I_"pixel"
$

= Werkzeuge <werkzeuge>
- Code Editor: Code - OSS 1.138.0
- C-Compiler: Clang 22.1.8
- Build System: GNU Make 4.4.1
- Version Control: Git 2.55.0
- Verwendete Libraries:
  - GLibC (C Standartbibliothek)
  - STB Image @stbi
  - STB Image Write @stbi_write
  - STB Image Resize2 @stbi_resize2

= Beispiele

Fünf ähnlichste Flaggen zur Flagge von Italien:

#align(center)[
  ```
  $ ./bin/aufgabe2 findsimilar assets/Italien.png assets
  assets/Italien.png <- 57.09% -> assets/Senegal.png
  assets/Italien.png <- 63.07% -> assets/Mali.png
  assets/Italien.png <- 65.00% -> assets/Elfenbeinküste.png
  assets/Italien.png <- 75.74% -> assets/Peru.png
  assets/Italien.png <- 75.85% -> assets/Frankreich.png
  ```
]

Fünf ähnlichste Flaggen zur Flagge von Vietnam:

#align(center)[
  ```
  $ ./bin/aufgabe2 findsimilar assets/Vietnam.png assets
  assets/Vietnam.png <- 67.19% -> assets/Türkei.png
  assets/Vietnam.png <- 68.12% -> assets/Marokko.png
  assets/Vietnam.png <- 68.36% -> assets/Albanien.png
  assets/Vietnam.png <- 73.05% -> assets/Tunesien.png
  assets/Vietnam.png <- 76.68% -> assets/Kirgisistan.png
  ```
]

Zehn ähnlichste Flaggenpaare:

#align(center)[
  ```
  $ ./bin/aufgabe2 findpairs assets
  Calculating... 100.00%
  assets/Jemen.png <- 83.75% -> assets/Sudan.png
  assets/Australien.png <- 88.43% -> assets/Cookinseln.png
  assets/Cookinseln.png <- 89.02% -> assets/Neuseeland.png
  assets/Bolivien.png <- 90.10% -> assets/Ghana.png
  assets/Irak.png <- 90.34% -> assets/Jemen.png
  assets/Ägypten.png <- 90.92% -> assets/Irak.png
  assets/Ägypten.png <- 91.44% -> assets/Jemen.png
  assets/Australien.png <- 94.34% -> assets/Neuseeland.png
  assets/Indonesien.png <- 95.24% -> assets/Monaco.png
  assets/Rumänien.png <- 98.36% -> assets/Tschad.png
  ```
]

#pagebreak()

= Quellcode

== Histogrammvergleich
#align(left)[#box(inset: 1mm)[
  ```c
  typedef struct _RGBAHistogram {
      u64* buffer;
      u64 mass;
      u32 size;
  } RGBAHistogram;
  
  double histogram_compare(RGBAHistogram* p_hist0, RGBAHistogram* p_hist1) {
      if (p_hist0->size != p_hist1->size) { printf("%d %d\n", p_hist0->size, p_hist1->size); }
      iassert(p_hist0->size == p_hist1->size, "Compared histograms must have equal size!");
      u64 overlap = 0;
      for (u32 i = 0; i < p_hist0->size * p_hist0->size * p_hist0->size; i++) {
          u64 a = p_hist0->buffer[i] * p_hist1->mass;
          u64 b = p_hist1->buffer[i] * p_hist0->mass;
          overlap += a < b ? a : b;
      }
      u64 fact = p_hist0->mass * p_hist1->mass;
      if (fact == 0) { return 0.0; }
      return (double) overlap / (double) fact;
  }
  ```
]]

== Kantenvergleich
#align(left)[#box(inset: 1mm)[
  ```c
  typedef struct _EdgeMap {
    u32 width;
    u32 height;
    u8* buffer;
  } EdgeMap;
  
  double edgemap_compare(EdgeMap* p_edgemap0, EdgeMap* p_edgemap1) {
    iassert(p_edgemap0->width == p_edgemap1->width
      && p_edgemap0->height == p_edgemap1->height,
      "Edgemap dimensions must match to compare!");
    u32 matching_ones = 0;
    u32 edgemap0_mass = 0, edgemap1_mass = 0;
    for (u32 y = 0; y < p_edgemap0->height; y++) {
        for (u32 x = 0; x < p_edgemap0->width; x++) {
            u32 pixel_idx = y * p_edgemap0->width + x;
            u32 edgpx0 = p_edgemap0->buffer[pixel_idx];
            u32 edgpx1 = p_edgemap1->buffer[pixel_idx];
            matching_ones += (edgpx0 == 255 && edgpx1 == 255);
            edgemap0_mass +=  edgpx0 == 255;
            edgemap1_mass +=  edgpx1 == 255;
        }
    }
    u32 full_mass = edgemap0_mass + edgemap1_mass;
    if (full_mass == 0) { return 0.0; }
    return 2.0 * (double) matching_ones / (double) full_mass;  
  }
  ```
]]

== Pixelvergleich
#align(left)[#box[
  ```c
  double image_pixel_compare(Image* p_image0, Image* p_image1) {
    iassert(p_image0->width == p_image1->width && p_image0->height == p_image1->height, "Images to compare must be of the same size!");
    iassert(p_image0->kind == p_image1->kind, "Images to compare must be of the same kind!");
    const u64 max_px_dist = p_image0->kind == IMGK_GRAY ? 255 : 442; // ceil(sqrt(3*255^2))
    const u64 threshold_dist = p_image0->kind == IMGK_GRAY ? 23 : 65;
    u64 max_dist = 0;
    u64 total_dist = 0;
    for (u32 y = 0; y < p_image0->height; y++) {
        for (u32 x = 0; x < p_image0->width; x++) {
            u32 pixel_idx = y * p_image0->width + x;
            if (p_image0->kind == IMGK_RGBA) {
                u32 dist_r = abs((i32) p_image0->buffer[4 * pixel_idx + 0] - (i32) p_image1->buffer[4 * pixel_idx + 0]);
                u32 dist_g = abs((i32) p_image0->buffer[4 * pixel_idx + 1] - (i32) p_image1->buffer[4 * pixel_idx + 1]);
                u32 dist_b = abs((i32) p_image0->buffer[4 * pixel_idx + 2] - (i32) p_image1->buffer[4 * pixel_idx + 2]);
                u32 img0_a = p_image0->buffer[4 * pixel_idx + 3];
                u32 img1_a = p_image1->buffer[4 * pixel_idx + 3];
                u32 cur_dist = (u32) sqrt((double) (dist_r * dist_r + dist_g * dist_g + dist_b * dist_b));
                if (img0_a != 0 || img1_a != 0) { max_dist += max_px_dist; }
                if (img0_a != 0 && img1_a != 0) { total_dist += (cur_dist < threshold_dist) ? cur_dist : max_px_dist; }
            }
            else if (p_image0->kind == IMGK_GRAY) {
                u32 dist = abs((i32) p_image0->buffer[2 * pixel_idx + 0] - (i32) p_image1->buffer[2 * pixel_idx + 0]);
                u32 img0_a = p_image0->buffer[2 * pixel_idx + 1];
                u32 img1_a = p_image1->buffer[2 * pixel_idx + 1];
                if (img0_a != 0 || img1_a != 0) { max_dist += max_px_dist; }
                if (img0_a != 0 && img1_a != 0) { total_dist += (dist < threshold_dist) ? dist : max_px_dist; }
            }
            else {
                panic("Unknown enum variant!");
            }
        }
    }
    return 1.0 - ((double) total_dist / (double) max_dist);
}
  ```
]]


= Hinweise
Programm entwickelt und getestet ausschließlich auf Arch Linux 7.2.6 mit den angegebenen Werkzeugen und Versionen in @werkzeuge.

#bibliography("bib.yaml", style: "ieee", title: "7 Bibliographie")