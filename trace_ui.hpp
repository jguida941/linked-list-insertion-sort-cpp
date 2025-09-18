// Minimal ANSI-colored, bordered trace UI for linked lists
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <regex>

#if defined(_WIN32)
#  include <io.h>
#  define ISATTY _isatty
#  define FILENO _fileno
#else
#  include <unistd.h>
#  define ISATTY isatty
#  define FILENO fileno
#endif

namespace traceui {

// New helper function
inline std::string strip_ansi(const std::string& s) {
    // Regex to match ANSI escape codes
    const std::regex ansi_re("\x1b\[[0-9;]*m");
    return std::regex_replace(s, ansi_re, "");
}

struct Term {
    bool color = ISATTY(FILENO(stdout)) != 0; // disable colors when not a TTY
    std::string reset()  const { return color ? "\x1b[0m"  : ""; }
    std::string bold()   const { return color ? "\x1b[1m"  : ""; }
    std::string dim()    const { return color ? "\x1b[2m"  : ""; }
    std::string fg(int c)const { return color ? ("\x1b[38;5;" + std::to_string(c) + "m") : ""; }
    std::string bg(int c)const { return color ? ("\x1b[48;5;" + std::to_string(c) + "m") : ""; }
} T;

// Palette (256-color safe)
static constexpr int C_HEAD = 39;    // cyan-ish
static constexpr int C_PREV = 220;   // yellow
static constexpr int C_CURR = 203;   // red
static constexpr int C_NEXT = 207;   // magenta
static constexpr int C_SPOT = 114;   // green
static constexpr int C_TEXT = 250;   // light gray
static constexpr int C_BORDER = 240; // border gray

template <class Node>
struct PtrRoles {
    const Node* H = nullptr; // head
    const Node* P = nullptr; // prev (end of sorted prefix)
    const Node* C = nullptr; // curr (node being placed)
    const Node* N = nullptr; // next (saved for loop)
    const Node* S = nullptr; // spot (after which to insert; null => head)
};

inline std::string repeat(const std::string& s, size_t n) {
    std::string out;
    out.reserve(s.size() * n);
    for (size_t i = 0; i < n; ++i) out += s;
    return out;
}
inline std::string boxTop(size_t w)    { return T.fg(C_BORDER) + "╔" + repeat("═", w) + "╗" + T.reset(); }
inline std::string boxBottom(size_t w) { return T.fg(C_BORDER) + "╚" + repeat("═", w) + "╝" + T.reset(); }
inline std::string boxMid(const std::string& s, size_t w) {
    size_t visible_len = strip_ansi(s).size();
    std::string pad = (w > visible_len) ? repeat(" ", w - visible_len) : "";
    return T.fg(C_BORDER) + "║ " + T.reset() + s + pad + T.fg(C_BORDER) + " ║" + T.reset();
}

template <class Node>
int roleColor(const Node* p, const PtrRoles<Node>& R) {
    if (p == R.C) return C_CURR;
    if (p == R.S) return C_SPOT;
    if (p == R.P) return C_PREV;
    if (p == R.N) return C_NEXT;
    if (p == R.H) return C_HEAD;
    return C_TEXT;
}

template <class Node>
char roleChar(const Node* p, const PtrRoles<Node>& R) {
    if (p == R.C) return 'C';
    if (p == R.S) return 'S';
    if (p == R.P) return 'P';
    if (p == R.N) return 'N';
    if (p == R.H) return 'H';
    return ' ';
}

/**
 * Pretty print the list row as: [v] -> [v] -> [v]
 * Below it, prints aligned role labels (H,P,C,N,S)
 * Wrapped in a border box with a title line.
 */
template <class Node, class GetterVal, class GetterNext>
void print_state(const char* title,
                 const Node* head,
                 const PtrRoles<Node>& R,
                 GetterVal getVal,      // int(const Node*)
                 GetterNext getNext) {  // const Node*(const Node*)
    std::vector<const Node*> order;
    std::vector<std::string> tokens;
    std::vector<size_t> widths; // width of token including trailing arrow

    const Node* p = head;
    while (p) {
        order.push_back(p);
        std::string box = "[" + std::to_string(getVal(p)) + "]";
        p = getNext(p);
        if (p) box += " -> ";
        tokens.push_back(box);
    }

    if (tokens.empty()) {
        tokens.push_back("(empty)");
        order.push_back(nullptr);
    }

    // Build first line with per-node color
    std::string line1;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Node* node = order[i];
        int col = roleColor(node, R);
        line1 += T.fg(col) + T.bold() + tokens[i] + T.reset();
        widths.push_back(tokens[i].size());
    }

    // Build aligned label line
    std::string line2;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Node* node = order[i];
        if (!node) continue; // Skip if null (for empty list case)
        char rc = roleChar(node, R);
        size_t w = widths[i];

        // Correctly center the role character under the [value] part
        std::string val_str = std::to_string(getVal(node));
        size_t box_width = val_str.size() + 2; // for '[' and ']'
        size_t target = box_width / 2;

        std::string seg(w, ' ');
        if (rc != ' ' && w > 0) seg[target] = rc;
        int col = roleColor(node, R);
        line2 += T.fg(col) + seg + T.reset();
    }

    // Title
    std::string titleLine = std::string(title);
    size_t w = std::max({strip_ansi(line1).size(), strip_ansi(line2).size(), titleLine.size()});
    std::cout << boxTop(w + 2) << "\n";
    std::cout << boxMid(T.bold() + titleLine + T.reset(), w) << "\n";
    std::cout << boxMid(line1, w) << "\n";
    std::cout << boxMid(line2, w) << "\n";
    std::cout << boxBottom(w + 2) << "\n";
}

// Convenience overload for Node with fields .data and .next
template <class Node>
void print_state(const char* title,
                 const Node* head,
                 const PtrRoles<Node>& R) {
    print_state<Node>(title, head, R,
        [](const Node* n) { return static_cast<int>(n->data); },
        [](const Node* n) { return n->next; }
    );
}

} // namespace traceui
