# coursework_oop
University coursework on dynamic data structures implemented in C++
<h3 align="center">🎵 Song Manager & Karaoke Desktop App</h3>
<p align="center">A feature-rich C++/CLI desktop application for advanced song text processing, analysis, and interactive karaoke playback.</p>

---

### 🚀 About the Project
An object-oriented application designed to manage, analyze, and manipulate song structures stored in `song.dat`. Built with a native C++ core and a polished Windows Forms (`C++/CLI`) graphical interface.

**Key Features:**
* **Text Operations:** Word search (in a single verse or entire song), line replacement, and verse rearrangement (swapping and shifting).
* **Sonnet Analyzer:** Automatically identifies if a song is a sonnet and detects its type (English, Italian, French, Spenserian) based on rhyme schemes.
* **Vowel Word Sorting:** Simple selection sorting of words starting with vowels (following the Ukrainian alphabet).
* **Interactive Karaoke Mode:** Features real-time line switching, smooth timers, progress bars, a custom equalizer visualization, and "Start", "Pause", "Reset" controls.
* **Full Localization:** Ukrainian UI including main/context menus, toolbars, tooltips, and status bars.

---

### 💻 Tech Stack
* **Language & Standard:** C++14/17 (Core logic) & C++/CLI (UI layer)
* **IDE & Framework:** Microsoft Visual Studio, .NET Framework, Windows Forms
* **Core Libraries:** `<vector>`, `<string>`, `<map>`, `<algorithm>`, `<fstream>`

---

### 🗂️ Project Structure
| File / Component | Description |
| :--- | :--- |
| **`Song.h/.cpp`** & **`Verse.h/.cpp`** | Core data models, file I/O (`>>` / `<<`), search, and analysis logic. |
| **`TextUtils.h/.cpp`** | UTF-8 text processing, vowel checks, and rhyme key analysis. |
| **`MainForm.h/.cpp`** | Main desktop window (menus, toolbars, status bar). |
| **`KaraokeForm.h/.cpp`** | Dedicated interactive Karaoke window with playback controls. |
| **Custom UI Elements** | `GradientPanel`, `NoteProgressBar`, `EqualizerBars`, and custom themes. |
| **`song.dat`** | Sample data file (featuring the sonnet *"Autumn Lviv"*). |

---

### ⚙️ How to Run
1. Open **Microsoft Visual Studio** and create a **CLR Empty Project (.NET Framework)**.
2. Copy all source files into your project directory (alongside `.vcxproj`) and add them via **Project → Add Existing Item**.
3. Configure **Project Properties** (All Configurations):
   * *Linker → System → Subsystem:* `Windows (/SUBSYSTEM:WINDOWS)`
   * *Linker → Advanced → Entry Point:* `main`
   * *C/C++ → General → Common Language Runtime Support:* `/clr`
   * *C++ Language Standard:* Standard or C++14/17 (avoid C++20).
4. Build the project (`Ctrl + Shift + B`) and run (`F5`). Ensure `song.dat` is in the working directory.

---

### ⌨️ Keyboard Shortcuts
| Action | Shortcut | Action | Shortcut |
| :--- | :--- | :--- | :--- |
| **Open / Save** | `Ctrl + O` / `Ctrl + S` | **Swap Verses** | `Ctrl + T` |
| **Search (Verse / Song)** | `Ctrl + F` / `Ctrl + Shift + F` | **Last Lines** | `Ctrl + L` |
| **Replace Line** | `Ctrl + R` | **Sonnet Check** | `Ctrl + E` |
| **Vowel Sort** | `Ctrl + B` | **Karaoke Mode** | `Ctrl + K` |
