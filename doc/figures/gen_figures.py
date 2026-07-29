# -*- coding: utf-8 -*-
"""Generate patent-style block diagrams for 3-ch LVDS pixel-compare tech disclosure."""
import os
from PIL import Image, ImageDraw, ImageFont

OUT = os.path.dirname(os.path.abspath(__file__))

candidates = [
    r"C:\Windows\Fonts\msyh.ttc",
    r"C:\Windows\Fonts\simhei.ttf",
    r"C:\Windows\Fonts\simsun.ttc",
]
FONT_PATH = next((p for p in candidates if os.path.exists(p)), None)


def font(size):
    if FONT_PATH:
        return ImageFont.truetype(FONT_PATH, size)
    return ImageFont.load_default()


BG = (255, 255, 255)
BOX = (232, 242, 252)
BOX2 = (255, 248, 230)
BOX3 = (232, 248, 232)
BOX4 = (245, 235, 250)
EDGE = (40, 70, 120)
TEXT = (20, 30, 50)
ARROW = (50, 80, 130)
TITLE = (15, 40, 90)


def rounded_rect(draw, xy, fill, outline, r=10, width=2):
    draw.rounded_rectangle(xy, radius=r, fill=fill, outline=outline, width=width)


def center_text(draw, box, text, f, fill=TEXT, pad=10, line_gap=6):
    x0, y0, x1, y1 = box
    x0 += pad
    y0 += pad
    x1 -= pad
    y1 -= pad
    lines = text.split("\n")
    heights, widths = [], []
    for ln in lines:
        bbox = draw.textbbox((0, 0), ln, font=f)
        widths.append(bbox[2] - bbox[0])
        heights.append(bbox[3] - bbox[1])
    total_h = sum(heights) + (len(lines) - 1) * line_gap
    cy = y0 + max(0, (y1 - y0 - total_h) / 2)
    for i, ln in enumerate(lines):
        cx = x0 + max(0, (x1 - x0 - widths[i]) / 2)
        draw.text((cx, cy), ln, font=f, fill=fill)
        cy += heights[i] + line_gap


def arrow_h(draw, x0, y, x1, color=ARROW, w=2):
    draw.line([(x0, y), (x1, y)], fill=color, width=w)
    if x1 > x0:
        draw.polygon([(x1, y), (x1 - 10, y - 5), (x1 - 10, y + 5)], fill=color)
    else:
        draw.polygon([(x1, y), (x1 + 10, y - 5), (x1 + 10, y + 5)], fill=color)


def arrow_v(draw, x, y0, y1, color=ARROW, w=2):
    draw.line([(x, y0), (x, y1)], fill=color, width=w)
    if y1 > y0:
        draw.polygon([(x, y1), (x - 5, y1 - 10), (x + 5, y1 - 10)], fill=color)
    else:
        draw.polygon([(x, y1), (x - 5, y1 + 10), (x + 5, y1 + 10)], fill=color)


def dbl_arrow_h(draw, x0, y, x1, color=ARROW, w=2):
    draw.line([(x0, y), (x1, y)], fill=color, width=w)
    draw.polygon([(x1, y), (x1 - 10, y - 5), (x1 - 10, y + 5)], fill=color)
    draw.polygon([(x0, y), (x0 + 10, y - 5), (x0 + 10, y + 5)], fill=color)


def save_cropped(im, path, pad=20):
    px = im.load()
    w, h = im.size
    top = 0
    for y in range(h):
        if any(px[x, y] != BG for x in range(0, w, 4)):
            top = max(0, y - pad)
            break
    bottom = h
    for y in range(h - 1, -1, -1):
        if any(px[x, y] != BG for x in range(0, w, 4)):
            bottom = min(h, y + pad + 1)
            break
    left = 0
    for x in range(w):
        if any(px[x, y] != BG for y in range(0, h, 4)):
            left = max(0, x - pad)
            break
    right = w
    for x in range(w - 1, -1, -1):
        if any(px[x, y] != BG for y in range(0, h, 4)):
            right = min(w, x + pad + 1)
            break
    im.crop((left, top, right, bottom)).save(path, "PNG")
    print("wrote", path)


