import sys


def main(args):
    if len(args) < 4:
        print('USAGE: nandsplit.py <input file> <page size> <spare size> <pages per block> <partition size > [...]')
        return -1
    idx = 0
    fname = args[0]
    pagesize = int(args[1], 0)
    sparesize = int(args[2], 0)
    pages_per_block = int(args[3], 0)
    blocksize = pages_per_block * pagesize
    parts = []

    with open(fname, 'rb') as f:
        for size in args[4:]:
            to_copy = int(size, 0)

            parts.append((idx, to_copy))

            to_copy += int(to_copy/pagesize) * sparesize # add spare

            with open(fname + 'p'+str(idx), 'wb') as fout:
                while to_copy != 0:
                    chunk = min(to_copy, 4096)
                    data = f.read(chunk)
                    if len(data) != chunk:
                        raise Exception('Input file too small')
                    fout.write(data)
                    to_copy -= chunk

            idx += 1

    print("Parts: "+(','.join([hex(int(x[1] / blocksize)) for x in parts])))

    print("nandwrite commands:")
    for partidx, _ in parts:
        print("nandwrite -o /dev/mtd{0} {1}p{0}".format(partidx, fname))

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
