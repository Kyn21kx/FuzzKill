# FuzzKill
## Fuzzy search everything, kill distractions.

<!--toc:start-->
- [FuzzKill](#fuzzkill)
  - [Showcase](#showcase)
  - [Building](#building)
  - [Shortcut](#shortcut)
  - [Why?](#why)
  - [Additional Features](#additional-features)
<!--toc:end-->

### Showcase

https://github.com/user-attachments/assets/8463186d-d534-4608-a0a2-6ca334cea582

### Building
This project uses the Zig toolchain for C and C++, [download the latest version of Zig](https://ziglang.org/learn/getting-started/)
Then run
```ps
zig build
```
on the root of the project

### Shortcut
Once the program starts it will listen for the `ALT + SHIFT + DELETE` command to pop up, this can be customized on `main.cpp` although I suggest using custom shortcuts like macros with ZMK to make this a better experience.

### Why?
The concept is simple, we do nog have to waste our time and brain power visually searching the right window to switch to, things can be faster, they can be better.

This is why I made FuzzKill, it was an idea that was floating around my head for a while, and it wasn't until [Adam Basis did something similar](https://youtu.be/pAbf3jtoovA?si=cK_uLqbNKmXrV02V), that I decided to dedicate a weekend to make this

### Additional Features
- Calculator
  - You can open up FuzzKill and type in any simple math expression, if you hit enter you will see the result pop up

https://github.com/user-attachments/assets/ef59d543-bc46-41e7-b052-bf37f3b3df59