def make_fig1(ft, fs, fss):
    w, h = 1500, 760
    im = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(im)

    b_host = (40, 140, 220, 300)
    rounded_rect(d, b_host, BOX2, EDGE)
    center_text(d, b_host, "上位机\n(PC/工控机)\n测试软件", fs)

    b_eth = (260, 170, 420, 270)
    rounded_rect(d, b_eth, BOX, EDGE)
    center_text(d, b_eth, "以太网\nRJ45/RGMII", fs)
    dbl_arrow_h(d, 220, 220, 260)

    d.rounded_rectangle((450, 60, 1460, 700), radius=14, outline=EDGE, width=3)
    d.text((470, 75), "3通道LVDS图像输入比较装置", font=fs, fill=TITLE)

    b_phy = (480, 140, 660, 250)
    rounded_rect(d, b_phy, BOX, EDGE)
    center_text(d, b_phy, "以太网PHY\nRTL8211F", fs)
    dbl_arrow_h(d, 420, 220, 480)

    b_soc = (700, 120, 1080, 420)
    rounded_rect(d, b_soc, BOX3, EDGE, r=12)
    center_text(d, (700, 130, 1080, 185), "ZynqMP  XCZU2CG", fs)
    mods = [
        (720, 200, 880, 275, "PS\nlwIP/固件"),
        (900, 200, 1060, 275, "PL\n解串+比较"),
        (720, 295, 880, 370, "VDMA\n参考/推流"),
        (900, 295, 1060, 370, "AXI-Lite\n寄存器"),
    ]
    for m in mods:
        rounded_rect(d, m[:4], (255, 255, 255), EDGE, r=6, width=1)
        center_text(d, m[:4], m[4], fss)
    dbl_arrow_h(d, 660, 210, 700)

    b_ddr = (700, 450, 920, 560)
    rounded_rect(d, b_ddr, BOX4, EDGE)
    center_text(d, b_ddr, "DDR\n参考/错误帧", fs)
    arrow_v(d, 800, 420, 450)

    b_des = (1120, 140, 1420, 320)
    rounded_rect(d, b_des, BOX2, EDGE)
    center_text(d, b_des, "解串前端\nMAX96752 ×3路\n双链路 OLDI/LVDS", fs)
    dbl_arrow_h(d, 1080, 230, 1120)

    b_lvds = (1120, 360, 1420, 500)
    rounded_rect(d, b_lvds, BOX, EDGE)
    center_text(d, b_lvds, "LVDS接口\nch0/ch1/ch2\nclk+data差分", fs)
    arrow_v(d, 1270, 320, 360)

    b_gmsl = (1120, 540, 1420, 660)
    rounded_rect(d, b_gmsl, BOX4, EDGE)
    center_text(d, b_gmsl, "远端GMSL2\n(MAX96717等)\n相机/注入源", fs)
    arrow_v(d, 1270, 500, 540)

    rounded_rect(d, (480, 450, 660, 640), (250, 250, 250), EDGE, r=8, width=1)
    d.text((495, 470), "电源模块", font=fss, fill=TEXT)
    d.text((495, 510), "启动Flash", font=fss, fill=TEXT)
    d.text((495, 550), "I2C / GPIO", font=fss, fill=TEXT)
    d.text((495, 590), "指示/复位", font=fss, fill=TEXT)

    save_cropped(im, os.path.join(OUT, "fig1_overall.png"))


