# MyGit — A Simple Git-Like Version Control System

MyGit is a small version-control system written in C++ to explore how Git works internally. It implements a basic content-addressed object store, a staging index, commits, commit history, file comparison, and restoring files from a commit.

> **Note:** MyGit is an educational project, not a drop-in replacement for Git. It uses a simplified storage format and currently has limitations described below.

## Features

- `init` — initialize a repository.
- `hash-object <file>` — calculate a SHA-1 hash for a file and store its blob object.
- `add <file>` — store a file object and stage its filename and hash in the index.
- `status` — report modifications and deletions for tracked files.
- `commit "<message>"` — create a snapshot from the staged index.
- `log` — display commit history, newest first.
- `diff <file>` — compare a tracked file's staged version with its current working copy.
- `checkout <commit-hash>` — restore files listed in a commit and update the index to that snapshot.

## How It Works

### Repository layout

Running `mygit init` creates a `.mygit` directory in the current working directory:

```text
.mygit/
├── HEAD
├── index
├── objects/
└── refs/
    └── heads/
```

- **`objects/`** stores file blobs and commit objects. The object's SHA-1 hash determines its path: the first two hash characters form a directory, and the remaining characters form the filename.
- **`index`** stores staged entries as lines containing a filename and its blob hash.
- **`HEAD`** points to the current branch reference. The initial value is `ref: refs/heads/main`.
- **`refs/heads/main`** stores the latest commit hash after the first commit.

### Basic workflow

1. A file is read and represented as a blob object with a header and its contents.
2. The blob's SHA-1 hash identifies the object.
3. `add` records the filename and hash in `.mygit/index`.
4. `commit` saves the staged entries in a commit object and updates the current branch reference.
5. Each new commit records its parent commit hash, allowing `log` to walk backwards through history.

The object format and index are simplified for learning purposes and are not compatible with Git's internal formats.

## Requirements

- A C++ compiler with C++17 support, such as `g++`
- GNU Make
- OpenSSL development headers and libraries (for SHA-1)

On Ubuntu/Debian, install the dependencies with:

```bash
sudo apt update
sudo apt install g++ make libssl-dev
```

## Build

From the project directory (the directory containing `main.cpp` and `Makefile`):

```bash
make
```

The Makefile builds an executable named `mygit`.

To remove the executable and rebuild:

```bash
make clean
make
```

## Usage

Run the commands from **inside the directory you want to version-control**. MyGit stores repository data in that directory's `.mygit/` folder.

### 1. Initialize a repository

```bash
/path/to/mygit init
```

### 2. Hash and store a file

```bash
/path/to/mygit hash-object README.md
```

The command prints the object's SHA-1 hash.

### 3. Stage a file

```bash
/path/to/mygit add README.md
```

This stores the blob object and adds or updates the file's entry in `.mygit/index`.

### 4. Check status

```bash
/path/to/mygit status
```

The current implementation reports tracked files that have been modified or deleted since they were staged. It does not yet report untracked files or distinguish staged changes from committed changes.

### 5. Compare a file

```bash
/path/to/mygit diff README.md
```

This compares the staged blob with the working copy. Lines prefixed with `-` are from the staged version; lines prefixed with `+` are from the working copy. The comparison is a simple line-by-line comparison, not a full Git-style diff algorithm.

### 6. Create a commit

```bash
/path/to/mygit add README.md
/path/to/mygit commit "Add README"
```

**Current command syntax:** pass the message as one quoted argument. Although the usage text may mention `commit -m`, the current implementation does not parse `-m`; `mygit commit "message"` is the supported form.

A commit is created from the current index. Stage files before committing.

### 7. View commit history

```bash
/path/to/mygit log
```

Displays the current branch's commit history, newest first.

### 8. Restore a commit

```bash
/path/to/mygit log
/path/to/mygit checkout <full-commit-hash>
```

Replace `<full-commit-hash>` with a commit hash shown by `log`.

**Warning:** `checkout` overwrites files listed in the selected commit and replaces the index with that commit's entries. Back up any work you need before using it. The current implementation does not move the branch reference, remove extra working-directory files that are absent from the commit, or implement branch switching.

## Example Session

Assuming the executable is at `../mygit/mygit` relative to your test repository:

```bash
../mini_git/mygit init
echo "Hello from MyGit" > hello.txt
../mini_git/mygit add hello.txt
../mini_git/mygit commit "Initial snapshot"
../mini_git/mygit log
echo "A later version" > hello.txt
../mini_git/mygit status
../mini_git/mygit diff hello.txt
```

Use the path to your built `mygit` executable in place of `../mini_git/mygit` if your directories differ.

## Current Limitations

- This is a learning project and does not aim for compatibility with Git.
- Objects are stored uncompressed; real Git compresses objects.
- The object and commit formats are custom, simplified formats.
- The index format is plain text and does not support filenames containing spaces.
- `status` reports changes to tracked files, but does not detect untracked files or fully separate staged, unstaged, and committed changes.
- `diff` compares the working file against the staged version using a simple line-by-line comparison.
- `checkout` restores files from a commit but does not switch branches or update the branch reference.
- Branch creation and merging are not implemented.
- The current implementation is a starting point; additional validation and error handling would be needed for robust use.

## Project Goal

The goal of MyGit is to learn core version-control concepts by implementing them incrementally in C++: hashing, object storage, staging, snapshots, commit-parent links, history traversal, file diffs, and restoration.

