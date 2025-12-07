/*
 * Minimal ANSI-colored, bordered trace UI for linked lists
 */
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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

/* Strip ANSI codes to get visible string length */
inline std::string strip_ansi(const std::string& s) {
    std::string result;
    bool in_escape = false;
    for (char c : s) {
        if (c == '\x1b') {
            in_escape = true;
        } else if (in_escape && c == 'm') {
            in_escape = false;
        } else if (!in_escape) {
            result += c;
        }
    }
    return result;
}

struct Term {
    bool color = ISATTY(FILENO(stdout)) != 0;
    std::string reset()  const { return color ? "\x1b[0m"  : ""; }
    std::string bold()   const { return color ? "\x1b[1m"  : ""; }
    std::string dim()    const { return color ? "\x1b[2m"  : ""; }
    std::string fg(int c)const { return color ? ("\x1b[38;5;" + std::to_string(c) + "m") : ""; }
    std::string bg(int c)const { return color ? ("\x1b[48;5;" + std::to_string(c) + "m") : ""; }
} T;

/* Palette (256-color safe) */
static constexpr int C_HEAD   = 51;   /* bright cyan */
static constexpr int C_PREV   = 226;  /* bright yellow */
static constexpr int C_CURR   = 196;  /* bright red */
static constexpr int C_NEXT   = 213;  /* pink/magenta */
static constexpr int C_SPOT   = 46;   /* bright green */
static constexpr int C_TEXT   = 252;  /* light gray */
static constexpr int C_BORDER = 244;  /* medium gray */
static constexpr int C_TITLE  = 255;  /* white */

template <class Node>
struct PtrRoles {
    const Node* H = nullptr;  /* head */
    const Node* P = nullptr;  /* prev (end of sorted prefix) */
    const Node* C = nullptr;  /* curr (node being placed) */
    const Node* N = nullptr;  /* next (saved for loop) */
    const Node* S = nullptr;  /* spot (after which to insert; null => head) */
};

inline std::string repeat(const std::string& s, size_t n) {
    std::string out;
    out.reserve(s.size() * n);
    for (size_t i = 0; i < n; ++i) out += s;
    return out;
}

inline std::string boxTop(size_t w) {
    return T.fg(C_BORDER) + "┌" + repeat("─", w) + "┐" + T.reset();
}

inline std::string boxBottom(size_t w) {
    return T.fg(C_BORDER) + "└" + repeat("─", w) + "┘" + T.reset();
}

inline std::string boxDivider(size_t w) {
    return T.fg(C_BORDER) + "├" + repeat("─", w) + "┤" + T.reset();
}

inline std::string boxMid(const std::string& s, size_t w) {
    size_t visible_len = strip_ansi(s).size();
    std::string pad = (w > visible_len) ? repeat(" ", w - visible_len) : "";
    return T.fg(C_BORDER) + "│ " + T.reset() + s + pad + T.fg(C_BORDER) + " │" + T.reset();
}

/* Color for a specific role letter */
inline int colorForRole(char role) {
    switch (role) {
        case 'H': return C_HEAD;
        case 'P': return C_PREV;
        case 'C': return C_CURR;
        case 'N': return C_NEXT;
        case 'S': return C_SPOT;
        default:  return C_TEXT;
    }
}

/* Get primary color for a node (first matching role) */
template <class Node>
int roleColor(const Node* p, const PtrRoles<Node>& R) {
    if (p == R.C) return C_CURR;
    if (p == R.S) return C_SPOT;
    if (p == R.P) return C_PREV;
    if (p == R.N) return C_NEXT;
    if (p == R.H) return C_HEAD;
    return C_TEXT;
}

/* Get roles as vector of chars */
template <class Node>
std::vector<char> getRoles(const Node* p, const PtrRoles<Node>& R) {
    std::vector<char> roles;
    if (p == R.H) roles.push_back('H');
    if (p == R.P) roles.push_back('P');
    if (p == R.C) roles.push_back('C');
    if (p == R.N) roles.push_back('N');
    if (p == R.S) roles.push_back('S');
    return roles;
}

/* Build colored label with each letter in its own color */
template <class Node>
std::string roleLabel(const Node* p, const PtrRoles<Node>& R) {
    std::vector<char> roles = getRoles(p, R);
    if (roles.empty()) return " ";

    std::string label;
    for (size_t i = 0; i < roles.size(); ++i) {
        if (i > 0) label += T.fg(C_TEXT) + "/" + T.reset();
        label += T.fg(colorForRole(roles[i])) + T.bold() + std::string(1, roles[i]) + T.reset();
    }
    return label;
}

