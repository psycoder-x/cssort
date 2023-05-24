# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "stdbool.h"

typedef struct ProtoLine {
  bool invalid;
  bool eof;
  size_t num;
  size_t cap;
  char *at;
} ProtoLine;

typedef struct Line {
  size_t num;
  const char *at;
} Line;

typedef struct LineList {
  bool invalid;
  size_t num;
  size_t cap;
  Line *at;
} LineList;

LineList get_all_lines();
LineList cs_sort(LineList list);
bool print_lines(LineList list);
ProtoLine get_proto_line();
Line get_line(ProtoLine pline);
Line *find_firstest(Line *begin, size_t num);
int compare_lines(Line a, Line b);

const char *e_alloc_msg = "cssort: error: fail to allocate memory";

int main() {
  LineList lines;
  lines = get_all_lines();
  lines = cs_sort(lines);
  bool ok = print_lines(lines);
  return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

LineList get_all_lines() {
  LineList list = {0};
  list.num = 0;
  list.cap = 10;
  list.invalid = false;
  list.at = malloc(sizeof(list.at[0]) * list.cap);
  if (!list.at) {
    fprintf(stderr, "%s\n", e_alloc_msg);
    list.invalid = true;
    return list;
  }
  for (;;) {
    ProtoLine pline = get_proto_line();
    if (pline.invalid) {
      list.invalid = true;
      return list;
    }
    if (pline.eof)
      break;
    if (list.cap == list.num) {
      list.cap *= 2;
      Line *ptr = realloc(list.at, sizeof(list.at[0]) * list.cap);
      if (!ptr) {
        fprintf(stderr, "%s\n", e_alloc_msg);
        list.invalid = true;
        return list;
      }
      list.at = ptr;
    }
    list.at[list.num] = get_line(pline);
    list.num++;
  }
  return list;
}

LineList cs_sort(LineList list) {
  if (list.invalid)
    return list;
  if (list.num <= 1)
    return list;
  for (size_t i = 0; i < list.num - 1; i++) {
    Line *firstest = find_firstest(&list.at[i], list.num - i);
    Line t = list.at[i];
    list.at[i] = *firstest;
    *firstest = t;
  }
  return list;
}

Line *find_firstest(Line *begin, size_t num) {
  if (num == 0)
    return begin;
  Line *line = begin;
  for (size_t i = 1; i < num; i++) {
    if (compare_lines(begin[i], *line) < 0)
      line = &begin[i];
  }
  return line;
}

int compare_lines(Line a, Line b) {
  size_t i = 0;
  for (; i < a.num && i < b.num; i++) {
    int diff = (int)a.at[i] - (int)b.at[i];
    if (diff != 0)
      return diff;
  }
  if (a.num < b.num)
    return -1;
  if (a.num > b.num)
    return 1;
  return 0;
}

bool print_lines(LineList list) {
  if (list.invalid)
    return false;
  for (size_t i = 0; i < list.num; i++) {
    fwrite(list.at[i].at, sizeof(list.at[i].at[0]), list.at[i].num, stdout);
    putchar('\n');
  }
  return true;
}

ProtoLine get_proto_line() {
  ProtoLine pline = {0};
  pline.num = 0;
  pline.cap = 1;
  pline.invalid = false;
  pline.eof = false;
  pline.at = malloc(sizeof(pline.at[0]) * pline.cap);
  if (!pline.at) {
    fprintf(stderr, "%s\n", e_alloc_msg);
    pline.invalid = true;
    return pline;
  }
  for (;;) {
    int ch = getchar();
    if (pline.num == 0 && ch == EOF) {
      pline.eof = true;
      return pline;
    }
    if (ch == '\n')
      break;
    if (pline.cap == pline.num) {
      pline.cap *= 2;
      char *ptr = realloc(pline.at, sizeof(pline.at[0]) * pline.cap);
      if (!ptr) {
        fprintf(stderr, "%s\n", e_alloc_msg);
        pline.invalid = true;
        return pline;
      }
      pline.at = ptr;
    }
    pline.at[pline.num] = (char)ch;
    pline.num++;
  }
  return pline;
}

Line get_line(ProtoLine pline) {
  return (Line) {
    .num = pline.num,
    .at = pline.at
  };
}
