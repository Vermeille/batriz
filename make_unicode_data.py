#!/usr/bin/env python3

GRAPHEME_CLUSTER_BREAK = [
    'CR', 'LF', 'Control', 'Extend', 'ZWJ', 'Regional_Indicator', 'Prepend',
    'SpacingMark', 'L', 'V', 'T', 'LV', 'LVT', 'E_Base', 'E_Modifier',
    'Glue_After_Zwj', 'E_Base_GAZ', 'Any'
]


class UChar:

    def __init__(self, l):
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
        self.grapheme_cluster_break = GRAPHEME_CLUSTER_BREAK.index('Any')


def read_unicode_data():
    response = open('data/UnicodeData.txt')

    data = {}
    lines = response.read().split('\n')
    range_start = -1
    for l in lines:
        l = l.split('#')[0].strip()
        if not l:
            continue
        l = l.split(';')
        if range_start != -1 and not l[1].endswith('Last>'):
            raise ValueError("End of range not found")
        if range_start != -1:
            for i in range(range_start, int(l[0], 16) + 1):
                data[i] = UChar(l)
            range_start = -1
        elif l[1].endswith('First>'):
            range_start = int(l[0], 16)
        u = UChar(l)
        data[u.ID] = u
    return data


def read_break_prop(data):
    response = open('data/GraphemeBreakProperty.txt')

    lines = response.read().split('\n')
    for l in lines:
        l = l.split('#')[0].strip()
        if not l:
            continue

        code, prop = [x.strip() for x in l.split(";")]
        c = code.split("..")
        if len(c) > 1:
            for i in range(int(c[0], 16), int(c[1], 16) + 1):
                if not i in data:
                    pass  #print(hex(i) + " absent")
                else:
                    data[
                        i].grapheme_cluster_break = GRAPHEME_CLUSTER_BREAK.index(
                            prop)
        else:
            i = int(c[0], 16)
            if not i in data:
                pass  #print(hex(i) + " absent")
            else:
                data[i].grapheme_cluster_break = GRAPHEME_CLUSTER_BREAK.index(
                    prop)
    return data


def compress_table(data, chunk_size, key, default='0'):
    ds = []
    idxs = []  #0] * (0x110000 // chunk_size)
    for i in range(0, 0x110000, chunk_size):
        cur = ', '.join([(str(key(data[j])) if j in data else default)
                         for j in range(i, i + chunk_size)])
        try:
            idx = ds.index(cur)
        except:
            idx = len(ds)
            ds.append(cur)
        idxs.append(idx)
    total = len(idxs) + chunk_size * len(ds)
    return total, (ds, idxs)


def find_best_split(data, key):
    best_total_so_far = 0x110000
    print(best_total_so_far)
    best_split = 0
    best_table = None
    for i in range(1, 16):
        total, tables = compress_table(data, 2**i, key)
        print("packet size: {}, total: {}, len(data): {}, len(indexs): {}".
              format(2**i, total, len(2**i * tables[0]), len(tables[1])))
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
            int read_{0}(int cp) {{
                return {0}_data[{0}_idxs[cp >> {0}_shift]]
                                     [cp & ((1 << ({0}_shift)) - 1)];
            }}'''.format(table_name),
            file=f)


from pprint import pprint
import sys

data = read_unicode_data()
data = read_break_prop(data)


def test(key):
    global best_table
    bt = ([[int(y) for y in x.split(',')]
           for x in best_table[0]], best_table[1])
    print(len(bt[0]))
    print(len(bt[0][0]))
    for i in range(0, 0x110000):
        if not i in data:
            continue
        ref = key(data[i])
        idx = bt[1][i >> best_split]
        idx2 = i & ((1 << (best_split)) - 1)
        tab = bt[0][idx][idx2]
        if not ref == tab and not data[i].name.endswith('>'):
            print(data[i].name + ": " + str(ref) + " / " + str(tab))


print('Generate combining tables...')
best_split, best_table = find_best_split(data, key=lambda u: u.combining)
test(key=lambda u: u.combining)
print_table('combining', best_split, best_table)
print('Generate uppercase tables...')
best_split, best_table = find_best_split(data, key=lambda u: u.uppercase)
test(key=lambda u: u.uppercase)
print_table('upper', best_split, best_table)
print('Generate lowercase tables...')
best_split, best_table = find_best_split(data, key=lambda u: u.lowercase)
test(key=lambda u: u.lowercase)
print_table('lower', best_split, best_table)
print('Generate grapheme_cluster_break tables...')
best_split, best_table = find_best_split(
    data, key=lambda u: u.grapheme_cluster_break)

print_table('grapheme_cluster_break', best_split, best_table)
