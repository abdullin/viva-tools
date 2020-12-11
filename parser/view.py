import cairo
import gi
gi.require_version('PangoCairo', '1.0')
gi.require_version('Pango', '1.0')
from gi.repository import Pango as pango, PangoCairo as pc

from parse import parse_text

data = parse_text("cases/obj_RAM.ipg")

obj = data.objects[0]

default_font = pango.font_description_from_string("Arial 8")

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


with cairo.SVGSurface("example.svg", 950, 400) as surface:
    ctx = cairo.Context(surface)


    ctx.move_to(10,10)
    ctx.show_text(obj.proto.name)
    ctx.move_to(20,20)

    for b in obj.texts:
        ctx.move_to(b.gui.x * 5, b.gui.y * 5)
        lay = pc.create_layout(ctx)

        fd = pango.font_description_from_string("Arial 8")
        lay.set_font_description(fd)
        lay.set_text(b.text)
        pc.show_layout(ctx, lay)

    for b in obj.behavior:

        x, y = b.gui.x * 5, b.gui.y * 5

        if b.name.startswith("Input"):

            ctx.set_source_rgb(1, 0, 0)
            ctx.move_to(x, y)
            ctx.rel_line_to(0, -4)
            ctx.rel_line_to(4,4)
            ctx.rel_line_to(-4, 4)
            ctx.close_path()
            ctx.stroke()

            ctx.move_to(x-4, y)
            print_text(ctx, b.outputs[0][1], right=True)

            continue

        if b.name.startswith("Output"):
            ctx.set_source_rgb(1, 0, 0)
            ctx.move_to(x, y)
            ctx.rel_line_to(4, -4)
            ctx.rel_line_to(0, 9)
            ctx.rel_line_to(-4, -4)
            ctx.close_path()
            ctx.stroke()

            ctx.move_to(x+8, y)
            print_text(ctx, b.inputs[0][1])
            continue

        if b.name.startswith("Junction"):
            ctx.move_to(x -4, y)
            ctx.rel_line_to(8, 0)
            ctx.move_to(x,y-4)
            ctx.rel_line_to(0, 8)
            ctx.stroke()


            continue

        ctx.move_to(x + 4, y - 6)
        tw, th = print_text(ctx, b.name, fd=behavior_label)

        ctx.set_source_rgb(0,0,1)
        ctx.set_line_width(1)
        ios = max(len(b.inputs), len(b.outputs))
        height = 15 * ios + 10
        width = max(tw + 10, 20)

        ctx.rectangle(x, y, width, height)
        ctx.stroke()




