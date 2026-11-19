#!/usr/bin/env python3
"""Genera assets/maps/cielo-aperto.png in pixel art.

Le piattaforme sono lette da mappe/mappe.json (mappa "CIELO APERTO"), quindi il
disegno resta allineato alle collisioni. Uso (dalla cartella del progetto):

    python3 tools/make_cielo_aperto.py

Richiede Pillow (pip install pillow).
"""
import json
import random

from PIL import Image, ImageDraw

MAP_NAME = "CIELO APERTO"
OUTPUT = "assets/maps/cielo-aperto.png"
PIXEL = 4            # ogni pixel "artistico" diventa 4x4 pixel nell'immagine
LOW_W, LOW_H = 400, 225

random.seed(11)

# ---- palette -------------------------------------------------------------
SKY = [(10, 12, 26), (14, 17, 34), (19, 22, 42), (25, 27, 50), (32, 32, 58),
       (40, 37, 64)]
STAR = (200, 205, 230)
FAR_BLD = (30, 30, 46)
FAR_WIN = (120, 96, 60)
BLD = [(70, 52, 44), (78, 60, 50), (62, 47, 42)]
BLD_DARK = (44, 34, 32)
BLD_LINE = (96, 76, 62)
WIN_OFF = (34, 30, 34)
WIN_ON = [(255, 206, 120), (240, 180, 96), (255, 226, 160)]
TREE = [(18, 34, 22), (26, 48, 28), (36, 62, 34)]
COBBLE_GAP = (46, 46, 52)
COBBLE = [(84, 84, 90), (94, 94, 100), (76, 76, 82), (104, 102, 106)]
COBBLE_HI = (122, 120, 124)
SLAB = (214, 204, 182)
SLAB_HI = (236, 228, 206)
SLAB_SHADE = (166, 156, 136)
SLAB_JOINT = (140, 130, 112)
LAMP = (255, 226, 150)
HEDGE = [(34, 66, 30), (46, 88, 38), (60, 108, 44)]
HEDGE_HI = (98, 146, 58)
HEDGE_DARK = (22, 44, 22)
POLE = (36, 36, 40)
GLOW = (255, 214, 140)


def load_map():
    with open("mappe/mappe.json") as f:
        maps = json.load(f)["maps"]
    return next(m for m in maps if m["name"] == MAP_NAME)


arena = load_map()
UNIT = arena["width"] / LOW_W          # unita' mappa per pixel artistico

img = Image.new("RGB", (LOW_W, LOW_H))
px = img.load()
d = ImageDraw.Draw(img)


def u(v):
    return int(round(v / UNIT))


def bounds(quad):
    xs = [p[0] for p in quad["points"]]
    ys = [p[1] for p in quad["points"]]
    return u(min(xs)), u(min(ys)), u(max(xs)), u(max(ys))


def put(x, y, c):
    if 0 <= x < LOW_W and 0 <= y < LOW_H:
        px[x, y] = c


# ---- cielo a bande con dithering -----------------------------------------
band_h = 150 / (len(SKY) - 1)
for y in range(LOW_H):
    t = min(y / band_h, len(SKY) - 1.001)
    i, f = int(t), t - int(t)
    for x in range(LOW_W):
        use_next = f > 0.5 if (x + y) % 2 else f > 0.75
        if f > 0.25 and (x + y) % 2 == 0:
            use_next = use_next or f > 0.5
        px[x, y] = SKY[min(i + (1 if use_next else 0), len(SKY) - 1)]

for _ in range(70):
    x, y = random.randrange(LOW_W), random.randrange(0, 90)
    put(x, y, STAR if random.random() < 0.7 else (150, 155, 190))

# luna
mx, my = 300, 22
d.ellipse([mx - 7, my - 7, mx + 7, my + 7], fill=(236, 230, 206))
d.ellipse([mx - 4, my - 9, mx + 10, my + 5], fill=SKY[0])

