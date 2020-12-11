import math

import cairo
import gi
gi.require_version('PangoCairo', '1.0')
gi.require_version('Pango', '1.0')
from gi.repository import Pango as pango, PangoCairo as pc

from parse import parse_text
from dto import *

data = parse_text("cases/obj_RAM.ipg")

obj = data.objects[0]

default_font = pango.font_description_from_string("Arial 8")
header_font = pango.font_description_from_string("Arial 14")

behavior_label = pango.font_description_from_string("Consolas 8")


def print_text(ctx : cairo.Context, text, right = False, fd = default_font):
    layout = pc.create_layout(ctx)
    ctx.set_source_rgb(0, 0, 0)

    layout.set_font_description(fd)
    layout.set_text(text)
    w, h = layout.get_pixel_size()

    if right:
        ctx.rel_move_to(-w, -h/2)
    else:
        ctx.rel_move_to(0, -h/2)

    pc.show_layout(ctx, layout)
    return (w, h)

def draw_net(ctx, points):
    if len(points) == 0:
        return
    x,y = points[0]
    ctx.move_to(x,y)

    for x,y in points[1:]:
        ctx.line_to(x, y)
    ctx.stroke()







with cairo.SVGSurface("example.svg", 950, 400) as surface:
    ctx = cairo.Context(surface)

    grid = {}

    ctx.move_to(10,10)
    print_text(ctx, obj.proto.type, fd=header_font)

    for b in obj.texts:
        ctx.move_to(b.gui.x * 5, b.gui.y * 5)
        lay = pc.create_layout(ctx)

        fd = pango.font_description_from_string("Arial 8")
        lay.set_font_description(fd)
        lay.set_text(b.text)
        pc.show_layout(ctx, lay)

    for b in obj.behavior:

        x, y = b.gui.x * 5, b.gui.y * 5

        #print(b.type)

        if b.type == "Input":

            grid[b.get_ref().to_pin_ref(0).to_str() + "->"] = (x+20, y)

            ctx.set_source_rgb(1, 0, 0)
            ctx.move_to(x, y-4)
            ctx.rel_line_to(16, 0)
            ctx.rel_line_to(4, 4)
            ctx.rel_line_to(-4,4)
            ctx.rel_line_to(-16, 0)
            ctx.close_path()
            ctx.stroke()

            ctx.move_to(x-4, y)
            print_text(ctx, b.outputs[0].name, right=True)

            continue

        if b.type == "Output":
            ref = "->" + b.get_ref().to_pin_ref(0).to_str()
            grid[ref] = (x,y)
            ctx.set_source_rgb(1, 0, 0)
            ctx.move_to(x, y)
            ctx.rel_line_to(4, -4)
            ctx.rel_line_to(16,0)
            ctx.rel_line_to(0, 9)
            ctx.rel_line_to(-16,0)
            ctx.rel_line_to(-4, -4)
            ctx.close_path()
            ctx.stroke()

            ctx.move_to(x+24, y)
            print_text(ctx, b.inputs[0].name)
            continue

        if b.type == "Junction":

            grid["->" + b.get_ref().to_pin_ref(0).to_str()] = (x, y)

            grid["->" + b.get_ref().to_pin_ref(1).to_str()] = (x, y)
            grid[b.get_ref().to_pin_ref(0).to_str() + "->"] = (x, y)
            grid[b.get_ref().to_pin_ref(1).to_str() + "->"] = (x, y)
            ctx.move_to(x, y)
            ctx.arc(x, y, 2, 0,math.pi * 2)
            ctx.fill()

            continue

        ctx.move_to(x + 4, y - 6)
        tw, th = print_text(ctx, b.type, fd=behavior_label)

        ctx.set_source_rgb(0,0,1)
        ctx.set_line_width(1)
        ios = max(len(b.inputs), len(b.outputs))
        height = 15 * ios + 10
        width = max(tw + 10, 20)

        ctx.rectangle(x, y, width, height)
        ctx.stroke()

        io_x = x-8
        io_y = y + 5
        for j, i in enumerate(b.inputs):
            ctx.rectangle(io_x, io_y, 8, 8)
            ctx.set_source_rgb(0,0,1)
            ctx.fill()
            ctx.move_to(io_x + 10, io_y + 4)
            print_text(ctx, i.name)

            grid["->" + b.get_ref().to_pin_ref(j).to_str()] = (io_x, io_y+4)

            io_y+=15

        io_x = x + width
        io_y = y+5

        for j,o in enumerate(b.outputs):
            ctx.rectangle(io_x, io_y, 8, 8)

            ctx.set_source_rgb(0,0,1)
            grid[b.get_ref().to_pin_ref(j).to_str() + "->"] = (io_x, io_y+4)
            ctx.fill()


            ctx.move_to(io_x -2, io_y + 4)
            print_text(ctx, o.name, right=True)


            io_y += 15



    for n in obj.net:
        points = []
        input = "->" + n.input.to_str()
        if input in grid:
            points.append(grid[input])
        else:
            print(f"NO grid for '{input}'")

        for p in n.gui:
            points.append((p.x*5, p.y*5))

        output = n.output.to_str() + "->"

        if output in grid:
            points.append(grid[output])
        else:
            print(f"NO grid for '{output}'")

        draw_net(ctx, points)

        pass #print(n.inpu)

