import sys


def main(args):
    if len(args) < 4:
        print('USAGE: stripspare.py <input file> <page size> <spare size> <output file>')
        return -1
    idx = 0
    infile = args[0]
    pagesize = int(args[1], 0)
    sparesize = int(args[2], 0)
    outfile = args[3]

    with open(infile, 'rb') as fin:
        with open(outfile, 'wb') as fout:
            while True:
                data = fin.read(pagesize+sparesize)
                if len(data) == 0:
                    break
                fout.write(data[0:pagesize])


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
