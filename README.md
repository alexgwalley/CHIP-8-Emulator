# CHIP-8-Emulator

What is this?
-------------
- This CHIP-8 emulator was made to be educational, so that I could learn the inner workings of emulators. It is meant to be a stepping stone to other emulators.
- I decided to go with CHIP-8 due to its simplicity and small instruction set and straight forward graphics in comparison to other systems such as the Gameboy or NES.


How can I run/build this?
-------------------
- Simple DirectMedia Layer 2 (SDL 2) is a requisite. If the libs and includes are in a different folder, you may need to modify the <code>Makefile</code> so that GCC can refer to them correctly.
- If on **Windows**:
- Make new project/solution in Visual Studio Code and include the files in the <code>src</code> folder. Link the Include and Lib folders to the correct SDL2 directories on your machine. Build!
- If on **UNIX** machine: 
- Run the command <code>make</code> when navigated to the <code>src</code> directory in the Terminal.
- If this doesn't work, you may need to change the compiler options in <code>Makefile</code> to point to the correct include and lib folders, or you may need to add SDL to your <code>/usr/lib</code> and <code>/usr/local/include</code> file paths.
- The executable is named <code>main</code> by default.


How can I make an CHIP-8 Emulator?
----------------------------------
- Here are a couple of links that I used, but this list is by no means exhaustive.
  - <https://en.wikipedia.org/wiki/CHIP-8>
  - <http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/>
  - <http://devernay.free.fr/hacks/chip8/C8TECH10.HTM>
  - <http://mattmik.com/files/chip8/mastering/chip8.html>