/* Plain text label for width calculation */
template <class Node>
std::string roleLabelPlain(const Node* p, const PtrRoles<Node>& R) {
    std::vector<char> roles = getRoles(p, R);
    if (roles.empty()) return " ";
    std::string label;
    for (size_t i = 0; i < roles.size(); ++i) {
        if (i > 0) label += "/";
        label += roles[i];
    }
    return label;
}

/* Build a colored legend string */
inline std::string buildLegend() {
    return T.fg(C_HEAD) + T.bold() + "H" + T.reset() + T.dim() + "=head " + T.reset() +
           T.fg(C_PREV) + T.bold() + "P" + T.reset() + T.dim() + "=prev " + T.reset() +
           T.fg(C_CURR) + T.bold() + "C" + T.reset() + T.dim() + "=curr " + T.reset() +
           T.fg(C_NEXT) + T.bold() + "N" + T.reset() + T.dim() + "=next " + T.reset() +
           T.fg(C_SPOT) + T.bold() + "S" + T.reset() + T.dim() + "=spot" + T.reset();
}

/**
 * Pretty print the list with colored nodes and role labels.
 */
template <class Node, class GetterVal, class GetterNext>
void print_state(const char* title,
                 const Node* head,
                 const PtrRoles<Node>& R,
                 GetterVal getVal,
                 GetterNext getNext,
                 const Node* isolated = nullptr) {
    std::vector<const Node*> order;
    std::vector<std::string> tokens;
    std::vector<size_t> widths;

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

    /* Build list line with per-node color */
    std::string line1 = "  ";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Node* node = order[i];
        int col = roleColor(node, R);
        line1 += T.fg(col) + T.bold() + tokens[i] + T.reset();
        widths.push_back(tokens[i].size());
    }

    /* Build aligned label line with colored labels */
    std::string line2 = "  ";
    std::string line2_plain = "  ";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Node* node = order[i];
        if (!node) continue;

        std::string label = roleLabel(node, R);
        std::string label_plain = roleLabelPlain(node, R);
        size_t w = widths[i];
        size_t label_len = label_plain.size();

        /* Center the label under the node */
        std::string val_str = std::to_string(getVal(node));
        size_t box_width = val_str.size() + 2;
        size_t center = box_width / 2;

        /* Calculate padding */
        size_t pad_left = (center >= label_len / 2) ? center - label_len / 2 : 0;
        size_t pad_right = (w > pad_left + label_len) ? w - pad_left - label_len : 0;

        line2 += std::string(pad_left, ' ') + label + std::string(pad_right, ' ');
        line2_plain += std::string(pad_left, ' ') + label_plain + std::string(pad_right, ' ');
    }

    /* Calculate box width */
    std::string titleStr = std::string(title);
    std::string legend = buildLegend();
    size_t w = std::max({strip_ansi(line1).size(),
                         line2_plain.size(),
                         titleStr.size(),
                         strip_ansi(legend).size()});

    /* Build isolated node line if present */
    std::string isolatedLine;
    std::string isolatedPlain;
    if (isolated) {
        isolatedLine = T.fg(C_CURR) + T.bold() + "C" + T.reset() + T.dim() + " (isolated): " + T.reset() +
                       T.fg(C_CURR) + T.bold() + "[" + std::to_string(getVal(isolated)) + "]" + T.reset();
        isolatedPlain = "C (isolated): [" + std::to_string(getVal(isolated)) + "]";
        w = std::max(w, isolatedPlain.size());
    }

    /* Print the box */
    std::cout << "\n";
    std::cout << boxTop(w + 2) << "\n";
    std::cout << boxMid(T.fg(C_TITLE) + T.bold() + titleStr + T.reset(), w) << "\n";
    std::cout << boxDivider(w + 2) << "\n";
    std::cout << boxMid(line1, w) << "\n";
    std::cout << boxMid(line2, w) << "\n";
    if (isolated) {
        std::cout << boxMid(isolatedLine, w) << "\n";
    }
    std::cout << boxDivider(w + 2) << "\n";
    std::cout << boxMid(legend, w) << "\n";
    std::cout << boxBottom(w + 2) << "\n";
}

/* Convenience overload for Node with fields .data and .next */
template <class Node>
void print_state(const char* title,
                 const Node* head,
                 const PtrRoles<Node>& R,
                 const Node* isolated = nullptr) {
    print_state<Node>(title, head, R,
        [](const Node* n) { return static_cast<int>(n->data); },
        [](const Node* n) { return n->next; },
        isolated
    );
}

} /* namespace traceui */
