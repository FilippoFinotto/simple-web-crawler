# C Multithreaded Web Crawler

This is a simple yet robust, multithreaded web crawler written entirely in C from the ground up. It is designed to be efficient and scalable, capable of fetching and parsing multiple web pages concurrently. This project was developed as a practical exercise in system programming, data structures, and concurrent programming in C.

## Table of Contents

- [Key Features](#key-features)
- [Project Architecture](#project-architecture)
- [Dependencies](#dependencies)
- [How to Build and Run](#how-to-build-and-run)
  - [Building](#building)
  - [Running](#running)
- [Detailed Module Implementation](#detailed-module-implementation)
  - [`queue.c` - The Frontier Manager](#queuec---the-frontier-manager)
  - [`hashtable.c` - The Visited Set](#hashtablec---the-visited-set)
  - [`http.c` - The Page Downloader](#httpc---the-page-downloader)
  - [`parser.c` - The Link Extractor](#parserc---the-link-extractor)
  - [`crawler.c` - The Main Orchestrator](#crawlerc---the-main-orchestrator)
- [Multithreading Model](#multithreading-model)
  - [Worker Thread (Producer/Consumer) Model](#worker-thread-producerconsumer-model)
  - [Synchronization with Mutexes](#synchronization-with-mutexes)
- [Future Work](#future-work)

## Key Features

- **Multithreaded Architecture**: Utilizes POSIX Threads (Pthreads) to download multiple pages concurrently, dramatically increasing throughput for an I/O-bound task.
- **Dynamic URL Frontier**: Manages the queue of URLs to visit using a custom-built FIFO queue implemented with a linked list for efficient O(1) enqueue and dequeue operations.
- **Duplicate URL Detection**: Employs a custom hash table with separate chaining to keep track of visited URLs, achieving O(1) average time complexity for insertions and lookups.
- **Robust HTTP Handling**: Leverages the `libcurl` library as a wrapper to handle complex HTTP/HTTPS requests, redirects, and timeouts.
- **HTML5 Parsing**: Uses the Google Gumbo library to parse potentially malformed real-world HTML into a navigable DOM tree.
- **URL Normalization & Validation**: Includes logic to convert relative URLs (e.g., `/about.html`) into absolute URLs and validates them to filter out non-HTTP links and common file types (images, documents).

## Project Architecture

The project is designed with a strong emphasis on **modularity and separation of concerns**. Each core component of the crawler is encapsulated in its own module, with a public interface defined in a corresponding header file (`.h`) and the implementation details hidden in the source file (`.c`).

This design makes the code easier to read, test, and maintain. The main modules are:

- **`queue.c`**: Manages the URL frontier.
- **`hashtable.c`**: Manages the set of visited URLs.
- **`http.c`**: Handles all network operations for downloading pages.
- **`parser.c`**: Handles all HTML parsing and link extraction.
- **`crawler.c`**: The main entry point that initializes all modules and orchestrates the worker threads.

## Dependencies

To build and run this project, you will need the following dependencies:

- A C compiler (e.g., `gcc`)
- `make`
- `libcurl` development files
- `gumbo-parser` development files

**On Fedora / RHEL / CentOS:**
```bash
sudo dnf install gcc make libcurl-devel gumbo-parser-devel
```

**On Debian / Ubuntu:**
```bash
sudo apt-get install gcc make libcurl4-openssl-dev libgumbo-dev
```

## How to Build and Run

### Building

The project includes a `Makefile` that handles all compilation and linking automatically.

1.  Clone the repository (or ensure all source files are in the same directory).
2.  Navigate to the project directory in your terminal.
3.  Run `make`:
    ```bash
    make
    ```
    This will compile all source files and create a single executable named `myprogram`. To do a clean rebuild, you can run `make clean && make`.

### Running

Execute the compiled program from the terminal:

```bash
./myprogram
```

The crawler will start with a hardcoded seed URL and begin crawling, printing its progress to the console. The number of threads and maximum pages to crawl can be configured via `#define` constants in `crawler.c`.

## Detailed Module Implementation

### `queue.c` - The Frontier Manager

- **Purpose**: To implement a thread-safe First-In-First-Out (FIFO) queue for managing the "frontier"—the list of URLs waiting to be crawled.
- **Implementation**: It is implemented as a **linked list**. A `Queue` struct maintains pointers to both the `head` and `tail` of the list. This design allows for highly efficient **O(1)** time complexity for both `enqueue` (adding to the tail) and `dequeue` (removing from the head) operations. Each `Node` in the queue allocates its own copy of the URL string, ensuring data independence.

### `hashtable.c` - The Visited Set

- **Purpose**: To provide a fast way to check if a URL has already been visited, preventing duplicate work and infinite loops.
- **Implementation**: It is implemented as a hash table using the **separate chaining** method for collision resolution.
  - The core data structure is an array of pointers. Each element of the array acts as a "bucket".
  - A hash function (**djb2 algorithm**) converts a URL string into a numerical hash value. This value, modulo the table size, determines the bucket index.
  - All URLs that hash to the same index are stored in a simple **linked list** at that bucket.
  - This provides **O(1) average time complexity** for insertions and searches. In the worst case (many collisions), performance degrades to O(n), but the djb2 algorithm provides a good distribution to make this rare.

### `http.c` - The Page Downloader

- **Purpose**: To abstract away the complexities of network programming. It acts as a **wrapper** around the powerful `libcurl` library.
- **Implementation**: The core function, `download_page`, orchestrates the entire download process for a single URL.
  - **Handle Management**: It uses the `curl_easy` interface, creating an "easy handle" (`CURL*`) for each request. This handle is configured with options like the URL, a user agent, and a timeout.
  - **Dynamic Buffer**: Since the size of a web page is unknown beforehand, this module implements a dynamically growing memory buffer.
  - **Callback Mechanism**: It uses `libcurl`'s callback system. We provide a `write_callback` function that `libcurl` calls repeatedly as it receives chunks of data from the network. This callback is responsible for:
    1.  Checking if the current buffer has enough capacity.
    2.  If not, using `realloc` to double the buffer's capacity.
    3.  Using `memcpy` to append the new data chunk to the end of the buffer.

### `parser.c` - The Link Extractor

- **Purpose**: To process the raw HTML text downloaded by `http.c` and extract all valid, crawlable links. It acts as a **wrapper** around the Google Gumbo HTML5 parser.
- **Implementation**:
  1.  **Parsing**: The main function `find_links` takes the HTML buffer and passes it to `gumbo_parse`, which returns a fully parsed **DOM tree**.
  2.  **Recursive Traversal**: A static helper function, `search_for_links_recursive`, navigates this tree. It starts at the root and recursively visits every node.
  3.  **Link Identification**: For each node, it checks if it's an HTML element (`GUMBO_NODE_ELEMENT`) and if its tag is `GUMBO_TAG_A` (an `<a>` link tag).
  4.  **Attribute Extraction**: If it's a link tag, it searches its attributes for an `href` value.
  5.  **URL Normalization and Validation**: Two helper functions are used:
      - `rel_to_abs_url`: Converts relative URLs (like `/path/page.html`) into absolute URLs by combining them with the `base_url` of the page they came from. It handles different cases like existing absolute URLs and root-relative URLs.
      - `is_valid`: Filters the resulting absolute URL. It ensures the protocol is `http` or `https` and checks the file extension against a blacklist to avoid crawling images, PDFs, CSS, etc.
  6.  **Collection**: Valid, absolute URLs are added to a new `Queue` which is returned to the caller.

### `crawler.c` - The Main Orchestrator

- **Purpose**: This is the "brain" of the program. It initializes all the data structures and manages the lifecycle of the worker threads.
- **Implementation**: Its `main` function is no longer the primary worker. It acts as a **manager**:
  1.  **Initialization**: Creates the shared `frontier` queue and `visited` hash table.
  2.  **Mutex Initialization**: Creates and initializes a `pthread_mutex_t` for each shared resource (`frontier`, `visited`, and a page counter) to ensure thread-safe access.
  3.  **Argument Struct**: Populates a `WorkerArgs` struct with pointers to all the shared resources and mutexes. This is done to pass state to the threads without using global variables.
  4.  **Thread Creation**: Launches a configurable number of worker threads using `pthread_create`, passing each one a pointer to the `WorkerArgs` struct.
  5.  **Waiting**: Waits for all worker threads to complete their execution using `pthread_join`.
  6.  **Cleanup**: Once all threads are finished, it safely destroys all mutexes and frees the memory used by the queue and the hash table.

## Multithreading Model

### Worker Thread (Producer/Consumer) Model

The crawler is implemented using a **Worker Thread** model, which is a variation of the Producer/Consumer pattern. This model was chosen because web crawling is an **I/O-bound** task, not a CPU-bound one. The program spends most of its time waiting for network responses.

- **The `main` thread** acts as the initial producer, seeding the `frontier` queue.
- **The worker threads** act as both consumers and producers:
  - They **consume** a URL from the `frontier`.
  - They process it (download, parse).
  - They **produce** new URLs and add them back to the `frontier`.

This concurrent model allows the program to have many network requests "in flight" simultaneously, effectively hiding the latency of waiting for any single request to complete. We use **Pthreads** as it is the standard threading library on POSIX-compliant systems and gives us the low-level control needed for this I/O-bound problem.

### Synchronization with Mutexes

Since all worker threads access the same `frontier` queue and `visited` hash table, their access must be synchronized to prevent race conditions and data corruption.

- **Granular Locking**: We use three separate mutexes (`pthread_mutex_t`): one for the `frontier`, one for the `visited` table, and one for the shared `pages_crawled` counter. This granular approach is more efficient than a single global lock, as it allows one thread to access the `frontier` while another simultaneously accesses the `visited` table.
- **Critical Sections**: Any block of code that reads from or writes to a shared resource is wrapped in `pthread_mutex_lock()` and `pthread_mutex_unlock()`.
- **Race Condition Prevention**: A local flag (`should_crawl`) is used to solve the classic "check-then-act" race condition. A thread locks the `visited` mutex, checks if a URL exists, inserts it if it doesn't, and sets its local flag—all within a single atomic operation. The decision to download the page is then made outside the critical section based on this safe, local flag.

## Future Work

- **Robots.txt Compliance**: Implement a parser for `robots.txt` to respectfully crawl websites and obey their disallow directives.
- **Politeness Delay**: Add a configurable delay between requests to the same domain to avoid overwhelming servers.
- **Sophisticated URL Filtering**: Improve the `is_valid` function to handle more edge cases and potentially use regular expressions.
- **Save to Disk**: Add functionality to save the crawled content or the list of visited URLs to a file.
- **Distributed Crawling**: As a major extension, the architecture could be adapted to run across multiple machines.
