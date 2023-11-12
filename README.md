# Baby_DB

Original Repository I am following is : https://github.com/cstack/db_tutorial

## Step 1: Introduction

### Components

![](https://www.sqlite.org/zipvfs/doc/trunk/www/arch1.gif)

 - Frontend
    - Tokenizer
    - Parser
    - Code generator
 - Backend
    - Virtual Machine
    - B-tree
    - Pager
    - OS Interface

Now for some definitions

**Byte Code:** Bytecode is computer object code that an interpreter converts into binary machine code so it can be read by a computer's hardware processor

**Virtual Machine:** A Virtual Machine (VM) is a compute resource that uses software instead of a physical computer to run programs and deploy apps.

**B-Tree:** a B-tree is a self-balancing tree data structure that maintains sorted data and allows searches, sequential access, insertions, and deletions in logarithmic time.

**Page:** Paging is a memory management technique in which process address space is broken into blocks of the same size called pages (size is power of 2, between 512 bytes and 8192 bytes). 



We will take input from front end as simple SQL queries and compile them into bytecode for backend. The VM will take those bytecode as instruction and operate on multiple tables.

Pager is responsible for reading and writing. We will write data in memory in pages.

The B-tree nodes are the pages. It can retrieve and save pages from and to disks by issuing commands to pager.

The os iterface as the name suggests works on operating system. 



## Step 2: Making a REPL

**REPL** means **A read–eval–print loop**, also termed an interactive toplevel or language shell, is a simple interactive computer programming environment that takes single user inputs, executes them, and returns the result to the user.

Here we will create a simple c code to take an input from user, then compare it with some string to print or exit

here only two type of command are used.
1. "Hello World!" prompt, which will echo "Bye Son"
1. ".exit" which will exit the loop
1. everythin else will print error

