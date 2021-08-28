// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Put your code here.
(CHECKFORZERO)
    @R0 // if R0 is zero go to end
    D = M
    @ZEROEND
    D, JEQ
    
    @R1  // if R1 is zero go to end
    D = M
    @ZEROEND
    D, JEQ  // D is currently R1
(INITRESULT)
    D = 0
    @R2
    M = D
(COMPARE)
    @R1
    D = M
    @R0
    D = D - M  // R1 - R0
    @r1Greater
    D, JGE
(r0greater)
    // idea is to add 'n' times 'add' to R2 
    @R1
    D = M
    @n     // n je R1
    M = D
    @R0
    D = M
    @add
    M = D  // add je R0
    @LOOP
    0, JMP
(r1Greater)
    @R0
    D = M
    @n     // n je R0
    M = D
    @R1
    D = M
    @add
    M = D  // add je R1
(LOOP)
    @n
    D = M
    @END
    D, JEQ
    @n
    M = M - 1
    @add
    D = M
    @R2
    M = M + D
    @LOOP
    0, JMP
(ZEROEND)
    @R2
    M = 0
(END)
    @END
    0, JMP