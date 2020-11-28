# viva-tools

This is a collection of tools to work with the files produced by Viva IDEs -
heterogeneous architecture compiler tools capable of targeting FPGAs.

This is a work in progress and is likely to stay this way for the years to
come. No promises :)

- parser - a Python utility for converting Viva files to [EDN
  format](https://github.com/edn-format/edn). Not because Clojure, but because
  lispy
- files - a collection of references, docs and files found on the Github
  earlier. Of particular interest are: partial sources of Viva, Azido.msi and
  Dexter Gateware.

## The story

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


