import math

import cairo
import gi

gi.require_version('PangoCairo', '1.0')
gi.require_version('Pango', '1.0')
from gi.repository import Pango as pango, PangoCairo as pc

from parse import parse_text
from dto import *

#data = parse_text("cases/dexter_main.ipg")
data = parse_text("cases/obj_RAM.ipg")
obj = data.sheets[0]

default_font = pango.font_description_from_string("Arial 8")
header_font = pango.font_description_from_string("Arial 14")
pin_font = pango.font_description_from_string("Arial 7")
behavior_label = pango.font_description_from_string("Arial 7")


def make_text(ctx: cairo.Context, text, fd=default_font):
    layout = pc.create_layout(ctx)
    ctx.set_source_rgb(0, 0, 0)

    layout.set_font_description(fd)
    layout.set_text(text)
    return layout

def print_text(ctx: cairo.Context, text, right=False, fd=default_font):
    layout = pc.create_layout(ctx)
    ctx.set_source_rgb(0, 0, 0)

    layout.set_font_description(fd)
    layout.set_text(text)
    w, h = layout.get_pixel_size()

    if right:
        ctx.rel_move_to(-w, -h / 2)
    else:
        ctx.rel_move_to(0, -h / 2)

    pc.show_layout(ctx, layout)
    return (w, h)


def draw_net(ctx, points):
    if len(points) == 0:
        return
    x, y = points[0]
    ctx.move_to(x, y)

    for x, y in points[1:]:
        ctx.line_to(x, y)
    ctx.stroke()

max_x, max_y = 0,0

for x in obj.symbols:
    max_x = max(max_x, x.pos.x)
    max_y = max(max_y, x.pos.y)



max_x += 20
max_y += 20

print(max_x, max_y)


with cairo.SVGSurface("example.svg", max_x * 5, max_y * 5) as surface:
    ctx = cairo.Context(surface)


    ctx.move_to(10, 10)
    print_text(ctx, obj.type, fd=header_font)

    for b in obj.texts:
        ctx.move_to(b.pos.x * 5, b.pos.y * 5)
        lay = pc.create_layout(ctx)

        fd = pango.font_description_from_string("Arial 8")
        lay.set_font_description(fd)
        lay.set_text(b.text)
        pc.show_layout(ctx, lay)

    grid = {}

    for b in obj.symbols:
        x, y = b.pos.x * 5, b.pos.y * 5
        # sheet input is output on the grid

        if b.type == "Input":
            grid[b.pins[0]] = (x, y)

            ctx.set_line_width(1)
            ctx.set_source_rgb(1, 0, 0)
            ctx.move_to(x, y)

            ctx.rel_line_to(-4, 4)
            ctx.rel_line_to(-5, 0)
            ctx.rel_line_to(0, -8)
            ctx.rel_line_to(5, 0)
            ctx.close_path()
            ctx.stroke()

            ctx.move_to(x - 12, y)
            print_text(ctx, b.pins[0].name, right=True)
            continue
        if b.type == "Output":
            grid[b.pins[0]] = (x, y)

            ctx.set_line_width(1)
            ctx.set_source_rgb(1, 0, 0)
            ctx.move_to(x, y)
            ctx.rel_line_to(4, -4)
            ctx.rel_line_to(5, 0)
            ctx.rel_line_to(0, 8)
            ctx.rel_line_to(-5, 0)
            ctx.rel_line_to(-4, -4)
            ctx.close_path()
            ctx.stroke()

            ctx.move_to(x + 12, y)
            print_text(ctx, b.pins[0].name)
            continue

        if b.type == "Junction":
            ctx.arc(x, y, 2, 0, math.pi * 2)
            ctx.fill()

            for p in b.pins:
                grid[p] = (x, y)
            continue

        if b.type == "ListIn":
            i1, o1, o2 = b.pins
            b.pins = [i1, o2, i1]

        if b.type == "ListOut":
            i1, i2, o1 = b.pins
            b.pins = [i2, i1, o1]

        ctx.move_to(x + 6, y)
        tw, th = print_text(ctx, b.type.strip('"'), fd=behavior_label)

        ctx.set_source_rgb(0, 0, 1)
        ctx.set_line_width(1)

        outputs = [p for p in b.pins if not p.is_input]
        inputs = [p for p in b.pins if p.is_input]
        ios = max(len(inputs), len(outputs))


        height = 15 * ios

        if ios == 1:
            height+=5


        left_w = [0] * ios
        for i,w in enumerate(inputs):
            txt = make_text(ctx, w.name, fd=pin_font)
            dw, dh = txt.get_pixel_size()
            left_w[i] = dw

        right_w = [0] * ios
        for i,w in enumerate(outputs):
            txt = make_text(ctx, w.name, fd=pin_font)
            dw, dh = txt.get_pixel_size()
            right_w[i] = dw

        width = tw + 15
        for l, w in zip(left_w, right_w):
            width = max(width, l + w + 10)


        ctx.move_to(x+4, y)
        ctx.line_to(x, y)
        ctx.rel_line_to(0, height)
        ctx.rel_line_to(width, 0)
        ctx.rel_line_to(0, -height)
        ctx.rel_line_to(tw - width + 8,0)

        #ctx.rectangle(x, y, width, height)
        ctx.stroke()

        pad_size = 8
        pad_space = 15
        pad_start_y = 6

        io_x = x - pad_size
        io_y = y + pad_start_y


        if len(inputs)==1:
            io_y+=5
        for j, i in enumerate(inputs):
            # push Done & Wait to the very botton
            if i.name == "Go":

                delta = len(outputs) - len(inputs)
                if delta > 0:
                    io_y += pad_space * delta

            ctx.rectangle(io_x, io_y, pad_size, pad_size)
            ctx.set_source_rgb(0, 0, 1)
            ctx.fill()
            ctx.move_to(io_x + 10, io_y + 4)
            print_text(ctx, i.name, fd=pin_font)

            grid[i] = (io_x + 7, io_y + 4)

            io_y += pad_space

        io_x = x + width
        io_y = y + pad_start_y


        if len(outputs)==1:
            io_y+=5

        for j, o in enumerate(outputs):
            # push Done & Wait to the very botton
            if o.name == "Done":

                delta = len(inputs) - len(outputs)
                if delta > 0:
                    io_y += pad_space * delta

            ctx.rectangle(io_x, io_y, pad_size, pad_size)

            ctx.set_source_rgb(0, 0, 1)

            grid[o] = (io_x, io_y + 4)

            ctx.fill()

            ctx.move_to(io_x - 2, io_y + 4)
            print_text(ctx, o.name, right=True, fd=pin_font)

            io_y += pad_space


    for n in obj.net:
        points = []

        net_start = grid[n.left]
        if net_start:
            points.append(net_start)
        else:
            print(f"No output pin '{n.left}' for net")

        for p in n.pos:
            points.append((p.x * 5, p.y * 5))

        net_end = grid[n.right]


        if net_end:
            points.append(net_end)
        else:
            print(f"No input pin '{n.right}' for net")

        draw_net(ctx, points)