def make_fig2(ft, fs, fss):
    w, h = 1300, 720
    im = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(im)

    d.rounded_rectangle((40, 80, 420, 660), radius=12, outline=(100, 60, 30), width=3)
    d.text((60, 95), "载板 Carrier", font=fs, fill=(100, 60, 30))
    base_items = [
        (70, 150, 390, 250, "电源模块\n多路稳压"),
        (70, 280, 390, 380, "以太网RJ45\n+ PHY"),
        (70, 410, 390, 510, "板间连接器\n电源/高速/控制"),
        (70, 540, 390, 630, "外围IO\nUART/继电器等"),
    ]
    for it in base_items:
        rounded_rect(d, it[:4], BOX2, (100, 60, 30))
        center_text(d, it[:4], it[4], fs)

    d.rounded_rectangle((460, 80, 860, 660), radius=12, outline=(30, 100, 60), width=3)
    d.text((480, 95), "Capture / 核心板", font=fs, fill=(30, 100, 60))
    core_items = [
        (490, 150, 830, 280, "ZynqMP\nXCZU2CG-SFVC784"),
        (490, 310, 830, 410, "DDR 存储器\n参考/推流缓冲"),
        (490, 440, 830, 540, "启动配置介质\nQSPI/eMMC"),
        (490, 570, 830, 640, "板间连接器"),
    ]
    for it in core_items:
        rounded_rect(d, it[:4], BOX3, (30, 100, 60))
        center_text(d, it[:4], it[4], fs)

    d.rounded_rectangle((900, 80, 1260, 660), radius=12, outline=(60, 40, 110), width=3)
    d.text((920, 95), "解串前端", font=fs, fill=(60, 40, 110))
    des_items = [
        (920, 150, 1240, 280, "MAX96752\nGMSL2解串×通道"),
        (920, 310, 1240, 430, "OLDI/LVDS\n双链路差分出"),
        (920, 460, 1240, 560, "I2C 配置口"),
        (920, 590, 1240, 640, "GMSL2 入线座"),
    ]
    for it in des_items:
        rounded_rect(d, it[:4], BOX4, (60, 40, 110))
        center_text(d, it[:4], it[4], fs)

    d.line([(420, 360), (460, 360)], fill=ARROW, width=4)
    d.line([(860, 360), (900, 360)], fill=ARROW, width=4)
    d.text((425, 375), "对接", font=fss, fill=ARROW)
    d.text((865, 375), "对接", font=fss, fill=ARROW)

    save_cropped(im, os.path.join(OUT, "fig2_boards.png"))


def make_fig3(ft, fs, fss):
    w, h = 1500, 980
    im = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(im)
    f_title = font(20)
    f_body = font(16)

    d.rounded_rectangle((28, 28, 1472, 952), radius=12, outline=EDGE, width=3)
    d.text((48, 42), "ZynqMP：PS + PL（system BD）", font=f_title, fill=TITLE)

    # PS row
    b_ps = (50, 90, 420, 260)
    rounded_rect(d, b_ps, BOX2, EDGE)
    center_text(d, b_ps, "PS\nlwIP UDP:5555\n固件 / I2C / GPIO\n命令0x30比较控制", f_body)

    b_gem = (460, 90, 720, 260)
    rounded_rect(d, b_gem, BOX, EDGE)
    center_text(d, b_gem, "GEM3 + RGMII\n以太网主机口", f_body)
    dbl_arrow_h(d, 420, 175, 460)

    b_ddr = (760, 90, 1100, 260)
    rounded_rect(d, b_ddr, BOX4, EDGE)
    center_text(d, b_ddr, "DDR\n参考帧 / 错误帧\n推流帧缓冲", f_body)

    b_ver = (1140, 90, 1440, 260)
    rounded_rect(d, b_ver, BOX, EDGE)
    center_text(d, b_ver, "AXI_LITE_REG\n版本 / 板级信息", f_body)

    # Three channels
    d.text((50, 290), "PL：三路独立并行（lvds_s0 / lvds_s1 / lvds_s2）", font=f_title, fill=TITLE)
    chs = [
        (50, 340, 480, 620, "通道0"),
        (510, 340, 940, 620, "通道1"),
        (970, 340, 1440, 620, "通道2"),
    ]
    for box, name in zip(chs, ["ch0", "ch1", "ch2"]):
        rounded_rect(d, box[:4], BOX3, EDGE)
        center_text(
            d,
            box[:4],
            f"{box[4]}\nlvds4x2_1to7 解串\nvid_in + 监视\nVDMA 参考\ncombiner+pixel_compare",
            f_body,
        )

    # Bottom shared
    b_sw = (50, 680, 520, 900)
    rounded_rect(d, b_sw, BOX2, EDGE)
    center_text(d, b_sw, "axis_switch\n推流源选择\n→ VDMA_lwip\n→ UDP视频", f_body)

    b_cmp = (560, 680, 1000, 900)
    rounded_rect(d, b_cmp, BOX4, EDGE)
    center_text(
        d,
        b_cmp,
        "AXI-Lite 比较控制\n容差/忽略色/ROI/点采样\n中断汇总至 PS",
        f_body,
    )

    b_io = (1040, 680, 1440, 900)
    rounded_rect(d, b_io, BOX, EDGE)
    center_text(d, b_io, "差分IO\n→ MAX96752\n三路双链路LVDS", f_body)

    arrow_v(d, 265, 620, 680)
    arrow_v(d, 725, 620, 680)
    arrow_v(d, 1205, 620, 680)

    save_cropped(im, os.path.join(OUT, "fig3_fpga.png"))


