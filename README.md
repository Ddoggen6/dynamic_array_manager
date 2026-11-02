
<div align="center">

# ⚙️ Dynamic Array Manager

**Understanding memory. Building systems. Pursuing security.**

A modular C project exploring dynamic memory allocation, pointers, array manipulation, and systems programming through a command-line application and an experimental web interface.

<br>

![C](https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=black)
![Systems Programming](https://img.shields.io/badge/Systems-Programming-8B0000?style=for-the-badge)
![Memory Management](https://img.shields.io/badge/Memory-Management-333333?style=for-the-badge)
![Web Interface](https://img.shields.io/badge/Web-Interface-6D4BC3?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-In_Development-555555?style=for-the-badge)

</div>

---

## 📌 About the Project

**Dynamic Array Manager** is a personal C project created to put my knowledge of pointers, dynamic memory allocation, arrays, and program design into practice.

Instead of keeping these concepts limited to isolated exercises, I wanted to combine them into a more complete program that would challenge me to reason about memory, manage resources, handle errors, and organize program logic.

The application allows users to create and manage dynamically allocated integer arrays through a command-line interface. Arrays are organized into groups, with each group containing a fixed number of available slots.

The project has also expanded beyond the command line. I am experimenting with a web interface that communicates directly with the C application through an HTTP server and a JSON API.

This introduces another layer of learning: understanding how a systems-level application can expose its functionality to a browser without moving its core logic into another language.

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
- Understand HTTP communication between a browser and a C application.
- Explore JSON generation and API design in C.
- Build a frontend that communicates with a backend.
- Improve debugging and problem-solving skills.
- Build a stronger foundation for systems programming.

## ⚙️ Features

### Core Array Management

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

### Web Interface and API

| Feature | Description |
|---|---|
| HTTP Server | Run a local HTTP server using Mongoose. |
| Static File Serving | Serve HTML, CSS, and JavaScript files. |
| JSON API | Expose array-manager information through HTTP endpoints. |
| Group Information | Retrieve information about all 10 groups. |
| Slot Statistics | Report occupied and available slots per group. |
| Frontend Communication | Retrieve backend data using JavaScript `fetch()`. |
| Dashboard | Display a web-based interface for memory management. |

**Current implementation note:** The group-information endpoint and frontend communication are working. Dynamic rendering and additional array-management operations through the web interface are still under development.

## 🏗️ Project Structure

The project uses a modular structure to separate declarations, implementation, user interaction, HTTP communication, and frontend resources.

```text
dynamic_array_manager/
│
├── src/
│   ├── main.c
│   ├── array_manager.c
│   ├── array_manager.h
│   └── web_server.c
│
├── mongoose/
│   ├── mongoose.c
│   └── mongoose.h
│
├── web/
│   ├── index.html
│   ├── style.css
│   └── script.js
│
├── README.md
│
├── array_manager
└── array_manager_web
```

### Source Files

| File | Responsibility |
|---|---|
| `main.c` | Command-line interface, menus, and user interaction. |
| `array_manager.c` | Implementation of memory and array-management operations. |
| `array_manager.h` | Constants, error codes, structures, and function declarations. |
| `web_server.c` | HTTP server initialization, request handling, and API endpoints. |
| `mongoose.c` | HTTP server implementation provided by Mongoose. |
| `mongoose.h` | Mongoose declarations and configuration. |
| `index.html` | Web dashboard structure. |
| `style.css` | Dashboard layout and visual styling. |
| `script.js` | Frontend logic and communication with the C backend. |

The CLI and web server are separate executables. Both use the same array-management implementation, but each has its own entry point.

## 🌐 Web Architecture

The web interface introduces a separation between the frontend and the application's core logic.

The browser communicates with the C backend through HTTP requests. Mongoose handles incoming connections, serves static frontend resources, and forwards API requests to the appropriate C functions.

The current architecture follows this flow:

```text
             Browser
                |
                |
        HTML / CSS / JS
                |
                | HTTP Request
                v
        Mongoose HTTP Server
                |
                v
          web_server.c
                |
                v
          array_manager.c
                |
                v
          ArrayManager
                |
                | JSON Response
                v
             Browser
```

### Current API Endpoints

| Method | Endpoint | Description |
|---|---|---|
| GET | `/api/hello` | Basic server connectivity test. |
| GET | `/api/groups` | Retrieve information about all groups. |

### Example API Response

The `/api/groups` endpoint returns a JSON representation of the manager's current state.

```json
{
    "groups": [
        {
            "group": 1,
            "occupied": 1,
            "available": 99,
            "arrays": [
                {
                    "slot": 1,
                    "size": 10
                }
            ]
        },
        {
            "group": 2,
            "occupied": 1,
            "available": 99,
            "arrays": [
                {
                    "slot": 1,
                    "size": 25
                }
            ]
        }
    ]
}
```

The response includes group numbers, occupied slots, available slots, and metadata about allocated arrays.

Group and slot numbers are represented using one-based indexing in the API, while the underlying C implementation uses zero-based indexing.

## 🛠️ Technologies and Tools

### Backend

- **C** — Primary programming language.
- **GCC** — Compilation.
- **Mongoose** — Embedded HTTP server.
- **Git** — Version control.
- **Linux** — Development environment.

### Frontend

- **HTML5** — Dashboard structure.
- **CSS3** — Layout and visual styling.
- **JavaScript** — Frontend logic and HTTP communication.
- **Fetch API** — Communication with the C backend.

### Compilation

The CLI and web server must be compiled separately because they have different entry points.

**Command-line application:**

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -Werror \
    src/main.c src/array_manager.c \
    -o array_manager
```

Run:

```bash
./array_manager
```

**Web server:**

```bash
gcc -std=gnu11 -Wall -Wextra -Wpedantic \
    -Imongoose \
    src/web_server.c src/array_manager.c mongoose/mongoose.c \
    -o array_manager_web
```

Run:

```bash
./array_manager_web
```

The web server listens locally at:

```text
http://127.0.0.1:8080
```

The frontend can then be accessed through the same address.

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

The project separates its declarations, implementations, and user interfaces into different source files.

Working with header files, function prototypes, and separate compilation helped me understand how multi-file C programs are organized and linked.

</details>

<details>
<summary><b>HTTP and API Development</b></summary>

The web interface introduced me to the relationship between HTTP servers, clients, and backend applications.

Through Mongoose, I am exploring:

- HTTP server initialization.
- Request handling and URI matching.
- Static file serving.
- HTTP response generation.
- JSON serialization.
- Passing application data to request handlers.

</details>

<details>
<summary><b>Frontend and Backend Communication</b></summary>

Using HTML, CSS, and JavaScript, I am learning how a browser can interact with an application written in C.

This includes:

- Structuring a dashboard with HTML.
- Styling interfaces with CSS.
- Making HTTP requests using `fetch()`.
- Parsing JSON responses.
- Working with JavaScript objects and arrays.
- Dynamically updating the interface with backend data.

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

Expanding it into a web-based application introduced another interesting challenge: connecting a low-level application to a browser while keeping its core logic independent of the frontend.

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

```text
C Fundamentals
      |
      v
Rust & Systems Programming
      |
      v
Computer Networking
      |
      v
Network Security
      |
      v
AI Security
```

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

The addition of a web interface also gave me an opportunity to explore how C applications can interact with other technologies without abandoning their original architecture.

I may return to C for future projects whenever its capabilities and learning opportunities make it the right tool for the job.

For now, this repository stands as a practical record of my progress and a foundation for the work that comes next.

---

<div align="center">

**C was the foundation. Systems programming is the curiosity. Cybersecurity is the direction.**

</div>

