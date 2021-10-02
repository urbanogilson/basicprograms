# Problem 06

## Description

### Compilation steps

#### Preprocessing

```sh
gcc -E sum.c > sum.i
gcc -E program-06.c > program-06.i
```

#### compiling

```sh
gcc -S -fverbose-asm -masm=att sum.i program-06.i
```

#### Assembly

```sh
gcc -c sum.s program-06.s
```

#### Linking

```sh
gcc sum.o program-06.o
```

### Tip

To create all intermediate files in one command

```sh
gcc -save-temps -fverbose-asm -masm=att sum.c program-06.c 
```

## References

1 - [Using the GNU Compiler Collection (GCC)](https://gcc.gnu.org/onlinedocs/gcc-11.2.0/gcc/)
2 - [The Four Stages of Compiling a C Program](https://www.calleluks.com/the-four-stages-of-compiling-a-c-program/)
3 - [Lecture 5: C Program Compilation](https://people.sc.fsu.edu/~jburkardt/classes/isc_2012/c_program_compilation.pdf)