def make_fig4(ft, fs, fss):
    w, h = 1400, 720
    im = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(im)

    rounded_rect(d, (40, 80, 260, 220), BOX2, EDGE)
    center_text(d, (40, 80, 260, 220), "远端源\n相机/注入\n+MAX96717", fs)

    rounded_rect(d, (320, 80, 560, 220), BOX4, EDGE)
    center_text(d, (320, 80, 560, 220), "GMSL2\n串行链路", fs)
    arrow_h(d, 260, 150, 320)

    # three deserializers
    for i, y in enumerate([80, 280, 480]):
        rounded_rect(d, (620, y, 880, y + 140), BOX, EDGE)
        center_text(d, (620, y, 880, y + 140), f"MAX96752\n通道{i}\n双OLDI输出", fs)
        if i == 0:
            arrow_h(d, 560, 150, 620)
        else:
            # fan-out lines from GMSL box conceptually — draw from mid
            d.line([(560, 150), (590, 150), (590, y + 70), (620, y + 70)], fill=ARROW, width=2)
            draw = d
            draw.polygon(
                [(620, y + 70), (610, y + 65), (610, y + 75)],
                fill=ARROW,
            )

        rounded_rect(d, (940, y, 1180, y + 140), BOX3, EDGE)
        center_text(d, (940, y, 1180, y + 140), f"lvds4x2_1to7_{i}\n1:7解串\nRGB映射", fs)
        arrow_h(d, 880, y + 70, 940)

        rounded_rect(d, (1240, y, 1360, y + 140), BOX2, EDGE)
        center_text(d, (1240, y, 1360, y + 140), f"比较核\nch{i}", fss)
        arrow_h(d, 1180, y + 70, 1240)

    d.text(
        (40, 640),
        "说明：三路解串与比较路径硬件独立并行；每路为双链路时钟+4lane数据，经1:7解串得到VS/HS/DE与RGB像素。",
        font=fss,
        fill=TEXT,
    )
    d.text(
        (40, 675),
        "实际板卡可根据拓扑将多通道解串器件分板布置；软件经I2C分别配置各通道MAX96752。",
        font=fss,
        fill=TEXT,
    )

    save_cropped(im, os.path.join(OUT, "fig4_lvds_ch.png"))


