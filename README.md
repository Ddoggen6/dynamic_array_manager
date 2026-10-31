
<div align="center">

# ⚙️ Dynamic Array Manager

**Understanding memory. Building systems. Pursuing security.**

A modular C project exploring dynamic memory allocation, pointers, array manipulation, and systems programming through a command-line application.

<br>

![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=black)
![Systems Programming](https://img.shields.io/badge/Systems-Programming-8B0000?style=for-the-badge)
![Memory Management](https://img.shields.io/badge/Memory-Management-333333?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completed_Learning_Project-555555?style=for-the-badge)

</div>

---

## 📌 About the Project

**Dynamic Array Manager** is a personal C project created to put my knowledge of pointers, dynamic memory allocation, arrays, and program design into practice.

Instead of keeping these concepts limited to isolated exercises, I wanted to combine them into a more complete program that would challenge me to reason about memory, manage resources, handle errors, and organize program logic.

The application allows users to create and manage dynamically allocated integer arrays through a command-line interface.

Arrays are organized into groups, with each group containing a fixed number of available slots. The program manages their allocation, modification, selection, and deletion.

This project represents a stage in my development as a programmer: moving from learning individual concepts to applying them together in a practical implementation.

It is not intended to demonstrate mastery of C. It is a record of what I have learned, the problems I have worked through, and the technical curiosity that continues to drive me forward.

## 🎯 Project Goals

The main objectives behind this project are:

- Understand dynamic memory allocation and deallocation.
- Practice working with pointers and dynamically allocated arrays.
- Develop functions with clearly defined responsibilities.
- Separate program logic from user interaction.
- Handle invalid input and memory allocation failures.
- Manage resources throughout the application's lifecycle.
- Improve debugging and problem-solving skills.
- Build a stronger foundation for systems programming.

## ⚙️ Features

The application currently supports:

| Feature | Description |
|---|---|
| Array Creation | Dynamically allocate integer arrays. |
| Group Management | Organize arrays into groups and slots. |
| Array Selection | Retrieve an existing array by its group and slot. |
| Resizing | Change an array's allocated size. |
| Element Insertion | Insert values at a specified position. |
| Element Removal | Remove elements from an array. |
| Array Deletion | Release allocated memory and update group accounting. |
| Input Validation | Reject invalid positions, sizes, and selections. |
| Error Handling | Handle invalid operations and allocation failures. |
| Memory Cleanup | Release remaining allocations before program termination. |

## 🏗️ Project Structure

The project uses a modular structure to separate declarations, implementation, and user interaction.

```text
dynamic_array_manager/
│
├── src/
│   ├── main.c
│   ├── array_manager.c
│   └── array_manager.h
│
├── README.md
└── array_manager
```

### Source Files

| File | Responsibility |
|---|---|
| `main.c` | Command-line interface, menus, and user interaction. |
| `array_manager.c` | Implementation of memory and array-management operations. |
| `array_manager.h` | Constants, error codes, structures, and function declarations. |

This separation makes the application easier to maintain and extend while keeping the core management logic independent of the command-line interface.

## 🛠️ Technologies and Tools

- **C11** — Primary programming language.
- **GCC** — Compilation.
- **Git** — Version control.
- **Linux** — Development environment.

### Compilation

The project can be compiled using GCC with strict warning flags:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror \
src/main.c src/array_manager.c -o array_manager
```

Run the application:

```bash
./array_manager
```

## 🧠 Concepts Explored

<details>
<summary><b>Memory Management</b></summary>

Understanding how programs request, use, resize, and release memory.

This includes working with:

- `malloc()`
- `calloc()`
- `realloc()`
- `free()`

An important part of this process is understanding allocation failures, pointer validity, and the responsibility of releasing resources correctly.

</details>

<details>
<summary><b>Pointers and Arrays</b></summary>

Pointers provide direct insight into memory addresses and indirect access to stored data.

Through array operations, I practice accessing elements, modifying values, inserting and removing elements, and understanding how pointer-based memory manipulation works.

</details>

<details>
<summary><b>Structures and Data Organization</b></summary>

The application uses structures to represent its data:

- `ManagedArray` stores an array pointer and its logical size.
- `Group` organizes multiple managed arrays.
- `ArrayManager` coordinates the collection of groups.

These structures helped me understand how related data can be organized into more complex representations.

</details>

<details>
<summary><b>Modular Programming</b></summary>

The project separates its declarations, implementations, and user interface into different source files.

Working with header files, function prototypes, and separate compilation helped me understand how multi-file C programs are organized and linked.

</details>

<details>
<summary><b>Error Handling and Input Validation</b></summary>

Memory-related programs must account for invalid operations and unexpected conditions.

This project provides an opportunity to practice:

- Validating user input.
- Checking allocation results.
- Returning operation-specific status codes.
- Handling invalid array selections.
- Preventing operations outside valid array boundaries.

</details>

## 🧠 Why I Built This Project

Learning C changed the way I approach programming.

I became increasingly interested in what happens beneath the abstractions: where data is stored, how memory is allocated, how pointers reference objects, and why certain mistakes can cause unexpected behavior.

I wanted to move beyond writing code that simply works and begin understanding **why it works, how it interacts with memory, and what can go wrong**.

Dynamic Array Manager gave me a practical environment in which to investigate those questions.

More importantly, it helped me discover an interest in systems programming and the relationship between software and the underlying machine.

## 🖥️ From C to Rust

C remains an important part of my programming journey.

Its explicit approach to memory management challenged me to think carefully about pointers, resource lifetimes, allocation, and the consequences of implementation decisions.

That experience also made Rust particularly intriguing to me.

I am now shifting my primary learning focus toward Rust to explore its ownership and borrowing systems, its compile-time memory-safety guarantees, and its approach to systems programming.

I do not see Rust as a replacement for the knowledge I gained from C. I see it as an opportunity to build on that foundation and investigate a different approach to writing reliable low-level software.

C taught me to pay attention to memory. Rust challenges me to understand how a language can enforce stronger guarantees around it.

## 🔐 From Systems Programming to Network Security

One of the most significant outcomes of learning C was discovering the direction I want to pursue professionally.

My interest in pointers, memory, and system behavior gradually led me toward cybersecurity. I became interested in understanding how systems operate, how vulnerabilities emerge, and how software and infrastructure can be protected.

My immediate specialization goal is **network security**.

I intend to develop a strong foundation in:

- Computer networking and TCP/IP.
- IPv4, subnetting, and routing.
- Network protocols and their behavior.
- Network monitoring and traffic analysis.
- Defensive security and vulnerability assessment.
- Secure network architecture and administration.

Programming is only one part of cybersecurity, but it gives me a way to investigate system behavior, automate tasks, and eventually develop security-oriented tools.

My experience with C helped establish that connection between understanding technology at a lower level and learning how to protect the systems built on top of it.

## 🚀 Long-Term Direction

My current priority is to establish solid technical foundations before specializing further.

My intended progression is:

<pre>
C Fundamentals
      │
      ▼
Rust & Systems Programming
      │
      ▼
Computer Networking
      │
      ▼
Network Security
      │
      ▼
AI Security
</pre>

AI security is a longer-term objective. Before pursuing it, I want to develop practical competence in networking, operating systems, programming, and cybersecurity fundamentals.

I see this progression as a direction to work toward rather than a rigid sequence. The skills I develop along the way will determine how I approach future projects and specializations.

## 📖 My Learning Philosophy

I prefer understanding the logic behind a solution before focusing on its syntax.

When approaching a problem, I try to identify its requirements, reason about the operations involved, and understand how the components should interact before implementing the solution.

I value experimentation, debugging, and learning from mistakes because they force me to examine my assumptions rather than simply reproduce an existing implementation.

This project reflects that philosophy: learning by building, investigating, and gradually improving my understanding.

## 🧭 What This Repository Represents

Dynamic Array Manager is more than an exercise in dynamic arrays and memory allocation.

It represents an important stage in my development as a programmer: the point at which learning C began to influence my broader technical interests and career direction.

The project helped strengthen my appreciation for low-level programming, encouraged me to explore Rust, and contributed to my decision to pursue network security.

I may return to C for future projects whenever its capabilities and learning opportunities make it the right tool for the job.

For now, this repository stands as a practical record of my progress and a foundation for the work that comes next.

---

<div align="center">

**C was the foundation. Systems programming is the curiosity. Cybersecurity is the direction.**

</div>
