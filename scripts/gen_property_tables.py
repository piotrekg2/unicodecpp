#!/usr/bin/env python3
import argparse
import os
import sys


def strip_comment(line):
  line = line.strip()
  hash_idx = line.find('#')
  return line if hash_idx == -1 else line[:hash_idx]


def parse_char_range(stripped_line):
  values = list(map(lambda x: int(x, 16), stripped_line.split('..', 1)))
  low, high = values[0], values[-1]
  if low > high:
    raise ValueError('low > high in "{0}"'.format(stripped_line))
  return low, high


def parse_line(line):
  columns = list(map(lambda s: s.strip(), line.split(';')))
  if len(columns) != 2:
    msg = 'Incorrect number of columns in "{0}", expected 2 found {1}'
    raise ValueError(msg.format(line, len(columns)))
  char_range = parse_char_range(columns[0])
  return (char_range, columns[1])


# Merges intersecting intervals
def normalize(ranges):
  res = []
  for low, high in sorted(ranges):
    if len(res) == 0 or res[-1][1] < low:
      res.append((low, high))
    else:
      res[-1] = (res[-1][0], max(res[-1][1], high))
  return res


def find_any_itersection(ranges1, ranges2):
  # Assumes that both lists are sorted
  i, j = 0, 0
  while i < len(ranges1) and j < len(ranges2):
    if ranges1[i][0] > ranges2[j][0]:
      i, j = j, i
      ranges1, ranges2 = ranges2, ranges1
    if ranges1[i][1] >= ranges2[j][0]:
      return ranges1[i], ranges2[j]
    i += 1
    j += 1
  return None


def verify_disjointness(properties):
  for p1 in properties:
    for p2 in properties:
      if p1 == p2:
        continue
      intersection = find_any_itersection(properties[p1], properties[p2])
      if intersection is not None:
        msg = 'Found two conflicting properties "{0}" and "{1}", ' +\
              'intersecting ranges are {2}'
        raise ValueError(msg.format(p1, p2, intersection))


def load_property_file(path):
  with open(path) as f:
    res = {}
    line_no = 0
    for line in f:
      line_no += 1
      line = strip_comment(line)
      # Ignore empty lines
      if len(line) == 0:
        continue
      try:
        char_range, property_name = parse_line(line)
      except ValueError as ex:
        raise ValueError('Incorrect line {0}'.format(line_no)) from ex
      ranges = res.get(property_name, [])
      ranges.append(char_range)
      res[property_name] = ranges
    for property_name in res:
      res[property_name] = normalize(res[property_name])
    verify_disjointness(res)
    return res


def generate_enum(enum_name, properties):
  template = '''enum class {0} : char
{{
  None = 0,

  {1},

  Last
}};'''
  return template.format(enum_name, ',\n  '.join(sorted(properties)))


def generate_tables(enum_name, table_name, snd_lvl_size, properties):
  ranges = []
  for p in properties:
    for low, high in properties[p]:
      ranges.append((low, high, p))
  ranges.sort()

  fst_lvl_table = []
  snd_lvl_tables = []
  low = 0
  i = 0
  while low <= ranges[-1][1]:
    snd_lvl_table = []

    for cp in range(low, low + snd_lvl_size):
      while i < len(ranges) and ranges[i][1] < cp:
        i += 1
      prop = ranges[i][2] if i < len(ranges) and ranges[i][0] <= cp else 'None'
      snd_lvl_table.append(prop)

    if all(x == 'None' for x in snd_lvl_table):
      fst_lvl_table.append('nullptr')
    else:
      snd_lvl_table_name = '{0}_{1}'.format(table_name, low // snd_lvl_size)
      fst_lvl_table.append(snd_lvl_table_name)
      snd_lvl_tables.append((snd_lvl_table_name, snd_lvl_table))

    low += snd_lvl_size

  res = ''
  template = '''static constexpr {0} {1}[] = {{
  {2}
}};'''
  for snd_lvl_table_name, snd_lvl_table in snd_lvl_tables:
    values = map(lambda x: '{0}::{1}'.format(enum_name, x), snd_lvl_table)
    res += template.format(enum_name, snd_lvl_table_name, ',\n  '.join(values))
    res += '\n\n'

  template = '''static constexpr const {0}* {1}[] = {{
  {2}
}};'''
  res += template.format(enum_name, table_name, ',\n  '.join(fst_lvl_table))
  res += '\n\nstatic constexpr uint32_t lastCp = {0};'\
          .format(hex(ranges[-1][1]))
  res += '\n\nstatic constexpr uint32_t sndLvlTableSize = {0};'\
          .format(snd_lvl_size)
  return res


def main():
  parser = argparse.ArgumentParser(
    description='Generate property tables.')
  parser.add_argument('file', metavar='FILE',
    help='A file with a definition of properties.')
  parser.add_argument('--enum-name', default='EnumName',
    help='A name of the generated enum class. Defaults to "EnumName".')
  parser.add_argument('--table-name', default='TableName',
    help='A name of the generated table. Defaults to "TableName".')
  parser.add_argument('--snd-lvl-size', type=int, default=256,
    help='A size of the second level table.')
  args = parser.parse_args()

  properties = load_property_file(args.file)
  template = '''// This file was generated automatically, do not edit directly.
{0}

{1}
'''
  print(template.format(generate_enum(args.enum_name, properties),
                        generate_tables(args.enum_name, args.table_name,
                                        args.snd_lvl_size, properties)))


if __name__ == '__main__':
  main()

