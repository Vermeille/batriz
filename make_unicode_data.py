#!/bin/env python3

response = open('data/UnicodeData.txt')


class UChar:

    def __init__(self, l):
        l = l.split(';')
        self.ID = int(l[0], 16)
        self.name = l[1]
        self.category = l[2]
        self.combining = int(l[3])
        self.bidi = l[4]
        self.decomp = l[5]
        self.decimal = l[6]
        self.digit = l[7]
        self.numeric = l[8]
        self.bidi_mirrored = l[9]
        self.unicode_1_name = l[10]
        self.ISO_comment = l[11]
        self.uppercase = int(l[12], 16) - self.ID if l[12] else 0
        self.lowercase = int(l[13], 16) - self.ID if l[13] else 0
        self.titlecase = l[14]


data = {}
lines = response.read().split('\n')
for l in lines:
    l = l.split('#')[0].strip()
    if not l:
        continue
    u = UChar(l)
    data[u.ID] = u


def compress_table(data, chunk_size):
    ds = []
    idxs = [0] * (0x110000 // chunk_size)
    for i in range(0, 0x110000, chunk_size):
        cur = ', '.join([(str(data[j].combining) if j in data else '0')
                         for j in range(i, i + chunk_size)])
        try:
            idx = ds.index(cur)
        except:
            idx = len(ds)
            ds.append(cur)
        idxs.append(idx)
    total = len(idxs) + chunk_size * len(ds)
    return total, (ds, idxs)


def find_best_split(data):
    best_total_so_far = 0x110000
    print(best_total_so_far)
    best_split = 0
    best_table = None
    for i in range(1, 16):
        total, tables = compress_table(data, 2**i)
        print("packet size: {}, total: {}, len(data): {}, len(indexs): {}".
              format(2**i, total, len(tables[0]), len(tables[1])))
        if total < best_total_so_far:
            best_table = tables
            best_split = i
            best_total_so_far = total
    print("best packet size: {}".format(2**best_split))
    print("%d%% of total size" % int(best_total_so_far / 0x110000 * 100))
    return best_split, best_table


def print_table(table_name, best_split, best_table):
    with open('src/str/%s_tables.cpp' % table_name, 'w') as f:
        print(
            'static const int {}_shift = {};'.format(table_name, best_split),
            file=f)
        print(
            'static const int {}_idxs[{}] = {{'.format(table_name,
                                                       len(best_table[1])),
            file=f)
        for i in best_table[1]:
            print('    {},'.format(i), file=f)
        print('};', file=f)
        print(
            'static const int {}_data[{}][{}] = {{'.format(
                table_name, len(best_table[0]), 2**best_split),
            file=f)
        for d in best_table[0]:
            print('    {{ {} }},'.format(d), file=f)
        print('};', file=f)
        print(
            '''
            bool read_{0}(int cp) {{
                return {0}_data[{0}_idxs[cp >> {0}_shift]]
                                     [cp & ((1 << ({0}_shift + 1)) - 1)];
            }}'''.format(table_name),
            file=f)


best_split, best_table = find_best_split(data)
print_table('combining', best_split, best_table)