def make_fig5(ft, fs, fss):
    w, h = 1400, 780
    im = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(im)

    d.rounded_rectangle((40, 70, 680, 360), radius=10, outline=EDGE, width=2)
    d.text((60, 85), "场景一：参考捕获 + 帧差使能", font=fs, fill=TITLE)
    rounded_rect(d, (70, 140, 230, 250), BOX2, EDGE)
    center_text(d, (70, 140, 230, 250), "上位机\n以太网", fss)
    rounded_rect(d, (280, 140, 480, 250), BOX3, EDGE)
    center_text(d, (280, 140, 480, 250), "本装置\n写DDR参考\n开比较", fss)
    rounded_rect(d, (530, 140, 650, 320), BOX, EDGE)
    center_text(d, (530, 140, 650, 320), "实时\nLVDS\n入", fss)
    dbl_arrow_h(d, 230, 195, 280)
    dbl_arrow_h(d, 480, 195, 530)
    d.text((70, 280), "配置容差/忽略色/ROI，IRQ上报错误", font=fss, fill=TEXT)

    d.rounded_rectangle((720, 70, 1360, 360), radius=10, outline=EDGE, width=2)
    d.text((740, 85), "场景二：三路并行比对", font=fs, fill=TITLE)
    for i, x in enumerate([760, 960, 1160]):
        rounded_rect(d, (x, 150, x + 160, 280), BOX3, EDGE)
        center_text(d, (x, 150, x + 160, 280), f"通道{i+1}\n独立比较核", fss)
    d.text((760, 300), "三路同时使能，互不时分复用", font=fss, fill=TEXT)

    d.rounded_rectangle((40, 400, 1360, 730), radius=10, outline=EDGE, width=2)
    d.text((60, 415), "场景三：推流监视 + 错误定位", font=fs, fill=TITLE)
    rounded_rect(d, (80, 480, 320, 620), BOX3, EDGE)
    center_text(d, (80, 480, 320, 620), "axis_switch\n选通道推流", fs)
    rounded_rect(d, (380, 480, 620, 620), BOX2, EDGE)
    center_text(d, (380, 480, 620, 620), "UDP视频\n上位机显示", fs)
    rounded_rect(d, (680, 480, 980, 620), BOX4, EDGE)
    center_text(d, (680, 480, 980, 620), "读首错坐标\n像素锁存\n错误帧缓冲", fs)
    rounded_rect(d, (1040, 480, 1300, 620), BOX, EDGE)
    center_text(d, (1040, 480, 1300, 620), "I2C配置\nGPIO复位\n重新开流", fs)
    arrow_h(d, 320, 550, 380)
    arrow_h(d, 620, 550, 680)
    arrow_h(d, 980, 550, 1040)
    d.text(
        (80, 660),
        "同一板卡完成采集、参考、比较、推流与故障截取，减少外置设备。",
        font=fss,
        fill=TEXT,
    )

    save_cropped(im, os.path.join(OUT, "fig5_modes.png"))


def make_fig6(ft, fs, fss):
    w, h = 1400, 560
    im = Image.new("RGB", (w, h), BG)
    d = ImageDraw.Draw(im)

    steps = [
        (40, 100, 220, 280, "1 LVDS入\n1:7解串\nRGB实时流"),
        (250, 100, 430, 280, "2 broadcaster\n实时旁路\n+写DDR参考"),
        (460, 100, 660, 280, "3 VDMA读出\n参考帧"),
        (690, 100, 900, 280, "4 combiner\n48bit拼流\nref|live"),
        (930, 100, 1140, 280, "5 pixel_compare\n帧差/ROI\n中断"),
        (1170, 100, 1360, 280, "6 PS固件\n上报/推流\n错误帧"),
    ]
    for i, s in enumerate(steps):
        rounded_rect(d, s[:4], BOX3 if i % 2 == 0 else BOX2, EDGE)
        center_text(d, s[:4], s[4], fss)
        if i < len(steps) - 1:
            arrow_h(d, s[2], 190, steps[i + 1][0])

    d.text(
        (40, 340),
        "帧差规则：非忽略色像素若任一通道 |ref−live| > PIXEL_THRESHOLD，则错误计数+1；达 ERR_PIXEL_CNT 产生 intr。",
        font=fss,
        fill=TEXT,
    )
    d.text(
        (40, 380),
        "首错锁存 ERR_COL/ERR_LINE 与参考/实时像素；下一帧 SOF 或写 INTR_CLEAR 清除中断相关状态。",
        font=fss,
        fill=TEXT,
    )
    d.text(
        (40, 420),
        "数据打包：AXIS[47:24]=参考，[23:0]=实时；颜色字节序为 RBG（固件/上位机可做 RGB 转换）。",
        font=fss,
        fill=TEXT,
    )
    d.text(
        (40, 460),
        "以太网命令 0x30 控制开/关比较、阈值、分辨率、状态块与自动上报；通道号从 1 起对应三路实例。",
        font=fss,
        fill=TEXT,
    )

    save_cropped(im, os.path.join(OUT, "fig6_datapath.png"))


def main():
    ft, fs, fss = font(28), font(18), font(15)
    make_fig1(ft, fs, fss)
    make_fig2(ft, fs, fss)
    make_fig3(ft, fs, fss)
    make_fig4(ft, fs, fss)
    make_fig5(ft, fs, fss)
    make_fig6(ft, fs, fss)
    print("ALL DONE")


if __name__ == "__main__":
    main()
