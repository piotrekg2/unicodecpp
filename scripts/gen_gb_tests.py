#!/usr/bin/env python3
import argparse
import sys


def strip_comment(line):
  line = line.strip()
  hash_idx = line.find('#')
  return line if hash_idx == -1 else line[:hash_idx]


def load_test_file(filepath):
  with open(filepath) as f:
    line_no = 0
    res = []
    for line in f:
      line_no += 1
      line = strip_comment(line)
      # Skip empty lines
      if len(line) == 0:
        continue
      chars = line.split()
      code_points = []
      breaks = []
      for c in chars:
        if c == '×':
          breaks.append(False)
        elif c == '÷':
          breaks.append(True)
        else:
          code_points.append(int(c, 16))
      if len(code_points) + 1 != len(breaks):
        raise ValueError('')
      res.append((code_points, breaks))
    return res


def generate_test_table(tests, table_name):
  lines = []
  template = '{{ {{ {0} }} , {{ {1} }} }}'
  for code_points, boundaries in tests:
    line = template.format(
      ', '.join(map(lambda c: hex(c), code_points)),
      ', '.join(map(lambda b: '1' if b else '0', boundaries)))
    lines.append(line)
  template = \
'''std::vector<std::pair<std::vector<uint32_t>, std::vector<bool>>> {0} = {{
  {1}
}};'''
  return template.format(table_name, ',\n  '.join(lines))


def main():
  parser = argparse.ArgumentParser(
    description='Generate grapheme break tests.')
  parser.add_argument('file', metavar='FILE',
    help='A file with testcases.')
  parser.add_argument('--table-name', default='generatedTests',
    help='A name of the generated table.')
  args = parser.parse_args()

  tests = load_test_file(args.file)
  template = '''// This file was generated automatically, do not edit directly.
{0}
'''
  print(template.format(generate_test_table(tests, args.table_name)))


if __name__ == '__main__':
  main()

