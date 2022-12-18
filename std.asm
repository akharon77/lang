jmp sqrt_end
sqrt:
    pop [0+rbp]
    push 0
    pop [1+rbp]
    push [0+rbp]
    push 1000
    add
    pop [2+rbp]
    push 0
    pop [3+rbp]
    push 0
    pop [4+rbp]
    sqrt_loop0:
        push [3+rbp]
        push 100000
        push rbp
        push 6
        add
        pop rbp
        call les
        push rbp
        push 6
        sub
        pop rbp
        pop rax
        cmp rax 0
        je sqrt_end_loop0
        push [1+rbp]
        push [2+rbp]
        add
        push 2000
        push 1000
        div
        div
        pop [5+rbp]
        push [5+rbp]
        push [5+rbp]
        push 1000
        div
        mul
        push [0+rbp]
        push rbp
        push 7
        add
        pop rbp
        call leq
        push rbp
        push 7
        sub
        pop rbp
        pop rax
        cmp rax 0
        jne sqrt_if0
        jmp sqrt_else0
        sqrt_if0:
            push [5+rbp]
            pop [1+rbp]
            jmp sqrt_end_if0
        sqrt_else0:
            push [5+rbp]
            pop [2+rbp]
        sqrt_end_if0:
        push [3+rbp]
        push 1000
        add
        pop [3+rbp]
        jmp sqrt_loop0
    sqrt_end_loop0:
    push [1+rbp]
    ret
    push 0
    ret
sqrt_end:

jmp and_end
and:
    pop [1+rbp]
    pop [0+rbp]
    push [0+rbp]
    push 0
    push rbp
    push 3
    add
    pop rbp
    call eq
    push rbp
    push 3
    sub
    pop rbp
    pop rax
    cmp rax 0
    jne and_if1
    jmp and_else1
    and_if1:
        push 0
        ret
        jmp and_end_if1
    and_else1:
        push [1+rbp]
        push 0
        push rbp
        push 3
        add
        pop rbp
        call eq
        push rbp
        push 3
        sub
        pop rbp
        pop rax
        cmp rax 0
        jne and_if2
        jmp and_else2
        and_if2:
            push 0
            ret
            jmp and_end_if2
        and_else2:
        and_end_if2:
        push 1000
        ret
    and_end_if1:
    push 0
    ret
and_end:

jmp or_end
or:
    pop [1+rbp]
    pop [0+rbp]
    push [0+rbp]
    push 0
    push rbp
    push 3
    add
    pop rbp
    call ger
    push rbp
    push 3
    sub
    pop rbp
    pop rax
    cmp rax 0
    jne or_if3
    jmp or_else3
    or_if3:
        push 1000
        ret
        jmp or_end_if3
    or_else3:
        push [1+rbp]
        push 0
        push rbp
        push 3
        add
        pop rbp
        call ger
        push rbp
        push 3
        sub
        pop rbp
        pop rax
        cmp rax 0
        jne or_if4
        jmp or_else4
        or_if4:
            push 1000
            ret
            jmp or_end_if4
        or_else4:
        or_end_if4:
        push 0
        ret
    or_end_if3:
    push 0
    ret
or_end:

jmp ger_end
ger:
    pop [1+rbp]
    pop [0+rbp]
    push [1+rbp]
    push [0+rbp]
    push rbp
    push 3
    add
    pop rbp
    call les
    push rbp
    push 3
    sub
    pop rbp
    ret
    push 0
    ret
ger_end:

jmp geq_end
geq:
    pop [1+rbp]
    pop [0+rbp]
    push [0+rbp]
    push [1+rbp]
    push rbp
    push 3
    add
    pop rbp
    call les
    push rbp
    push 3
    sub
    pop rbp
    push rbp
    push 3
    add
    pop rbp
    call not
    push rbp
    push 3
    sub
    pop rbp
    ret
    push 0
    ret
geq_end:

jmp leq_end
leq:
    pop [1+rbp]
    pop [0+rbp]
    push [0+rbp]
    push [1+rbp]
    push rbp
    push 3
    add
    pop rbp
    call ger
    push rbp
    push 3
    sub
    pop rbp
    push rbp
    push 3
    add
    pop rbp
    call not
    push rbp
    push 3
    sub
    pop rbp
    ret
    push 0
    ret
leq_end:

jmp neq_end
neq:
    pop [1+rbp]
    pop [0+rbp]
    push [0+rbp]
    push [1+rbp]
    push rbp
    push 3
    add
    pop rbp
    call eq
    push rbp
    push 3
    sub
    pop rbp
    push rbp
    push 3
    add
    pop rbp
    call not
    push rbp
    push 3
    sub
    pop rbp
    ret
    push 0
    ret
neq_end:

jmp les_end
les:
    pop [1+rbp]
    pop [0+rbp]
    cmp [0+rbp] [1+rbp]
    jb  les_if0
    jmp les_else0
    les_if0:
        push 1
        ret
        jmp les_end_if0
    les_else0:
        push 0
        ret
    les_end_if0:
les_end:

jmp eq_end
eq:
    pop [0+rbp]
    pop [1+rbp]
    cmp [0+rbp] [1+rbp]
    je  eq_if0
    jmp eq_else0
    eq_if0:
        push 1
        ret
        jmp eq_end_if0
    eq_else0:
        push 0
        ret
    eq_end_if0:
eq_end:

jmp not_end
not:
    pop [0+rbp]
    push [0+rbp]
    pop rax
    cmp rax 0
    je  not_if0
    jmp not_else0
    not_if0:
        push 1
        ret
        jmp not_end_if0
    not_else0:
        push 0
        ret
    not_end_if0:
not_end:

jmp out_end
out:
    out
    pop
    push 0
    ret
out_end:

jmp inp_end
inp:
    inp
    ret
inp_end:
