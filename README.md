# viva-tools

This is a collection of tools to work with the files produced by Viva IDEs -
heterogeneous architecture compiler tools capable of targeting FPGAs.

This is a work in progress and is likely to stay this way for the years to
come. No promises :)

## Files

Files folder contains a collection of references, docs and files found on the Github
earlier. Of particular interest are: partial sources of Viva, Azido.msi and
Dexter Gateware.

## Parser

Parser - a Python utility to parse Viva text files and experiment with
converting it into different formats. The purpose is to explore the domain
and figure out the most convenient format (human-readable for git diffing,
yet convenient to read/write from the software). 

Currently using EDN and JSON. EDN displays nicely but is pain to read from the
.NET C#. JSON is ugly to read but easier to deserialize.

## Viewer

Viewer is an experiment in loading Viva/Azido files and displaying them. The
purpose is to find a set of technologies that fit the constraints and are the
most comfortable for me to iterate rapidly upon. It is an extremely subjective thing.

Current version does this with C# and GTK.

Failed attepmts that didn't quite work out for me:

- C++ + Borland/Embarcadero - I'm not fluent in C++, it will hamper any progress.
- Lazarus (Pascal + Delphi IDEs) - UI experience is a breath of fresh air (and a blast from the familiar past). Pascal doesn't feel like a productive environment to me anymore, though.
- Clojure + Spring - Clojure is nice, fits EDN. Drawing with it in Spring
  requires figuring out threading in Clojure.
- Qt + Python - Qt is C++, so bindings are messy in every single language. Plus
  it feels heavy to install and use.
- Go + GTK - I love go, but it just gets too verbose in complex projects. Let's
  leave it to Hashicorp and K8S guys.


# The story

In year 2019 I got inspired with the robotic hand design (published and open
sourced by the Haddington Dynamics). There was some exploration and blog posts:

- [Picking a robotic hobbyi (2019)](https://abdullin.com/child-friendly-hobby/)
- [Dive into FPGA, PCB and 3D printing
  (2019)](https://abdullin.com/dive-into-fpga/)
- [Robotic R&D: FPGA and ML Pipelines
  (2019)](https://abdullin.com/running-on-a-real-fpga/)

At some point the project got put on hold. The biggest roadblock was the
difficulty of getting proficient with the FPGA programming. Proficient enough
to implement the FPGA-driven loop between the optical encoders and the motors.

Fast-forward some time later and I'm back at the project again. However, this
time trying a different approach: learn enough about the Viva tool used to
program Dexter FPGA. It has a unique approach that is different from the
Verilog/VHDL.

The Viva tool itself was published at some point by [Haddington Dynamics](https://github.com/HaddingtonDynamics/Dexter) as a
binary, along with a source code for the Dexter gateware. Plus, the partial
source code of Viva was made available at some point under the GPL-3.0 license
(all included in this repository).

That information might be enough to find a way to:

- read CoreLib and Dexter files;
- display and manipulate them in a GUI;
- perform recursive resolution of the files;
- synthesize the solutions for x86/ARM (to simulate) and for the Zynq/Xilinx
  boards (to execute).

Even if I fail (and I probably will), the journey is already worth the lessons
learnt.