# nuvole pixel scure
for cx, cy, w in [(60, 30, 40), (180, 18, 30), (360, 50, 36)]:
    for i in range(w):
        h = 2 + int(2 * (1 - abs(i - w / 2) / (w / 2)))
        for j in range(h):
            put(cx - w // 2 + i, cy - j, (34, 36, 60))

# ---- palazzi lontani -----------------------------------------------------
x = -4
while x < LOW_W:
    w = random.randint(14, 30)
    top = random.randint(60, 95)
    d.rectangle([x, top, x + w, LOW_H], fill=FAR_BLD)
    for wy in range(top + 3, 170, 5):
        for wx in range(x + 2, x + w - 1, 4):
            if random.random() < 0.18:
                put(wx, wy, FAR_WIN)
    x += w + random.randint(0, 4)


# ---- palazzi vicini (stile condominio, balconi) ------------------------
def building(x0, w, top):
    col = random.choice(BLD)
    d.rectangle([x0, top, x0 + w, LOW_H], fill=col)
    d.rectangle([x0, top - 2, x0 + w, top], fill=BLD_DARK)      # cornicione
    d.line([x0, top + 1, x0 + w, top + 1], fill=BLD_LINE)
    d.line([x0 + w, top, x0 + w, LOW_H], fill=BLD_DARK)
    floor_h = 9
    for fy in range(top + 5, LOW_H, floor_h):
        d.line([x0 + 1, fy + floor_h - 2, x0 + w - 1, fy + floor_h - 2],
               fill=BLD_LINE)
        col_w = 7
        for wx in range(x0 + 3, x0 + w - 4, col_w):
            lit = random.random() < 0.16
            c = random.choice(WIN_ON) if lit else WIN_OFF
            d.rectangle([wx, fy, wx + 2, fy + 3], fill=c)
            if random.random() < 0.35:          # balcone
                d.rectangle([wx - 1, fy + 4, wx + 4, fy + 5], fill=BLD_DARK)
                d.line([wx - 1, fy + 4, wx + 4, fy + 4], fill=BLD_LINE)


for bx, bw, bt in [(-6, 70, 40), (68, 96, 22), (176, 60, 54), (240, 74, 14),
                   (318, 52, 46), (372, 40, 30)]:
    building(bx, bw, bt)

# ---- parco: alberi scuri e, sotto, la citta' lontana nella foschia --------
DEPTH = [(20, 22, 36), (16, 18, 30), (12, 14, 24), (9, 10, 18)]
for y in range(150, LOW_H):
    t = (y - 150) / (LOW_H - 150) * (len(DEPTH) - 1)
    i = int(t)
    nxt = min(i + 1, len(DEPTH) - 1)
    for x in range(LOW_W):
        px[x, y] = DEPTH[nxt] if (t - i) > 0.5 and (x + y) % 2 else DEPTH[i]
for _ in range(90):                              # luci lontane
    x, y = random.randrange(LOW_W), random.randint(172, LOW_H - 4)
    put(x, y, random.choice([(150, 120, 70), (120, 100, 70), (90, 90, 120)]))
for layer, (base, rmin, rmax) in enumerate([(146, 6, 11), (154, 5, 9)]):
    for x in range(-6, LOW_W + 6, 4):
        r = random.randint(rmin, rmax)
        b = base + random.randint(0, 4)
        d.ellipse([x - r, b - r, x + r, b + r], fill=TREE[layer])
        for _ in range(6):                        # foglie piu' chiare
            put(x + random.randint(-r + 1, r - 1), b - r + random.randint(1, 3),
                TREE[2])
d.rectangle([0, 156, LOW_W, 162], fill=TREE[0])
for x in range(LOW_W):                            # bordo irregolare in basso
    for y in range(162, 162 + random.randint(0, 3)):
        put(x, y, TREE[0])


# ---- materiali ------------------------------------------------------------
def cobbles(x0, y0, x1, y1, mask=None):
    """Sanpietrini grigi sfalsati nell'area [x0,x1) x [y0,y1)."""
    for y in range(y0, y1):
        for x in range(x0, x1):
            if mask is None or mask(x, y):
                put(x, y, COBBLE_GAP)
    y = y0
    while y < y1:
        h = random.choice([2, 3, 3])
        x = x0 - random.randint(0, 3)
        while x < x1:
            w = random.choice([2, 3, 3, 4])
            c = random.choice(COBBLE)
            for yy in range(y, min(y + h, y1)):
                for xx in range(x, x + w):
                    corner = (xx in (x, x + w - 1)) and (yy in (y, y + h - 1))
                    if (x0 <= xx < x1 and not (corner and w > 2 and h > 2)
                            and (mask is None or mask(xx, yy))):
                        put(xx, yy, c)
            if x0 <= x + 1 < x1 and (mask is None or mask(x + 1, y)):
                put(x + 1, y, COBBLE_HI)
            x += w + 1
        y += h + 1


def slab(x0, x1, y, h=3, lamps=True):
    """Bordo in pietra bianca (travertino) con giunti e faretti."""
    d.rectangle([x0, y, x1 - 1, y + h - 1], fill=SLAB)
    d.line([x0, y, x1 - 1, y], fill=SLAB_HI)
    d.line([x0, y + h - 1, x1 - 1, y + h - 1], fill=SLAB_SHADE)
    d.line([x0, y + h, x1 - 1, y + h], fill=(60, 56, 52))
    for jx in range(x0 + 12, x1 - 2, 14):
        d.line([jx, y + 1, jx, y + h - 1], fill=SLAB_JOINT)
    if lamps:
        for lx in range(x0 + 20, x1 - 8, 42):
            d.rectangle([lx, y + 1, lx + 2, y + 1], fill=LAMP)


def hedge(x0, x1, ground_y, h=8):
    """Siepe squadrata appoggiata sul bordo (dietro ai giocatori)."""
    top = ground_y - h
    d.rectangle([x0, top + 1, x1 - 1, ground_y - 1], fill=HEDGE[1])
    d.line([x0 + 1, top, x1 - 2, top], fill=HEDGE[2])
    for x in range(x0 + 1, x1 - 1):               # cima irregolare
        if random.random() < 0.4:
            put(x, top - 1, HEDGE[2] if random.random() < 0.6 else HEDGE_HI)
    d.line([x0, top + 1, x0, ground_y - 1], fill=HEDGE_DARK)
    d.line([x1 - 1, top + 1, x1 - 1, ground_y - 1], fill=HEDGE_DARK)
    for x in range(x0, x1):
        for y in range(top, ground_y):
            r = random.random()
            if r < 0.18:
                put(x, y, HEDGE[0])
            elif r < 0.26:
                put(x, y, HEDGE_HI if y < top + 2 else HEDGE[2])
    d.line([x0, ground_y - 1, x1 - 1, ground_y - 1], fill=HEDGE_DARK)


def lamp_post(x, ground_y, height=26):
    d.line([x, ground_y - height, x, ground_y - 1], fill=POLE)
    d.rectangle([x - 1, ground_y - height - 2, x + 1, ground_y - height],
                fill=GLOW)
    glow = Image.new("RGBA", img.size, (0, 0, 0, 0))
    gd = ImageDraw.Draw(glow)
    gx, gy = x, ground_y - height - 1
    for r, a in [(12, 22), (7, 40), (4, 70)]:
        gd.ellipse([gx - r, gy - r, gx + r, gy + r], fill=GLOW + (a,))
    img.paste(Image.alpha_composite(img.convert("RGBA"), glow).convert("RGB"))


def stepped_island(x0, x1, top, depth, step=4, shrink=3, hedges=()):
    """Blocco sospeso a gradoni: lastra bianca sopra, sanpietrini sotto."""
    def inside(x, y):
        k = max(0, (y - top - 4) // step)
        return x0 + k * shrink <= x < x1 - k * shrink and top + 3 <= y < top + depth

    cobbles(x0, top + 3, x1, top + depth, inside)
    # bordo scuro dei gradini
    for y in range(top + 3, top + depth):
        k = max(0, (y - top - 4) // step)
        put(x0 + k * shrink, y, COBBLE_GAP)
        put(x1 - k * shrink - 1, y, COBBLE_GAP)
        if (y - top - 4) % step == 0 and k > 0:
            for x in range(x0 + (k - 1) * shrink, x0 + k * shrink):
                put(x, y, SLAB_SHADE)
            for x in range(x1 - k * shrink, x1 - (k - 1) * shrink):
                put(x, y, SLAB_SHADE)
    slab(x0, x1, top)
    for hx0, hx1 in hedges:
        hedge(hx0, hx1, top)


quads = {q["id"]: q for q in arena["quads"]}

# pavimento principale: prosegue sotto lo schermo
fx0, fy, fx1, _ = bounds(quads["main_floor"])
cobbles(fx0, fy + 3, fx1, LOW_H)
for y in range(fy + 3, LOW_H):
    put(fx0, y, COBBLE_GAP)
    put(fx1 - 1, y, COBBLE_GAP)
slab(fx0, fx1, fy)
d.rectangle([fx0 + 8, fy + 7, fx1 - 9, fy + 9], fill=SLAB)         # gradino
d.line([fx0 + 8, fy + 9, fx1 - 9, fy + 9], fill=SLAB_SHADE)
hedge(fx0 + 2, fx0 + 40, fy)
hedge(fx1 - 40, fx1 - 2, fy)
lamp_post(fx0 + 50, fy)
lamp_post(fx1 - 50, fy)

# piattaforma centrale
mx0, my, mx1, _ = bounds(quads["middle_platform"])
stepped_island(mx0, mx1, my, 22, hedges=[(mx0 + 2, mx0 + 26),
                                         (mx1 - 26, mx1 - 2)])

# scogliera sinistra (attaccata al bordo)
lx0, ly, lx1, _ = bounds(quads["left_cliff_top"])
_, _, _, lbottom = bounds(quads["left_cliff_bottom"])
stepped_island(lx0 - 10, lx1, ly, lbottom - ly, step=12, shrink=2,
               hedges=[(lx0, lx0 + 14)])

# isola destra
rx0, ry, rx1, _ = bounds(quads["right_island_top"])
_, _, _, rbottom = bounds(quads["right_island_bottom"])
stepped_island(rx0, rx1, ry, rbottom - ry, step=8, shrink=3,
               hedges=[(rx1 - 18, rx1 - 2)])

img = img.resize((LOW_W * PIXEL, LOW_H * PIXEL), Image.NEAREST)
img.save(OUTPUT, optimize=True)
print("salvato", OUTPUT)
